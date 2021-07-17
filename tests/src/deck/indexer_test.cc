// Copyright 2021 Marzuk Rashid

#include <array>
#include <cstdint>

#include "catch2/catch.hpp"
#include "deck/definitions.h"
#include "deck/indexer.h"
#include "hand_strengths/ochs.h"

TEST_CASE("Basic flop indexer tests", "[deck][indexer]") {
  const hand_index_t n_flops = 1286792;
  deck::Indexer flop(2, {2, 3});
  std::array<deck::ISO_Card, 5> rollout;
  std::array<hand_index_t, 2> indicies;

  hand_index_t i = 0;
  for (hand_index_t idx = 0; idx < 1500000; ++idx) {
    flop.unindex(1, idx, &rollout);
    hand_index_t array_index_last = flop.index(rollout);
    hand_index_t array_index = flop.index(rollout, &indicies);
    hand_index_t init_list = flop.index({rollout[0], rollout[1], rollout[2],
                                         rollout[3], rollout[4]});
    REQUIRE(array_index_last == array_index);
    REQUIRE(array_index == init_list);
    REQUIRE(init_list == indicies[1]);
    if (array_index_last != idx || array_index != idx || init_list != idx) {
      break;
    }
    REQUIRE(indicies[0] < deck::kUniqueHands);
    REQUIRE(indicies[1] < n_flops);
    ++i;
  }
  REQUIRE(i == n_flops);  // number of flops
}  // TEST_CASE "Basic flop indexer tests"
