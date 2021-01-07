// copyright Emily Dale 2021

#ifndef SRC_POKER_ENGINE_GAMESTATE_H_
#define SRC_POKER_ENGINE_GAMESTATE_H_

#include <stdint.h>
#include <iostream>

namespace poker_engine {

class GameState {
 public:
  GameState(char num_players, char num_rounds, long small_blind, long big_blind,
            long starting_amounts, char small_blind_pos);
  GameState(const GameState&);
  GameState& operator=(const GameState&);
  friend std::ostream& operator<<(std::ostream&, const GameState&);
  ~GameState();
  void TakeAction(long action);
  void PrintAction(double action);
  void UpdateNeedsCard();
  void UndoAction(char acted_player, long action, long old_max_bet,
                  long old_min_raise, long old_pot_good, char old_round,
                  char old_betting_round, char old_all_in, long old_pot,
                  char old_num_left, bool old_is_done);
  char small_blind_pos_;  // index of person who has small blind
  long* chip_amounts_;  // each player's remaining chips
  bool* in_game_;  // false if player has folded, true otherwise
  long* total_bets_;  // amount of chips this player has in pot for this hand
  char acting_player_;  // current acting player
  char current_round_;  // 0 = preflop, 1 = flop, 2 = turn, 3 = river
  long min_raise_;  // min raise for the hand
  long max_bet_;  // max amount that anyone has put in the pot this hand
  long pot_;  // amount in pot
  bool is_done_;  // false if game is over, true otherwise
  char betting_round_;  // number of times betting has gone around this round
  char num_players_;  // total number of players
  char num_rounds_;  // total rounds for the game
  long starting_amounts_;  // starting chip amounts in terms of big blind
  char num_left_;  // num of players not folded (all-in players are not folded)
  char num_all_in_;  // number of players that are all in
  bool needs_card_;  // game needs to have a card dealt
  char pot_good_;  // number of players that still need to check/bet for this
                   // round. Folded and all in players are automatically good.
                   // the round increments when pot_good_ = 0
  long small_blind_;
  long big_blind_;
};  // GameState

}  // namespace poker_engine

#endif  // SRC_POKER_ENGINE_GAMESTATE_H_
