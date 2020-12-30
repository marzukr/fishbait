#include "GameState.h"
#include <string>
#include <iostream>
#include <algorithm>
#include <iterator>

static unsigned int counter = 0;
static unsigned int othercounter = 0;
static unsigned int gameovercounter = 0;

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
        gameovercounter++;
        // if(gameovercounter % 10000 == 1){
        //     std::cout << "game over counter " <<  gameovercounter << std::endl;
        // }
        // if(num <= 1 || num_all_in == NUM_PLAYERS){
            // counter++;
            // if(counter % 100000 == 0){
            //     std::cout << counter << std::endl;
            // }
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
void printAction(GameState state, double action){
    if(action == -2){
        std::cout << "player " << state.acting_player << " was already out round " <<  state.current_round << std::endl;
    }
    else if(action == -1){
        std::cout << "player " << state.acting_player << " folds round " << state.current_round << std::endl;
    }
    else{
        std::cout << "player " << state.acting_player << " bets " << action << " round " << state.current_round << std::endl;
    }
}

void TakeAction( GameState& state, double action){
    // std::cout << "acting player " << state.acting_player << std::endl;
    // std::cout << "action: " << action << std::endl;
    // std::cout << "before" << std::endl;
    // printAction(state, action);
    counter++;





    // std::cout << "initial" << std::endl;
    // std::cout << state.chip_amounts[0] << std::endl;
    // state.chip_amounts[0] = 56;
    // std::cout << "new" << std::endl;
    // std::cout << state.chip_amounts[0] << std::endl;
    // std::cout << "old" << std::endl;
    // std::cout << state.chip_amounts[0] << std::endl;





    
    state.acting_player = (state.acting_player+1) % NUM_PLAYERS;
    std::cout << "acting player " << state.acting_player << std::endl;
    int acting_player = state.acting_player;
    state.pot_good++;

    // get new amount that this player will have in pot and their new chip amounts
    double new_total_bet = state.total_bets[acting_player];
    double new_chip_amount = state.chip_amounts[acting_player];
    double raise_amount = 0;
    if(action >= 0){
        new_total_bet += action;
        new_chip_amount -= action;
        state.pot += action;
        raise_amount = new_total_bet - state.max_bet;
    }
    // fold
    else if(action == -1){
        state.in_game[acting_player] = false;
    }
    state.chip_amounts[acting_player] = new_chip_amount;
    state.total_bets[acting_player] = new_total_bet;
    state.betting_round++;

    //check or call doesn't need to change any additional variables

    // raise less than min raise (should only happen if all in)
    if(raise_amount > 0 && raise_amount < state.min_raise){
        double extra = raise_amount;
        state.min_raise = 2*state.min_raise + extra;
        state.max_bet = new_total_bet;
        state.pot_good = 1;

    }

    // raise min raise or more
    else if(raise_amount >= state.min_raise){
        state.min_raise = 2*raise_amount;
        state.max_bet = new_total_bet;
        state.pot_good = 1;
    }

    //check if we need to go to next round and update accordingly
    if(state.pot_good == NUM_PLAYERS){
        state.current_round++;
        state.pot_good = 0;
        state.min_raise = 1;
        state.acting_player = 0;
        state.betting_round = 0;


    }


    if(GameOver(state)){
        state.is_done = true;
    }
    else{
        othercounter++;
    }
    // std::cout << "after" << std::endl;

    // printChips(state);
    //printChips(state);
    // if(counter % 10000 == 0){
    //     std::cout << "total: " << counter << std::endl;
        
    // }
    // if(othercounter % 10000 == 0){
        
    //     std::cout << "non-leaf total: " << othercounter << std::endl;
    // }

}