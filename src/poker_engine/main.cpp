#include <iostream>

#include "Game.h"

int main() {
    // game with 100 bb / player, player 0 on the button
    Game g(100);
    g.play(0);

    return 0;
}
