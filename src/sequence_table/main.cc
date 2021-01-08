#include "sequence_table/SequenceTable.h"
#include <iostream>
#include <chrono> 
using namespace std::chrono;
int main(){
  std::vector<char> num_raise_sizes = {6,3,2,2};
  std::vector<float> raise_sizes = {0.25,0.5,0.75,1,1.25,1.5,
  0.25,0.5,1,
  0.5,1,
  0.5,1};
  // float num_raise_sizes [4] = {2,2,2,2};
  // std::vector<float> raise_sizes = {0.5,1,1.5};
  long starting_chips = 200;
  long big_blind = 2;
  long small_blind = 1;
  char num_players = 6;
  char num_rounds = 4;
  unsigned int num_rows [4] = {4,1,1,1};
  auto start = high_resolution_clock::now(); 
  blueprint_strategy::SequenceTable seq = blueprint_strategy::SequenceTable(raise_sizes, num_raise_sizes, starting_chips, 
                              small_blind, big_blind, num_players, num_rounds, true);
  auto stop = high_resolution_clock::now();
  auto duration = duration_cast<microseconds>(stop - start);
  std::cout << duration.count() << std::endl;
  seq.PrintTable(false, num_rounds);
  return 0;
}
