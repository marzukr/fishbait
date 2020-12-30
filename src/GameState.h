
#include <stdint.h>


#define NUM_PLAYERS 2
#define MAX_TURNS 1
#define SMALL_BLIND 0.5


struct GameState{

    double chip_amounts[NUM_PLAYERS];
    bool in_game[NUM_PLAYERS];
    int acting_player;
    int current_round;
    // minimum raise for this round
    double min_raise;
    // maximum amount that has been put in by any individual, = call amount
    double max_bet;
    double pot;
    // represents the amount each player has in the pot FOR THE WHOLE HAND
    double total_bets[NUM_PLAYERS];
    char pot_good;
    bool is_done;
    int betting_round;

    // GameState& operator = (const GameState& other){
    //     chip_amounts = other.chip_amounts;
    //     in_game = other.in_game;
    //     total_bets = other.total_bets;
    //     acting_player = other.acting_player;
    //     current_round = other.current_round;
    //     min_raise = other.min_raise;
    //     max_bet = other.max_bet;
    //     pot = other.pot;
    //     pot_good = other.pot_good;
    //     is_done = other.is_done;
    //     return *this;
    // }


};