#include <iostream>

#include "SevenEval.h"
#include "Game.h"

int main() {
    // game with 100 bb / player
    Game g(100);
    bool game_complete = false;

    // play the preflop with player 0 on the button
    if (!game_complete) game_complete = g.preflop(0);
    if (!game_complete) game_complete = g.flop();
    if (!game_complete) game_complete = g.turn();
    if (!game_complete) game_complete = g.river();

    // std::cout << SevenEval::GetRank(0, 4, 8, 12, 16, 20, 24) << std::endl;
    return 0;
}
