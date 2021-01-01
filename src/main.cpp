#include <iostream>

#include "Game.h"

int main() {
    // game with 100 bb / player, player 0 on the button
    Game g(6,4,0.5,100);
    
    g.Play(0);
    std::cout << "wtf" << std::endl;

    return 0;
}
