#include "SequenceTable.h"
#include <iostream>

int main(){
  int num_raise_sizes [4] = {1,1,1,1};
  float raise_sizes [4][1] = {{1},
  {1},
  {1},
  {1}};
  // float num_raise_sizes [4] = {2,2,2,2};
  double starting_chips = 2;
  char num_players = 2;
  char num_rounds = 4;
  float small_blind_multiplier = 0.5;
  // unsigned int num_rows [4] = {4,1,1,1};
  SequenceTable seq = SequenceTable(raise_sizes, num_raise_sizes, starting_chips, 
                              small_blind_multiplier, num_players, num_rounds);
  unsigned int total_num_rows = seq.getTotalRows();

  // for(int i = 0; i < total_num_rows; i++){
  //   std::cout << "" << std::endl;
  //   for(int j = 0; j < num_raise_sizes[i]+3; j++){
  //     std::cout << seq.table_[i][j] << ' ';
  //   }
  // }
  return 0;
}