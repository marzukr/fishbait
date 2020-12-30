
// -2: this player is already all in 
// -1: fold
// check/call
// all in
#include <string>
#include <iostream>

#include "GameLogic.h"

#define FOLD -1
#define NUM_ROWS 10000




int NumPossible(float raise_sizes[][14], float num_raise_sizes[], GameState state){

    int acting_player = state.acting_player;

    if(state.is_done == true){
        //std::cout << "returning " << state.pot << std::endl;
        return 1;
    }

    int total_num = 0;
    GameState new_state;

    //go to next if already folded or all in
    if(state.in_game[acting_player] == false || state.chip_amounts[acting_player] == 0){
        //std::cout << "Player " << acting_player << " has no choices, round " << state.current_round << std::endl;
        new_state = TakeAction(state, -2);
        total_num += NumPossible(raise_sizes, num_raise_sizes, new_state);

    }
    //loop through all actions if not already folded// all in
    else{
        // check/call if possible
        double call = state.max_bet - state.total_bets[acting_player];
        if(state.chip_amounts[acting_player] > call){
            //std::cout << "Player " << acting_player << " calls, round " << state.current_round << std::endl;
            new_state = TakeAction(state, call);
            total_num += NumPossible(raise_sizes, num_raise_sizes, new_state);
        }

        // all in
        //std::cout << "Player " << acting_player << " goes all in" << " round " << state.current_round << std::endl;
        new_state  = TakeAction(state, state.chip_amounts[acting_player]);


        total_num += NumPossible(raise_sizes, num_raise_sizes, new_state);

        // all other raise sizes NEED TO CHANGE IF YOU WANT TO CHANGE WHAT A "RAISE" IS
        if(state.current_round <= 2 || state.betting_round < 1){
            for(int i = 0; i < num_raise_sizes[state.current_round]; i++){
                double raise_size = (raise_sizes[state.current_round][i])*(state.pot+call);
                double action = call+raise_size;

                // legal bet (not equal to call or all in because that is already done)
                if(raise_size >= state.min_raise && action < state.chip_amounts[acting_player]){
                    //std::cout << "Player " << acting_player << "bets " << action << " round " << state.current_round << std::endl;
                    new_state = TakeAction(state, action);
                    total_num += NumPossible(raise_sizes, num_raise_sizes, new_state);
                }

            }
        }
        else{
                double raise_size = state.pot+call;
                double action = call+raise_size;

                // legal bet (not equal to call or all in because that is already done)
                if(raise_size >= state.min_raise && action < state.chip_amounts[acting_player]){
                    //std::cout << "Player " << acting_player << "bets " << action << " round " << state.current_round << std::endl;
                    new_state = TakeAction(state, action);
                    total_num += NumPossible(raise_sizes, num_raise_sizes, new_state);
                }

            }
        
        // fold
        if(call != 0){
            //std::cout << "Player " << acting_player << " folds " << "round " << state.current_round << std::endl;
            new_state = TakeAction(state, -1);
            total_num += NumPossible(raise_sizes, num_raise_sizes, new_state);
        }


    }
    return total_num;
    

}

int GetNumPossible(float raise_sizes[][14], float num_raise_sizes[], float starting_chips){
    bool in_game [NUM_PLAYERS];
    for(int i = 0; i < NUM_PLAYERS; i++){
        in_game[i] = true;
    }
    double chips_amounts [NUM_PLAYERS];
    for(int i = 0; i < NUM_PLAYERS; i++){
        chips_amounts[i] = starting_chips;
        if(i == 0){
            chips_amounts[i] -= SMALL_BLIND;
        }
        if(i == 1){
            chips_amounts[i] -= 1;
        }
    }
    int current_round = 0;
    int acting_player = 2 % NUM_PLAYERS;
    double min_raise = 1;
    double total_bets [NUM_PLAYERS];
    for(int i = 0; i < NUM_PLAYERS; i++){
        total_bets[i] = 0;
        if(i == 0){
            total_bets[i] = SMALL_BLIND;
        }
        if(i == 1){
            total_bets[i] = 1;
        }
    }
    double pot = 1+SMALL_BLIND;
    double max_bet = 1;
    GameState state = {
        {99.5,99,100,100},
        {1,1,1,1},
        acting_player,
        current_round,
        min_raise,
        max_bet,
        pot,
        {0.5,1,0,0},
        0,
        false,
        0
    };
    //printChips(state);


    return(NumPossible(raise_sizes, num_raise_sizes, state));

}

