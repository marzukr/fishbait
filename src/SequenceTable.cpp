const unsigned int kColumnsAdd = 4;
const unsigned int kRoundNum = 0;
const unsigned int kFold = 1;
const unsigned int kCall = 2;
const unsigned int kAllIn = 3;
const double kBetAllowedInBigGame = 1;
const char kMultiwayThreshold = 3;

#include "SequenceTable.h"
#include <iostream>

SequenceTable::SequenceTable(float raise_sizes [kNumRounds][kNumRaises], 
                             int num_raise_sizes[kNumRounds], 
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
  num_rows_[0] = 1;
  num_rows_[1] = 0;
  num_rows_[2] = 0;
  num_rows_[3] = 0;
  //figure out amount of memory needed to allocate
  GameState state(num_players, num_rounds, small_blind_multiplier, starting_chips);
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
  table_ = new unsigned int*[total_rows_];
  AllocateRow(0,0);
  GameState state_new(num_players, num_rounds, small_blind_multiplier, 
                      starting_chips);
  Update(state_new, 0);
}

void SequenceTable::AllocateRow(unsigned int index, char current_round) {
  unsigned int num_needed = num_raise_sizes_[current_round] + kColumnsAdd;
  table_[index] = new unsigned int[num_needed];
  table_[index][0] = (unsigned int) num_raise_sizes_[current_round] + kColumnsAdd;
  for (int j = 1; j < num_needed; ++j) {
        table_[index][j] = kIllegalActionVal;
  }
}

SequenceTable::~SequenceTable() {
  for (int i = 0; i  < total_rows_; ++i) {
    delete[] table_[i];
  }
  delete[] table_;
}

void SequenceTable::Update(const GameState& state, unsigned int orig_index) {
  unsigned int recursive_index = orig_index;
  char acting_player = state.acting_player_;

  //go to next if already folded or all in
  if (state.in_game_[acting_player] == false || 
      state.chip_amounts_[acting_player] == 0) {
    GameState no_action_state = state;
    no_action_state.TakeAction(-2);
    if(!no_action_state.is_done_){
        if(no_action_state.current_round_ >=4 ){
          std::cout << "wtf" << std::endl;
        }
        Update(no_action_state, current_index_);
    }
  } else { //loop through all actions if not already folded or all in

    // check/call if possible
    double call = state.max_bet_ - state.total_bets_[acting_player];
    if (state.chip_amounts_[acting_player] > call) {
      GameState check_call_state = state;
      check_call_state.TakeAction(call);
      if (!check_call_state.is_done_) {
        current_index_ += 1;
        if (check_call_state.current_round_ >=4 ) {
          std::cout << "wtf" << std::endl;
        }
        if (update_nums_){
          num_rows_[check_call_state.current_round_] += 1;
        }
        else {
          AllocateRow(current_index_, check_call_state.current_round_);
          table_[recursive_index][kCall] = current_index_;
        }
        Update(check_call_state, current_index_);
      }
      else if (!update_nums_) {
        table_[recursive_index][kCall] = terminal_index_ + total_rows_;
        terminal_index_++;
      }
    }

    // all in
    GameState all_in_state = state;
    all_in_state.TakeAction(state.chip_amounts_[acting_player]);
    if (!all_in_state.is_done_) {
        current_index_ += 1;
        if (update_nums_) {
          num_rows_[all_in_state.current_round_] += 1;
        }
        else {
          AllocateRow(current_index_, all_in_state.current_round_);
          table_[recursive_index][kAllIn] = current_index_;
        }
        Update(all_in_state, current_index_);
    }
    else if (!update_nums_) {
      table_[recursive_index][kAllIn] = terminal_index_ + total_rows_;
      terminal_index_++;
    }
    // all other raise sizes that are legal
    // for pre flop, flop, or the first betting round on the turn or river allow
    // many bet sizes
    for (int i = 0; i < num_raise_sizes_[state.current_round_]; ++i) {
      double raise_size = (raise_sizes_[state.current_round_][i]) *
                          (state.pot_+call);
      double action = call+raise_size;
      if(IsLegalAction(action, raise_size, state)){
        GameState raise_state = state;
        raise_state.TakeAction(action);
        if (!raise_state.is_done_) {
          current_index_ += 1;
          if (raise_state.current_round_ >=4 ) {
            std::cout << "wtf" << std::endl;
          }
          if (update_nums_) {
            num_rows_[raise_state.current_round_] += 1;
          }
          else {
            table_[recursive_index][i+kColumnsAdd] = current_index_;
            AllocateRow(current_index_, raise_state.current_round_);             
          }
          Update(raise_state, current_index_);
        }
        else if (!update_nums_) { 
          table_[recursive_index][i+kColumnsAdd] = terminal_index_ + total_rows_;
          terminal_index_++;
        }
      }
    }
    // fold
    if (call != 0) {
      GameState fold_state = state;

      fold_state.TakeAction(-1);
      if (!fold_state.is_done_) {
        current_index_ += 1;
        if (update_nums_) {
          num_rows_[fold_state.current_round_] += 1;
        }
        else {
          AllocateRow(current_index_, fold_state.current_round_);
          table_[recursive_index][kFold] = current_index_;
        }
        Update(fold_state, current_index_);

      }
      else if (!update_nums_){
        table_[recursive_index][kFold] = terminal_index_ + total_rows_;
        terminal_index_++;
      }
    }
  }
  return;
}


double SequenceTable::IndexToAction(unsigned int action_index, GameState state){
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
unsigned int SequenceTable::GetTotalRows() {
  return (num_rows_[0] + num_rows_[1] + num_rows_[2] + num_rows_[3]);
}
unsigned int SequenceTable::GetPreflopRows() {
  return num_rows_[0];
}
unsigned int SequenceTable::GetFlopRows() {
  return num_rows_[1];
}
unsigned int SequenceTable::GetTurnRows() {
  return num_rows_[2];
}
unsigned int SequenceTable::GetRiverRows() {
  return num_rows_[3];
}

bool SequenceTable::IsLegalAction(double action, double raise_size, GameState state) {
  double call = action - raise_size;
  // only allow less than min raise if going all in 
  if (raise_size < state.min_raise_ && action < state.chip_amounts_[state.acting_player_]) {
    return false;
  }

  // never allow betting more chips than the player has
  if (action > state.chip_amounts_[state.acting_player_]) {
    return false;
  }

  // no other limits in first two rounds or 1st betting round in turn/river
  if (state.current_round_ <= 1 || state.betting_round_ < 1) {
    return true;
  }
  // turn/river 2nd betting round and beyond
  else {
    // only allow predefined bet size
    // should probably use epsilon instead of ==
    if (raise_size != (kBetAllowedInBigGame * (state.pot_ + call))) {
      return false;
    }

    // if too many people you just can't raise other than all in 
    if (state.num_left_ > kMultiwayThreshold){
      return false;
    }
  }
  return true;
}

void SequenceTable::PrintTable(bool full_table) {
  if (full_table){
      for(int i = 0; i < total_rows_; ++i){
        std::cout << "" << std::endl;
        int max = num_raise_sizes_[table_[i][0]] + kColumnsAdd;
        for(int j = 0; j < max; ++j) {
          if(table_[i][j] == -1){
            std::cout << "-1 ";
          }
          else{
            std::cout << table_[i][j] << " ";
          }
        }
    }
  }
  std::cout << "number pre flop: " << num_rows_[0] << std::endl;
  std::cout << "number flop: " << num_rows_[1] << std::endl;
  std::cout << "number turn: " << num_rows_[2] << std::endl;
  std::cout << "number river: " << num_rows_[3] << std::endl;
}




