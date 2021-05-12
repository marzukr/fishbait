// Copyright Emily Dale 2021

#include <iostream>
#include <random>
#include <string>
#include <vector>

#include "SKPokerEval/src/SevenEval.h"
#include "SKPokerEval/src/Deckcards.h"
#include "SKPokerEval/src/Constants.h"

#include "poker_engine/game.h"

namespace poker_engine {

Game::Game(char num_players, char num_rounds, int32_t small_blind,
           int32_t big_blind, int32_t starting_amounts, char small_blind_pos) :
           game_state_(num_players, num_rounds, small_blind, big_blind,
           starting_amounts, small_blind_pos) {
  // initialize the deck
  for (int i = 0; i < kDeckSize; ++i) {
    deck_[i] = i;
  }
  deck_index_ = 0;
  std::string default_names[6] = {"Trump", "McConnell", "Pelosi", "Schumer",
    "McCarthy", "Roberts"};

  agents_ = new Agent*[num_players];

  // initialize the players
  for (int i = 0; i < num_players; ++i) {
    Agent* a = new Agent(default_names[i]);
    agents_[i] = a;
  }
}

void Game::Play() {
  Preflop();

  bool flop_done = false;
  bool turn_done = false;
  bool river_done = false;
  // else if on same line
  while (game_state_.is_done_ == false) {
    std::cout << "There is " << game_state_.pot_ << " in the pot" << std::endl;
    if (game_state_.current_round_ == 1 && flop_done == false) {
      DealCard(3);
      std::cout << "------------- Flop --------------" << std::endl;
      flop_done = true;
    } else if (game_state_.current_round_ == 2 && turn_done == false) {
      DealCard(1);
      std::cout << "------------- Turn --------------" << std::endl;
      turn_done = true;
    } else if (game_state_.current_round_ == 3 && river_done == false) {
      DealCard(1);
      std::cout << "------------- River --------------" << std::endl;
      river_done = true;
    }
    // std::cout << "acting player " << game_state.acting_player << std::endl;
    double action = -2;
    if (game_state_.in_game_[game_state_.acting_player_] &&
       game_state_.chip_amounts_[game_state_.acting_player_] != 0) {
       action = agents_[game_state_.acting_player_]->
        action(game_state_.max_bet_, game_state_.min_raise_,
               game_state_.total_bets_[game_state_.acting_player_],
               game_state_.chip_amounts_[game_state_.acting_player_]);
    }
    game_state_.TakeAction(action);
    std::cout << " max bet: " << game_state_.max_bet_ << std::endl;
  }
  AwardPot();
}  // Play

void Game::ShuffleDeck() {
  std::random_device dev;
  std::mt19937 rng(dev());

  // knuth shuffle
  for (int i = DECK_SIZE - 1; i >= 1; i--) {
    std::uniform_int_distribution<std::mt19937::result_type> dist6(0, i);
    int j = dist6(rng);
    int temp = deck_[i];
    deck_[i] = deck_[j];
    deck_[j] = temp;
  }
}

void Game::Preflop() {
  std::cout << "------------- Preflop --------------" << std::endl;
  // shuffle and deal
  ShuffleDeck();
  for (int i = 0; i < game_state_.num_players_; ++i) {
    agents_[i]->deal_cards(deck_[2*i], deck_[2*i + 1]);
    deck_index_ += 2;
  }
}

void Game::DealCard(char num_cards) {
  for (int i = 0; i < num_cards; ++i) {
    std::cout << "card: " << pretty_card[deck_[deck_index_]] << std::endl;
    ++deck_index_;
  }
}

void Game::AwardPot() {
  char num_players = game_state_.num_players_;
  char players_left = game_state_.num_left_;
  std::vector<double> awards(num_players);
  std::vector<int> ranks(num_players);

  // if only one player left don't evaluate hands
  if (players_left <= 1) {
    for (int i = 0; i < num_players; ++i) {
      if (game_state_.in_game_[i] == true) {
        awards[i] = game_state_.pot_;
        std::cout << *agents_[i];
        std::cout << " wins " << awards[i] << std::endl;
      } else {
        awards[i] = 0;
      }
    }
    return;
  }

  char card_start = 2 * num_players;
  for (int i = 0; i < num_players; ++i) {
      ranks[i] = SevenEval::GetRank(
          agents_[i]->get_c1(), agents_[i]->get_c2(),
          deck_[card_start], deck_[card_start+1], deck_[card_start+2],
          deck_[card_start+3],
          deck_[card_start+4]);
      awards[i] = 0;
  }

  while (players_left > 0) {
      double min_bet = game_state_.pot_;
      for (int i = 1; i < num_players; ++i) {
          double bet_i = game_state_.total_bets_[i];
          if (bet_i < min_bet && bet_i > 0) min_bet = bet_i;
      }

      double side_pot = 0;

      int best_hand = -1;
      int best_players = 0;

      for (int i = 0; i < num_players; ++i) {
          if (game_state_.total_bets_[i] >= min_bet) {
              side_pot += min_bet;
              game_state_.total_bets_[i] = game_state_.total_bets_[i] - min_bet;
          }
          if (game_state_.in_game_[i]) {
              if (ranks[i] > best_hand) {
                  best_hand = ranks[i];
                  best_players = 1;
              } else if (ranks[i] == best_hand) {
                  best_players += 1;
              }
          }
      }

      for (int i = 0; i < num_players; ++i) {
          if (game_state_.in_game_[i] && ranks[i] == best_hand) {
              awards[i] += side_pot / best_players;
          }
          if (game_state_.total_bets_[i] == 0) {
              if (game_state_.in_game_[i]) players_left += -1;
              game_state_.in_game_[i] = false;
          }
      }
  }

  for (int i = 0; i < num_players; ++i) {
      if (awards[i] > 0) {
          std::cout << *agents_[i];
          std::cout << " wins " << awards[i] << std::endl;
      }
  }
}  // AwardPot

}  // namespace poker_engine
