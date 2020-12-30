#include "GameState.h"
#include <string>
#include <iostream>
#include <algorithm>
#include <iterator>

static int counter = 0;


bool GameOver(GameState state){
    int num = 0;
    int num_all_in = 0;
    for(int i = 0; i < NUM_PLAYERS; i++){
        if(state.in_game[i] == true){
            num++;
        }
        if(state.chip_amounts[i] == 0){
            num_all_in += 1;
        }
    }
    if(num <= 1 || state.current_round > (MAX_TURNS-1) || num_all_in == NUM_PLAYERS){
        // if(num <= 1 || num_all_in == NUM_PLAYERS){
            counter++;
            if(counter % 100000 == 0){
                std::cout << counter << std::endl;
            }
        // }
        return true;
    }
    return false;
}

void printChips(GameState state){
    std::cout << "round: " << state.current_round << " player: " << state.acting_player << " pot: " << state.pot << std::endl;
    for(int i = 0; i < NUM_PLAYERS; i++){
        std::cout << "Player: " << i << " in game: " << state.in_game[i] << " amount bet: " << state.total_bets[i] << " , amount left: " << state.chip_amounts[i] << std::endl;
    }
}

GameState TakeAction(GameState state, double action){
    // std::cout << "action: " << action << std::endl;
    // std::cout << "before" << std::endl;

    int acting_player = state.acting_player;

    GameState new_state = state;

    // std::cout << "initial" << std::endl;
    // std::cout << new_state.chip_amounts[0] << std::endl;
    // state.chip_amounts[0] = 56;
    // std::cout << "new" << std::endl;
    // std::cout << new_state.chip_amounts[0] << std::endl;
    // std::cout << "old" << std::endl;
    // std::cout << state.chip_amounts[0] << std::endl;





    
    new_state.acting_player = (new_state.acting_player+1) % NUM_PLAYERS;
    new_state.pot_good++;

    // get new amount that this player will have in pot and their new chip amounts
    double new_total_bet = new_state.total_bets[acting_player];
    double new_chip_amount = new_state.chip_amounts[acting_player];
    double raise_amount = 0;
    if(action >= 0){
        new_total_bet += action;
        new_chip_amount -= action;
        new_state.pot += action;
        raise_amount = new_total_bet - state.max_bet;
    }
    // fold
    else if(action == -1){
        new_state.in_game[acting_player] = false;
    }
    new_state.chip_amounts[acting_player] = new_chip_amount;
    new_state.total_bets[acting_player] = new_total_bet;
    new_state.betting_round++;

    //check or call doesn't need to change any additional variables

    // raise less than min raise (should only happen if all in)
    if(raise_amount > 0 && raise_amount < state.min_raise){
        double extra = raise_amount;
        new_state.min_raise = 2*state.min_raise + extra;
        new_state.max_bet = new_total_bet;
        new_state.pot_good = 1;

    }

    // raise min raise or more
    else if(raise_amount >= state.min_raise){
        new_state.min_raise = 2*raise_amount;
        new_state.max_bet = new_total_bet;
        new_state.pot_good = 1;
    }

    //check if we need to go to next round and update accordingly
    if(new_state.pot_good == NUM_PLAYERS){
        new_state.current_round++;
        new_state.pot_good = 0;
        new_state.min_raise = 1;
        new_state.acting_player = 0;
        new_state.betting_round = 0;


    }


    if(GameOver(new_state)){
        new_state.is_done = true;
    }
    // std::cout << "after" << std::endl;

    // printChips(new_state);
    //printChips(new_state);
    return new_state;

}