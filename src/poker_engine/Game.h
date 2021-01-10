// copyright Emily Dale 2021

#ifndef SRC_POKER_ENGINE_GAME_H_
#define SRC_POKER_ENGINE_GAME_H_

#include "SKPokerEval/src/Constants.h"

#include "poker_engine/Agent.h"
#include "poker_engine/GameState.h"

const int kDeckSize = 52;

namespace poker_engine {

class Game {
 public:
  Game(char num_players, char num_rounds, int32_t small_blind,
       int32_t big_blind, int32_t starting_amounts, char small_blind_pos);
  void Play();
 private:
  int deck_[kDeckSize];
  Agent** agents_;
  GameState game_state_;
  char deck_index_;  // index of next card to be dealt
  void ShuffleDeck();
  void AwardPot();
  void Preflop();
  void DealCard(char num_cards);
};  // class Game

}  // namespace poker_engine

#endif  // SRC_POKER_ENGINE_GAME_H_
