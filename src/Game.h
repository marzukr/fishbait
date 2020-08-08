#ifndef GAME
#define GAME

#include "Constants.h"
#include "Agent.h"

#define NUM_PLAYERS 6
 
class Game
{
    private:
        int deck[DECK_SIZE];
        Agent* agents[NUM_PLAYERS];
        int button;

        double pot;
        double max_bet;
        double min_raise;
        bool in_game[NUM_PLAYERS];
        int players_left;

        // returns true if the game is complete, false otherwise
        bool main_game_loop(int first_to_act);

    public:
        Game(double bb_per_player);
    
        void shuffle_deck();

        // each street function returns true if the game is complete, false
        // otherwise
        bool preflop(int button_pos);
        bool flop();
        bool turn();
        bool river();
};
 
#endif
