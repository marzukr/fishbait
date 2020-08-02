#include <iostream>
#include <random>
#include "SevenEval.h"
#include "Constants.h"

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

    for (int i = 0; i < DECK_SIZE; i++) {
        std::cout << deck[i] << std::endl;
    }

    // std::cout << SevenEval::GetRank(0, 4, 8, 12, 16, 20, 24) << std::endl;
    return 0;
}
