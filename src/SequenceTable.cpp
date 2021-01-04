#include "SequenceTable.h"
#include <iostream>
#include <stdint.h>
#include <math.h>

const uint32_t kColumnsAdd = 4;
const uint32_t kRoundNum = 0;
const uint32_t kFold = 1;
const uint32_t kCall = 2;
const uint32_t kAllIn = 3;
const double kBetAllowedInBigGame = 1;
const char kMultiwayThreshold = 3;

namespace blueprint_strategy {

SequenceTable::SequenceTable(float raise_sizes [kNumRounds][kNumRaises], 
                             char num_raise_sizes[kNumRounds], 
                             double starting_chips, float small_blind_multiplier, 
                             char num_players, char num_rounds) {

  current_index_ = 0;
  update_nums_ = true;
  //copy raise sizes table
  for(int i = 0; i < kNumRounds; i++){
    num_raise_sizes_[i] = num_raise_sizes[i];
    for(int j = 0; j < kNumRaises; j++){
      raise_sizes_[i][j] = raise_sizes[i][j];
    }
  }
  // explain
  num_rows_[0] = 1;
  num_rows_[1] = 0;
  num_rows_[2] = 0;
  num_rows_[3] = 0;
  num_rounds_ = num_rounds;

  //figure out amount of memory needed to allocate
  game_engine::GameState state(num_players, num_rounds, small_blind_multiplier, starting_chips, 0);
  Update(state, 0);
  update_nums_ = false;
  //actually make the sequence table
  current_index_ = 0;
  terminal_index_ = 0;
  total_rows_ = 0;
  PrintTable(false);
  for (int i = 0; i < kNumRounds; ++i) {
    total_rows_ += num_rows_[i];
  }
  table_ = new uint32_t*[total_rows_];
  AllocateRow(0,0);
  game_engine::GameState state_new(num_players, num_rounds, small_blind_multiplier, 
                      starting_chips, 0);
  Update(state_new, 0);
}

void SequenceTable::AllocateRow(uint32_t index, char current_round) {
  uint32_t num_needed = num_raise_sizes_[current_round] + kColumnsAdd;
  table_[index] = new uint32_t[num_needed];
  table_[index][0] = (uint32_t) num_raise_sizes_[current_round] + kColumnsAdd;
  for (int j = 1; j < num_needed; ++j) {
    table_[index][j] = kIllegalActionVal;
  }
}  // SequenceTable constructor

SequenceTable::~SequenceTable() {
  for (int i = 0; i  < total_rows_; ++i) {
    delete[] table_[i];
  }
  delete[] table_;
}

void SequenceTable::Update(game_engine::GameState& state, uint32_t orig_index) {
  uint32_t recursive_index = orig_index;
  char acting_player = state.acting_player_;
  double max_bet = state.max_bet_;
  double pot_good = state.pot_good_;
  double min_raise = state.min_raise_;
  char round = state.current_round_;
  char betting_round = state.betting_round_;
  double pot = state.pot_;
  bool is_done = state.is_done_;
  char num_all_in = state.num_all_in_;
  char num_left = state.num_left_;
  //go to next if already folded or all in
  if (state.in_game_[acting_player] == false || 
      state.chip_amounts_[acting_player] == 0) {
    // GameState no_action_state = state;
    state.TakeAction(-2);
    if(!state.is_done_){
        if(state.current_round_ >=4 ){
          std::cout << "wtf" << std::endl;
        }
        Update(state, current_index_);
    }
    state.UndoAction(acting_player, -2, max_bet, min_raise, pot_good, round, betting_round, num_all_in, pot, num_left, is_done);
  } else { //loop through all actions if not already folded or all in

    // check/call if possible
    double call = state.max_bet_ - state.total_bets_[acting_player];
    if (state.chip_amounts_[acting_player] > call) {
      // GameState check_call_state = state;
      state.TakeAction(call);
      if (!state.is_done_) {
        current_index_ += 1;
        if (state.current_round_ >=4 ) {
          std::cout << "wtf" << std::endl;
        }
        if (update_nums_){
          num_rows_[state.current_round_] += 1;
        }
        else {
          AllocateRow(current_index_, state.current_round_);
          table_[recursive_index][kCall] = current_index_;
        }
        Update(state, current_index_);
      }
      else if (!update_nums_) {
        table_[recursive_index][kCall] = terminal_index_ + total_rows_;
        terminal_index_++;
      }
      state.UndoAction(acting_player, call, max_bet, min_raise, pot_good, round, betting_round, num_all_in, pot, num_left, is_done);
    }


    // all in
    // GameState all_in_state = state;
    double action = state.chip_amounts_[acting_player];
    state.TakeAction(action);
    if (!state.is_done_) {
        current_index_ += 1;
        if (update_nums_) {
          num_rows_[state.current_round_] += 1;
        }
        else {
          AllocateRow(current_index_, state.current_round_);
          table_[recursive_index][kAllIn] = current_index_;
        }
        Update(state, current_index_);
    }
    else if (!update_nums_) {
      table_[recursive_index][kAllIn] = terminal_index_ + total_rows_;
      terminal_index_++;
    }
    state.UndoAction(acting_player, action, max_bet, min_raise, pot_good, round, betting_round, num_all_in, pot, num_left, is_done);
    // all other raise sizes that are legal
    // for pre flop, flop, or the first betting round on the turn or river allow
    // many bet sizes
    for (int i = 0; i < num_raise_sizes_[state.current_round_]; ++i) {
      float raise_multiplier = raise_sizes_[state.current_round_][i];
      double raise_size = raise_multiplier * (state.pot_+call);
      double action = call+raise_size;
      double legalized_action = LegalAction(action, call, raise_multiplier, state);
      if(legalized_action > 0){
        // GameState raise_state = state;
        state.TakeAction(legalized_action);
        if (!state.is_done_) {
          current_index_ += 1;
          if (state.current_round_ >=4 ) {
            std::cout << "wtf" << std::endl;
          }
          if (update_nums_) {
            num_rows_[state.current_round_] += 1;
          }
          else {
            table_[recursive_index][i+kColumnsAdd] = current_index_;
            AllocateRow(current_index_, state.current_round_);             
          }
          Update(state, current_index_);
        }
        else if (!update_nums_) { 
          table_[recursive_index][i+kColumnsAdd] = terminal_index_ + total_rows_;
          terminal_index_++;
        }
        state.UndoAction(acting_player, legalized_action, max_bet, min_raise, pot_good, round, betting_round, num_all_in, pot, num_left, is_done);
      }
    }
    // fold
    if (call != 0) {
      // GameState fold_state = state;
      state.TakeAction(-1);
      if (!state.is_done_) {
        current_index_ += 1;
        if (update_nums_) {
          num_rows_[state.current_round_] += 1;
        }
        else {
          AllocateRow(current_index_, state.current_round_);
          table_[recursive_index][kFold] = current_index_;
        }
        Update(state, current_index_);
      }
      else if (!update_nums_){
        table_[recursive_index][kFold] = terminal_index_ + total_rows_;
        terminal_index_++;
      }
      state.UndoAction(acting_player, -1, max_bet, min_raise, pot_good, round, betting_round, num_all_in, pot, num_left, is_done);
    }
  }
  return;
}  // Update

double SequenceTable::IndexToAction(uint32_t action_index, game_engine::GameState state) {
  if (action_index == 0) {
    std::cout << "bad action index" << std::endl;
    return -2;
  }
  double call = state.max_bet_ - state.total_bets_[state.acting_player_];
  if (action_index >= kColumnsAdd) {
    return (raise_sizes_[state.current_round_][action_index - kColumnsAdd])*
           (state.pot_+call) + call;
  }
  else if (action_index == kFold) {
    return -1;
  }
  else if (action_index == kCall) {
    return call;
  }
  else if (action_index == kAllIn) {
    return state.chip_amounts_[state.acting_player_];
  }
  std::cout << "bad action index" << std::endl;
  return -2;
}

uint32_t SequenceTable::GetTotalRows() {
  return (num_rows_[0] + num_rows_[1] + num_rows_[2] + num_rows_[3]);
}
uint32_t SequenceTable::GetPreflopRows() {
  return num_rows_[0];
}
uint32_t SequenceTable::GetFlopRows() {
  return num_rows_[1];
}
uint32_t SequenceTable::GetTurnRows() {
  return num_rows_[2];
}
uint32_t SequenceTable::GetRiverRows() {
  return num_rows_[3];
}

double SequenceTable::LegalAction(double action, double call, float raise_mult, game_engine::GameState state) {
  // actual action can only be in multiples of 0.5 big blinds
  float real_action = (round(action * 2))/2.0;
  // only allow less than min raise if going all in 
  if (real_action - call < state.min_raise_ && real_action < state.chip_amounts_[state.acting_player_]) {
    return -1;
  }

  // never allow betting more chips than the player has
  if (real_action >= state.chip_amounts_[state.acting_player_]) {
    return -1;
  }

  // no other limits in first two rounds or 1st betting round in turn/river
  if (state.current_round_ <= 1 || state.betting_round_ < 1) {
    return real_action;
  }
  // turn/river 2nd betting round and beyond
  else {
    // only allow predefined bet size
    // should probably use epsilon instead of ==
    if (raise_mult != (kBetAllowedInBigGame)) {
      return -1;
    }

    // if too many people you just can't raise other than all in 
    if (state.num_left_ > kMultiwayThreshold) {
      return -1;
    }
  }
  return real_action;
}  // LegalAction

void SequenceTable::PrintTable(bool full_table) {
  if (full_table) {
      for (int i = 0; i < total_rows_; ++i) {
        std::cout << "" << std::endl;
        int max = table_[i][0];
        for (int j = 0; j < max; ++j) {
          if (table_[i][j] == -1) {
            std::cout << "-1 ";
          }
          else {
            std::cout << table_[i][j] << " ";
          }
        }
    }
  }
  std::cout << "number pre flop: " << num_rows_[0] << std::endl;
  std::cout << "number flop: " << num_rows_[1] << std::endl;
  std::cout << "number turn: " << num_rows_[2] << std::endl;
  std::cout << "number river: " << num_rows_[3] << std::endl;
  double total = 0;
  for (int i = 0; i < num_rounds_; i++) { 
    total += (num_raise_sizes_[i]+kColumnsAdd)*num_rows_[i]*32/8000000000;
  }
  double gig = total/8000000000;
  std::cout << "memory: " << total << " gb" << std::endl;
}

}  // namespace blueprint_strategy
