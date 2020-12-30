#include <iostream>
#include <random>
#include <string>

#include "hand_evaluator/SevenEval.h"
#include "hand_evaluator/Deckcards.h"
#include "hand_evaluator/Constants.h"

#include "Game.h"

// IT APPEARS THAT:
// action is the total chips that have been put in for this round
// min_raise is minimum raise that anyone has made
// max_bet is the maximum amount of total chips that someone has put in this round

Game::Game(double bb_per_player) {
    // initialize the deck
    for (int i = 0; i < DECK_SIZE; i++) {
        deck[i] = i;
    }

    std::string default_names[6] = {"Trump", "McConnell", "Pelosi", "Schumer", 
        "McCarthy", "Roberts"};

    // initialize the players
    for (int i = 0; i < NUM_PLAYERS; i++) {
        Agent* a = new Agent(bb_per_player, default_names[i]);
        agents[i] = a;
    }

    //initialize game state
    // set initial game state variables
    game_state.pot = 1+SMALL_BLIND;
    game_state.max_bet = 1;
    game_state.min_raise = 1;
    for (int i = 0; i < NUM_PLAYERS; i++) {
        game_state.in_game[i] = true;
    }
    for(int i = 0; i < NUM_PLAYERS; i++){
        game_state.chip_amounts[i] = starting_chips;
        if(i == 0){
            game_state.chip_amounts[i] -= SMALL_BLIND;
        }
        if(i == 1){
            game_state.chip_amounts[i] -= 1;
        }
    }
    game_state.current_round = 0;
    game_state.acting_player = 2 % NUM_PLAYERS;
    game_state.min_raise = 1;
    for(int i = 0; i < NUM_PLAYERS; i++){
        game_state.total_bets[i] = 0;
        if(i == 0){
            game_state.total_bets[i] = SMALL_BLIND;
        }
        if(i == 1){
            game_state.total_bets[i] = 1;
        }
    }

}

void Game::play(int button_pos) {

    preflop(button_pos);

    bool flop_done = false;
    bool turn_done = false;
    bool river_done = false;
    while(game_state.is_done == false){
        std::cout << "There is " << game_state.pot << " in the pot" << std::endl;
        if(game_state.current_round == 1 && flop_done == false){
            flop();
            flop_done = true;
        }
        else if(game_state.current_round == 2 && turn_done == false){
            turn();
            turn_done = true;
        }
        else if(game_state.current_round == 3 && river_done == false){
            river();
            river_done = true;
        }
        double action = agents[(game_state.acting_player+button)% NUM_PLAYERS]->action(game_state.max_bet, game_state.min_raise);
        double chips = agents[(game_state.acting_player+button)% NUM_PLAYERS]->get_chips();
        TakeAction(game_state, action);

    }
    for (int i = 0; i < NUM_PLAYERS; i++) {
            if (game_state.in_game[i]) {
                std::cout << *agents[i];
                std::cout << " wins " << game_state.pot << std::endl;
            }
        }
    award_pot();

}

void Game::shuffle_deck() {
    std::random_device dev;
    std::mt19937 rng(dev());

    // knuth shuffle
    for (int i = DECK_SIZE - 1; i >= 1; i--) {
        std::uniform_int_distribution<std::mt19937::result_type> dist6(0,i);
        int j = dist6(rng);
        int temp = deck[i];
        deck[i] = deck[j];
        deck[j] = temp;
    }
}

bool Game::preflop(int button_pos) {
    std::cout << "------------- Preflop --------------" << std::endl;

    // shuffle and deal
    shuffle_deck();
    for (int i = 0; i < NUM_PLAYERS; i++) {
        agents[i]->deal_cards(deck[2*i], deck[2*i + 1]);
    }

    // assign positions and blinds
    button = button_pos;
    agents[button + 1]->assign_blind(0.5); // small blind
    agents[button + 2]->assign_blind(1); // big blind

    
    players_left = NUM_PLAYERS;
}

bool Game::flop() {
    std::cout << "--------------- Flop ---------------" << std::endl;
    std::cout << "The flop is "
        << pretty_card[deck[FLOP_1]] << ", "
        << pretty_card[deck[FLOP_2]] << ", "
        << pretty_card[deck[FLOP_3]] << std::endl;

    // main game loop start with SB
    return main_game_loop(button + 1);
}

bool Game::turn() {
    std::cout << "--------------- Turn ---------------" << std::endl;
    std::cout << "The turn is " << pretty_card[deck[TURN]] << std::endl;
}

void Game::river() {
    std::cout << "-------------- River ---------------" << std::endl;
    std::cout << "The river is " << pretty_card[deck[RIVER]] << std::endl;
    // bool folded = main_game_loop(button + 1);
}
