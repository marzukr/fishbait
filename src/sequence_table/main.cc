#include "sequence_table/SequenceTable.h"
#include <iostream>
#include <chrono> 
using namespace std::chrono;
int main(){
  char num_raise_sizes [4] = {3,2,1,1};
  float raise_sizes [4][3] = {{0.5,1,1.5},
  {0.5,1},
  {1},
  {1}};
  // float num_raise_sizes [4] = {2,2,2,2};
  long starting_chips = 200;
  long big_blind = 2;
  long small_blind = 1;
  char num_players = 5;
  char num_rounds = 4;
  float small_blind_multiplier = 0.5;
  unsigned int num_rows [4] = {4,1,1,1};
  auto start = high_resolution_clock::now(); 
  blueprint_strategy::SequenceTable seq = blueprint_strategy::SequenceTable(raise_sizes, num_raise_sizes, starting_chips, 
                              small_blind, big_blind, num_players, num_rounds, false);
  auto stop = high_resolution_clock::now();
  auto duration = duration_cast<microseconds>(stop - start);
  std::cout << duration.count() << std::endl;
  seq.PrintTable(false, num_rounds);
  return 0;
}
