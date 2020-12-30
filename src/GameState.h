
#include <stdint.h>

#define NUM_PLAYERS 2
#define MAX_TURNS 1

struct GameState{

    float chip_amounts [NUM_PLAYERS];
    bool in_game [NUM_PLAYERS];
    int acting_player;
    int current_round;
    // minimum raise for this round
    float min_raise;
    // maximum amount that has been put in by any individual, = call amount
    float max_bet;
    float pot;
    // represents the amount each player has in the pot FOR THE WHOLE HAND
    float total_bets [NUM_PLAYERS];
    char pot_good;
    bool is_done;

};