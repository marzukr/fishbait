// Copyright Emily Dale 2021

#include <iostream>
#include <chrono>  // NOLINT(*)

#include "sequence_table/SequenceTable.h"

int main() {
  std::vector<char> num_raise_sizes = {4, 2, 1, 1};
  std::vector<float> raise_sizes =
  {0.5,1,1.5,2,
  0.5, 1,
  1,
  1};
  int32_t starting_chips = 200;
  int32_t big_blind = 2;
  int32_t small_blind = 1;
  char num_players = 6;
  char num_rounds = 4;
  std::vector<float> late_raises = {0.5, 1};
  auto start = std::chrono::high_resolution_clock::now();
  blueprint_strategy::SequenceTable seq = blueprint_strategy::SequenceTable
                                         (raise_sizes, num_raise_sizes,
                                          starting_chips, small_blind,
                                          big_blind, num_players, num_rounds,
                                          true, 2, 1, 1, late_raises);
  auto stop = std::chrono::high_resolution_clock::now();
  auto d = std::chrono::duration_cast<std::chrono::seconds>(stop - start);
  std::cout << d.count() << std::endl;
  std::cout << seq;
  return 0;
}
