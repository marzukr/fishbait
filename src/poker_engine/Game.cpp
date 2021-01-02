#include <iostream>
#include <random>
#include <string>

#include "hand_evaluator/SevenEval.h"
#include "hand_evaluator/Deckcards.h"
#include "hand_evaluator/Constants.h"

#include "Game.h"

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
}

void Game::play(int button_pos) {
    bool game_complete = false;

    if (!game_complete) game_complete = preflop(button_pos);
    if (!game_complete) game_complete = flop();
    if (!game_complete) game_complete = turn();
    if (!game_complete) river();
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

    // set initial game state variables
    pot = 1.5;
    max_bet = 1;
    min_raise = 1;
    for (int i = 0; i < NUM_PLAYERS; i++) {
        in_game[i] = true;
    }
    players_left = NUM_PLAYERS;

    // main game loop start with UTG
    return main_game_loop(button + 3);
}

bool Game::flop() {
    std::cout << "--------------- Flop ---------------" << std::endl;
    std::cout << "The flop is "
        << pretty_card[deck[FLOP_1]] << ", "
        << pretty_card[deck[FLOP_2]] << ", "
        << pretty_card[deck[FLOP_3]] << std::endl;

    // the minimum bet amount is the big blind
    min_raise = 1;

    // main game loop start with SB
    return main_game_loop(button + 1);
}

bool Game::turn() {
    std::cout << "--------------- Turn ---------------" << std::endl;
    std::cout << "The turn is " << pretty_card[deck[TURN]] << std::endl;

    // the minimum bet amount is the big blind
    min_raise = 1;

    // main game loop start with SB
    return main_game_loop(button + 1);
}

void Game::river() {
    std::cout << "-------------- River ---------------" << std::endl;
    std::cout << "The river is " << pretty_card[deck[RIVER]] << std::endl;

    // the minimum bet amount is the big blind
    min_raise = 1;

    // main game loop start with SB
    bool folded = main_game_loop(button + 1);
    if (!folded) award_pot();
}

bool Game::main_game_loop(int first_to_act) {
    int pot_good = NUM_PLAYERS;
    int no_raise = 0;

    std::cout << "There is " << pot << " in the pot" << std::endl;

    for (int i = 0; i < pot_good + no_raise; i++) {
        // don't allow action to continue if there is only one player remaining
        if (players_left <= 1) break; 

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
            players_left += -1;
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

    if (players_left <= 1) {
        for (int i = 0; i < NUM_PLAYERS; i++) {
            if (in_game[i]) {
                std::cout << *agents[i];
                std::cout << " wins " << pot << std::endl;
                return true;
            }
        }
    }
    return false;
}

void Game::award_pot() {
    double bets[NUM_PLAYERS];
    double awards[NUM_PLAYERS];
    int ranks[NUM_PLAYERS];
    for (int i = 0; i < NUM_PLAYERS; i++) {
        bets[i] = agents[i]->get_bet();
        ranks[i] = SevenEval::GetRank(
            agents[i]->get_c1(), agents[i]->get_c2(),
            deck[FLOP_1], deck[FLOP_2], deck[FLOP_3],
            deck[TURN],
            deck[RIVER]
        );
        awards[i] = 0;
    }

    while (players_left > 0) {
        double min_bet = pot;
        for (int i = 1; i < NUM_PLAYERS; i++) {
            double bet_i = bets[i];
            if (bet_i < min_bet && bet_i > 0) min_bet = bet_i;
        }

        double side_pot = 0;

        int best_hand = -1;
        int best_players = 0;

        for (int i = 0; i < NUM_PLAYERS; i++) {
            if (bets[i] >= min_bet) {
                side_pot += min_bet;
                bets[i] = bets[i] - min_bet;
            }
            if (in_game[i]) {
                if (ranks[i] > best_hand) {
                    best_hand = ranks[i];
                    best_players = 1;
                }
                else if (ranks[i] == best_hand) {
                    best_players += 1;
                }
            }
        }

        for (int i = 0; i < NUM_PLAYERS; i++) {
            if (in_game[i] && ranks[i] == best_hand) {
                awards[i] += side_pot / best_players;
            }
            if (bets[i] == 0) {
                if (in_game[i]) players_left += -1;
                in_game[i] = false;
            }
        }
    }

    for (int i = 0; i < NUM_PLAYERS; i++) {
        if (awards[i] > 0) {
            std::cout << *agents[i];
            std::cout << " wins " << awards[i] << std::endl;
        }
    }
}
