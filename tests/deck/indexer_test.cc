// Copyright 2021 Marzuk Rashid

#include <array>
#include <cstdint>

#include "catch2/catch.hpp"

#include "deck/indexer.h"
#include "deck/constants.h"
#include "hand_strengths/ochs.h"

TEST_CASE("Basic flop indexer tests", "[deck][indexer]") {
  const uint32_t n_flops = 1286792;
  deck::Indexer flop(2, {2, 3});
  std::array<uint8_t, 5> rollout;
  std::array<uint64_t, 2> indicies;

  uint32_t i = 0;
  for (uint32_t idx = 0; idx < 1500000; ++idx) {
    flop.unindex(1, idx, &rollout);
    uint32_t array_index_last = flop.index(rollout);
    uint32_t array_index = flop.index(rollout, &indicies);
    uint32_t init_list = flop.index({rollout[0], rollout[1], rollout[2],
                                     rollout[3], rollout[4]});
    REQUIRE(array_index_last == array_index);
    REQUIRE(array_index == init_list);
    REQUIRE(init_list == indicies[1]);
    if (array_index_last != idx || array_index != idx || init_list != idx) {
      break;
    }
    REQUIRE(indicies[0] >= 0);
    REQUIRE(indicies[0] < deck::kUniqueHands);
    REQUIRE(indicies[1] >= 0);
    REQUIRE(indicies[1] < n_flops);
    ++i;
  }
  REQUIRE(i == n_flops);  // number of flops
}  // TEST_CASE "Basic flop indexer tests"
