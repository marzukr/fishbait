// copyright Emily Dale 2021
#include <iostream>

#include "poker_engine/Game.h"

int main() {
    // game with 6 players, 4 rounds, 0.5 sb, 100 bb / player,
    // player 0 small blind
    poker_engine::Game g(6, 4, 1, 2, 200, 0);
    g.Play();
    return 0;
}
