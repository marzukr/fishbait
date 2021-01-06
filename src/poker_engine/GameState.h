// copyright Emily Dale 2021

#ifndef SRC_POKER_ENGINE_GAMESTATE_H_
#define SRC_POKER_ENGINE_GAMESTATE_H_

#include <stdint.h>
#include <iostream>

namespace poker_engine {

class GameState {
 public:
  GameState(char num_players, char num_rounds, double small_blind_multiplier,
            double starting_bb_amounds, char small_blind_pos);
  GameState(const GameState&);
  GameState& operator=(const GameState&);
  friend std::ostream& operator<<(std::ostream&, const GameState&);
  ~GameState();
  void TakeAction(double action);
  void PrintAction(double action);
  void UpdateNeedsCard();
  void UndoAction(char acted_player, double action, double old_max_bet,
                  double old_min_raise, double old_pot_good, char old_round,
                  char old_betting_round, char old_all_in, double old_pot,
                  char old_num_left, bool old_is_done);
  char small_blind_pos_;  // index of person who has small blind
  double* chip_amounts_;  // each player's remaining chips
  bool* in_game_;  // false if player has folded, true otherwise
  double* total_bets_;  // amount of chips this player has in pot for this hand
  char acting_player_;  // current acting player
  char current_round_;  // 0 = preflop, 1 = flop, 2 = turn, 3 = river
  double min_raise_;  // min raise for the hand
  double max_bet_;  // max amount that anyone has put in the pot this hand
  double pot_;  // amount in pot
  bool is_done_;  // false if game is over, true otherwise
  char betting_round_;  // number of times betting has gone around this round
  char num_players_;  // total number of players
  char num_rounds_;  // total rounds for the game
  float small_blind_multiplier_;  // small blind amount relative to big blind
  double starting_bb_amounts_;  // starting chip amounts in terms of big blind
  char num_left_;  // num of players not folded (all-in players are not folded)
  char num_all_in_;  // number of players that are all in
  bool needs_card_;  // game needs to have a card dealt
  char pot_good_;  // number of players that still need to check/bet for this
                   // round. Folded and all in players are automatically good.
                   // the round increments when pot_good_ = 0
};  // GameState

}  // namespace poker_engine

#endif  // SRC_POKER_ENGINE_GAMESTATE_H_
