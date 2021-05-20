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
    @brief Construct a node with arbitrary card information. By default,
        constructs a standard 50/100 game with each player starting with 100 big
        blinds, with arbitrary card information.

    @param button The index of the player on the button.
    @param big_blind How many chips the big blind is.
    @param small_blind How many chips the small blind is.
    @param ante How many chips each player pays as an ante each hand.
    @param big_blind_ante If the big blind pays the ante for all players
    @param default_stack How many chips each player starts with.
    @param hands 2d array of each player's hand. Rows for each player, columns
        for each card. i.e. row 2 column 0 is player 2's 0th card. If nullptr is
        passed then each player get's an arbitrary hand.
    @param board Array of the public board cards. Follows the standard order.
        i.e. indexes [0,2] are the flop, index 3 is the turn, and index 4 is the
        river. If nullptr is passed then the board is filled with arbitrary
        cards.
  */
  Node(uint8_t button = 0, uint32_t big_blind = 100, uint32_t small_blind = 50,
       uint32_t ante = 0, bool big_blind_ante = false,
       bool blind_before_ante = true, uint32_t default_stack = 10000,
       uint8_t hands[kPlayers][2] = nullptr, uint8_t board[5] = nullptr)

         // Attributes
       : big_blind_{big_blind}, small_blind_{small_blind}, ante_{ante},
         big_blind_ante_{big_blind_ante}, blind_before_ante_{blind_before_ante},

         // Progress Information
         button_{button}, in_progress_{true}, round_{Round::kPreFlop},
         cycled_{0}, acting_player_{(button + 3) % kPlayers},
         pot_good_{kPlayers}, no_raise_{0}, folded_{false},
         players_left_{kPlayers},

         // Chip Information
         pot_{0}, bets_{0}, stack_{default_stack}, min_raise_{big_blind},
         max_bet_{big_blind}, effective_ante_{ante},
         
         // Card Information
         hands_{0}, board_{0} {
    if (blind_before_ante_) PostAnte();
    PostBlind(PlayerIndex(1), small_blind_);
    PostBlind(PlayerIndex(2), big_blind_);
    if (!blind_before_ante_) PostAnte();

    max_bet_ = big_blind_ + effective_ante_;

    if (hands != nullptr) {
      std::copy(&hands, &hands + 2*kPlayers, hands_);
    } else {
      for (uint8_t i = 0; i < kPlayers; ++i) {
        hands_[i][0] = 2*i;
        hands_[i][1] = 2*i + 1;
      }
    }

    if (board != nullptr) {
      std::copy(&board, &board + 5, board_);
    } else {
      uint8_t first_deck_card = 2*kPlayers;
      for (uint8_t i = 0; i < 5; ++i) {
        board_[i] = first_deck_card + i;
      }
    }

    CyclePlayers(false);
  }  // Node()

  /*
    @brief Get the index of a player relative to the button where default
        position is when the button is at index 0.

    @param default_position The default position of the player to get the index
        of. Button is 0, small blind 1, big blind 2, etc.
  */
  uint8_t PlayerIndex(uint8_t default_position) const {
    return (button_ + default_position) % kPlayers;
  }

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
    return in_progress_;
  }  // Apply()

 private:
  /*
    @brief Make the given player post the given blind. If the player doesn't
        have enough chips to post the blind, the player goes all in.
        
    @returns The actual size of the blind posted.
  */
  uint32_t PostBlind(uint8_t player, uint32_t size) {
    uint32_t blind = std::min(size, stack_[player]);
    bets_[player] += blind;
    stack_[player] -= blind;
    pot_ += blind;
    return blind;
  }

  /*
    @brief Have the appropriate players pay the ante. If it is a big blind ante
        and the amount the big blind can put in is not divisibly by the number
        of players, then the change is counted towards the big blind's bet
        total.
  */
  void PostAnte() {
    if (big_blind_ante_) {
      // Determine the effective_ante_
      uint32_t bb_stack = stack_[PlayerIndex(2)];
      uint32_t effective_ante_sum = std::min(bb_stack, ante_*kPlayers);
      effective_ante_ = effective_ante_sum / kPlayers;

      /* Mark the big blind's ante in their bet, and charge them for the antes
         of all other players */
      bets_[PlayerIndex(2)] += effective_ante_;
      stack_[PlayerIndex(2)] -= effective_ante_sum;
      pot_ += effective_ante_sum;

      /* Assign any change in the ante amount as counting towards the big
         blind's bet. */
      bets_[PlayerIndex(2)] += effective_ante_sum % kPlayers;

      // Mark the ante in the bet of all other players
      for (uint8_t i = 0; i < kPlayers; ++i) {
        if (i != PlayerIndex(2)) {
          bets_[i] += effective_ante_;
        }
      }
    } else {
      effective_ante_ = ante_;
      for (uint8_t i = 0; i < kPlayers; ++i) {
        uint32_t affordable_ante = std::min(stack_[i], effective_ante_);
        bets_[i] += affordable_ante;
        stack_[i] -= affordable_ante;
        pot_ += affordable_ante;
      }
    }
  }  // PostAnte()

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

  // Chip information
  uint32_t pot_;                     // how many chips are in the pot
  uint32_t bets_[kPlayers];          // how much each player has bet
  uint32_t stack_[kPlayers];         // how many chips each player has
  uint32_t min_raise_;               // the minimum bet amount
  uint32_t max_bet_;                 /* the maximum amount that has been bet so
                                        far */
  uint32_t effective_ante_;          /* usually the same as ante_, except if
                                        it is a big blind ante and the big blind
                                        cannot post the full ante */

  // Card Information
  uint8_t hands_[kPlayers][2];       // each player's hand, SKEval indexing
  uint8_t board_[5];                 // the public board cards, SKEval indexing
};  // Node

}  // namespace poker_engine

#endif  // SRC_POKER_ENGINE_NODE_H_
