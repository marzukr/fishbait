#include <iostream>

#include "Game.h"

int main() {
    // game with 6 players, 4 rounds, 0.5 sb, 100 bb / player, player 0 small blind
    game_engine::Game g(6,4,0.5,100,0);
    g.Play();
    return 0;
}
