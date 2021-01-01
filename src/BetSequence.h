#include <string>
#include <iostream>
#include "GameState.h"

#define NUM_RAISES 3

static int leaf_counter = 0;
static unsigned int counter = 0;
static unsigned int preflop = 0;
static unsigned int flop = 0;
static unsigned int turn = 0;
static unsigned int river = 0;

int NumPossible(float raise_sizes[][NUM_RAISES], float num_raise_sizes[], 
                const GameState& state){
  char acting_player = state.acting_player_;
  if(counter % 10000000 == 0){
      std::cout << "total:" << counter << std::endl;
  }
  if(preflop % 10000000 == 0){
      std::cout << "preflop:" << preflop << std::endl;
  }
  if(flop % 10000000 == 0){
      std::cout << "flop:" << flop << std::endl;
  }
  if(turn % 10000000 == 0){
      std::cout << "turn:" << turn << std::endl;
  }
  if(river % 10000000 == 0){
      std::cout << "river:" << river << std::endl;
  }
  if (state.is_done_ == true){
    // std::cout << "returning " << state.pot_ << std::endl;
    leaf_counter++;
    if(leaf_counter % 10000000 == 0){
      std::cout << "leaf:" << leaf_counter << std::endl;
    }
    return 1;
  }
  int total_num = 0;
  // GameState new_state = state;

  //go to next if already folded or all in
  if (state.in_game_[acting_player] == false || 
      state.chip_amounts_[acting_player] == 0) {
    GameState no_action_state = state;
    counter++;
    if(state.current_round_ == 0){
      preflop++;
    }
    else if(state.current_round_ == 1){
      flop++;
    }
    else if(state.current_round_ == 2){
      turn++;
    }
    else{
      river++;
    }
    no_action_state.TakeAction(-2);
    total_num += NumPossible(raise_sizes, num_raise_sizes, no_action_state);
  } else { //loop through all actions if not already folded or all in

    // check/call if possible
    double call = state.max_bet_ - state.total_bets_[acting_player];
    if (state.chip_amounts_[acting_player] > call){
      GameState check_call_state = state;
      counter++;
      if(state.current_round_ == 0){
        preflop++;
      }
      else if(state.current_round_ == 1){
        flop++;
      }
      else if(state.current_round_ == 2){
        turn++;
      }
      else{
        river++;
      }
      check_call_state.TakeAction(call);
      total_num += NumPossible(raise_sizes, num_raise_sizes, check_call_state);
    }

    // all in
    GameState all_in_state = state;
    counter++;
    if(state.current_round_ == 0){
      preflop++;
    }
    else if(state.current_round_ == 1){
      flop++;
    }
    else if(state.current_round_ == 2){
      turn++;
    }
    else{
      river++;
    }
    all_in_state.TakeAction(state.chip_amounts_[acting_player]);
    total_num += NumPossible(raise_sizes, num_raise_sizes, all_in_state);

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
          counter++;
          if(state.current_round_ == 0){
            preflop++;
          }
          else if(state.current_round_ == 1){
            flop++;
          }
          else if(state.current_round_ == 2){
            turn++;
          }
          else{
            river++;
          }
          raise_state.TakeAction(action);
          total_num += NumPossible(raise_sizes, num_raise_sizes, raise_state);
        }
      }
    }
    else { // if after the first betting round on the turn or river
      double raise_size = state.pot_+call;
      double action = call+raise_size;

      // legal bet (not equal to call or all in because that is already done)
      if (raise_size >= state.min_raise_ && 
          action < state.chip_amounts_[acting_player]){
        GameState raise_state = state;
        counter++;
        if(state.current_round_ == 0){
          preflop++;
        }
        else if(state.current_round_ == 1){
          flop++;
        }
        else if(state.current_round_ == 2){
          turn++;
        }
        else{
          river++;
        }
        raise_state.TakeAction(action);
        total_num += NumPossible(raise_sizes, num_raise_sizes, raise_state);
      }
    }
    // fold
    if (call != 0){
      GameState fold_state = state;
      counter++;
      if(state.current_round_ == 0){
        preflop++;
      }
      else if(state.current_round_ == 1){
        flop++;
      }
      else if(state.current_round_ == 2){
        turn++;
      }
      else{
        river++;
      }
      fold_state.TakeAction(-1);
      total_num += NumPossible(raise_sizes, num_raise_sizes, fold_state);
    }
  }

  return total_num;
}

int GetNumPossible(float raise_sizes[][NUM_RAISES], float num_raise_sizes[], 
                   double starting_chips, float small_blind_multiplier, 
                   char num_players, char num_rounds) {
  GameState state(num_players, num_rounds, 
                  small_blind_multiplier, starting_chips);
  return(NumPossible(raise_sizes, num_raise_sizes, state));
}

