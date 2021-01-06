// copyright Emily Dale 2021
#ifndef SRC_POKER_ENGINE_GAME_H_
#define SRC_POKER_ENGINE_GAME_H_

#include "hand_evaluator/Constants.h"
#include "Agent.h"
#include "GameState.h"

const int kDeckSize = 52;

namespace game_engine {

class Game {
 private:
  int deck_[kDeckSize];
  Agent** agents_;
  GameState game_state_;
  char deck_index_;  // index of next card to be dealt
  void ShuffleDeck();
  void AwardPot();
  void Preflop();
  void DealCard(char num_cards);

 public:
  Game(char num_players, char num_rounds, double small_blind_multiplier,
        double starting_bb_amounts, char small_blind_pos);
  void Play();
};  // class Game

}  // namespace game_engine

#endif  // SRC_POKER_ENGINE_GAME_H_
