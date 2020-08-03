#include <iostream>
#include <random>
#include <algorithm>
#include "SevenEval.h"
#include "Constants.h"
#include "Agent.h"

const int num_players = 6;

void MCCFR_P(int T) {
    
}

int main() {

    int deck[DECK_SIZE];

    for (int i = 0; i < DECK_SIZE; i++) {
        deck[i] = i;
    }
   
    std::random_device dev;
    std::mt19937 rng(dev());
    for (int i = DECK_SIZE - 1; i >= 1; i--) {
        std::uniform_int_distribution<std::mt19937::result_type> dist6(0,i);
        int j = dist6(rng);
        int temp = deck[i];
        deck[i] = deck[j];
        deck[j] = temp;
    }
    
    std::cout << "Shuffled Deck" << std::endl;

    Agent* agents[6];

    for (int i = 0; i < num_players; i++) {
        double blind = 0;
        if (i == 1) blind = 0.5;
        if (i == 2) blind = 1;
        Agent* a = new Agent(deck[2*i], deck[2*i + 1], blind);
        agents[i] = a;
    }

    std::cout << "Dealt Cards" << std::endl;

    int button = 0;
    double max_bet = 1;
    double min_raise = 1;
    double pot = 1.5;
    int pot_good = num_players;
    int no_raise = 0;
    bool in_game[] = {true, true, true, true, true, true};

    std::cout << "Preflop:" << std::endl;
    std::cout << "There is " << pot << " in the pot" << std::endl;

    for (int i = 0; i < pot_good + no_raise; i++) {
        int acting_player = (button + 3 + i) % num_players;
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
                no_raise = num_players - (pot_good - i);
                max_bet = action;
            }
            // raise
            else {
                double raise_amount = action - max_bet;
                min_raise = raise_amount;
                max_bet = action;
                pot_good = i + num_players;
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
            pot_good = i + num_players;
            no_raise = 0;
            pot += action - prev_bet;
        }

        std::cout << "There is " << pot << " in the pot" << std::endl;
    }

    std::cout << "The flop is "
        << deck[12] << ", "
        << deck[13] << ", "
        << deck[14] << std::endl;

    // std::cout << SevenEval::GetRank(0, 4, 8, 12, 16, 20, 24) << std::endl;
    return 0;
}
