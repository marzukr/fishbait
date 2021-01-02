#define FOLD 0
#define CALL 1
#define ALLIN 2
#include "SequenceTable.h"

SequenceTable::SequenceTable(float raise_sizes [NUM_ROUNDS][NUM_RAISES], int num_raise_sizes[NUM_ROUNDS], 
                   double starting_chips, float small_blind_multiplier, 
                   char num_players, char num_rounds){

  bool update_nums_ = true;
  //copy raise sizes table
  for(int i = 0; i < NUM_ROUNDS; i++){
    num_raise_sizes_[i] = num_raise_sizes[i];
    for(int j = 0; j < NUM_RAISES; j++){
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
  unsigned int total_rows = 0;
  for(int i = 0; i < NUM_ROUNDS; i++){
    total_rows += num_rows_[i];
  }
  table_ = new unsigned int*[total_rows];
  for(int i = 0; i < total_rows; ++i){
    if (i < num_rows_[1]) {
      table_[i] = new unsigned int[num_raise_sizes[1]+3];
    }
    else if (i < num_rows_[1] + num_rows_[2]){
      table_[i] = new unsigned int[num_raise_sizes[2]+3];
    }
    else if (i < num_rows_[1] + num_rows_[2] + num_rows_[3]){
      table_[i] = new unsigned int[num_raise_sizes[3]+3];
    }
    else {
      table_[i] = new unsigned int[num_raise_sizes[4]+3];
    }
  }
  GameState state_new(num_players, num_rounds, small_blind_multiplier, starting_chips);
  Update(state_new, 0);
}

void SequenceTable::Update(const GameState& state, unsigned int orig_index){
  unsigned int recursive_index = orig_index;
  char acting_player = state.acting_player_;

  //go to next if already folded or all in
  if (state.in_game_[acting_player] == false || 
      state.chip_amounts_[acting_player] == 0) {
    GameState no_action_state = state;
    no_action_state.TakeAction(-2);
    if(!no_action_state.is_done_){
        Update(no_action_state, current_index_);
    }
  } else { //loop through all actions if not already folded or all in

    // check/call if possible
    double call = state.max_bet_ - state.total_bets_[acting_player];
    if (state.chip_amounts_[acting_player] > call){
      GameState check_call_state = state;
      check_call_state.TakeAction(call);
      if(!check_call_state.is_done_){
        if(update_nums_){
          num_rows_[check_call_state.current_round_] += 1;
        }
        else{
          current_index_ += 1;
          table_[recursive_index][CALL] = current_index_;
        }
        Update(check_call_state, current_index_);
      }
      else if (!update_nums_){
        table_[recursive_index][CALL] = -1;
      }
    }

    // all in
    GameState all_in_state = state;
    all_in_state.TakeAction(state.chip_amounts_[acting_player]);
    if(!all_in_state.is_done_){
        if (update_nums_) {
          num_rows_[all_in_state.current_round_] += 1;
        }
        else {
          current_index_ += 1;
          table_[recursive_index][ALLIN] = current_index_;
        }
        Update(all_in_state, current_index_);
    }
    else if (!update_nums_){
      table_[recursive_index][ALLIN] = -1;
    }
    // all other raise sizes that are legal
    // for pre flop, flop, or the first betting round on the turn or river allow
    // many bet sizes
    if (state.current_round_ <= 1 || state.betting_round_ < 1){
      for (int i = 0; i < num_raise_sizes_[state.current_round_]; i++){
        double raise_size = (raise_sizes_[state.current_round_][i]) *
                            (state.pot_+call);
        double action = call+raise_size;

        // legal raise (not equal to call or all in)
        if (raise_size >= state.min_raise_ && 
            action < state.chip_amounts_[acting_player]){
          GameState raise_state = state;
          raise_state.TakeAction(action);
          if(!raise_state.is_done_){
            if(update_nums_){
              num_rows_[raise_state.current_round_] += 1;
            }
            else {
              current_index_ += 1;
              table_[recursive_index][i+3] = current_index_;
            }
            Update(raise_state, current_index_);
          }
          else if (!update_nums_){
            table_[recursive_index][i+3] = -1;
          }
        }
      }
    }
    // if after the first betting round on the turn or river, and we're not
    // limiting actions to only fold/call/all in
    else if(state.num_left_ <= 3) {
      double raise_size = state.pot_+call;
      double action = call+raise_size;

      // legal bet (not equal to call or all in because that is already done)
      if (raise_size >= state.min_raise_ && 
          action < state.chip_amounts_[acting_player]){
        GameState raise_state = state;
        raise_state.TakeAction(action);
        if(!raise_state.is_done_){
            if(update_nums_){
              num_rows_[raise_state.current_round_] += 1;
            }
            else {
              current_index_ += 1;
              table_[recursive_index][3] = current_index_;
            }
            Update(raise_state, current_index_);
          }
        else if (!update_nums_){
          table_[recursive_index][3] = -1;
        }
      }
    }
    // fold
    if (call != 0){
      GameState fold_state = state;

      fold_state.TakeAction(-1);
      if(!fold_state.is_done_){
        if(update_nums_){
          num_rows_[fold_state.current_round_] += 1;
        }
        else{
          current_index_ += 1;
          table_[recursive_index][FOLD] = current_index_;
        }
        Update(fold_state, current_index_);

      }
      else if (!update_nums_){
        table_[recursive_index][FOLD] = -1;
      }
    }
  }
  return;
}

void SequenceTable::NumPossible(const GameState& state){
  char acting_player = state.acting_player_;
  //go to next if already folded or all in
  if (state.in_game_[acting_player] == false || 
      state.chip_amounts_[acting_player] == 0) {
    GameState no_action_state = state;
    no_action_state.TakeAction(-2);
    if (!no_action_state.is_done_) {
      NumPossible(no_action_state);
    }
  } else { //loop through all actions if not already folded or all in

    // check/call if possible
    double call = state.max_bet_ - state.total_bets_[acting_player];
    if (state.chip_amounts_[acting_player] > call){
      GameState check_call_state = state;
      check_call_state.TakeAction(call);
      if(!check_call_state.is_done_){
        num_rows_[check_call_state.current_round_] += 1;
        NumPossible(check_call_state);
      }
    }

    // all in
    GameState all_in_state = state;
    all_in_state.TakeAction(state.chip_amounts_[acting_player]);
    if(!all_in_state.is_done_){
        num_rows_[all_in_state.current_round_] += 1;
        NumPossible(all_in_state);
    }

    // all other raise sizes that are legal

    // for pre flop, flop, or the first betting round on the turn or river allow
    // many bet sizes
    if (state.current_round_ <= 1 || state.betting_round_ < 1){
      for (int i = 0; i < num_raise_sizes_[state.current_round_]; i++){
        double raise_size = (raise_sizes_[state.current_round_][i]) *
                            (state.pot_+call);
        double action = call+raise_size;
        // legal raise (not equal to call or all in)
        if (raise_size >= state.min_raise_ && 
            action < state.chip_amounts_[acting_player]){
          GameState raise_state = state;
          raise_state.TakeAction(action);
          if(!raise_state.is_done_){
            num_rows_[raise_state.current_round_] += 1;
            NumPossible(raise_state);
          }
        }
      }
    }
    // if after the first betting round on the turn or river, and we're not
    // limiting actions to only fold/call/all in
    else if(state.num_left_ <= 3) {
      double raise_size = state.pot_+call;
      double action = call+raise_size;

      // legal bet (not equal to call or all in because that is already done)
      if (raise_size >= state.min_raise_ && 
          action < state.chip_amounts_[acting_player]){
        GameState raise_state = state;
        raise_state.TakeAction(action);
        if(!raise_state.is_done_){
          num_rows_[raise_state.current_round_] += 1;
          NumPossible(raise_state);
        }
      }
    }
    // fold
    if (call != 0){
      GameState fold_state = state;
      fold_state.TakeAction(-1);
      if(!fold_state.is_done_){
        num_rows_[fold_state.current_round_] += 1;
        NumPossible(fold_state);
      }
    }
  }
  return;
}


