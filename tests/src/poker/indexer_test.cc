// Copyright 2021 Marzuk Rashid

#include <array>
#include <cstdint>
#include <set>

#include "catch2/catch.hpp"
#include "hand_strengths/ochs.h"
#include "poker/definitions.h"
#include "poker/indexer.h"

TEST_CASE("Basic flop indexer tests", "[poker][indexer]") {
  const hand_index_t n_flops =
      fishbait::ImperfectRecallHands(fishbait::Round::kFlop);
  fishbait::Indexer<2, 3> flop;
  std::array<fishbait::ISO_Card, 5> rollout;
  std::array<hand_index_t, 2> indicies;
  std::array<hand_index_t, 2> indicies_2;

  hand_index_t i = 0;
  std::set<hand_index_t> indicies_set;
  for (hand_index_t idx = 0; idx < 1500000; ++idx) {
    REQUIRE(flop.Unindex<0>(idx).size() == 2);
    rollout = flop.Unindex<1>(idx);
    bool invalid_cards = false;
    for (fishbait::Card c : rollout) {
      if (c >= fishbait::kDeckSize) {
        invalid_cards = true;
        break;
      }
    }
    if (invalid_cards) break;
    hand_index_t array_index_last = flop.IndexLast(rollout);
    indicies = flop.Index(rollout);
    indicies_2 = flop.Index({rollout[0], rollout[1], rollout[2], rollout[3],
                             rollout[4]});
    hand_index_t init_list = flop.IndexLast({rollout[0], rollout[1], rollout[2],
                                             rollout[3], rollout[4]});
    REQUIRE(array_index_last == indicies_2[1]);
    REQUIRE(indicies_2[1] == init_list);
    REQUIRE(init_list == indicies[1]);
    if (array_index_last != idx || indicies_2[1] != idx || init_list != idx) {
      break;
    }
    REQUIRE(indicies[0] < fishbait::kUniqueHands);
    REQUIRE(indicies[1] < n_flops);
    indicies_set.insert(array_index_last);
    ++i;
  }
  REQUIRE(i == n_flops);  // number of flops
  REQUIRE(indicies_set.size() == n_flops);
}  // TEST_CASE "Basic flop indexer tests"
