// Copyright 2021 Marzuk Rashid

#include <iostream>

#include "poker_engine/game.h"

int main() {
    // game with 100 bb / player, player 0 on the button
    poker_engine::Game g(100);
    g.play(0);

    return 0;
}
