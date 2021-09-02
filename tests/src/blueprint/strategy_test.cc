// Copyright 2021 Marzuk Rashid

#include <string>

#include "catch2/catch.hpp"
#include "blueprint/strategy.h"
#include "clustering/definitions.h"
#include "poker/definitions.h"
#include "poker/indexer.h"
#include "poker/node.h"

/* Dummy clustering scheme that just partitions each round into 4 clusters with
   modulus of the ISO card index. */
class TestClusters {
 private:
  fishbait::Indexer<2> preflop_indexer_;
  fishbait::Indexer<2, 3> flop_indexer_;
  fishbait::Indexer<2, 4> turn_indexer_;
  fishbait::Indexer<2, 5> river_indexer_;
  static constexpr fishbait::CardCluster kNClusters = 4;

 public:
  TestClusters() = default;
  TestClusters(const TestClusters&) {}
  TestClusters& operator=(const TestClusters&) { return *this; }

  static constexpr fishbait::CardCluster NumClusters(fishbait::Round) {
    return kNClusters;
  }

  template <fishbait::PlayerN kPlayers>
  std::array<fishbait::CardCluster, kPlayers> ClusterArray(
      const fishbait::Node<kPlayers>& node) {
    std::array<fishbait::CardCluster, kPlayers> card_clusters;
    for (fishbait::PlayerId i = 0; i < kPlayers; ++i) {
      if (!node.folded(i) && node.stack(i) != 0) {
        card_clusters[i] = Cluster(node, i);
      }
    }
    return card_clusters;
  }  // ClusterArray()

  template <fishbait::PlayerN kPlayers>
  fishbait::CardCluster Cluster(const fishbait::Node<kPlayers>& node,
                                fishbait::PlayerId player) {
    std::array player_cards = node.PlayerCards(player);
    hand_index_t idx;
    switch (node.round()) {
      case fishbait::Round::kPreFlop:
        idx = preflop_indexer_.IndexLast(player_cards);
        break;
      case fishbait::Round::kFlop:
        idx = flop_indexer_.IndexLast(player_cards);
        break;
      case fishbait::Round::kTurn:
        idx = turn_indexer_.IndexLast(player_cards);
        break;
      case fishbait::Round::kRiver:
        idx = river_indexer_.IndexLast(player_cards);
        break;
      default:
        std::stringstream ss;
        ss << +(+node.round()) << " is not a valid round." << std::endl;
        throw std::out_of_range(ss.str());
    }
    return idx % kNClusters;
  }
};  // class TestClusters

TEST_CASE("mccfr 6 players all in or fold", "[blueprint][strategy]") {
  fishbait::Node<3> start_state;
  start_state.SetSeed(fishbait::Random::Seed(7));
  std::array<fishbait::AbstractAction, 13> actions = {{
      {fishbait::Action::kFold, 0, 0, fishbait::Round::kPreFlop},
      {fishbait::Action::kFold, 0, 0, fishbait::Round::kFlop},
      {fishbait::Action::kFold, 0, 0, fishbait::Round::kTurn},
      {fishbait::Action::kFold, 0, 0, fishbait::Round::kRiver},
      {fishbait::Action::kCheckCall, 0, 0, fishbait::Round::kPreFlop},
      {fishbait::Action::kCheckCall, 0, 0, fishbait::Round::kFlop},
      {fishbait::Action::kCheckCall, 0, 0, fishbait::Round::kTurn},
      {fishbait::Action::kCheckCall, 0, 0, fishbait::Round::kRiver},
      {fishbait::Action::kAllIn, 0, 0, fishbait::Round::kPreFlop},
      {fishbait::Action::kAllIn, 0, 0, fishbait::Round::kFlop},
      {fishbait::Action::kAllIn, 0, 0, fishbait::Round::kTurn},
      {fishbait::Action::kAllIn, 0, 0, fishbait::Round::kRiver},
      {fishbait::Action::kBet, 1.0, 1, fishbait::Round::kPreFlop}
  }};
  TestClusters info_abstraction;
  int iterations = 6;
  int strategy_interval = 2;
  int prune_threshold = 1;
  double prune_probability = 0.5;
  int prune_constant = 0;
  int LCFR_threshold = 3;
  int discount_interval = 2;
  int regret_floor = -15000;
  int snapshot_interval = 2;
  int strategy_delay = 3;
  std::string_view save_dir = "../tests/blueprint";
  bool verbose = false;
  fishbait::Strategy s(start_state, actions, info_abstraction, iterations,
                       strategy_interval, prune_threshold, prune_probability,
                       prune_constant, LCFR_threshold, discount_interval,
                       regret_floor, snapshot_interval, strategy_delay,
                       save_dir, fishbait::Random::Seed{68}, verbose);

  start_state.SetSeed(fishbait::Random::Seed{});
}  // TEST_CASE "mccfr 6 players all in or fold"
