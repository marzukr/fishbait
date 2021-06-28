// Copyright 2021 Marzuk Rashid

#include <vector>

#include "catch2/catch.hpp"
#include "blueprint/sequence_table.h"
#include "poker_engine/node.h"

TEST_CASE("6 players all in or fold", "[poker_engine][node]") {
  std::vector<blueprint::Action> actions = {{poker_engine::Action::kFold},
                                            {poker_engine::Action::kAllIn}};
  poker_engine::Node<6> start_state;
  blueprint::SequenceTable<6> seq{actions, start_state};

  std::vector<uint32_t> correct = {1, 31, 2, 16, 3, 9, 4, 6, 0, 5, 0, 0, 7, 8,
                                   0, 0, 0, 0, 10, 13, 11, 12, 0, 0, 0, 0, 14,
                                   15, 0, 0, 0, 0, 17, 24, 18, 21, 19, 20, 0, 0,
                                   0, 0, 22, 23, 0, 0, 0, 0, 25, 28, 26, 27, 0,
                                   0, 0, 0, 29, 30, 0, 0, 0, 0, 32, 47, 33, 40,
                                   34, 37, 35, 36, 0, 0, 0, 0, 38, 39, 0, 0, 0,
                                   0, 41, 44, 42, 43, 0, 0, 0, 0, 45, 46, 0, 0,
                                   0, 0, 48, 55, 49, 52, 50, 51, 0, 0, 0, 0, 53,
                                   54, 0, 0, 0, 0, 56, 59, 57, 58, 0, 0, 0, 0,
                                   60, 61, 0, 0, 0, 0};
  REQUIRE(seq.States() == 62);
  REQUIRE(seq.Actions() == 2);
  for (size_t i = 0; i < seq.States(); ++i) {
    for (size_t j = 0; j < seq.Actions(); ++j) {
      REQUIRE(seq.Next(i, j) == correct[i * seq.Actions() + j]);
    }
  }
}  // TEST_CASE "6 players all in or fold"
