#include <iostream>
#include <random>
#include <algorithm>
#include "Agent.h"

Agent::Agent(int card1, int card2, double blind) {
    c1 = card1;
    c2 = card2;
    bet = blind;
    chips = 100 - blind;
}

// -2: this player is already all in 
// -1: fold
//  any other amount is a bet. if the value is the same as max_bet, then it is
//  a check/call, otherwise a raise.
//
//  this agent is dumb and will check/fold, check/call, and raise with equal
//  probability. when raising, it will always min raise.
double Agent::action(double max_bet, double min_raise) {

    if (chips == 0) return -2;

    std::random_device dev;
    std::mt19937 rng(dev());
    std::uniform_int_distribution<std::mt19937::result_type> dist6(0,2);
    int decision = dist6(rng);
    
    // check/fold
    if (decision == 0) {
        if (max_bet > bet) return -1; // fold
        return bet; // check
    }
    // check/call
    else if (decision == 1) {
        if (max_bet > bet) { // call
            double call_size = std::min(chips, max_bet - bet);
            chips = chips - call_size;
            bet = bet + call_size;
            return bet;
        }
        return bet; // check
    }
    // raise
    else {
        double raise_size = std::min(chips, max_bet - bet + min_raise);
        chips = chips - raise_size;
        bet = bet + raise_size;
        return bet;
    }
}

std::ostream& operator<<(std::ostream &strm, const Agent &a) {
    return strm << "Agent(" << a.c1 << ", " << a.c2 << ", " << a.bet << ")";
}
