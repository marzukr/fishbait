#include <iostream>
#include <random>
#include <string>

#include "hand_evaluator/SevenEval.h"
#include "hand_evaluator/Deckcards.h"
#include "hand_evaluator/Constants.h"

#include "Game.h"

Game::Game(char num_players, char num_rounds, float small_blind_multiplier, 
           double starting_bb_amounts) : game_state_(num_players, num_rounds,
                                                     small_blind_multiplier, 
                                                     starting_bb_amounts) {
  // initialize the deck
  for (int i = 0; i < DECK_SIZE; i++) {
    deck_[i] = i;
  }

  std::string default_names[6] = {"Trump", "McConnell", "Pelosi", "Schumer", 
    "McCarthy", "Roberts"};

  agents_ = new Agent*[num_players];

  // initialize the players
  for (int i = 0; i < num_players; i++) {
    Agent* a = new Agent(starting_bb_amounts, default_names[i]);
    agents_[i] = a;
  }
}

void Game::Play(int button_pos) {
  Preflop(button_pos);

  bool flop_done = false;
  bool turn_done = false;
  bool river_done = false;
  while(game_state_.is_done_ == false){
    std::cout << "There is " << game_state_.pot_ << " in the pot" << std::endl;
    if(game_state_.current_round_ == 1 && flop_done == false){
      Flop();
      flop_done = true;
    }
    else if(game_state_.current_round_ == 2 && turn_done == false){
      Turn();
      turn_done = true;
    }
    else if(game_state_.current_round_ == 3 && river_done == false){
      River();
      river_done = true;
    }
    // std::cout << "acting player " << game_state.acting_player << std::endl;
    char agent_index = game_state_.acting_player_ + button_pos % 
                       game_state_.num_players_;
    double action = agents_[agent_index]->action(game_state_.max_bet_, 
                                                  game_state_.min_raise_);
    double chips = agents_[agent_index]->get_chips();
    game_state_.TakeAction(action);
    std::cout << " max bet: " << game_state_.max_bet_ << std::endl;

  }

  AwardPot();

}

void Game::ShuffleDeck() {
  std::random_device dev;
  std::mt19937 rng(dev());

  // knuth shuffle
  for (int i = DECK_SIZE - 1; i >= 1; i--) {
    std::uniform_int_distribution<std::mt19937::result_type> dist6(0,i);
    int j = dist6(rng);
    int temp = deck_[i];
    deck_[i] = deck_[j];
    deck_[j] = temp;
  }
}

void Game::Preflop(int button_pos) {
  std::cout << "------------- Preflop --------------" << std::endl;

  // shuffle and deal
  ShuffleDeck();
  for (int i = 0; i < game_state_.num_players_; i++) {
    agents_[i]->deal_cards(deck_[2*i], deck_[2*i + 1]);
  }

  // assign positions and blinds
  agents_[button_pos]->assign_blind(0.5); // small blind
  agents_[button_pos+1]->assign_blind(1); // big blind

}

void Game::Flop() {
  std::cout << "--------------- Flop ---------------" << std::endl;
  std::cout << "The flop is "
    << pretty_card[deck_[FLOP_1]] << ", "
    << pretty_card[deck_[FLOP_2]] << ", "
    << pretty_card[deck_[FLOP_3]] << std::endl;

  // main game loop start with SB
}

void Game::Turn() {
  std::cout << "--------------- Turn ---------------" << std::endl;
  std::cout << "The turn is " << pretty_card[deck_[TURN]] << std::endl;
}

void Game::River() {
  std::cout << "-------------- River ---------------" << std::endl;
  std::cout << "The river is " << pretty_card[deck_[RIVER]] << std::endl;
}

void Game::AwardPot() {
  double bets [game_state_.num_players_];
  bool in_game [game_state_.num_players_];
  double awards[game_state_.num_players_];
  char players_left = 0;
  // create bets, in_game, and awards arrays that are adjusted for the small blind
  // position
  for(int i = 0; i < game_state_.num_players_; i++){
    bets[(i+small_blind_pos_)% game_state_.num_players_] = 
                                                    game_state_.total_bets_[i];
    in_game[(i+small_blind_pos_)% game_state_.num_players_] = 
                                                    game_state_.in_game_[i];
    if (in_game[(i+small_blind_pos_)% game_state_.num_players_]) {
      players_left++;
    }
    awards[(i+small_blind_pos_)% game_state_.num_players_] = 
                                                    game_state_.in_game_[i];
  }

  int ranks[game_state_.num_players_];
  for (int i = 0; i < game_state_.num_players_; i++) {
    bets[i] = agents_[i]->get_bet();
    ranks[i] = SevenEval::GetRank(
      agents_[i]->get_c1(), agents_[i]->get_c2(),
      deck_[FLOP_1], deck_[FLOP_2], deck_[FLOP_3],
      deck_[TURN],
      deck_[RIVER]
    );
    awards[i] = 0;
    std::cout << "player: " << i << "rank: " << ranks[i] << std::endl;
  }
  
  while (players_left > 0) {
    double min_bet = game_state_.pot_;
    for (int i = 1; i < game_state_.num_players_; i++) {
      double bet_i = bets[i];
      if (bet_i < min_bet && bet_i > 0) min_bet = bet_i;
    }

    double side_pot = 0;

    int best_hand = -1;
    int best_players = 0;

    for (int i = 0; i < game_state_.num_players_; i++) {
      if (bets[i] >= min_bet) {
        side_pot += min_bet;
        bets[i] = bets[i] - min_bet;
      }
      if (in_game[i]) {
        if (ranks[i] > best_hand) {
          best_hand = ranks[i];
          best_players = 1;
        }
        else if (ranks[i] == best_hand) {
          best_players += 1;
        }
      }
    }
    std::cout << "side pot: " << side_pot << std::endl;
    std::cout << "best players: " << best_players << std::endl;

    for (int i = 0; i < game_state_.num_players_; i++) {
      if (in_game[i] && ranks[i] == best_hand) {
        awards[i] += side_pot / best_players;
      }
      if (bets[i] == 0) {
        if (in_game[i]) players_left += -1;
        in_game[i] = false;
      }
    }
  }

  for (int i = 0; i < game_state_.num_players_; i++) {
    if (awards[i] > 0) {
      std::cout << *agents_[i];
      std::cout << " wins " << awards[i] << std::endl;
    }
  }
}
