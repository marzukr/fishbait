#include <iostream>

#include "SevenEval.h"
#include "Game.h"

int main() {
    // game with 100 bb / player
    Game g(100);

    // play the preflop with player 0 on the button
    g.preflop(0);

    // std::cout << "The flop is "
    //     << deck[12] << ", "
    //     << deck[13] << ", "
    //     << deck[14] << std::endl;

    // std::cout << SevenEval::GetRank(0, 4, 8, 12, 16, 20, 24) << std::endl;
    return 0;
}
