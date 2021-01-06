#include "sequence_table/SequenceTable.h"
#include <iostream>

int main(){
  char num_raise_sizes [4] = {2,1,1,1};
  float raise_sizes [4][2] = {{0.5,1},
  {1},
  {1},
  {1}};
  // float num_raise_sizes [4] = {2,2,2,2};
  double starting_chips = 4;
  char num_players = 2;
  char num_rounds = 1;
  float small_blind_multiplier = 0.5;
  // unsigned int num_rows [4] = {4,1,1,1};
  blueprint_strategy::SequenceTable seq = blueprint_strategy::SequenceTable(raise_sizes, num_raise_sizes, starting_chips, 
                              small_blind_multiplier, num_players, num_rounds);
  std::cout << "here3" << std::endl;
  // seq.PrintTable(false, num_rounds);
  return 0;
}
