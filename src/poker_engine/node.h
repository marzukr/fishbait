// Copyright 2021 Marzuk Rashid

#ifndef SRC_POKER_ENGINE_NODE_H_
#define SRC_POKER_ENGINE_NODE_H_

#include <algorithm>
#include <cstdint>
#include <optional>
#include <random>

#include "SKPokerEval/src/SevenEval.h"
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
    @brief Construct a node with arbitrary card information.

    @param button The index of the player on the button. Defaults to 0.
    @param big_blind How many chips the big blind is. Defaults to 100.
    @param small_blind How many chips the small blind is. Defaults to 50.
    @param default_stack How many chips each player starts with. Defaults to
        10,000.
  */
  Node(uint8_t button = 0, uint32_t big_blind = 100, uint32_t small_blind = 50,
       uint32_t default_stack = 10000)
       : acting_player_{(button + 3) % kPlayers}, button_{button},
       big_blind_{big_blind}, small_blind_{small_blind},
       pot_{big_blind + small_blind}, bets_{0}, stack_{default_stack},
       min_raise_{big_blind}, max_bet_{big_blind} {
    bets_[PlayerIndex(1)] = small_blind;
    bets_[PlayerIndex(2)] = big_blind;
    stack_[PlayerIndex(1)] -= small_blind;
    stack_[PlayerIndex(2)] -= big_blind;

    for (uint8_t i = 0; i < kPlayers; ++i) {
      hands_[i][0] = 2*i;
      hands_[i][1] = 2*i + 1;
    }
    uint8_t first_deck_card = 2*kPlayers;
    for (uint8_t i = 0; i < 5; ++i) {
      board_[i] = first_deck_card + i;
    }
  }  // Node()

  bool InProgress() const { return in_progress_; }

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
    @brief Apply the given move to this Node object.

    @param next The Move to apply to this Node. If the move is invalid, the
        acting_player_ is folded.

    @return True if the game is still in progress after this move, false if this
        move has ended the game.
  */
  bool Apply(Move next) {
    if (!in_progress_) return in_progress_;

    // Process the move
    uint32_t prev_bet = bets_[acting_player_];
    uint32_t chips = stack_[acting_player_];
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
          Fold();
        }
        break;
      case Action::kBet:
        if (CanBet(next.size)) {
          Bet(next.size);
        } else {
          Fold();
        }
        break;
    }  // switch next.play

    CyclePlayers(true);

    // The round has ended
    if (pot_good_ + no_raise_ == 0) {
      NextRound();
    }

    return in_progress_;
  }  // Apply()

 private:
  /*
    @brief Get the index of a player relative to the button where default
        position is when the button is at index 0.

    @param default_position The default position of the player to get the index
        of. Button is 0, small blind 1, big blind 2, etc.
  */
  uint8_t PlayerIndex(uint8_t default_position) const {
    return (button_ + default_position) % kPlayers;
  }

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
  }  // CyclePlayers()

  /*
    @brief Continue to the next betting round
  */
  void NextRound() {
    /* If there is only one player left, the current acting_player_ is the
       singular winner and the game is over. */
    if (players_left_ == 1) {
        AwardPot();
        in_progress_ = false;
        return;
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
        AwardPot();
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
    @brief Allocate the pot to the winner(s) at the end of a hand. After
        calling, pot will be 0, all bets will be 0, and the winner(s)'s stack
        will have increased by the appropriate amount.
  */
  void AwardPot() {
    /* If everyone else has folded, the current acting_player_ is the singular
       winner. */
    if (players_left_ == 1) {
      stack_[acting_player_] += pot_;
      pot_ = 0;
      std::fill(&bets_, &bets_ + kPlayers, 0);
      return;
    }

    // Otherwise we must conduct a showdown:
    // First, get the hand rank of each player still in the game.
    uint16_t ranks[kPlayers];
    for (uint8_t i = 0; i < kPlayers; ++i) {
      if (!folded_[i]) {
        ranks[i] = SevenEval::GetRank(hands_[i][0], hands_[i][1], board_[0],
                                      board_[1], board_[2], board_[3],
                                      board_[4]);
      }
    }

    // Variables for the while loop
    utils::Random rng;
    std::uniform_int_distribution<uint8_t> random_player(0, kPlayers - 1);
    bool processed[kPlayers];
    std::copy(&folded_, &folded_ + kPlayers, &processed);
    uint8_t players_to_award = players_left_;

    // Loop through each side pot and award it to the appropriate player(s)
    while (players_to_award > 0) {
      // Find the smallest non zero bet
      uint32_t min_bet = pot_;
      for (uint8_t i = 0; i < kPlayers; ++i) {
        if (bets_[i] < min_bet && bets_[i] > 0) min_bet = bets_[i];
      }

      /* Consider the side pot formed by the smallest bet. For instance, if
         there are 3 players remaining, with player 1 all in for 100 chips,
         player 2 all in for 50 chips, and player 3 all in for 25 chips, the
         smallest bet is 25 chips. So, this forms a 75 chip (25 chips * 3
         players) side pot out of the total 175 chip pot. */
      uint32_t side_pot = 0;
      for (uint8_t i = 0; i < kPlayers; ++i) {
        if (bets_[i] >= min_bet) {
          side_pot += min_bet;
          bets_[i] -= min_bet;
        }
      }
      pot_ -= side_pot;

      /* Find the player(s) who still need to be processed with the best
         hand(s). If there is a tie, count how many players are tied. */
      std::optional<uint16_t> best_rank;
      uint8_t best_players = 0;
      for (uint8_t i = 0; i < kPlayers; ++i) {
        if (!processed[i]) {
          if (!best_rank || ranks[i] > *best_rank) {
            best_rank = ranks[i];
            best_players = 1;
          } else if (ranks[i] == *best_rank) {
            best_players += 1;
          }
        }
      }


      // Award the side_pot equally among the winners
      for (uint8_t i = 0; i < kPlayers; ++i) {
        if (!processed[i] && ranks[i] == *best_rank) {
          uint32_t award = side_pot / best_players;
          stack_[i] += award;
          side_pot -= award;
        }
      }

      /* If the side pot is not perfectly divisible among the eligible winners,
         award the remaining change at random. */
      while (side_pot > 0) {
        uint8_t chosen = random_player(rng());
        if (!processed[chosen] && ranks[chosen] == *best_rank) {
          stack_[chosen] += 1;
          side_pot -= 1;
        }
      }

      /* If this side pot allocated the remainder of a player's bet, they are no
         longer eligible to win any more chips, so mark them as processed. In
         the above example, if all three players tied, player 1 would win 1/3rd
         of the 75 chip side pot, and would then be ineligible to win any of the
         remaining 100 chips in the pot because they only contributed 25 chips
         to the pot. */
      for (uint8_t i = 0; i < kPlayers; ++i) {
        if (bets_[i] == 0 && !processed[i]) {
          players_to_award += -1;
          processed[i] = true;
        }
      }
    }  // while players_to_award > 0
  }  // AwardPot()

  // Attributes
  const uint8_t button_;             // index of player on the button
  const uint32_t big_blind_;         // how many chips the big blind is
  const uint32_t small_blind_;       // how many chips the small blind is

  // Progress information
  bool in_progress_{true};           /* true if a hand is in progress, false if
                                        the hand ended */
  Round round_{Round::kPreFlop};     // the current betting round
  uint8_t cycled_{0};                /* how many players have been cycled
                                        through on this betting round */
  uint8_t acting_player_;            // index of player whose turn it is
  uint8_t pot_good_{kPlayers};       /* the number of players who still need to
                                        act before this round is over */
  uint8_t no_raise_{0};              /* the number of players who still need to
                                        act, but can only call or fold because
                                        another player went all in less than the
                                        min-raise */
  bool folded_[kPlayers] = {false};  /* true for each player that is folded,
                                        false for each player still in the
                                        game */
  uint8_t players_left_{kPlayers};   // the number of players who haven’t folded

  // Chip information
  uint32_t pot_;                     // how many chips are in the pot
  uint32_t bets_[kPlayers];          // how much each player has bet
  uint32_t stack_[kPlayers];         // how many chips each player has
  uint32_t min_raise_;               // the minimum bet amount
  uint32_t max_bet_;                 /* the maximum amount that has been bet so
                                        far */

  // Card Information
  uint8_t hands_[kPlayers][2];       // each player's hand
  uint8_t board_[5];                 // the public board cards
};  // Node

}  // namespace poker_engine

#endif  // SRC_POKER_ENGINE_NODE_H_
