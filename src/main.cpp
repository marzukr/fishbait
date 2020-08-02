#include <iostream>
#include <random>
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
        Agent* a = new Agent(deck[2*i], deck[2*i + 1]);
        agents[i] = a;
    }

    for (int i = 0; i < num_players; i++) {
        std::cout << *agents[i] << std::endl;
    }

    std::cout << "Dealt Cards" << std::endl;

    int button = 0;

    std::cout << "Preflop:" << std::endl;

    for (int i = 0; i < num_players; i++) {
        int acting_player = (button + 3 + i) % num_players;
        std::cout << "Action on player " << acting_player << std::endl;
        std::cout << "Player " << acting_player 
            << ": " << agents[acting_player]->action()
            << std::endl;
    }

    // std::cout << SevenEval::GetRank(0, 4, 8, 12, 16, 20, 24) << std::endl;
    return 0;
}
