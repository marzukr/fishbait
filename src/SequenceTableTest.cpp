#include "SequenceTable.h"
#include <iostream>

int main(){
  int num_raise_sizes [4] = {1,2,2,2};
  float raise_sizes [4][1] = {{1},
  {0.5},
  {0.5},
  {0.5}};
  // float num_raise_sizes [4] = {2,2,2,2};
  double starting_chips = 2;
  char num_players = 2;
  char num_rounds = 1;
  float small_blind_multiplier = 0.5;
  // unsigned int num_rows [4] = {4,1,1,1};
  unsigned int num_rows [4] = {4,1,1,1};
  unsigned int total_num_rows =  7;
  SequenceTable seq = SequenceTable(raise_sizes, num_raise_sizes, starting_chips, 
                              small_blind_multiplier, num_players, num_rounds);
  for(int i = 0; i < total_num_rows; i++){
    std::cout << "" << std::endl;
    for(int j = 0; j < NUM_RAISES+3; j++){
      std::cout << seq.table_[i][j] << ' ';
    }
  }
  return 0;
}