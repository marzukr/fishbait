#include <string>
#include <iostream>
#include "GameState.h"

#define NUM_RAISES 3

static int leaf_counter = 0;
static int ind = 0;
static int non_leaf_index = 0;
static unsigned int counter = 0;
static unsigned int preflop = 0;
static unsigned int flop = 0;
static unsigned int turn = 0;
static unsigned int river = 0;

void PrintAction(const GameState& state, double action) {
  if (action == -2) {
    std::cout << "player " << state.acting_player_ << " was already out round " 
              <<  (int) state.current_round_ << std::endl;
  }
  else if (action == -1) {
    std::cout << "player " << (int) state.acting_player_ << " folds round " 
              << (int) state.current_round_ << std::endl;
  }
  else {
    std::cout << "player " << (int) state.acting_player_ << " bets " << action << " round " 
              << (int) state.current_round_ << std::endl;
  }
}
int NumPossible(float raise_sizes[][NUM_RAISES], float num_raise_sizes[], 
                const GameState& state, int total_num){
  int orig_total = total_num;
  // std::cout << "total:" << total_num << std::endl;
  // if(ind % 100000 == 0){
  //   std::cout << "index:" << ind << std::endl;
  // }
  // if(non_leaf_index % 100000 == 0){
  //   std::cout << "non-leaf:" << non_leaf_index << std::endl;
  // }
  char acting_player = state.acting_player_;
  if (state.is_done_ == true){
    // std::cout << "returning " << state.pot_ << std::endl;
    leaf_counter++;
    if(leaf_counter % 100000 == 0){
      std::cout << "leaf:" << leaf_counter << std::endl;
    }
    non_leaf_index--;
    return orig_total-total_num;
  }
  // GameState new_state = state;

  //go to next if already folded or all in
  if (state.in_game_[acting_player] == false || 
      state.chip_amounts_[acting_player] == 0) {
    GameState no_action_state = state;
    no_action_state.TakeAction(-2);
    return orig_total-NumPossible(raise_sizes, num_raise_sizes, no_action_state, total_num);
  } else { //loop through all actions if not already folded or all in

    // check/call if possible
    double call = state.max_bet_ - state.total_bets_[acting_player];
    if (state.chip_amounts_[acting_player] > call){
      GameState check_call_state = state;
      ind++;
      non_leaf_index++;
      check_call_state.TakeAction(call);
      total_num += NumPossible(raise_sizes, num_raise_sizes, check_call_state, total_num+1);
    }

    // all in
    GameState all_in_state = state;
    ind++;
    non_leaf_index++;
    all_in_state.TakeAction(state.chip_amounts_[acting_player]);
    total_num += NumPossible(raise_sizes, num_raise_sizes, all_in_state, total_num+1);

    // all other raise sizes that are legal

    // for pre flop, flop, or the first betting round on the turn or river allow
    // many bet sizes
    if (state.current_round_ <= 1 || state.betting_round_ < 1){
      for (int i = 0; i < num_raise_sizes[state.current_round_]; i++){
        double raise_size = (raise_sizes[state.current_round_][i]) *
                            (state.pot_+call);
        double action = call+raise_size;

        // legal raise (not equal to call or all in)
        if (raise_size >= state.min_raise_ && 
            action < state.chip_amounts_[acting_player]){
          GameState raise_state = state;
          ind++;
          non_leaf_index++;
          raise_state.TakeAction(action);
          total_num += NumPossible(raise_sizes, num_raise_sizes, raise_state, total_num+1);
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
        ind++;
        non_leaf_index++;
        raise_state.TakeAction(action);
        total_num += NumPossible(raise_sizes, num_raise_sizes, raise_state, total_num+1);
      }
    }
    // fold
    if (call != 0){
      GameState fold_state = state;
      ind++;
      non_leaf_index++;
      fold_state.TakeAction(-1);
      total_num += NumPossible(raise_sizes, num_raise_sizes, fold_state, total_num+1);
    }
  }

  return orig_total-total_num;
}

int GetNumPossible(float raise_sizes[][NUM_RAISES], float num_raise_sizes[], 
                   double starting_chips, float small_blind_multiplier, 
                   char num_players, char num_rounds) {
  GameState state(num_players, num_rounds, 
                  small_blind_multiplier, starting_chips);
  return(NumPossible(raise_sizes, num_raise_sizes, state, 0));
}

