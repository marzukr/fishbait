#include "BetSequence.h"
#include <iostream>


int main(){
  //add 0s or whatever for the columns you don't want to use
  // float raise_sizes [4][3] = {{0.25, 0.35, 0.45,0.55,0.65,0.75,0.85,0.95,1.05,1.15,1.25,1.35,1.45,1.55},
  //                              {0.25,0.5,0.75,1,1.5,0,0,0,0,0,0,0,0,0},
  //                              {0.5,1,0,0,0,0,0,0,0,0,0,0,0,0},
  //                              {0.5,1,0,0,0,0,0,0,0,0,0,0,0,0}};
  // number of raise sizes for each round
  float num_raise_sizes [4] = {3,3,2,2};
  float raise_sizes [4][3] = {{.25,.5,1},
  {0.25,0.5,1},
  {.5,1,0},
  {.5,1,0}};
  // float num_raise_sizes [4] = {2,2,2,2};
  double starting_chips = 100;
  char num_players = 6;
  char num_rounds = 4;
  float small_blind_multiplier = 0.5;
  std::cout << GetNumPossible(raise_sizes, num_raise_sizes, starting_chips, 
                              small_blind_multiplier, num_players, num_rounds) 
                              << std::endl;
  return 0;
}