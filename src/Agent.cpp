#include <iostream>
#include "Agent.h"

std::ostream& operator<<(std::ostream &strm, const Agent &a) {
    return strm << "Agent(" << a.c1 << ", " << a.c2 << ")";
}

// -1: fold
//  0: check
//  any other amount is a bet. if the value is the same as max_bet, then it is
//  a call, otherwise a raise.
//
//  this agent is dumb and will fold, check/call, and raise with equal
//  probability. when raising, it will always min raise.
int Agent::action(int max_bet, int min_raise) {
    return 0;
}
