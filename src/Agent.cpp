#include <iostream>
#include <random>
#include <algorithm>
#include <string>

#include "Agent.h"
#include "hand_evaluator/Deckcards.h"

Agent::Agent(double start_chips, std::string agent_name) {
    bet_ = 0;
    chips_ = start_chips;
    name_ = agent_name;
    in_game_ = true;
}

// -2: this player is already all in 
// -1: fold
//  any other amount is a bet. if the value is the same as max_bet, then it is
//  a check/call, otherwise a raise.
//
//  this agent is dumb and will check/fold, check/call, and raise with equal
//  probability. when raising, it will always min raise.
double Agent::action(double max_bet, double min_raise) {

    if (chips_ == 0 || in_game_ == false) return -2;

    std::random_device dev;
    std::mt19937 rng(dev());
    std::uniform_int_distribution<std::mt19937::result_type> dist6(0,2);
    int decision = dist6(rng);
    // check/fold
    if (decision == 0) {
        std::cout << name_ << " has checked/folded" << std::endl;
        if (max_bet > bet_) {
            in_game_ = false;
            return -1; // fold
        } 
        return bet_; // check
    }
    // check/call
    else if (decision == 1) {
        if (max_bet > bet_) { // call
            double call_size = std::min(chips_, max_bet - bet_);
            chips_ = chips_ - call_size;
            bet_ += call_size;
            std::cout << name_ << " has checked/called " << call_size << std::endl;
            std::cout << name_ << " has " << bet_ << " in the pot " << std::endl;
            return call_size;
        }
        return 0; // check
    }
    // raise
    else {
        double raise_size = std::min(chips_, max_bet - bet_ + min_raise);
        chips_ -= raise_size;
        bet_ += raise_size;
        std::cout << name_ << " has bet " << raise_size << std::endl;
        std::cout << name_ << " has " << bet_ << " in the pot " << std::endl;
        return raise_size;
    }
}

void Agent::assign_blind(double blind) {
    double post_amount = std::min(chips_, blind);
    bet_ = bet_ + post_amount;
    chips_ = chips_ - post_amount;
}

void Agent::deal_cards(int card1, int card2) {
    c1_ = card1;
    c2_ = card2;
}

std::ostream& operator<<(std::ostream &strm, const Agent &a) {
    return strm << a.name_ << " (" 
        << pretty_card[a.c1_] << "," << pretty_card[a.c2_] << "," 
        << a.bet_ << ")";
}
