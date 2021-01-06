// Copyright 2021 Emily Dale
#include <iostream>
#include <random>
#include <algorithm>
#include <string>

#include "poker_engine/Agent.h"
#include "SKPokerEval/src/Deckcards.h"

namespace poker_engine {

// -2: this player is already all in
// -1: fold
//  any other amount is a bet. if the value is the same as max_bet, then it is
//  a check/call, otherwise a raise.
//
//  this agent is dumb and will check/fold, check/call, and raise with equal
//  probability. when raising, it will always min raise.
double Agent::action(double max_bet, double min_raise, double total_bet,
           double chips) {
  if (chips == 0) return -2;
  std::random_device dev;
  std::mt19937 rng(dev());
  std::uniform_int_distribution<std::mt19937::result_type> dist6(0, 2);
  int decision = dist6(rng);
  // check/fold
  if (decision == 0) {
    std::cout << name_ << " has checked/folded" << std::endl;
    if (max_bet > total_bet) {
      return -1;  // fold
    }
    return total_bet;  // check
  } else if (decision == 1) {     // check/call
    if (max_bet > total_bet) {  // call
      double call_size = std::min(chips, max_bet - total_bet);
      total_bet += call_size;
      std::cout << name_ << " has checked/called " << call_size << std::endl;
      std::cout << name_ << " has " << total_bet << " in the pot " << std::endl;
      return call_size;
    }
    return 0;  // check
  } else {   // raise
    double raise_size = std::min(chips, max_bet - total_bet + min_raise);
    chips -= raise_size;
    total_bet += raise_size;
    std::cout << name_ << " has bet " << raise_size << std::endl;
    std::cout << name_ << " has " << total_bet << " in the pot " << std::endl;
    return raise_size;
  }
}  // Agent constructor

void Agent::deal_cards(int card1, int card2) {
  c1_ = card1;
  c2_ = card2;
}

std::ostream& operator<<(std::ostream &strm, const Agent &a) {
  return strm << a.name_ << " ("
    << pretty_card[a.c1_] << "," << pretty_card[a.c2_] << ", )" << std::endl;
}

}  // namespace poker_engine
