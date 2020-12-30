#include "GameState.h"
#include <string>
#include <iostream>

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

GameState TakeAction(GameState state, float action){
    std::cout << "before" << std::endl;
    printChips(state);

    bool* new_in_game = state.in_game;
    float* new_chip_amounts = state.chip_amounts;
    float* new_total_bets = state.total_bets;
    int new_acting_player = (state.acting_player+1) % NUM_PLAYERS;
    float new_pot = state.pot;
    int new_current_round = state.current_round;
    float new_min_raise = state.min_raise;
    float new_max_bet = state.max_bet;
    float new_is_done = state.is_done;

    // this player will always be good
    char new_pot_good = state.pot_good + 1;

    int acting_player = state.acting_player;

    // get new amount that this player will have in pot and their new chip amounts
    float new_total_bet = state.total_bets[acting_player];
    float new_chip_amount = state.chip_amounts[acting_player];
    float raise_amount = 0;
    if(action >= 0){
        new_total_bet += action;
        new_chip_amount -= action;
        new_pot += action;
        raise_amount = new_total_bet - state.max_bet;


    }
    // fold
    else if(action == -1){
        new_in_game[acting_player] = false;
    }
    new_chip_amounts[acting_player] = new_chip_amount;
    new_total_bets[acting_player] = new_total_bet;

    //check or call doesn't need to change any additional variables

    // raise less than min raise (should only happen if all in)
    if(raise_amount > 0 && raise_amount < state.min_raise){
        float extra = raise_amount;
        new_min_raise = 2*state.min_raise + extra;
        new_max_bet = new_total_bet;
        new_pot_good = 1;

    }

    // raise min raise or more
    else if(raise_amount >= state.min_raise){
        new_min_raise = 2*raise_amount;
        new_max_bet = new_total_bet;
        new_pot_good = 1;
    }

    //check if we need to go to next round and update accordingly
    if(new_pot_good == NUM_PLAYERS){
        new_current_round++;
        new_pot_good = 0;
        new_min_raise = 1;
        new_acting_player = 0;


    }

    // construct new state
    GameState new_state = {
        new_chip_amounts,
        new_in_game,
        new_acting_player,
        new_current_round,
        new_min_raise,
        new_max_bet,
        new_pot,
        new_total_bets,
        new_pot_good,
        new_is_done,
    };
    if(GameOver(new_state)){
        new_state.is_done = true;
    }
    std::cout << "after" << std::endl;

    printChips(new_state);
    return new_state;

}