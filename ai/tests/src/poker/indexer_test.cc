#include <array>
#include <cstdint>
#include <set>

#include "catch2/catch.hpp"
#include "hand_strengths/ochs.h"
#include "poker/definitions.h"
#include "poker/indexer.h"

TEST_CASE("Basic preflop indexer tests", "[poker][indexer]") {
  const hand_index_t n_hands =
      fishbait::ImperfectRecallHands(fishbait::Round::kPreFlop);
  fishbait::Indexer<2> preflop;
  std::array<fishbait::ISO_Card, 2> rollout;

  hand_index_t i = 0;
  std::set<hand_index_t> indicies_set;
  for (hand_index_t idx = 0; idx < n_hands; ++idx) {
    rollout = preflop.Unindex<0>(idx);
    bool invalid_cards = false;
    for (fishbait::Card c : rollout) {
      if (c >= fishbait::kDeckSize) {
        invalid_cards = true;
        break;
      }
    }
    if (invalid_cards) break;
    INFO(idx);
    hand_index_t array_index_last = preflop.IndexLast(rollout);
    if (array_index_last != idx) {
      break;
    }
    indicies_set.insert(array_index_last);
    ++i;
  }
  REQUIRE(i == n_hands);  // number of flops
  REQUIRE(indicies_set.size() == n_hands);
}  // TEST_CASE "Basic preflop indexer tests"

TEST_CASE("Basic flop indexer tests", "[poker][indexer]") {
  const hand_index_t n_flops =
      fishbait::ImperfectRecallHands(fishbait::Round::kFlop);
  fishbait::Indexer<2, 3> flop;
  std::array<fishbait::ISO_Card, 5> rollout;
  std::array<hand_index_t, 2> indicies;
  std::array<hand_index_t, 2> indicies_2;

  hand_index_t i = 0;
  std::set<hand_index_t> indicies_set;
  for (hand_index_t idx = 0; idx < n_flops + 10; ++idx) {
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

TEST_CASE("Basic turn indexer tests", "[.][poker][indexer]") {
  const hand_index_t n_turns =
      fishbait::ImperfectRecallHands(fishbait::Round::kTurn);
  fishbait::Indexer<2, 4> turn;
  std::array<fishbait::ISO_Card, 6> rollout;

  hand_index_t i = 0;
  std::set<hand_index_t> indicies_set;
  for (hand_index_t idx = 0; idx < n_turns + 10; ++idx) {
    rollout = turn.Unindex<1>(idx);
    bool invalid_cards = false;
    for (fishbait::Card c : rollout) {
      if (c >= fishbait::kDeckSize) {
        invalid_cards = true;
        break;
      }
    }
    if (invalid_cards) break;
    hand_index_t array_index_last = turn.IndexLast(rollout);
    if (array_index_last != idx) {
      break;
    }
    indicies_set.insert(array_index_last);
    ++i;
  }
  REQUIRE(i == n_turns);  // number of flops
  REQUIRE(indicies_set.size() == n_turns);
}  // TEST_CASE "Basic turn indexer tests"

TEST_CASE("Basic river indexer tests", "[.][poker][indexer]") {
  const hand_index_t n_rivers =
      fishbait::ImperfectRecallHands(fishbait::Round::kRiver);
  fishbait::Indexer<2, 5> river;
  std::array<fishbait::ISO_Card, 7> rollout;

  hand_index_t i = 0;
  std::set<hand_index_t> indicies_set;
  for (hand_index_t idx = 0; idx < n_rivers + 10; ++idx) {
    rollout = river.Unindex<1>(idx);
    bool invalid_cards = false;
    for (fishbait::Card c : rollout) {
      if (c >= fishbait::kDeckSize) {
        invalid_cards = true;
        break;
      }
    }
    if (invalid_cards) break;
    hand_index_t array_index_last = river.IndexLast(rollout);
    if (array_index_last != idx) {
      break;
    }
    indicies_set.insert(array_index_last);
    ++i;
  }
  REQUIRE(i == n_rivers);  // number of flops
  REQUIRE(indicies_set.size() == n_rivers);
}  // TEST_CASE "Basic river indexer tests"
