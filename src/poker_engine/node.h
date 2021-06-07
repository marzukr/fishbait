// Copyright 2021 Marzuk Rashid

#ifndef SRC_POKER_ENGINE_NODE_H_
#define SRC_POKER_ENGINE_NODE_H_

#include <algorithm>
#include <cstdint>
#include <limits>
#include <numeric>
#include <optional>
#include <random>
#include <stdexcept>
#include <string>

#include "SKPokerEval/src/SevenEval.h"
#include "utils/fraction.h"
#include "utils/random.h"

namespace poker_engine {

enum class Round : uint8_t { kPreFlop, kFlop, kTurn, kRiver };

enum class Action : uint8_t { kFold, kCheckCall, kBet, kAllIn };

struct Move {
  Action play;    // The action played by this move
  uint32_t size;  /* If the play is kBet, how much more the player is adding to
                     the pot. Otherwise it has no significance. */
};

template <uint8_t kPlayers = 6>
class Node {
 public:
  /*
    @brief Construct a node with arbitrary card information. By default,
        constructs a standard 50/100 game with each player starting with 100 big
        blinds, with arbitrary card information.

    @param button The index of the player on the button.
    @param big_blind How many chips the big blind is.
    @param small_blind How many chips the small blind is.
    @param ante How many chips each player pays as an ante each hand.
    @param big_blind_ante If the big blind pays the ante for all players
    @param default_stack How many chips each player starts with.
    @param straddles The number of players straddling on the first hand.
  */
  Node(uint8_t button = 0, uint32_t big_blind = 100, uint32_t small_blind = 50,
       uint32_t ante = 0, bool big_blind_ante = false,
       bool blind_before_ante = true, uint32_t default_stack = 10000,
       uint8_t straddles = 0)

         // Attributes
       : big_blind_{big_blind}, small_blind_{small_blind}, ante_{ante},
         big_blind_ante_{big_blind_ante}, blind_before_ante_{blind_before_ante},

         // Progress Information
         button_{button}, in_progress_{false}, round_{Round::kPreFlop},
         cycled_{0}, acting_player_{0}, pot_good_{kPlayers}, no_raise_{0},
         folded_{}, players_left_{kPlayers}, players_all_in_{0},

         // Chip Information
         pot_{0}, bets_{}, stack_{}, min_raise_{big_blind},
         max_bet_{big_blind} {
    button_ = button_ + kPlayers - 1; /* Since NewHand() increments the button
                                         position. */
    std::fill(stack_, stack_ + kPlayers, default_stack);
    NewHand(straddles);
  }  // Node()

  Node(const Node& other) = default;
  Node& operator=(const Node& other) = default;

  /*
    @brief Reset the state variables for the start of a new hand.

    @param straddles The number of players straddling on this hand.
  */
  void NewHand(uint8_t straddles = 0) {
    if (pot_ != 0) {
      throw std::logic_error("NewHand() called when pot is not empty. If the "
                             "pot isn't 0, this means AwardPot() hasn't been "
                             "called yet. The last pot must be awarded before "
                             "a new hand starts.");
    }

    // Attributes are constants and don't need to be set

    // Progress Information
    button_ = (button_ + 1) % kPlayers;
    in_progress_ = true;
    round_ = Round::kPreFlop;
    cycled_ = 0;
    acting_player_ = PlayerIndex(3);
    pot_good_ = kPlayers;
    no_raise_ = 0;
    std::fill(folded_, folded_ + kPlayers, false);
    players_left_ = kPlayers;
    players_all_in_ = 0;

    // Chip Information
    /* pot_ should already be zero from either the constructor or AwardPot() on
       the previous hand */
    /* bets_ should already be all zero from either the constructor or
       AwardPot() on the previous hand */
    /* stack_ should already set to the right amounts from either the
       constructor or AwardPot() on the previous hand */
    // min_raise_ will be set after the straddle is posted
    // max_bet_ will be set after the blinds and antes are posted

    // Post Blinds, Antes, and Straddles
    uint32_t effective_ante = ante_;
    if (ante_ > 0 && !blind_before_ante_) effective_ante = PostAntes();
    PostBlind(PlayerIndex(1), small_blind_);
    PostBlind(PlayerIndex(2), big_blind_);
    if (ante_ > 0 && blind_before_ante_) effective_ante = PostAntes();
    uint32_t effective_blind = PostStraddles(straddles);
    min_raise_ = effective_blind;
    max_bet_ = effective_blind + effective_ante;

    CyclePlayers(false);
  }  // NewHand()

  /*
    @brief Get the index of a player relative to the button where default
        position is when the button is at index 0.

    @param default_position The default position of the player to get the index
        of. Button is 0, small blind 1, big blind 2, etc. If it's heads up, 0
        and 1 return the same index.
  */
  uint8_t PlayerIndex(uint8_t default_position) const {
    if (kPlayers == 2 && default_position > 0) default_position -= 1;
    return (button_ + default_position) % kPlayers;
  }

  /* 
    @brief How many times the betting has gone around on this betting round.
  */ 
  uint8_t Rotation() const { return cycled_ / kPlayers; }

  /*
    @brief Returns true if the current acting_player_ can check/call. Otherwise
        returns false. Also returns false if the player must go all in to call.
  */
  bool CanCheckCall() const {
    uint32_t prev_bet = bets_[acting_player_];
    uint32_t needed_to_call = max_bet_ - prev_bet;
    return in_progress_ && needed_to_call < stack_[acting_player_];
  }

  /*
    @brief Returns true if the current acting_player_ can bet the given amount.
        Otherwise returns false. Also returns false if the given amount is all
        of the players remaining chips.
    
    @param size The amount of additional chips that the player would put into
        the pot.
  */
  bool CanBet(uint32_t size) const {
    uint32_t prev_bet = bets_[acting_player_];
    uint32_t total_bet = size + prev_bet;
    uint32_t raise_size = total_bet - max_bet_;
    return in_progress_ &&
           pot_good_ > 0 &&
           total_bet > max_bet_ &&
           raise_size >= min_raise_ &&
           size < stack_[acting_player_];
  }  // CanBet()

  /*
    @brief Apply the given move to this Node object. Must only be called when
        the game is in progress.

    @param next The Move to apply to this Node.

    @return True if the game is still in progress after this move, false if this
        move has ended the game.
  */
  bool Apply(Move next) {
    if (!in_progress_) {
      throw std::logic_error("Apply() called when a game is not in progress.");
    }

    switch (next.play) {
      case Action::kFold:
        Fold();
        break;
      case Action::kAllIn:
        AllIn();
        break;
      case Action::kCheckCall:
        if (CanCheckCall()) {
          CheckCall();
        } else {
          throw std::invalid_argument("Check/Call is not a valid move for the "
                                      "current acting player.");
        }
        break;
      case Action::kBet:
        if (CanBet(next.size)) {
          Bet(next.size);
        } else {
          std::string err = "Betting " + std::to_string(next.size) + " is not "
                            "a valid move for the current acting player.";
          throw std::invalid_argument(err);
        }
        break;
    }  // switch next.play
    CyclePlayers(true);
    return in_progress_;
  }  // Apply()

  constexpr static struct SameInitialStack {} same_initial_stack_{};
  /*
    @brief Allocate the pot to the winner(s) at the end of a hand.
    
    After calling, pot will be 0, all bets will be 0, and the winner(s)'s stack
    will have increased by the appropriate amount. Must only be called when the
    game is no longer in progress and AwardPot() hasn't already been called for
    the current hand. This version applies optimizations based on the assumption
    that all players started the hand with the same stack amount. The pot will
    not be awarded correctly if this is not the case. Note, this function will
    also not work with a big blind ante because the big blind has less effective
    chips than the other players. The template type determines what type is used
    to represent exact award amounts before rounding (double or
    utils::Fraction). This choice could impact the accuracy of the awarded chip
    amounts.

    @param hands 2d array of each player's hand. Rows for each player, columns
        for each card. i.e. row 2 column 0 is player 2's 0th card. SKEval
        indexing. Nullptr is a valid option if all but one player has
        folded.
    @param board Array of the public board cards. Follows the standard order.
        i.e. columns [0,2] are the flop, column 3 is the turn, and column 4 is
        the river. SKEval indexing. Nullptr is a valid option if all but one
        player has folded.
  */
  template <typename T>
  void AwardPot(SameInitialStack, const uint8_t hands[kPlayers][2] = nullptr,
                const uint8_t board[5] = nullptr) {
    VerifyAwardablePot();
    if (players_left_ == 1) return FoldVictory(folded_);
    uint16_t ranks[kPlayers];
    RankPlayers(hands, board, folded_, ranks);
    BestPlayersData best_players = BestPlayers(ranks, folded_);
    T exact_awards[kPlayers]{};
    T pot_precise = static_cast<T>(pot_);
    DivideSidePot(pot_precise, ranks, folded_, best_players, exact_awards);
    DistributeChips(&pot_, exact_awards, stack_);
    std::fill(bets_, bets_ + kPlayers, 0);
  }  // AwardPot()

  constexpr static struct SingleRun {} single_run_{};
  /*
    @brief Allocate the pot to the winner(s) at the end of a hand.
    
    This version can only do one run of the board. After calling, pot will be 0,
    all bets will be 0, and the winner(s)'s stack will have increased by the
    appropriate amount. Must only be called when the game is no longer in
    progress and AwardPot() hasn't already been called for the current hand. The
    template type determines what type is used to represent exact award amounts
    before rounding (double or utils::Fraction). This choice could impact the
    accuracy of the awarded chip amounts.

    @param hands 2d array of each player's hand. Rows for each player, columns
        for each card. i.e. row 2 column 0 is player 2's 0th card. SKEval
        indexing. If a player's two cards are the same, this represents a mucked
        or folded hand. Nullptr is a valid option if all but one player has
        folded.
    @param board Array of the public board cards. Follows the standard order.
        i.e. columns [0,2] are the flop, column 3 is the turn, and column 4 is
        the river. SKEval indexing. Nullptr is a valid option if all but one
        player has folded.
  */
  template <typename T>
  void AwardPot(SingleRun, const uint8_t hands[kPlayers][2] = nullptr,
                const uint8_t board[5] = nullptr) {
    VerifyAwardablePot();

    bool processed[kPlayers];
    uint8_t players_to_award = PlayersToProcess(hands, processed);
    if (players_to_award == 1) {
      return FoldVictory(processed);
    } else if (hands == nullptr || board == nullptr) {
      throw std::invalid_argument("AwardPot() called with hands = nullptr or "
                                  "board = nullptr when there is more than 1 "
                                  "player remaining in the game. If there is "
                                  "more than 1 player who has not folded, we "
                                  "need hand and card information to award the "
                                  "pot.");
    }

    uint16_t ranks[kPlayers];
    RankPlayers(hands, board, processed, ranks);

    // Loop through each side pot and award it to the appropriate player(s)
    T exact_awards[kPlayers]{};
    while (players_to_award > 0) {
      T side_pot = static_cast<T>(AllocateSidePot(bets_, processed));
      BestPlayersData best_players = BestPlayers(ranks, processed);
      DivideSidePot(side_pot, ranks, processed, best_players, exact_awards);
      players_to_award -= MarkProcessedPlayers(bets_, processed);
    }  // while players_to_award > 0
    DistributeChips(&pot_, exact_awards, stack_);
  }  // AwardPot()

  constexpr static struct MultiRun {} multi_run_{};
  /*
    @brief Allocate the pot to the winner(s) at the end of a hand.
    
    This version runs the board multiple times. After calling, pot will be 0,
    all bets will be 0, and the winner(s)'s stack will have increased by the
    appropriate amount. Must only be called when the game is no longer in
    progress and AwardPot() hasn't already been called for the current hand. The
    template type determines what type is used to represent exact award amounts
    before rounding (double or utils::Fraction). This choice could impact the
    accuracy of the awarded chip amounts.

    @param hands 2d array of each player's hand. Rows for each player, columns
        for each card. i.e. row 2 column 0 is player 2's 0th card. SKEval
        indexing. If a player's two cards are the same, this represents a mucked
        or folded hand.
    @param boards Array of the public board cards. Follows the standard order.
        i.e. columns [0,2] are the flop, column 3 is the turn, and column 4 is
        the river. SKEval indexing. Each row is a different run out.
    @param n_runs How many times the board is being run.
  */
  template <typename T>
  void AwardPot(MultiRun, const uint8_t hands[kPlayers][2],
                const uint8_t boards[][5], const uint8_t n_runs) {
    VerifyAwardablePot();

    bool processed[kPlayers];
    uint8_t players_to_award = PlayersToProcess(hands, processed);
    if (players_to_award == 1) return FoldVictory(processed);

    T exact_awards[kPlayers]{};

    uint16_t ranks_run[kPlayers];
    bool processed_run[kPlayers];
    uint32_t bets_run[kPlayers];
    T side_pot;
    BestPlayersData best_players_run;
    uint8_t players_to_award_run;
    for (uint8_t run = 0; run < n_runs; ++run) {
      std::copy(processed, processed + kPlayers, processed_run);
      RankPlayers(hands, boards[run], processed_run, ranks_run);
      std::copy(bets_, bets_ + kPlayers, bets_run);
      players_to_award_run = players_to_award;
      while (players_to_award_run > 0) {
        side_pot = AllocateSidePot(bets_run, processed_run) * (T{1} / n_runs);
        best_players_run = BestPlayers(ranks_run, processed_run);
        DivideSidePot(side_pot, ranks_run, processed_run, best_players_run,
                      exact_awards);
        players_to_award_run -= MarkProcessedPlayers(bets_run, processed_run);
      }  // while players_to_award_run > 0
    }  // for run
    DistributeChips(&pot_, exact_awards, stack_);
    std::fill(bets_, bets_ + kPlayers, 0);
  }  // AwardPot()

  /*
    Attribute getter functions
  */
  uint32_t big_blind() const { return big_blind_; }
  uint32_t small_blind() const { return small_blind_; }
  uint32_t ante() const { return ante_; }
  bool big_blind_ante() const { return big_blind_ante_; }
  bool blind_before_ante() const { return blind_before_ante_; }

  /*
    Progress getter functions
  */
  uint8_t button() const { return button_; }
  bool in_progress() const { return in_progress_; }
  Round round() const { return round_; }
  uint8_t cycled() const { return cycled_; }
  uint8_t acting_player() const { return acting_player_; }
  // no pot_good_ getter because it is an implementation detail
  // no no_raise_ getter because it is an implementation detail
  bool folded(uint8_t player) const { return folded_[player]; }
  uint8_t players_left() const { return players_left_; }
  uint8_t players_all_in() const { return players_all_in_; }

  /*
    Chip information getter functions
  */
  uint32_t pot() const { return pot_; }
  uint32_t bets(uint8_t player) const { return bets_[player]; }
  uint32_t stack(uint8_t player) const { return stack_[player]; }
  // no min_raise_ getter because it is an implementation detail
  // no max_bet_ getter because it is an implementation detail

 private:
  /*
    @brief Make the given player post the given blind. If the player doesn't
        have enough chips to post the blind, the player goes all in. Does not
        change max_bet_ or min_raise_.
        
    @return The actual size of the blind posted.
  */
  uint32_t PostBlind(uint8_t player, uint32_t size) {
    uint32_t blind = std::min(size, stack_[player]);
    bets_[player] += blind;
    stack_[player] -= blind;
    pot_ += blind;
    if (stack_[player] == 0) players_all_in_ += 1;
    return blind;
  }

  /*
    @brief Have the appropriate players pay the ante. If it is a big blind ante
        and the amount the big blind can put in is not divisibly by the number
        of players, then the change is counted towards the big blind's bet
        total. Does not change max_bet_ or min_raise_.

    @return The effective ante (can be less than the mandated ante if a player
        must go all in to pay it).
  */
  uint32_t PostAntes() {
    uint32_t effective_ante = ante_;
    if (big_blind_ante_) {
      // Determine the effective ante
      uint32_t bb_stack = stack_[PlayerIndex(2)];
      uint32_t effective_ante_sum = std::min(bb_stack, ante_*kPlayers);
      effective_ante = effective_ante_sum / kPlayers;

      /* Mark the big blind's ante in their bet, and charge them for the antes
         of all other players */
      bets_[PlayerIndex(2)] += effective_ante;
      stack_[PlayerIndex(2)] -= effective_ante_sum;
      pot_ += effective_ante_sum;
      if (stack_[PlayerIndex(2)] == 0) players_all_in_ += 1;

      /* Assign any change in the ante amount as counting towards the big
         blind's bet. */
      bets_[PlayerIndex(2)] += effective_ante_sum % kPlayers;

      // Mark the ante in the bet of all other players
      for (uint8_t i = 0; i < kPlayers; ++i) {
        if (i != PlayerIndex(2)) {
          bets_[i] += effective_ante;
        }
      }
    } else {
      for (uint8_t i = 0; i < kPlayers; ++i) {
        PostBlind(i, effective_ante);
      }
    }
    return effective_ante;
  }  // PostAntes()

  /*
    @brief Have the appropriate players straddle. If a player does not have
        enough chips to straddle the appropriate amount, they do not straddle
        and no players behind them can straddle either. Does not change max_bet_
        or min_raise_.
    
    @return The largest amount straddled. If no player straddles, returns the
        big blind. 
  */
  uint32_t PostStraddles(uint8_t n) {
    uint32_t max_straddle_size = big_blind_;
    for (uint8_t i = 0; i < n; ++i) {
      uint8_t player = PlayerIndex(3 + i);
      uint32_t straddle_size = big_blind_ * (1 << (i + 1));
      if (straddle_size < stack_[player]) {
        break;
      } else {
        PostBlind(player, straddle_size);
        max_straddle_size = std::max(straddle_size, max_straddle_size);
      }
    }  // for i
    return max_straddle_size;
  }  // PostStraddles()

  /*
    @brief Fold the current acting player
  */
  void Fold() {
    folded_[acting_player_] = true;
    players_left_ += -1;
  }

  /*
    @brief The current acting player goes all in
  */
  void AllIn() {
    uint32_t prev_bet = bets_[acting_player_];
    uint32_t chips = stack_[acting_player_];
    uint32_t total_bet = prev_bet + chips;

    // call
    if (total_bet < max_bet_) {
      // do nothing

    // call + extra
    } else if (total_bet < max_bet_ + min_raise_) {
      uint8_t already_acted = kPlayers - pot_good_;
      no_raise_ = already_acted;
      max_bet_ = total_bet;

    // raise
    } else {
      uint32_t raise_amount = total_bet - max_bet_;
      min_raise_ = raise_amount;
      max_bet_ = total_bet;
      pot_good_ = kPlayers;
      no_raise_ = 0;
    }

    pot_ += chips;
    stack_[acting_player_] -= chips;
    bets_[acting_player_] += chips;
    players_all_in_ += 1;
  }  // AllIn()

  /*
    @brief The current acting player check/calls
  */
  void CheckCall() {
    uint32_t prev_bet = bets_[acting_player_];
    uint32_t additional_bet = max_bet_ - prev_bet;

    // check
    if (additional_bet == 0) {
      // no nothing

    // call
    } else {
      pot_ += additional_bet;
      stack_[acting_player_] -= additional_bet;
      bets_[acting_player_] += additional_bet;
    }
  }  // CheckCall()

  /*
    @brief The current acting player bets.

    @param size The amount of additional chips the player is contributing to
        the pot.
  */
  void Bet(uint32_t size) {
    uint32_t prev_bet = bets_[acting_player_];
    uint32_t total_bet = size + prev_bet;
    uint32_t raise_size = total_bet - max_bet_;
    max_bet_ = total_bet;
    min_raise_ = raise_size;
    pot_good_ = kPlayers;
    no_raise_ = 0;
    pot_ += size;
    stack_[acting_player_] -= size;
    bets_[acting_player_] += size;
  }  // Bet()

  /*
    @brief Cycle through the players until the next player who needs to act.

    @param cycleOnceBeforeCheck If true, this function will cycle one player
        before checking if it should continue to cycle.
  */
  void CyclePlayers(bool cycleBeforeCheck) {
    do {
      if (!cycleBeforeCheck) {
        cycleBeforeCheck = true;
        continue;
      }
      if (pot_good_ > 0) {
        pot_good_ -= 1;
      } else {
        no_raise_ -= 1;
      }
      cycled_ += 1;
      acting_player_ = (acting_player_ + 1) % kPlayers;
    } while (pot_good_ + no_raise_ > 0 &&
             (folded_[acting_player_] || stack_[acting_player_] == 0));

    // The round has ended
    if (pot_good_ + no_raise_ == 0) {
      NextRound();
    }
  }  // CyclePlayers()

  /*
    @brief Continue to the next betting round
  */
  void NextRound() {
    /* If there is only one player left, the current acting_player_ is the
       singular winner and the game is over. */
    if (players_left_ == 1) {
        in_progress_ = false;
        return;

    /* If all players are all in, then we can skip to the showdown. If all but
       one player is all in, we can also skip to showdown because that player
       can no longer take any actions. */
    } else if (players_left_ - players_all_in_ <= 1) {
      round_ = Round::kRiver;
    }

    switch (round_) {
      case Round::kPreFlop:
        round_ = Round::kFlop;
        break;
      case Round::kFlop:
        round_ = Round::kTurn;
        break;
      case Round::kTurn:
        round_ = Round::kRiver;
        break;
      case Round::kRiver:
        in_progress_ = false;
        return;  /* When the game is over, we dont' need to cycle acting players
                    or change other game state variables. */
    }  // switch round_
    cycled_ = 0;
    acting_player_ = PlayerIndex(1);
    pot_good_ = kPlayers;
    min_raise_ = big_blind_;
    CyclePlayers(false);
  }  // NextRound()

  /*
    @brief Checks if the pot can be awarded. Throws if not.
  */
  void VerifyAwardablePot() const {
    if (in_progress_) {
      throw std::logic_error("VerifyAwardablePot() called when the game is "
                             "still in progress. If the game is still in "
                             "progress, we cannot award the pot yet.");
    } else if (pot_ == 0) {
      throw std::logic_error("VerifyAwardablePot() called when the pot is 0. "
                             "If the pot is 0, this means we already awarded "
                             "the pot for this hand and we can't do it again.");
    }
  }  // VerifyAwardablePot()

  /*
    @brief Finds all the non folded or mucked players and marks them as
        processed. Mark all other players as not processed.

    @param hands 2d array of each player's hand. Rows for each player, columns
        for each card. i.e. row 2 column 0 is player 2's 0th card. SKEval
        indexing. Can be nullptr if all but one player has folded.
    @param processed An array to write if a player has been processed or not to.
    
    @return The number of players that have not been processed.
  */
  uint8_t PlayersToProcess(const uint8_t hands[kPlayers][2],
                           bool processed[kPlayers]) const {
    uint8_t players_to_award = kPlayers;
    for (uint8_t i = 0; i < kPlayers; ++i) {
      if (folded_[i] || (hands && hands[i][0] == hands[i][1])) {
        processed[i] = true;
        players_to_award -= 1;
      } else {
        processed[i] = false;
      }
    }  // for i
    return players_to_award;
  }  // PlayersToProcess()

  /*
    @brief Awards the pot to the singular player that remains (it is assumed
        there is only one player left who hasn't folded or mucked their cards).

    @param processed An array of whether each player is folded or mucked.
  */
  void FoldVictory(const bool processed[kPlayers]) {
    uint8_t winner = std::find(processed, processed + kPlayers, false) -
                     processed;
    stack_[winner] += pot_;
    pot_ = 0;
    std::fill(bets_, bets_ + kPlayers, 0);
  }  // FoldVictory()

  /*
    @brief Writes the hand ranking of each player to a given output array.

    @param hands 2d array of each player's hand. Rows for each player, columns
        for each card. i.e. row 2 column 0 is player 2's 0th card. SKEval
        indexing.
    @param board Array of the public board cards. Follows the standard order.
        i.e. columns [0,2] are the flop, column 3 is the turn, and column 4 is
        the river. SKEval indexing.
    @param filter Players marked as true in this array will not be ranked.
    @param output Array to write hand rankings to.
    
    @return If this function awarded the pot.
  */
  void RankPlayers(const uint8_t hands[kPlayers][2], const uint8_t board[5],
                   const bool filter[kPlayers],
                   uint16_t output[kPlayers]) const {
    for (uint8_t i = 0; i < kPlayers; ++i) {
      if (!filter[i]) {
        output[i] = SevenEval::GetRank(hands[i][0], hands[i][1], board[0],
                                       board[1], board[2], board[3], board[4]);
      }
    }
  }  // RankPlayers()

  /*
    @brief Takes chips from each player's bets to allocate the smallest side pot
        that still needs to be processed.

    @param bets An array of how much each player's bet still hasn't been
        awarded.
    @param processed An array of if each player has been processed or not. We
        don't form side pots from the bets of processed players.
    
    @return The size of the side pot formed.
  */
  uint32_t AllocateSidePot(uint32_t bets[kPlayers],
                           const bool processed[kPlayers]) const {
    // Find the smallest non zero bet from a non processed player
    uint32_t min_bet = std::numeric_limits<uint32_t>::max();
    for (uint8_t i = 0; i < kPlayers; ++i) {
      if (bets[i] < min_bet && !processed[i]) min_bet = bets[i];
    }

    /* Consider the side pot formed by the smallest bet. For instance, if there
       are 3 players remaining, with player 1 all in for 100 chips, player 2 all
       in for 50 chips, and player 3 all in for 25 chips, the smallest bet is 25
       chips. So, this forms a 75 chip (25 chips * 3 players) side pot out of
       the total 175 chip pot. */
    uint32_t side_pot = 0;
    for (uint8_t i = 0; i < kPlayers; ++i) {
      if (bets[i] >= min_bet) {
        side_pot += min_bet;
        bets[i] -= min_bet;
      } else if (bets[i] < min_bet) {
        side_pot += bets[i];
        bets[i] -= bets[i];
      }
    }
    return side_pot;
  }  // AllocateSidePot()

  struct BestPlayersData { uint16_t rank; uint8_t n; };
  /*
    @brief Find the player(s) with the best hand(s). If there is a tie, count
        how many players are tied.

    @param ranks Hand rankings for each player.
    @param filter Players marked as true in this array will not be considered.
    
    @return Struct containing the best rank (rank) and the number of players
        with that rank (n).
  */
  BestPlayersData BestPlayers(const uint16_t ranks[kPlayers],
                              const bool filter[kPlayers]) const {
    std::optional<uint16_t> best_rank;
    uint8_t best_players = 0;
    for (uint8_t i = 0; i < kPlayers; ++i) {
      if (!filter[i]) {
        if (!best_rank || ranks[i] > *best_rank) {
          best_rank = ranks[i];
          best_players = 1;
        } else if (ranks[i] == *best_rank) {
          best_players += 1;
        }
      }
    }
    return BestPlayersData{*best_rank, best_players};
  }  // BestPlayers()

  /*
    @brief Divides the given side_pot equally among the winners.

    @param side_pot The side pot to divide.
    @param ranks Hand rankings for each player.
    @param filter Players marked as true in this array will not be considered.
    @param best_players Specifies the rank and number of players to award.
    @param exact_awards Array to add awards for each player to.
  */
  template <typename T>
  void DivideSidePot(T side_pot, const uint16_t ranks[kPlayers],
                     const bool filter[kPlayers],
                     const BestPlayersData best_players,
                     T exact_awards[kPlayers]) const {
    T prize = side_pot / best_players.n;
    for (uint8_t i = 0; i < kPlayers; ++i) {
      if (!filter[i] && ranks[i] == best_players.rank) {
        exact_awards[i] += prize;
      }
    }
  }  // DivideSidePot()

  /*
    @brief If the entirety of a player's bet has been allocated, they are no
       longer eligible to win any more chips because a player can only win as
       many chips as they put in from each other player. Thus, mark them as
       processed.

    @param bets An array of how much each player's bet still hasn't been
        awarded.
    @param processed An array of if each player has been processed or not.

    @return The number of players marked as processed.
  */
  uint8_t MarkProcessedPlayers(const uint32_t bets[kPlayers],
                               bool processed[kPlayers]) const {
    uint8_t players_processed = 0;
    for (uint8_t i = 0; i < kPlayers; ++i) {
      if (bets[i] == 0 && !processed[i]) {
        players_processed += 1;
        processed[i] = true;
      }
    }
    return players_processed;
  }  // MarkProcessedPlayers()

  /*
    @brief Distribute the given pot to the players, minimizing roundoff error.

    Takes an array of exact awards for each player, and maps it as best as
    possible to discrete chip values while preserving the total amount of chips
    and minimizing roundoff error.

    @param pot The pot to award. It will be left at zero after this function is
        called.
    @param exact_awards What exact amount (represented by a floating point or
        fraction type) the player has won.
    @param distributions Where to distribute each player's actual discrete
        winnings.
  */
  template <typename T>
  void DistributeChips(uint32_t* pot, const T exact_awards[kPlayers],
                       uint32_t distributions[kPlayers]) const {
    /* Calculate the decimal part of each player's award (the decimal part of
       10.82 is 0.82). */
    T decimals[kPlayers];
    for (uint8_t i = 0; i < kPlayers; ++i) {
      decimals[i] = exact_awards[i] - static_cast<uint32_t>(exact_awards[i]);
    }

    /* Sort the player indexes in order of who will get awarded rounding change
       first. Player's with a higher decimal part of their award are "first in
       line" to get change. */
    uint8_t change_line[kPlayers];
    std::iota(change_line, change_line + kPlayers, 0);
    std::sort(change_line, change_line + kPlayers,
              [&](const uint8_t& a, const uint8_t& b) {
                return decimals[a] > decimals[b];
              });

    // Award each player the floor of their exact award.
    uint32_t pot_awarded = 0;
    for (uint8_t i = 0; i < kPlayers; ++i) {
      if (exact_awards[i] > 0) {
        uint32_t to_award = static_cast<uint32_t>(exact_awards[i]);
        distributions[i] += to_award;
        pot_awarded += to_award;
      }
    }

    /* Award change in order of change_line until all the change has been
       awarded. */
    uint8_t position = 0;
    while (*pot > pot_awarded) {
      uint8_t player = change_line[position];
      distributions[player] += 1;
      pot_awarded += 1;
      ++position;
    }
    if (pot_awarded > *pot) {
      throw std::runtime_error("Rounding errors resulted in more chips being "
                               "awarded to players than were in the pot.");
    }
    *pot -= pot_awarded;
  }  // DistributePot()

  // Attributes
  const uint32_t big_blind_;         // how many chips the big blind is
  const uint32_t small_blind_;       // how many chips the small blind is
  const uint32_t ante_;              /* how many chips each player must
                                        contribute to the pot on each hand */
  const bool big_blind_ante_;        /* true if the big blind pays the ante for
                                        everyone in the game */
  const bool blind_before_ante_;     /* true if the player should cover the
                                        blind instead of the ante if they don't
                                        have enough chips to pay both */

  // Progress information
  uint8_t button_;                   // index of player on the button
  bool in_progress_;                 /* true if a hand is in progress, false if
                                        the hand ended */
  Round round_;                      // the current betting round
  uint8_t cycled_;                   /* how many players have been cycled
                                        through on this betting round */
  uint8_t acting_player_;            // index of player whose turn it is
  uint8_t pot_good_;                 /* the number of players who still need to
                                        act before this round is over */
  uint8_t no_raise_;                 /* the number of players who still need to
                                        act, but can only call or fold because
                                        another player went all in less than the
                                        min-raise */
  bool folded_[kPlayers];            /* true for each player that is folded,
                                        false for each player still in the
                                        game */
  uint8_t players_left_;             // the number of players who haven’t folded
  uint8_t players_all_in_;           // the number of players who are all in

  // Chip information
  uint32_t pot_;                     // how many chips are in the pot
  uint32_t bets_[kPlayers];          // how much each player has bet
  uint32_t stack_[kPlayers];         // how many chips each player has
  uint32_t min_raise_;               // the minimum bet amount
  uint32_t max_bet_;                 /* the maximum amount that has been bet so
                                        far */
};  // Node

}  // namespace poker_engine

#endif  // SRC_POKER_ENGINE_NODE_H_
