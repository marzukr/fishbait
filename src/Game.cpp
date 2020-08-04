#include <iostream>
#include <random>

#include "Game.h"
#include "Constants.h"

Game::Game(double bb_per_player) {
    // initialize the deck
    for (int i = 0; i < DECK_SIZE; i++) {
        deck[i] = i;
    }

    // initialize the players
    for (int i = 0; i < NUM_PLAYERS; i++) {
        Agent* a = new Agent(bb_per_player);
        agents[i] = a;
    }
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

void Game::preflop(int button_pos) {
    // shuffle and deal
    shuffle_deck();
    for (int i = 0; i < NUM_PLAYERS; i++) {
        agents[i]->deal_cards(deck[2*i], deck[2*i + 1]);
    }

    // assign positions and blinds
    button = button_pos;
    agents[button + 1]->assign_blind(0.5); // small blind
    agents[button + 2]->assign_blind(1); // big blind

    // set initial game state variables
    pot = 1.5;
    max_bet = 1;
    min_raise = 1;
    for (int i = 0; i < NUM_PLAYERS; i++) {
        in_game[i] = true;
    }

    // main game loop start with UTG
    main_game_loop(button + 3);
}

void Game::main_game_loop(int first_to_act) {
    int pot_good = NUM_PLAYERS;
    int no_raise = 0;

    std::cout << "There is " << pot << " in the pot" << std::endl;

    for (int i = 0; i < pot_good + no_raise; i++) {
        int acting_player = (first_to_act + i) % NUM_PLAYERS;
        if (!in_game[acting_player]) continue;
        std::cout << *agents[acting_player];

        double prev_bet = agents[acting_player]->get_bet();
        double action = agents[acting_player]->action(max_bet, min_raise);
        double chips = agents[acting_player]->get_chips();

        // already all in
        if (action == -2) {
            std::cout << " is already all in" << std::endl;
            continue;
        }
        // fold 
        else if (action == -1) {
            in_game[acting_player] = false;
            std::cout << " has folded" << std::endl; 
        }
        // going all in on this turn
        else if (chips == 0) {
            std::cout << " is all in";

            // call
            if (action <= max_bet) {}
            // call + extra
            else if (action < max_bet + min_raise) {
                no_raise = NUM_PLAYERS - (pot_good - i);
                max_bet = action;
            }
            // raise
            else {
                double raise_amount = action - max_bet;
                min_raise = raise_amount;
                max_bet = action;
                pot_good = i + NUM_PLAYERS;
                no_raise = 0;
            }
            pot += action - prev_bet;
        }
        // check/call
        else if (action == max_bet) {
            if (action == prev_bet) { // check
                std::cout << " has checked" << std::endl;
            }
            else {
                std::cout << " has called" << std::endl;
                pot += action - prev_bet;
            }
        }
        // raise
        else {
            std::cout << " has raised " << "to " << action << std::endl;
            double raise_amount = action - max_bet;
            max_bet = action;
            min_raise = raise_amount;
            pot_good = i + NUM_PLAYERS;
            no_raise = 0;
            pot += action - prev_bet;
        }

        std::cout << "There is " << pot << " in the pot" << std::endl;
    }
}

// std::ostream& operator<<(std::ostream &strm, const Agent &a) {
//     return strm << "Agent(" << a.c1 << ", " << a.c2 << ", " << a.bet << ")";
// }
