#include "SequenceTable.h"
#include <iostream>

int main(){
  int num_raise_sizes [4] = {14,1,1,1};
  float raise_sizes [4][14] = {{0.1,0.2,0.3,0.4,0.5,0.6,0.7,0.8,0.9,1,1.5,2,2.5},
  {1},
  {1},
  {1}};
  // float num_raise_sizes [4] = {2,2,2,2};
  double starting_chips = 100;
  char num_players = 6;
  char num_rounds = 4;
  float small_blind_multiplier = 0.5;
  // unsigned int num_rows [4] = {4,1,1,1};
  SequenceTable seq = SequenceTable(raise_sizes, num_raise_sizes, starting_chips, 
                              small_blind_multiplier, num_players, num_rounds);
  seq.PrintTable(false);
  return 0;
}