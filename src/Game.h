#ifndef GAME
#define GAME

#include "hand_evaluator/Constants.h"
#include "Agent.h"

#define NUM_PLAYERS 6
#define FLOP_1 12
#define FLOP_2 13
#define FLOP_3 14
#define TURN 15
#define RIVER 16
 
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

        void shuffle_deck();

        // returns true if the game is complete, false otherwise
        bool main_game_loop(int first_to_act);
        void award_pot();
        void TakeAction();


        // each street function returns true if the game is complete, false
        // otherwise
        bool preflop(int button_pos);
        bool flop();
        bool turn();
        void river();

    public:
        Game(double bb_per_player);
        void play(int button_pos);
};
 
#endif
