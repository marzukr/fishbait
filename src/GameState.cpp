#include "GameState.h"

#include <iostream>

GameState::GameState(char num_players, char num_rounds, 
                     float small_blind_multiplier, double starting_bb_amounts) {
  // initialize arrays
  chip_amounts_  = new double[num_players];
  in_game_ = new bool[num_players];
  total_bets_ = new double[num_players];
  for (int i = 0; i < num_players; i++){
    chip_amounts_[i] = starting_bb_amounts;
    in_game_[i] = true;
    total_bets_[i] = 0;
    if (i == 0){
      chip_amounts_[i] -= small_blind_multiplier;
      total_bets_[i] = small_blind_multiplier;
    }
    else if (i == 1){
      chip_amounts_[i] -= 1;
      total_bets_[i] = 1;
    }
  }

  // initialize everything else
  acting_player_ = 2 % num_players;
  current_round_ = 0;
  min_raise_ = 1;
  max_bet_ = 1;
  pot_ = 1 + small_blind_multiplier;
  is_done_ = 0;
  betting_round_ = 0;
  num_players_ = num_players;
  num_rounds_ = num_rounds;
  small_blind_multiplier_ = small_blind_multiplier;
  starting_bb_amounts_ = starting_bb_amounts;
  pot_good_ = num_players;
  num_left_ = num_players;
  num_all_in_ = 0;
  needs_card_ = false;
}

GameState::GameState(const GameState& other) {
    pot_good_ = other.pot_good_;
    num_left_ = other.num_left_;
    acting_player_ = other.acting_player_;
    current_round_ = other.current_round_;
    min_raise_ = other.min_raise_;
    max_bet_ = other.max_bet_;
    pot_ = other.pot_;
    is_done_ = other.is_done_;
    betting_round_ = other.betting_round_;
    num_players_ = other.num_players_;
    num_rounds_ = other.num_rounds_;
    small_blind_multiplier_ = other.small_blind_multiplier_;
    starting_bb_amounts_ = other.starting_bb_amounts_;
    num_all_in_ = other.num_all_in_;
    needs_card_ = other.needs_card_;


    chip_amounts_ = new double[num_players_];
    in_game_ = new bool[num_players_];
    total_bets_ = new double[num_players_];

    for(int i = 0; i < num_players_; i++){
      chip_amounts_[i] = other.chip_amounts_[i];
      in_game_[i] = other.in_game_[i];
      total_bets_[i] = other.total_bets_[i];
    }
}

GameState& GameState::operator=(const GameState& other) {
  pot_good_ = other.pot_good_;
  num_left_ = other.num_left_;
  acting_player_ = other.acting_player_;
  current_round_ = other.current_round_;
  min_raise_ = other.min_raise_;
  max_bet_ = other.max_bet_;
  pot_ = other.pot_;
  is_done_ = other.is_done_;
  betting_round_ = other.betting_round_;
  num_players_ = other.num_players_;
  num_rounds_ = other.num_rounds_;
  small_blind_multiplier_ = other.small_blind_multiplier_;
  starting_bb_amounts_ = other.starting_bb_amounts_;
  num_all_in_ = other.num_all_in_;
  needs_card_ = other.needs_card_;

  for(int i = 0; i < num_players_; i++){
    chip_amounts_[i] = other.chip_amounts_[i];
    in_game_[i] = other.in_game_[i];
    total_bets_[i] = other.total_bets_[i];
  }
  return *this;
}

GameState::~GameState() {
  delete[] chip_amounts_;
  delete[] in_game_;
  delete[] total_bets_;
}

void GameState::PrintChips() {
  std::cout << "round: " << (int) current_round_ << " player: " 
            << (int) acting_player_ << " pot: " << pot_ << std::endl;
  for (int i = 0; i < num_players_; i++){
    std::cout << "Player: " << i << " in game: " << in_game_[i] 
              << " amount bet: " << total_bets_[i] << " , amount left: " 
              << chip_amounts_[i] << std::endl;
  }
}

void GameState::UpdateNeedsCard() {
  if (needs_card_ == false) {
    std::cout << "bad! trying to deal a card when the game doesn't need it" << std::endl;
  }
  needs_card_ = false;
}
void GameState::PrintAction(double action) {
  if (action == -2) {
    std::cout << "player " << acting_player_ << " was already out round " 
              <<  (int) current_round_ << std::endl;
  }
  else if (action == -1) {
    std::cout << "player " << (int) acting_player_ << " folds round " 
              << (int) current_round_ << std::endl;
  }
  else {
    std::cout << "player " << (int) acting_player_ << " bets " << action << " round " 
              << (int) current_round_ << std::endl;
  }
}
void GameState::TakeAction(double action) {
  if(acting_player_ == 0){
    betting_round_++;
  }
  // get new amount that this player will have in pot and their new chip amounts
  double new_total_bet = total_bets_[acting_player_];
  double new_chip_amount = chip_amounts_[acting_player_];
  double raise_amount = 0;
  if (action >= 0) {
    pot_good_--;
    new_total_bet += action;
    new_chip_amount -= action;
    pot_ += action;
    raise_amount = new_total_bet - max_bet_;
  }
  // fold
  else if (action == -1) {
    in_game_[acting_player_] = false;
    num_left_ -= 1;
    pot_good_--;
  }
  chip_amounts_[acting_player_] = new_chip_amount;
  total_bets_[acting_player_] = new_total_bet;

  //check or call doesn't need to change any additional variables

  // raise less than min raise (should only happen if all in)
  if(raise_amount > 0 && raise_amount < min_raise_) {
    double extra = raise_amount;
    min_raise_ = 2 * min_raise_ + extra;
    max_bet_ = new_total_bet;
    pot_good_ = num_left_-num_all_in_-1;
  }

  // raise min raise or more
  else if (raise_amount >= min_raise_) {
    min_raise_ = 2 * raise_amount;
    max_bet_ = new_total_bet;
    pot_good_ = num_left_-num_all_in_-1;
  }
  // update all in if this action puts player all in
  if (action >= chip_amounts_[acting_player_]) {
    num_all_in_ += 1;
  }

  // check if we need to go to next round and update accordingly
  if (pot_good_ == 0) {
    current_round_++;
    pot_good_ = num_left_-num_all_in_;
    min_raise_ = 1;
    acting_player_ = 0;
    betting_round_ = 0;
    needs_card_ = true;
  }
  else {
    acting_player_ = (acting_player_+1) % num_players_;
  }

  // check if game over and update accordingly
  if (current_round_ >= num_rounds_ || num_left_ <= 1 || num_all_in_ == num_left_){
    is_done_ = true;
  }
}

void GameState::UndoAction(char acted_player, double action, double old_max_bet, 
                           double old_min_raise, double old_pot_good, char old_round,
                           char old_betting_round, char old_all_in, double old_pot,
                           char old_num_left, bool old_is_done) {
  acting_player_ = acted_player;

  // get new amount that this player will have in pot and their new chip amounts
  double old_total_bet = total_bets_[acting_player_];
  double old_chip_amount = chip_amounts_[acting_player_];
  double raise_amount = 0;
  if (action >= 0) {
    old_total_bet -= action;
    old_chip_amount += action;
  }
  // fold
  else if (action == -1) {
    in_game_[acting_player_] = true;
  }
  chip_amounts_[acted_player] = old_chip_amount;
  total_bets_[acted_player] = old_total_bet;

  min_raise_ = old_min_raise;
  max_bet_ = old_max_bet;
  pot_good_ = old_pot_good;
  current_round_ = old_round;
  betting_round_ = old_betting_round;
  num_all_in_ = old_all_in;
  pot_ = old_pot;
  num_left_ = old_num_left;
  is_done_ = old_is_done;
}


