// Copyright Emily Dale 2021

#include "sequence_table/SequenceTable.h"

#include <stdint.h>
#include <math.h>
#include <iostream>

#include "utils/Matrix.h"


const uint32_t kColumnsAdd = 3;
const uint32_t kFold = 0;
const uint32_t kCall = 1;
const uint32_t kAllIn = 2;

namespace blueprint_strategy {

SequenceTable::SequenceTable(const std::vector<float> &raise_sizes,
                             const std::vector<char> &num_raise_sizes,
                             int32_t starting_chips, int32_t small_blind,
                             int32_t big_blind, char num_players,
                             char num_rounds, bool only_nums,
                             char multi_player_max, char num_raises_limit,
                             char round_limit_multi,
                             const std::vector<float> pre_late_raise_sizes) {
  num_raise_sizes_ = num_raise_sizes;
  total_rows_ = 0;
  num_rounds_ = num_rounds;
  terminal_rows_ = 0;
  num_illegal_ = 0;
  multi_player_max_ = multi_player_max;
  num_raises_limit_ = num_raises_limit;
  round_limit_multi_ = round_limit_multi;
  pre_late_raise_sizes_ = pre_late_raise_sizes;
  char length = 0;
  only_nums_ = only_nums;
  for (int i = 0; i < num_rounds_; ++i) {
    length += num_raise_sizes[i];
  }
  // make a vector of all raise sizes that includes fold/call/all in,
  // it is 1d vector to be fast, so we also have another vector saying the
  // indicies of where each round starts and stopped
  raise_sizes_.reserve(length + num_rounds_*kColumnsAdd);
  std::vector<int> raise_size_index(num_rounds_ + 1);
  raise_size_index[0] = 0;
  int temp_index = 0;
  int raises_index = 0;
  for (int i = 0; i < num_rounds_; ++i) {
    raise_sizes_[temp_index + kFold] = kIllegalActionVal;
    raise_sizes_[temp_index + kCall] = kIllegalActionVal;
    raise_sizes_[temp_index + kAllIn] = kIllegalActionVal;
    temp_index += kColumnsAdd;
    for (int j = 0; j < num_raise_sizes[i]; ++j) {
      raise_sizes_[temp_index + j] = raise_sizes[raises_index + j];
    }
    temp_index += num_raise_sizes[i];
    raises_index += num_raise_sizes[i];
    raise_size_index[i+1] = temp_index;
  }
  // number of rows for preflop starts at 1 to account for root node, all others
  // initialize to 0
  num_rows_.assign(num_rounds_, 0);
  num_rows_[0] = 1;
  // figure out amount of memory needed to allocate
  bool update_nums = true;
  uint32_t orig_index = 0;
  poker_engine::GameState state(num_players, num_rounds, small_blind, big_blind,
                                starting_chips, 0);
  Update(state, orig_index, raise_size_index, update_nums);
  if (only_nums) {
    return;
  }
  // actually make the table
  // initialization
  table_ = new uint32_t*[total_rows_];
  terminal_rows_ = total_rows_;
  total_rows_ = 1;
  for (int i = 0; i < num_rounds_; i++) {
    num_rows_[i] = 0;
  }
  // set up first row of table
  num_rows_[0] = 1;
  uint32_t index = 0;
  char current_round = 0;
  AllocateRow(index, current_round);
  update_nums = false;
  poker_engine::GameState state_new(num_players, num_rounds_, small_blind,
                                    big_blind, starting_chips, 0);
  // recursion
  Update(state_new, total_rows_, raise_size_index, update_nums);
  total_rows_ += 1;
}  // SequenceTable constructor

SequenceTable::~SequenceTable() {
  if (!only_nums_) {
    for (int i = 0; i  < total_rows_; ++i) {
      delete[] table_[i];
    }
    delete[] table_;
  }
}

void SequenceTable::AllocateRow(const uint32_t& index,
                                const char& current_round) {
  uint32_t num_needed = num_raise_sizes_[current_round] + kColumnsAdd;
  table_[index] = new uint32_t[num_needed];
  for (int j = 0; j < num_needed; ++j) {
        table_[index][j] = kIllegalActionVal;
  }
}

void SequenceTable::Update(poker_engine::GameState& state,
                           const uint32_t& orig_index,
                           const std::vector<int> &raise_size_index,
                           const bool& update_nums) {
  uint32_t recursive_index = orig_index;
  char acting_player = state.acting_player_;
  int32_t max_bet = state.max_bet_;
  int32_t pot_good = state.pot_good_;
  int32_t min_raise = state.min_raise_;
  char round = state.current_round_;
  char betting_round = state.betting_round_;
  int32_t pot = state.pot_;
  bool is_done = state.is_done_;
  char num_all_in = state.num_all_in_;
  char num_left = state.num_left_;
  int32_t number_of_raises = state.number_of_raises_;
  // go to next if already folded or all in
  if (state.in_game_[acting_player] == false ||
    state.chip_amounts_[acting_player] == 0) {
    state.TakeAction(-2);
    if (!state.is_done_) {
      Update(state, orig_index, raise_size_index, update_nums);
    } else {
      terminal_rows_ += 1;
    }
    state.UndoAction(acting_player, -2, max_bet, min_raise, pot_good, round,
                     betting_round, num_all_in, pot, num_left, is_done,
                     number_of_raises);
  } else {  // loop through all actions if not already folded or all in
    // loop through fold, call, all in, all raise sizes that are legal
    int32_t call = state.max_bet_ - state.total_bets_[acting_player];
    int32_t prev = -2;
    for (int i = raise_size_index[state.current_round_];
         i < raise_size_index[state.current_round_+1]; ++i) {
      uint32_t col_index = i-raise_size_index[state.current_round_];
      float raise_multiplier = raise_sizes_[i];
      double action = -1;
      double raise_size = raise_multiplier * (state.pot_+call);
      int32_t legalized_action = -1;
      if (col_index == kFold) {
        legalized_action = LegalActionFold(call);
      } else if (col_index == kCall) {
        legalized_action = LegalActionCall(call, state);
      } else if (col_index == kAllIn) {
        action = state.chip_amounts_[state.acting_player_];
        legalized_action = state.chip_amounts_[state.acting_player_];
      } else {
        action = call+raise_size;
        legalized_action = LegalAction(action, call, raise_multiplier, prev,
                                       state);
        if (legalized_action != -2) {
          prev = legalized_action;
        }
      }
      if (legalized_action >= -1) {
        state.TakeAction(legalized_action);
        if (!state.is_done_) {
          num_rows_[state.current_round_] += 1;
          total_rows_+=1;
          uint32_t assign_index = total_rows_;
          if (!update_nums) {
            AllocateRow(assign_index, state.current_round_);
            table_[recursive_index][col_index] = assign_index;
          }
          Update(state, assign_index, raise_size_index, update_nums);
        } else {
            terminal_rows_ += 1;
            if (!update_nums) {
              table_[recursive_index][col_index] = terminal_rows_;
            }
        }
        state.UndoAction(acting_player, legalized_action, max_bet, min_raise,
                         pot_good, round, betting_round, num_all_in, pot,
                         num_left, is_done, number_of_raises);
      }
    }
  }
  return;
}  // Update

uint32_t SequenceTable::GetTotalRows() {
  return total_rows_;
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
int32_t SequenceTable::LegalAction(const double& action, const int32_t& call,
                                   const float& raise_mult, const int32_t& prev,
                                   const poker_engine::GameState& state) {
  int32_t real_action = round(action);
  if (real_action == prev) {
    num_illegal_ += 1;
    return -2;
  }
  // always allow all in and call, only count once (only at kCall and kAllIn)
  if (real_action >= state.chip_amounts_[state.acting_player_]) {
    num_illegal_ += 1;
    return -2;
  }
  // actual action can only be in multiples of 0.5 big blinds
  // only allow less than min raise if going all in
  if (real_action - call < state.min_raise_) {
    num_illegal_ += 1;
    return -2;
  }
  // turn/river only allow 1x pot, doesn't depend on parameters
  if (state.current_round_ > 1 && state.number_of_raises_ > 0 &&
      raise_mult != 1) {
    num_illegal_ += 1;
    return -2;
  }
  // limits before the round to limit multi player
  if (state.current_round_ < round_limit_multi_) {
    if (state.num_left_ > (multi_player_max_) &&
        state.number_of_raises_ > (num_raises_limit_ + 1)) {
      num_illegal_ += 1;
      return -2;
    }
  } else {  // limits after
    if (state.num_left_ > multi_player_max_ &&
        state.number_of_raises_ > num_raises_limit_) {
      num_illegal_ += 1;
      return -2;
    }
  }
  if (state.current_round_ == 0 &&
      state.number_of_raises_ > num_raises_limit_) {
     for (float r : pre_late_raise_sizes_) {
       if (raise_mult == r) {
         return real_action;
       }
     }
     return -2;
  }
  return real_action;
}  // LegalAction

std::ostream& operator<<(std::ostream &strm, const SequenceTable &s) {
  double total = 0;
  for (int i = 0; i < s.num_rounds_; i++) {
    total += (s.num_raise_sizes_[i]+kColumnsAdd) * (s.num_rows_[i]/8000000000.0)
              * 32;
  }
  return strm << "number pre flop: " << s.num_rows_[0] << '\n' <<
              "number flop: " << s.num_rows_[1] << "\n" << "number turn: " <<
              s.num_rows_[2] << "\n" << "number river: " << s.num_rows_[3] <<
              "\n" << "number terminal " << s.terminal_rows_ << "\n" <<
              "number illegal " << s.num_illegal_ << "\n" << "memory: " <<
              total << " gb";
}

}  // namespace blueprint_strategy
