#include "sequence_table/SequenceTable.h"
#include "utils/Matrix.h"
#include <iostream>
#include <stdint.h>
#include <math.h>

const uint32_t kColumnsAdd = 3;
const uint32_t kFold = 0;
const uint32_t kCall = 1;
const uint32_t kAllIn = 2;
const double kBetAllowedInBigGame = 1;
const char kMultiwayThreshold = 3;

namespace blueprint_strategy {

SequenceTable::SequenceTable(float raise_sizes_2d[kNumRounds][kNumRaises], 
                             char num_raise_sizes[kNumRounds], 
                             double starting_chips, float small_blind_multiplier, 
                             char num_players, char num_rounds) {

  total_rows_ = 0;
  num_rounds_ = num_rounds;
  // convert raise sizes from 2d to 1d
  float raise_sizes[kNumRounds*(kNumRaises+kColumnsAdd)];
  int raise_size_index[kNumRounds+1];
  raise_size_index[0] = 0;
  int temp_index = 0;
  for (int i = 0; i < num_rounds; ++i) {
    raise_sizes[temp_index + kFold] = -1;
    raise_sizes[temp_index + kCall] = -1;
    raise_sizes[temp_index + kAllIn] = -1;
    temp_index += kColumnsAdd;
    for (int j = 0; j < num_raise_sizes[i]; ++j) {
      raise_sizes[temp_index + j] = raise_sizes_2d[i][j];
    }
    temp_index += num_raise_sizes[i];
    std::cout << (int) num_raise_sizes[i] << std::endl;
    raise_size_index[i+1] = temp_index;
  }
  // number of rows for preflop starts at 1 to account for root node, all others
  // initialize to 0
  num_rows_[0] = 0;
  num_rows_[1] = 0;
  num_rows_[2] = 0;
  num_rows_[3] = 0;

  // figure out amount of memory needed to allocate
  poker_engine::GameState state(num_players, num_rounds, small_blind_multiplier, starting_chips, 0);
  Update(state, 0, raise_sizes, raise_size_index, num_raise_sizes, true);
  std::cout << "0: "<< num_rows_[0] << std::endl;

  for (int i = 0; i < num_rounds; ++i) {
    table_[i] = new utils::Matrix<uint32_t>(num_rows_[i]+1, (num_raise_sizes[i]+kColumnsAdd));
    table_[i]->Fill(kIllegalActionVal);
  }
  for(int i = 0; i < num_rounds; i++){
    terminal_rows_[i] = num_rows_[i] + 1;
    num_rows_[i] = 0;
  }

  poker_engine::GameState state_new(num_players, num_rounds, small_blind_multiplier, 
                      starting_chips, 0);
  Update(state_new, total_rows_, raise_sizes, raise_size_index, num_raise_sizes, false);
  total_rows_ += 1;
  std::cout << num_rows_[0] << std::endl;
  std::cout << num_rows_[1] << std::endl;
  std::cout << num_rows_[2] << std::endl;
  std::cout << num_rows_[3] << std::endl;
  PrintTable(true, num_rounds);
} // SequenceTable constructor

SequenceTable::~SequenceTable() {
  for (int i = 0; i  < num_rounds_; ++i) {
    delete table_[i];
  }
}

void SequenceTable::Update(poker_engine::GameState& state, uint32_t orig_index, 
                           float raise_sizes [kNumRounds*kNumRaises], int raise_size_index[kNumRounds+1],
                           char num_raise_sizes[kNumRounds], bool update_nums) {
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
    state.TakeAction(-2);
    if(!state.is_done_){
      Update(state, orig_index, raise_sizes, raise_size_index, num_raise_sizes, update_nums);
    }
    state.UndoAction(acting_player, -2, max_bet, min_raise, pot_good, round, betting_round, num_all_in, pot, num_left, is_done);
  } else { //loop through all actions if not already folded or all in
    // loop through fold, call, all in, all raise sizes that are legal
    double call = state.max_bet_ - state.total_bets_[acting_player];
    for (int i = raise_size_index[state.current_round_]; i < raise_size_index[state.current_round_+1]; ++i) {
      
      uint32_t col_index = i-raise_size_index[state.current_round_];
      float raise_multiplier = raise_sizes[i];
      double action = -1;
      double raise_size = raise_multiplier * (state.pot_+call);
      if (col_index == kCall) {
        action = call;
      } else if (col_index == kAllIn) {
        action = state.chip_amounts_[state.acting_player_];
      } else if (col_index >= kColumnsAdd){
        action = call+raise_size;
      }
      double legalized_action = LegalAction(action, call, raise_multiplier, state);
      std::cout << legalized_action << std::endl;
      if(legalized_action >= -1){
        state.TakeAction(legalized_action);
        if (!state.is_done_) {
          num_rows_[state.current_round_] += 1;
          uint32_t assign_index = num_rows_[state.current_round_];
          if (!update_nums) {
            (*(table_[round]))(recursive_index,col_index) = assign_index;             
          }
          Update(state, assign_index, raise_sizes, raise_size_index, num_raise_sizes, update_nums);
        } else if (!update_nums) { 
        (*(table_[round]))(recursive_index, col_index) = terminal_rows_[round];
        terminal_rows_[round] += 1;
        }
        state.UndoAction(acting_player, legalized_action, max_bet, min_raise, pot_good, round, betting_round, num_all_in, pot, num_left, is_done);
      }
    }
  }
  return;
}  // Updated

double SequenceTable::IndexToAction(uint32_t action_index, poker_engine::GameState state) {
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

// returns -2 for illegal actions, otherwise the action
double SequenceTable::LegalAction(double action, double call, float raise_mult, poker_engine::GameState state) {
  // only allow fold if the player can't check
  if (action == -1) {
    if (call == 0) {
      return -2;
    } else {
      return -1;
    }
  }
  float real_action = (round(action * 2))/2.0;
  // always allow all in and call, only count once (only at kCall and kAllIn)
  if (real_action == state.chip_amounts_[state.acting_player_] || real_action == call) {
    if (raise_mult == -1) {
      return real_action;
    } else return -2;
  }
  // actual action can only be in multiples of 0.5 big blinds
  // only allow less than min raise if going all in 
  if (real_action - call < state.min_raise_ && real_action < state.chip_amounts_[state.acting_player_]) {
    return -2;
  }

  // never allow betting more chips than the player has
  if (real_action >= state.chip_amounts_[state.acting_player_]) {
    return -2;
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
      return -2;
    }

    // if too many people you just can't raise other than all in 
    if (state.num_left_ > kMultiwayThreshold) {
      return -2;
    }
  }
  return real_action;
}  // LegalAction

void SequenceTable::PrintTable(bool full_table, char num_rounds) {
  if (full_table) {
      for (int i = 0; i < num_rounds_; ++i) {
        int m = table_[i] -> m();
        int n = table_[i] -> n();
        for (int j = 0; j < n; ++j) {
          std::cout << "" << std::endl;
          for(int k = 0; k < m; ++k) {
            std::cout << table_[i]->Access(j,k) << " ";
          }
        }
    }
  }
  std::cout << "number pre flop: " << num_rows_[0] << std::endl;
  std::cout << "number flop: " << num_rows_[1] << std::endl;
  std::cout << "number turn: " << num_rows_[2] << std::endl;
  std::cout << "number river: " << num_rows_[3] << std::endl;
  // double total = 0;
  // for (int i = 0; i < num_rounds_; i++) { 
  //   total += (num_raise_sizes_[i]+kColumnsAdd)*num_rows_[i]*32/8000000000;
  // }
  // double gig = total/8000000000;
  // std::cout << "memory: " << total << " gb" << std::endl;
}

}  // namespace blueprint_strategy
