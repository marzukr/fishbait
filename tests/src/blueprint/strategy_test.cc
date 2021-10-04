// Copyright 2021 Marzuk Rashid

#include <algorithm>
#include <array>
#include <cmath>
#include <iostream>
#include <stack>
#include <string>
#include <vector>

#include "catch2/catch.hpp"
#include "blueprint/sequence_table.h"
#include "blueprint/strategy.h"
#include "clustering/definitions.h"
#include "poker/definitions.h"
#include "poker/indexer.h"
#include "poker/node.h"
#include "utils/print.h"

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

  /* @brief TestClusters serialize function */
  template<class Archive>
  void serialize(Archive&) {
    return;
  }

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

TEST_CASE("mccfr test", "[blueprint][strategy]") {
  constexpr int kPlayers = 3;
  constexpr int kActions = 5;

  fishbait::Node<kPlayers> start_state;
  start_state.SetSeed(fishbait::Random::Seed(7));
  std::array<fishbait::AbstractAction, kActions> actions = {{
      {fishbait::Action::kFold},
      {fishbait::Action::kAllIn},
      {fishbait::Action::kCheckCall},

      {fishbait::Action::kBet, 2.0, 1, fishbait::Round::kTurn,
       fishbait::Round::kTurn, 2, 0},
      {fishbait::Action::kBet, 0.25, 1, fishbait::Round::kFlop,
       fishbait::Round::kRiver, 0, 10000}
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
  fishbait::Strategy s(start_state, actions, info_abstraction,
                       strategy_interval, prune_threshold, prune_probability,
                       prune_constant, LCFR_threshold, discount_interval,
                       regret_floor, snapshot_interval, strategy_delay,
                       save_dir, fishbait::Random::Seed{68}, verbose);
  s.MCCFR(iterations);

  auto strategy_4 =
      fishbait::Strategy<kPlayers, kActions, TestClusters>::LoadSnapshot(
        "out/tests/blueprint/strategy_4.cereal");
  std::size_t legal_actions;

  std::vector<fishbait::Regret> it_4_regrets_preflop = {5017, -4983, -33, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, -4983, 17, 4967, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -10050, 10050, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 2458, -7492, 5033, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      4950, -4950, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 4166, 4192, 2902, 0, 0, 0, -2525, 2525, 0, 0, 0, 0, -2538, 2538, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  legal_actions = strategy_4.action_abstraction()
                            .NumLegalActions(fishbait::Round::kPreFlop);
  for (std::size_t i = 0; i < it_4_regrets_preflop.size(); ++i) {
    fishbait::CardCluster cluster = i / legal_actions;
    std::size_t offset = i % legal_actions;
    REQUIRE(strategy_4.regrets()[+fishbait::Round::kPreFlop](cluster, offset) ==
            it_4_regrets_preflop[i]);
  }

  std::vector<fishbait::Regret> it_4_regrets_flop = {0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, -1186, 1186, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, -2425, 2425, 0, 0, 0, 0, -5050, 5050, 0, 0,
      4950, -4950, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2475, -2475, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  legal_actions = strategy_4.action_abstraction()
                            .NumLegalActions(fishbait::Round::kFlop);
  for (std::size_t i = 0; i < it_4_regrets_flop.size(); ++i) {
    fishbait::CardCluster cluster = i / legal_actions;
    std::size_t offset = i % legal_actions;
    REQUIRE(strategy_4.regrets()[+fishbait::Round::kFlop](cluster, offset) ==
            it_4_regrets_flop[i]);
  }

  std::vector<fishbait::Regret> it_4_regrets_turn = {0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, -2496, 2579, -84, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0};
  legal_actions = strategy_4.action_abstraction()
                            .NumLegalActions(fishbait::Round::kTurn);
  for (std::size_t i = 0; i < it_4_regrets_turn.size(); ++i) {
    fishbait::CardCluster cluster = i / legal_actions;
    std::size_t offset = i % legal_actions;
    REQUIRE(strategy_4.regrets()[+fishbait::Round::kTurn](cluster, offset) ==
            it_4_regrets_turn[i]);
  }

  std::vector<fishbait::Regret> it_4_regrets_river = {0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, -2662, 2662, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0};
  legal_actions = strategy_4.action_abstraction()
                            .NumLegalActions(fishbait::Round::kRiver);
  for (std::size_t i = 0; i < it_4_regrets_river.size(); ++i) {
    fishbait::CardCluster cluster = i / legal_actions;
    std::size_t offset = i % legal_actions;
    REQUIRE(strategy_4.regrets()[+fishbait::Round::kRiver](cluster, offset) ==
            it_4_regrets_river[i]);
  }

  std::vector<fishbait::ActionCount> it_4_action_counts = {0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0,
      0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 1,
      0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0};
  legal_actions = strategy_4.action_abstraction()
                            .NumLegalActions(fishbait::Round::kPreFlop);
  for (std::size_t i = 0; i < it_4_action_counts.size(); ++i) {
    fishbait::CardCluster cluster = i / legal_actions;
    std::size_t offset = i % legal_actions;
    REQUIRE(strategy_4.action_counts()(cluster, offset) ==
            it_4_action_counts[i]);
  }

  start_state.SetSeed(fishbait::Random::Seed{});
}  // TEST_CASE "mccfr test"

TEST_CASE("mccfr test helper", "[blueprint][strategy]") {
  Catch::StringMaker<double>::precision = 19;
  constexpr int kNumActions = 5;
  constexpr int kPlayers = 3;
  constexpr int kRegretFloor = -15000;
  constexpr int kDiscountInterval = 2;

  TestClusters info_abstraction;

  fishbait::Node<kPlayers> start_state;
  start_state.SetSeed(fishbait::Random::Seed(7));

  std::array<fishbait::AbstractAction, kNumActions> actions = {{
      {fishbait::Action::kFold},
      {fishbait::Action::kAllIn},
      {fishbait::Action::kCheckCall},

      {fishbait::Action::kBet, 1.0, 1, fishbait::Round::kTurn,
       fishbait::Round::kTurn, 2, 0},
      {fishbait::Action::kBet, 0.25, 1, fishbait::Round::kFlop,
       fishbait::Round::kRiver, 0, 10000}
  }};
  fishbait::SequenceTable seq(actions, start_state);

  fishbait::Random rng(fishbait::Random::Seed{68});
  std::uniform_real_distribution<double> sampler(0, 1);

  using DoubleArray = std::array<double, kNumActions>;
  struct TraverseCall {
    fishbait::Node<kPlayers> node;
    std::array<double, kNumActions> strategy;
    double value;
    std::array<double, kNumActions> action_values;

    TraverseCall(const fishbait::Node<kPlayers>& node_c,
                 std::array<double, kNumActions> strategy_c, double value_c,
                 std::array<double, kNumActions> action_values_c)
                 : node{node_c}, strategy{strategy_c}, value{value_c},
                   action_values{action_values_c} { }
  };
  std::stack<TraverseCall> call_stack;
  double tmp;
  double sampled;
  fishbait::CardCluster cluster;
  fishbait::Regret regret;
  fishbait::Chips chips;
  double discount;
  fishbait::ActionCount action_count;

  // Iteration 1, player 0, no pruning
  call_stack.emplace(start_state, DoubleArray{0}, 0.0, DoubleArray{0});
  // preflop chance node, depth 0
  call_stack.emplace(call_stack.top());
  call_stack.top().node.Deal();
  call_stack.top().node.ProceedPlay();
  // player 0, depth 1, preflop seq 0
  call_stack.top().strategy = DoubleArray{1.0/3, 1.0/3, 1.0/3};
  call_stack.top().value = 0.0;
  call_stack.top().action_values = DoubleArray{0};
  // player 0, depth 1, preflop seq 0, action 0
  call_stack.emplace(call_stack.top());
  call_stack.top().node.Apply(fishbait::Action::kFold);
  // player 1, depth 2, preflop seq 1, player 0 folded
  call_stack.emplace(call_stack.top());
  call_stack.top().node.Apply(fishbait::Action::kFold);
  // terminal node, depth 3, player 0 folded
  call_stack.top().node.AwardPot(start_state.same_stack_no_rake_);
  call_stack.top().value = call_stack.top().node.stack(0);
  // player 1, depth 2, preflop seq 1, player 0 folded
  tmp = call_stack.top().value;
  call_stack.pop();
  call_stack.top().value = tmp;
  // player 0, depth 1, preflop seq 0, action 0
  tmp = call_stack.top().value;
  call_stack.pop();
  call_stack.top().action_values[0] = tmp;
  call_stack.top().value += call_stack.top().action_values[0] *
                            call_stack.top().strategy[0];
  // player 0, depth 1, preflop seq 0, action 1
  call_stack.emplace(call_stack.top());
  call_stack.top().node.Apply(fishbait::Action::kAllIn);
  // player 1, depth 2, preflop seq 5
  call_stack.top().strategy = DoubleArray{0.5, 0.5, 0};
  sampled = sampler(rng());
  REQUIRE(sampled == 0.40450036205802381);
  call_stack.emplace(call_stack.top());
  call_stack.top().node.Apply(fishbait::Action::kFold);
  // player 2, depth 3, preflop seq 6
  call_stack.top().strategy = DoubleArray{0.5, 0.5, 0};
  sampled = sampler(rng());
  REQUIRE(sampled == 0.77064202074971078);
  call_stack.emplace(call_stack.top());
  call_stack.top().node.Apply(fishbait::Action::kAllIn);
  // terminal node, depth 4
  call_stack.top().node.Deal();  // flop
  call_stack.top().node.ProceedPlay();
  call_stack.top().node.Deal();  // turn
  call_stack.top().node.ProceedPlay();
  call_stack.top().node.Deal();  // river
  call_stack.top().node.ProceedPlay();
  call_stack.top().node.AwardPot(start_state.same_stack_no_rake_);
  call_stack.top().value = call_stack.top().node.stack(0);
  // player 2, depth 3, preflop seq 6
  tmp = call_stack.top().value;
  call_stack.pop();
  call_stack.top().value = tmp;
  // player 1, depth 2, preflop seq 5
  tmp = call_stack.top().value;
  call_stack.pop();
  call_stack.top().value = tmp;
  // player 0, depth 1, preflop seq 0, action 1
  tmp = call_stack.top().value;
  call_stack.pop();
  call_stack.top().action_values[1] = tmp;
  call_stack.top().value += call_stack.top().action_values[1] *
                            call_stack.top().strategy[1];
  // player 0, depth 1, preflop seq 0, action 2
  call_stack.emplace(call_stack.top());
  call_stack.top().node.Apply(fishbait::Action::kCheckCall);
  // player 1, depth 2, preflop seq 8
  call_stack.top().strategy = DoubleArray{1.0/3, 1.0/3, 1.0/3};
  sampled = sampler(rng());
  REQUIRE(sampled == 0.14729553008964991);
  call_stack.emplace(call_stack.top());
  call_stack.top().node.Apply(fishbait::Action::kFold);
  // player 2, depth 3, preflop seq 9
  call_stack.top().strategy = DoubleArray{0.0, 0.5, 0.5};
  sampled = sampler(rng());
  REQUIRE(sampled == 0.53596345244732502);
  call_stack.emplace(call_stack.top());
  call_stack.top().node.Apply(fishbait::Action::kCheckCall);
  call_stack.top().node.Deal();
  call_stack.top().node.ProceedPlay();
  // player 2, depth 4, flop seq 4
  call_stack.top().strategy = DoubleArray{0.0, 0.5, 0.5, 0.0};
  sampled = sampler(rng());
  REQUIRE(sampled == 0.06191769792017918);
  call_stack.emplace(call_stack.top());
  call_stack.top().node.Apply(fishbait::Action::kAllIn);
  // player 0, depth 5, flop seq 5
  call_stack.top().strategy = DoubleArray{0.5, 0.5, 0.0, 0.0};
  call_stack.top().value = 0.0;
  call_stack.top().action_values = DoubleArray{0};
  // player 0, depth 5, flop seq 5, action 0
  call_stack.emplace(call_stack.top());
  call_stack.top().node.Apply(fishbait::Action::kFold);
  // terminal node, depth 6, player 0 folded
  call_stack.top().node.AwardPot(start_state.same_stack_no_rake_);
  call_stack.top().value = call_stack.top().node.stack(0);
  // player 0, depth 5 flop seq 5, action 0
  tmp = call_stack.top().value;
  call_stack.pop();
  call_stack.top().action_values[0] = tmp;
  call_stack.top().value += call_stack.top().action_values[0] *
                            call_stack.top().strategy[0];
  // player 0, depth 5, flop seq 5, action 1
  call_stack.emplace(call_stack.top());
  call_stack.top().node.Apply(fishbait::Action::kAllIn);
  // terminal node, depth 6
  call_stack.top().node.Deal();  // turn
  call_stack.top().node.ProceedPlay();
  call_stack.top().node.Deal();  // river
  call_stack.top().node.ProceedPlay();
  call_stack.top().node.AwardPot(start_state.same_stack_no_rake_);
  call_stack.top().value = call_stack.top().node.stack(0);
  // player 0, depth 5, flop seq 5, action 1
  tmp = call_stack.top().value;
  call_stack.pop();
  call_stack.top().action_values[1] = tmp;
  call_stack.top().value += call_stack.top().action_values[1] *
                            call_stack.top().strategy[1];
  // player 0, depth 5, flop seq 5
  cluster = info_abstraction.Cluster(call_stack.top().node, 0);
  REQUIRE(cluster == 2);
  regret = std::rint(0 + call_stack.top().action_values[0] -
                     call_stack.top().value);
  regret = std::max(regret, kRegretFloor);
  REQUIRE(regret == 4950);
  regret = std::rint(0 + call_stack.top().action_values[1] -
                     call_stack.top().value);
  regret = std::max(regret, kRegretFloor);
  REQUIRE(regret == -4950);
  // player 2, depth 4, flop seq 4
  tmp = call_stack.top().value;
  call_stack.pop();
  call_stack.top().value = tmp;
  // player 2, depth 3, preflop seq 9
  tmp = call_stack.top().value;
  call_stack.pop();
  call_stack.top().value = tmp;
  // player 1, depth 2, preflop seq 8
  tmp = call_stack.top().value;
  call_stack.pop();
  call_stack.top().value = tmp;
  // player 0, depth 1, preflop seq 0, action 2
  tmp = call_stack.top().value;
  call_stack.pop();
  call_stack.top().action_values[2] = tmp;
  call_stack.top().value += call_stack.top().action_values[2] *
                            call_stack.top().strategy[2];
  // player 0, depth 1, preflop seq 0
  cluster = info_abstraction.Cluster(call_stack.top().node, 0);
  REQUIRE(cluster == 3);
  regret = std::rint(0 + call_stack.top().action_values[0] -
                     call_stack.top().value);
  regret = std::max(regret, kRegretFloor);
  REQUIRE(regret == -1667);
  regret = std::rint(0 + call_stack.top().action_values[1] -
                     call_stack.top().value);
  regret = std::max(regret, kRegretFloor);
  REQUIRE(regret == 8383);
  regret = std::rint(0 + call_stack.top().action_values[2] -
                     call_stack.top().value);
  regret = std::max(regret, kRegretFloor);
  REQUIRE(regret == -6717);
  // preflop chance node, depth 0
  call_stack.pop();
  // Iteration 1, player 0, no pruning
  call_stack.pop();
  REQUIRE(call_stack.size() == 0);

  // Iteration 1, player 1, no pruning
  call_stack.emplace(start_state, DoubleArray{0}, 0.0, DoubleArray{0});
  // preflop chance node
  call_stack.emplace(call_stack.top());
  call_stack.top().node.Deal();
  call_stack.top().node.ProceedPlay();
  // player 0, depth 1, preflop seq 0
  call_stack.emplace(call_stack.top());
  cluster = info_abstraction.Cluster(call_stack.top().node, 0);
  REQUIRE(cluster == 2);
  call_stack.top().strategy = DoubleArray{1.0/3.0, 1.0/3.0, 1.0/3.0};
  sampled = sampler(rng());
  REQUIRE(sampled == 0.36901124177882166);
  call_stack.emplace(call_stack.top());
  call_stack.top().node.Apply(fishbait::Action::kAllIn);
  // player 1, depth 2, preflop seq 5
  call_stack.top().strategy = DoubleArray{0.5, 0.5};
  call_stack.top().value = 0.0;
  call_stack.top().action_values = DoubleArray{0.0, 0.0};
  // player 1, depth 2, preflop seq 5, action 0
  call_stack.emplace(call_stack.top());
  call_stack.top().node.Apply(fishbait::Action::kFold);
  // player 2, depth 3, preflop seq 6, player 1 folded
  call_stack.emplace(call_stack.top());
  call_stack.top().node.Apply(fishbait::Action::kFold);
  // terminal node
  call_stack.top().node.AwardPot(start_state.same_stack_no_rake_);
  call_stack.top().value = call_stack.top().node.stack(1);
  // player 2, depth 3, preflop seq 6
  tmp = call_stack.top().value;
  call_stack.pop();
  call_stack.top().value = tmp;
  // player 1, depth 2, preflop seq 5, action 0
  tmp = call_stack.top().value;
  call_stack.pop();
  REQUIRE(tmp == 9950);
  call_stack.top().action_values[0] = tmp;
  call_stack.top().value += call_stack.top().action_values[0] *
                            call_stack.top().strategy[0];
  // player 1, depth 2, preflop seq 5, action 1
  call_stack.emplace(call_stack.top());
  call_stack.top().node.Apply(fishbait::Action::kAllIn);
  // player 2, depth 3, preflop seq 7
  call_stack.top().strategy = DoubleArray{0.5, 0.5};
  sampled = sampler(rng());
  REQUIRE(sampled == 0.32266640961339049);
  call_stack.emplace(call_stack.top());
  call_stack.top().node.Apply(fishbait::Action::kFold);
  // terminal node
  call_stack.top().node.Deal();  // flop
  call_stack.top().node.ProceedPlay();
  call_stack.top().node.Deal();  // turn
  call_stack.top().node.ProceedPlay();
  call_stack.top().node.Deal();  // river
  call_stack.top().node.ProceedPlay();
  call_stack.top().node.AwardPot(start_state.same_stack_no_rake_);
  call_stack.top().value = call_stack.top().node.stack(1);
  // player 2, depth 3, preflop seq 7
  tmp = call_stack.top().value;
  call_stack.pop();
  call_stack.top().value = tmp;
  // player 1, depth 2, preflop seq 5, action 1
  tmp = call_stack.top().value;
  REQUIRE(tmp == 20100);
  call_stack.pop();
  call_stack.top().action_values[1] = tmp;
  call_stack.top().value += call_stack.top().strategy[1] *
                            call_stack.top().action_values[1];
  // player 1, depth 2, preflop seq 5
  cluster = info_abstraction.Cluster(call_stack.top().node, 1);
  REQUIRE(cluster == 3);
  regret = std::rint(0 + call_stack.top().action_values[0] -
                     call_stack.top().value);
  regret = std::max(regret, kRegretFloor);
  REQUIRE(regret == -5075);
  regret = std::rint(0 + call_stack.top().action_values[1] -
                     call_stack.top().value);
  regret = std::max(regret, kRegretFloor);
  REQUIRE(regret == 5075);
  // player 0, depth 1, preflop seq 0
  tmp = call_stack.top().value;
  call_stack.pop();
  call_stack.top().value = tmp;
  // preflop chance node
  tmp = call_stack.top().value;
  call_stack.pop();
  call_stack.top().value = tmp;
  // Iteration 1, player 1, no pruning
  tmp = call_stack.top().value;
  call_stack.pop();
  call_stack.top().value = tmp;
  call_stack.pop();
  REQUIRE(call_stack.size() == 0);

  // Iteration 1, player 2, no pruning
  call_stack.emplace(start_state, DoubleArray{0}, 0.0, DoubleArray{0});
  // preflop chance node
  call_stack.emplace(call_stack.top());
  call_stack.top().node.Deal();
  call_stack.top().node.ProceedPlay();
  call_stack.emplace(call_stack.top());
  // player 0, depth 1, preflop seq 0
  cluster = info_abstraction.Cluster(call_stack.top().node, 0);
  REQUIRE(cluster == 2);
  call_stack.top().strategy = DoubleArray{1.0/3.0, 1.0/3.0, 1.0/3.0};
  sampled = sampler(rng());
  REQUIRE(sampled == 0.30912128535444794);
  call_stack.emplace(call_stack.top());
  call_stack.top().node.Apply(fishbait::Action::kFold);
  // player 1, depth 2, preflop seq 1
  cluster = info_abstraction.Cluster(call_stack.top().node, 1);
  REQUIRE(cluster == 2);
  call_stack.top().strategy = DoubleArray{1.0/3.0, 1.0/3.0, 1.0/3.0};
  sampled = sampler(rng());
  REQUIRE(sampled == 0.54279205661735042);
  call_stack.emplace(call_stack.top());
  call_stack.top().node.Apply(fishbait::Action::kAllIn);
  // player 2, depth 3, preflop seq 2
  cluster = info_abstraction.Cluster(call_stack.top().node, 2);
  REQUIRE(cluster == 3);
  call_stack.top().strategy = DoubleArray{0.5, 0.5};
  call_stack.top().value = 0;
  call_stack.top().action_values = DoubleArray{0.0, 0.0};
  // player 2, depth 3, preflop seq 2, action 0
  call_stack.emplace(call_stack.top());
  call_stack.top().node.Apply(fishbait::Action::kFold);
  // terminal node
  call_stack.top().node.AwardPot(start_state.same_stack_no_rake_);
  call_stack.top().value = call_stack.top().node.stack(2);
  // player 2, depth 3, preflop seq 2, action 0
  tmp = call_stack.top().value;
  call_stack.pop();
  call_stack.top().action_values[0] = tmp;
  call_stack.top().value += call_stack.top().strategy[0] *
                            call_stack.top().action_values[0];
  // player 2, depth 3, preflop seq 2, action 1
  call_stack.emplace(call_stack.top());
  call_stack.top().node.Apply(fishbait::Action::kAllIn);
  // terminal node
  call_stack.top().node.Deal();  // flop
  call_stack.top().node.ProceedPlay();
  call_stack.top().node.Deal();  // turn
  call_stack.top().node.ProceedPlay();
  call_stack.top().node.Deal();  // river
  call_stack.top().node.ProceedPlay();
  call_stack.top().node.AwardPot(start_state.same_stack_no_rake_);
  call_stack.top().value = call_stack.top().node.stack(2);
  // player 2, depth 3, preflop seq 2, action 1
  tmp = call_stack.top().value;
  call_stack.pop();
  call_stack.top().action_values[1] = tmp;
  call_stack.top().value += call_stack.top().strategy[1] *
                            call_stack.top().action_values[1];
  // player 2, depth 3, preflop seq 2
  cluster = info_abstraction.Cluster(call_stack.top().node, 2);
  REQUIRE(cluster == 3);
  regret = std::rint(0 + call_stack.top().action_values[0] -
                     call_stack.top().value);
  regret = std::max(regret, kRegretFloor);
  REQUIRE(regret == -5050);
  regret = std::rint(0 + call_stack.top().action_values[1] -
                     call_stack.top().value);
  regret = std::max(regret, kRegretFloor);
  REQUIRE(regret == 5050);
  // player 1, depth 2, preflop seq 1
  tmp = call_stack.top().value;
  call_stack.pop();
  call_stack.top().value = tmp;
  // player 0, depth 1, preflop seq 0
  tmp = call_stack.top().value;
  call_stack.pop();
  call_stack.top().value = tmp;
  // preflop chance node
  tmp = call_stack.top().value;
  call_stack.pop();
  call_stack.top().value = tmp;
  // Iteration 1, player 2, no pruning
  tmp = call_stack.top().value;
  call_stack.pop();
  call_stack.top().value = tmp;
  call_stack.pop();
  REQUIRE(call_stack.size() == 0);

  // Iteration 2, player 0
  sampled = sampler(rng());
  REQUIRE(sampled == 0.67632256656254086);
  // Iteration 2, player 0, no pruning
  call_stack.emplace(start_state, DoubleArray{0}, 0.0, DoubleArray{0});
  // preflop chance node
  call_stack.emplace(call_stack.top());
  call_stack.top().node.Deal();
  call_stack.top().node.ProceedPlay();
  call_stack.emplace(call_stack.top());
  // player 0, depth 1, preflop seq 0
  cluster = info_abstraction.Cluster(call_stack.top().node, 0);
  REQUIRE(cluster == 3);
  call_stack.top().strategy = DoubleArray{0.0, 1.0, 0.0};
  call_stack.top().value = 0.0;
  call_stack.top().action_values = DoubleArray{0.0, 0.0, 0.0};
  // player 0, depth 1, preflop seq 0, action 0
  call_stack.emplace(call_stack.top());
  call_stack.top().node.Apply(fishbait::Action::kFold);
  // player 1, depth 2, preflop seq 1, player 0 folded
  call_stack.emplace(call_stack.top());
  call_stack.top().node.Apply(fishbait::Action::kFold);
  // terminal node
  call_stack.top().node.AwardPot(start_state.same_stack_no_rake_);
  call_stack.top().value = call_stack.top().node.stack(0);
  // player 1, depth 2, preflop seq 1, player 0 folded
  tmp = call_stack.top().value;
  call_stack.pop();
  call_stack.top().value = tmp;
  // player 0, depth 1, preflop seq 0, action 0
  tmp = call_stack.top().value;
  call_stack.pop();
  call_stack.top().action_values[0] = tmp;
  call_stack.top().value += call_stack.top().strategy[0] *
                            call_stack.top().action_values[0];
  // player 0, depth 1, preflop seq 0, action 1
  call_stack.emplace(call_stack.top());
  call_stack.top().node.Apply(fishbait::Action::kAllIn);
  // player 1, depth 2, preflop seq 5
  cluster = info_abstraction.Cluster(call_stack.top().node, 1);
  REQUIRE(cluster == 3);
  call_stack.top().strategy = DoubleArray{0.0, 1.0};
  sampled = sampler(rng());
  REQUIRE(sampled == 0.93839846454612186);
  call_stack.emplace(call_stack.top());
  call_stack.top().node.Apply(fishbait::Action::kAllIn);
  // player 2, depth 3, preflop seq 7
  cluster = info_abstraction.Cluster(call_stack.top().node, 2);
  REQUIRE(cluster == 3);
  call_stack.top().strategy = DoubleArray{0.5, 0.5};
  sampled = sampler(rng());
  REQUIRE(sampled == 0.38050718647426901);
  call_stack.emplace(call_stack.top());
  call_stack.top().node.Apply(fishbait::Action::kFold);
  // terminal node
  call_stack.top().node.Deal();  // flop
  call_stack.top().node.ProceedPlay();
  call_stack.top().node.Deal();  // turn
  call_stack.top().node.ProceedPlay();
  call_stack.top().node.Deal();  // river
  call_stack.top().node.ProceedPlay();
  call_stack.top().node.AwardPot(start_state.same_stack_no_rake_);
  call_stack.top().value = call_stack.top().node.stack(0);
  // player 2, depth 3, preflop seq 7
  tmp = call_stack.top().value;
  call_stack.pop();
  call_stack.top().value = tmp;
  // player 1, depth 2, preflop seq 5
  tmp = call_stack.top().value;
  call_stack.pop();
  call_stack.top().value = tmp;
  // player 0, depth 1, preflop seq 0, action 1
  tmp = call_stack.top().value;
  call_stack.pop();
  call_stack.top().action_values[1] = tmp;
  call_stack.top().value += call_stack.top().strategy[1] *
                            call_stack.top().action_values[1];
  // player 0, depth 1, preflop seq 0, action 2
  call_stack.emplace(call_stack.top());
  call_stack.top().node.Apply(fishbait::Action::kCheckCall);
  // player 1, depth 2, preflop seq 8
  cluster = info_abstraction.Cluster(call_stack.top().node, 1);
  REQUIRE(cluster == 3);
  call_stack.top().strategy = DoubleArray{1.0/3.0, 1.0/3.0, 1.0/3.0};
  sampled = sampler(rng());
  REQUIRE(sampled == 0.27515632123053618);
  call_stack.emplace(call_stack.top());
  call_stack.top().node.Apply(fishbait::Action::kFold);
  // player 2, depth 3, preflop seq 9
  cluster = info_abstraction.Cluster(call_stack.top().node, 2);
  REQUIRE(cluster == 3);
  call_stack.top().strategy = DoubleArray{0.5, 0.5};
  sampled = sampler(rng());
  REQUIRE(sampled == 0.76365894114973176);
  call_stack.emplace(call_stack.top());
  call_stack.top().node.Apply(fishbait::Action::kCheckCall);
  // flop chance node
  call_stack.top().node.Deal();
  call_stack.top().node.ProceedPlay();
  call_stack.emplace(call_stack.top());
  // player 2, depth 4, flop seq 4
  cluster = info_abstraction.Cluster(call_stack.top().node, 2);
  REQUIRE(cluster == 1);
  call_stack.top().strategy = DoubleArray{0.5, 0.5};
  sampled = sampler(rng());
  REQUIRE(sampled == 0.66186632693447933);
  call_stack.emplace(call_stack.top());
  call_stack.top().node.Apply(fishbait::Action::kCheckCall);
  // player 0, depth 5, flop seq 6
  cluster = info_abstraction.Cluster(call_stack.top().node, 0);
  REQUIRE(cluster == 0);
  call_stack.top().strategy = DoubleArray{0.5, 0.5};
  call_stack.top().value = 0.0;
  call_stack.top().action_values = DoubleArray{0.0, 0.0};
  // player 0, depth 5, flop seq 6, action 0
  call_stack.emplace(call_stack.top());
  call_stack.top().node.Apply(fishbait::Action::kAllIn);
  // player 2, depth 6, flop seq 7
  cluster = info_abstraction.Cluster(call_stack.top().node, 0);
  REQUIRE(cluster == 0);
  call_stack.top().strategy = DoubleArray{0.5, 0.5};
  sampled = sampler(rng());
  REQUIRE(sampled == 0.28942032669550527);
  call_stack.emplace(call_stack.top());
  call_stack.top().node.Apply(fishbait::Action::kFold);
  // terminal node
  call_stack.top().node.AwardPot(start_state.same_stack_no_rake_);
  call_stack.top().value = call_stack.top().node.stack(0);
  // player 2, depth 6, flop seq 7
  tmp = call_stack.top().value;
  call_stack.pop();
  call_stack.top().value = tmp;
  // player 0, depth 5, flop seq 6, action 0
  tmp = call_stack.top().value;
  REQUIRE(tmp == 10150);
  call_stack.pop();
  call_stack.top().action_values[0] = tmp;
  call_stack.top().value += call_stack.top().strategy[0] *
                            call_stack.top().action_values[0];
  // player 0, depth 5, flop seq 6, action 1
  call_stack.emplace(call_stack.top());
  call_stack.top().node.Apply(fishbait::Action::kCheckCall);
  // turn chance node
  call_stack.top().node.Deal();
  call_stack.top().node.ProceedPlay();
  call_stack.emplace(call_stack.top());
  // player 2, depth 6, turn seq 8
  cluster = info_abstraction.Cluster(call_stack.top().node, 2);
  REQUIRE(cluster == 2);
  call_stack.top().strategy = DoubleArray{1.0/3.0, 1.0/3.0, 1.0/3.0};
  sampled = sampler(rng());
  REQUIRE(sampled == 0.88673216355398687);
  call_stack.emplace(call_stack.top());
  chips = call_stack.top().node.ConvertBet(2.0);
  call_stack.top().node.Apply(fishbait::Action::kBet, chips);
  // player 0, depth 7, turn seq 14
  cluster = info_abstraction.Cluster(call_stack.top().node, 0);
  REQUIRE(cluster == 0);
  call_stack.top().strategy = DoubleArray{1.0/3.0, 1.0/3.0, 1.0/3.0};
  call_stack.top().value = 0.0;
  call_stack.top().action_values = DoubleArray{0.0, 0.0, 0.0};
  // player 0, depth 7, turn seq 14, action 0
  call_stack.emplace(call_stack.top());
  call_stack.top().node.Apply(fishbait::Action::kFold);
  // terminal node
  call_stack.top().node.AwardPot(start_state.same_stack_no_rake_);
  call_stack.top().value = call_stack.top().node.stack(0);
  // player 0, depth 7, turn seq 14, action 0
  tmp = call_stack.top().value;
  call_stack.pop();
  call_stack.top().action_values[0] = tmp;
  call_stack.top().value += call_stack.top().strategy[0] *
                            call_stack.top().action_values[0];
  // player 0, depth 7, turn seq 14, action 1
  call_stack.emplace(call_stack.top());
  call_stack.top().node.Apply(fishbait::Action::kAllIn);
  // player 2, depth 8, turn seq 15
  cluster = info_abstraction.Cluster(call_stack.top().node, 2);
  REQUIRE(cluster == 2);
  call_stack.top().strategy = DoubleArray{0.5, 0.5};
  sampled = sampler(rng());
  REQUIRE(sampled == 0.65468411696854878);
  call_stack.emplace(call_stack.top());
  call_stack.top().node.Apply(fishbait::Action::kAllIn);
  // terminal node
  call_stack.top().node.Deal();  // river
  call_stack.top().node.ProceedPlay();
  call_stack.top().node.AwardPot(start_state.same_stack_no_rake_);
  call_stack.top().value = call_stack.top().node.stack(0);
  // player 2, depth 8, turn seq 15
  tmp = call_stack.top().value;
  call_stack.pop();
  call_stack.top().value = tmp;
  // player 0, depth 7, turn seq 14, action 1
  tmp = call_stack.top().value;
  call_stack.pop();
  call_stack.top().action_values[1] = tmp;
  call_stack.top().value += call_stack.top().strategy[1] *
                            call_stack.top().action_values[1];
  // player 0, depth 7, turn seq 14, action 2
  call_stack.emplace(call_stack.top());
  call_stack.top().node.Apply(fishbait::Action::kCheckCall);
  // river chance node
  call_stack.top().node.Deal();
  call_stack.top().node.ProceedPlay();
  call_stack.emplace(call_stack.top());
  // player 2, depth 8, river seq 20
  cluster = info_abstraction.Cluster(call_stack.top().node, 2);
  REQUIRE(cluster == 0);
  call_stack.top().strategy = DoubleArray{0.5, 0.5};
  sampled = sampler(rng());
  REQUIRE(sampled == 0.19793249950019598);
  call_stack.emplace(call_stack.top());
  call_stack.top().node.Apply(fishbait::Action::kAllIn);
  // player 0, depth 9, river seq 21
  cluster = info_abstraction.Cluster(call_stack.top().node, 0);
  REQUIRE(cluster == 0);
  call_stack.top().strategy = DoubleArray{0.5, 0.5};
  call_stack.top().value = 0.0;
  call_stack.top().action_values = DoubleArray{0.0, 0.0};
  // player 0, depth 9, river seq 21, action 0
  call_stack.emplace(call_stack.top());
  call_stack.top().node.Apply(fishbait::Action::kFold);
  // terminal node
  call_stack.top().node.AwardPot(start_state.same_stack_no_rake_);
  call_stack.top().value = call_stack.top().node.stack(0);
  // player 0, depth 9, river seq 21, action 0
  tmp = call_stack.top().value;
  call_stack.pop();
  call_stack.top().action_values[0] = tmp;
  call_stack.top().value += call_stack.top().strategy[0] *
                            call_stack.top().action_values[0];
  // player 0, depth 9, river seq 21, action 1
  call_stack.emplace(call_stack.top());
  call_stack.top().node.Apply(fishbait::Action::kAllIn);
  // terminal node
  call_stack.top().node.AwardPot(start_state.same_stack_no_rake_);
  call_stack.top().value = call_stack.top().node.stack(0);
  // player 0, depth 9, river seq 21, action 1
  tmp = call_stack.top().value;
  call_stack.pop();
  call_stack.top().action_values[1] = tmp;
  call_stack.top().value += call_stack.top().strategy[1] *
                            call_stack.top().action_values[1];
  // player 0, depth 9, river seq 21
  cluster = info_abstraction.Cluster(call_stack.top().node, 0);
  REQUIRE(cluster == 0);
  regret = std::rint(0 + call_stack.top().action_values[0] -
                     call_stack.top().value);
  regret = std::max(regret, kRegretFloor);
  REQUIRE(regret == -5325);
  regret = std::rint(0 + call_stack.top().action_values[1] -
                     call_stack.top().value);
  regret = std::max(regret, kRegretFloor);
  REQUIRE(regret == 5325);
  // player 2, depth 8, river seq 20
  tmp = call_stack.top().value;
  call_stack.pop();
  call_stack.top().value = tmp;
  // river chance node
  tmp = call_stack.top().value;
  call_stack.pop();
  call_stack.top().value = tmp;
  // player 0, depth 7, turn seq 14, action 2
  tmp = call_stack.top().value;
  call_stack.pop();
  call_stack.top().action_values[2] = tmp;
  call_stack.top().value += call_stack.top().strategy[2] *
                            call_stack.top().action_values[2];
  // player 0, depth 7, turn seq 14
  cluster = info_abstraction.Cluster(call_stack.top().node, 0);
  REQUIRE(cluster == 0);
  regret = std::rint(0 + call_stack.top().action_values[0] -
                     call_stack.top().value);
  regret = std::max(regret, kRegretFloor);
  REQUIRE(regret == -4992);
  regret = std::rint(0 + call_stack.top().action_values[1] -
                     call_stack.top().value);
  regret = std::max(regret, kRegretFloor);
  REQUIRE(regret == 5158);
  regret = std::rint(0 + call_stack.top().action_values[2] -
                     call_stack.top().value);
  regret = std::max(regret, kRegretFloor);
  REQUIRE(regret == -167);
  // player 2, depth 6, turn seq 8
  tmp = call_stack.top().value;
  call_stack.pop();
  call_stack.top().value = tmp;
  // turn chance node
  tmp = call_stack.top().value;
  call_stack.pop();
  call_stack.top().value = tmp;
  // player 0, depth 5, flop seq 6, action 1
  tmp = call_stack.top().value;
  call_stack.pop();
  call_stack.top().action_values[1] = tmp;
  call_stack.top().value += call_stack.top().strategy[1] *
                            call_stack.top().action_values[1];
  // player 0, depth 5, flop seq 6
  cluster = info_abstraction.Cluster(call_stack.top().node, 0);
  REQUIRE(cluster == 0);
  regret = std::rint(0 + call_stack.top().action_values[0] -
                     call_stack.top().value);
  regret = std::max(regret, kRegretFloor);
  REQUIRE(regret == -2371);
  regret = std::rint(0 + call_stack.top().action_values[1] -
                     call_stack.top().value);
  regret = std::max(regret, kRegretFloor);
  REQUIRE(regret == 2371);
  // player 2, depth 4, flop seq 4
  tmp = call_stack.top().value;
  call_stack.pop();
  call_stack.top().value = tmp;
  // flop chance node
  tmp = call_stack.top().value;
  call_stack.pop();
  call_stack.top().value = tmp;
  // player 2, depth 3, preflop seq 9
  tmp = call_stack.top().value;
  call_stack.pop();
  call_stack.top().value = tmp;
  // player 1, depth 2, preflop seq 8
  tmp = call_stack.top().value;
  call_stack.pop();
  call_stack.top().value = tmp;
  // player 0, depth 1, preflop seq 0, action 2
  tmp = call_stack.top().value;
  call_stack.pop();
  call_stack.top().action_values[2] = tmp;
  call_stack.top().value += call_stack.top().strategy[2] *
                            call_stack.top().action_values[2];
  // player 0, depth 1, preflop seq 0
  cluster = info_abstraction.Cluster(call_stack.top().node, 0);
  REQUIRE(cluster == 3);
  regret = std::rint(-1667 + call_stack.top().action_values[0] -
                     call_stack.top().value);
  regret = std::max(regret, kRegretFloor);
  REQUIRE(regret == 8333);
  regret = std::rint(8383 + call_stack.top().action_values[1] -
                     call_stack.top().value);
  regret = std::max(regret, kRegretFloor);
  REQUIRE(regret == 8383);
  regret = std::rint(-6717 + call_stack.top().action_values[2] -
                     call_stack.top().value);
  regret = std::max(regret, kRegretFloor);
  REQUIRE(regret == 5804);
  // preflop chance node
  call_stack.pop();
  // Iteration 2, player 0, no pruning
  call_stack.pop();
  call_stack.pop();
  REQUIRE(call_stack.size() == 0);

  // Iteration 2, player 1
  sampled = sampler(rng());
  REQUIRE(sampled == 0.34531241030976884);
  // Iteration 2, player 1, with pruning
  call_stack.emplace(start_state, DoubleArray{0}, 0.0, DoubleArray{0});
  // preflop chance node
  call_stack.emplace(call_stack.top());
  call_stack.top().node.Deal();
  call_stack.top().node.ProceedPlay();
  call_stack.emplace(call_stack.top());
  // player 0, depth 1, preflop seq 0
  cluster = info_abstraction.Cluster(call_stack.top().node, 0);
  REQUIRE(cluster == 3);
  call_stack.top().strategy = DoubleArray{8333.0/22520, 8383.0/22520,
                                          5804.0/22520};
  sampled = sampler(rng());
  REQUIRE(sampled == 0.57044036594027492);
  call_stack.emplace(call_stack.top());
  call_stack.top().node.Apply(fishbait::Action::kAllIn);
  // player 1, depth 2, preflop seq 5
  cluster = info_abstraction.Cluster(call_stack.top().node, 1);
  REQUIRE(cluster == 0);
  call_stack.top().strategy = DoubleArray{0.5, 0.5};
  call_stack.top().value = 0.0;
  call_stack.top().action_values = DoubleArray{0.0, 0.0};
  // player 1, depth 2, preflop seq 5, action 0
  // pruned
  // player 1, depth 2, preflop seq 5, action 1
  // pruned
  // player 1, depth 2, preflop seq 5
  // no actions explored
  // player 0, depth 1, preflop seq 0
  call_stack.pop();
  // preflop chance node
  call_stack.pop();
  // Iteration 2, player 1, with pruning
  call_stack.pop();
  call_stack.pop();
  REQUIRE(call_stack.size() == 0);

  // Iteration 2, player 2
  sampled = sampler(rng());
  REQUIRE(sampled == 0.41891060472224451);
  // Iteration 2, player 1, with pruning
  call_stack.emplace(start_state, DoubleArray{0}, 0.0, DoubleArray{0});
  // preflop chance node
  call_stack.emplace(call_stack.top());
  call_stack.top().node.Deal();
  call_stack.top().node.ProceedPlay();
  call_stack.emplace(call_stack.top());
  // player 0, depth 1, preflop seq 0
  cluster = info_abstraction.Cluster(call_stack.top().node, 0);
  REQUIRE(cluster == 0);
  call_stack.top().strategy = DoubleArray{1.0/3.0, 1.0/3.0, 1.0/3.0};
  sampled = sampler(rng());
  REQUIRE(sampled == 0.99458047433987062);
  call_stack.emplace(call_stack.top());
  call_stack.top().node.Apply(fishbait::Action::kCheckCall);
  // player 1, depth 2, preflop seq 8
  cluster = info_abstraction.Cluster(call_stack.top().node, 1);
  REQUIRE(cluster == 1);
  call_stack.top().strategy = DoubleArray{1.0/3.0, 1.0/3.0, 1.0/3.0};
  sampled = sampler(rng());
  REQUIRE(sampled == 0.83246902811618417);
  call_stack.emplace(call_stack.top());
  call_stack.top().node.Apply(fishbait::Action::kCheckCall);
  // player 2, depth 3, preflop seq 14
  cluster = info_abstraction.Cluster(call_stack.top().node, 1);
  REQUIRE(cluster == 1);
  call_stack.top().strategy = DoubleArray{0.5, 0.5};
  call_stack.top().value = 0.0;
  call_stack.top().action_values = DoubleArray{0.0, 0.0};
  // player 2, depth 3, preflop seq 14, action 0
  // pruned
  // player 2, depth 3, preflop seq 14, action 1
  // pruned
  // player 2, depth 3, preflop seq 14
  // no actions explored
  // player 1, depth 2, preflop seq 8
  call_stack.pop();
  // player 0, depth 1, preflop seq 0
  call_stack.pop();
  // preflop chance node
  call_stack.pop();
  // Iteration 2, player 1, with pruning
  call_stack.pop();
  call_stack.pop();
  REQUIRE(call_stack.size() == 0);

  // Iteration 2, discount
  discount = 2.0 / kDiscountInterval / (2.0 / kDiscountInterval + 1.0);
  REQUIRE(discount == 0.5);

  // Iteration 3, player 0
  sampled = sampler(rng());
  REQUIRE(sampled == 0.94303320541278624);
  // Iteration 3, player 0, no pruning
  call_stack.emplace(start_state, DoubleArray{0}, 0.0, DoubleArray{0});
  // preflop chance node
  call_stack.emplace(call_stack.top());
  call_stack.top().node.Deal();
  call_stack.top().node.ProceedPlay();
  call_stack.emplace(call_stack.top());
  // player 0, depth 1, preflop seq 0
  cluster = info_abstraction.Cluster(call_stack.top().node, 0);
  REQUIRE(cluster == 0);
  call_stack.top().strategy = DoubleArray{1.0/3.0, 1.0/3.0, 1.0/3.0};
  call_stack.top().value = 0;
  call_stack.top().action_values = DoubleArray{0.0, 0.0, 0.0};
  // player 0, depth 1, preflop seq 0, action 0
  call_stack.emplace(call_stack.top());
  call_stack.top().node.Apply(fishbait::Action::kFold);
  // player 1, depth 2, preflop seq 1, player 0 folded
  call_stack.emplace(call_stack.top());
  call_stack.top().node.Apply(fishbait::Action::kFold);
  // terminal node
  call_stack.top().node.AwardPot(start_state.same_stack_no_rake_);
  call_stack.top().value = call_stack.top().node.stack(0);
  // player 1, depth 2, preflop seq 1, player 0 folded
  tmp = call_stack.top().value;
  call_stack.pop();
  call_stack.top().value = tmp;
  // player 0, depth 1, preflop seq 0, action 0
  tmp = call_stack.top().value;
  call_stack.pop();
  call_stack.top().action_values[0] = tmp;
  call_stack.top().value += call_stack.top().strategy[0] *
                            call_stack.top().action_values[0];
  // player 0, depth 1, preflop seq 0, action 1
  call_stack.emplace(call_stack.top());
  call_stack.top().node.Apply(fishbait::Action::kAllIn);
  // player 1, depth 2, preflop seq 5
  cluster = info_abstraction.Cluster(call_stack.top().node, 1);
  REQUIRE(cluster == 0);
  call_stack.top().strategy = DoubleArray{0.5, 0.5};
  sampled = sampler(rng());
  REQUIRE(sampled == 0.96651413247224405);
  call_stack.emplace(call_stack.top());
  call_stack.top().node.Apply(fishbait::Action::kAllIn);
  // player 2, depth 3, preflop seq 7
  cluster = info_abstraction.Cluster(call_stack.top().node, 2);
  REQUIRE(cluster == 2);
  call_stack.top().strategy = DoubleArray{0.5, 0.5};
  sampled = sampler(rng());
  REQUIRE(sampled == 0.16112559591085523);
  call_stack.emplace(call_stack.top());
  call_stack.top().node.Apply(fishbait::Action::kFold);
  // terminal node
  call_stack.top().node.Deal();  // flop
  call_stack.top().node.ProceedPlay();
  call_stack.top().node.Deal();  // turn
  call_stack.top().node.ProceedPlay();
  call_stack.top().node.Deal();  // river
  call_stack.top().node.ProceedPlay();
  call_stack.top().node.AwardPot(start_state.same_stack_no_rake_);
  call_stack.top().value = call_stack.top().node.stack(0);
  // player 2, depth 3, preflop seq 7
  tmp = call_stack.top().value;
  call_stack.pop();
  call_stack.top().value = tmp;
  // player 1, depth 2, preflop seq 5
  tmp = call_stack.top().value;
  call_stack.pop();
  call_stack.top().value = tmp;
  // player 0, depth 1, preflop seq 0, action 1
  tmp = call_stack.top().value;
  call_stack.pop();
  call_stack.top().action_values[1] = tmp;
  call_stack.top().value += call_stack.top().strategy[1] *
                            call_stack.top().action_values[1];
  // player 0, depth 1, preflop seq 0, action 2
  call_stack.emplace(call_stack.top());
  call_stack.top().node.Apply(fishbait::Action::kCheckCall);
  // player 1, depth 2, preflop seq 8
  cluster = info_abstraction.Cluster(call_stack.top().node, 1);
  REQUIRE(cluster == 0);
  call_stack.top().strategy = DoubleArray{1.0/3.0, 1.0/3.0, 1.0/3.0};
  sampled = sampler(rng());
  REQUIRE(sampled == 0.23014522664555601);
  call_stack.emplace(call_stack.top());
  call_stack.top().node.Apply(fishbait::Action::kFold);
  // player 2, depth 3, preflop seq 9
  cluster = info_abstraction.Cluster(call_stack.top().node, 2);
  REQUIRE(cluster == 2);
  call_stack.top().strategy = DoubleArray{0.5, 0.5};
  sampled = sampler(rng());
  REQUIRE(sampled == 0.50556625694706703);
  call_stack.emplace(call_stack.top());
  call_stack.top().node.Apply(fishbait::Action::kCheckCall);
  // flop chance node
  call_stack.top().node.Deal();
  call_stack.top().node.ProceedPlay();
  call_stack.emplace(call_stack.top());
  // player 2, depth 4, flop seq 4
  cluster = info_abstraction.Cluster(call_stack.top().node, 2);
  REQUIRE(cluster == 1);
  call_stack.top().strategy = DoubleArray{0.5, 0.5};
  sampled = sampler(rng());
  REQUIRE(sampled == 0.46519970129203597);
  call_stack.emplace(call_stack.top());
  call_stack.top().node.Apply(fishbait::Action::kAllIn);
  // player 0, depth 5, flop seq 5
  cluster = info_abstraction.Cluster(call_stack.top().node, 0);
  REQUIRE(cluster == 1);
  call_stack.top().strategy = DoubleArray{0.5, 0.5};
  call_stack.top().value = 0.0;
  call_stack.top().action_values = DoubleArray{0.0, 0.0};
  // player 0, depth 5, flop seq 5, action 0
  call_stack.emplace(call_stack.top());
  call_stack.top().node.Apply(fishbait::Action::kFold);
  // terminal node
  call_stack.top().node.AwardPot(start_state.same_stack_no_rake_);
  call_stack.top().value = call_stack.top().node.stack(0);
  // player 0, depth 5, flop seq 5, action 0
  tmp = call_stack.top().value;
  call_stack.pop();
  call_stack.top().action_values[0] = tmp;
  call_stack.top().value += call_stack.top().strategy[0] *
                            call_stack.top().action_values[0];
  // player 0, depth 5, flop seq 5, action 1
  call_stack.emplace(call_stack.top());
  call_stack.top().node.Apply(fishbait::Action::kAllIn);
  // terminal node
  call_stack.top().node.Deal();  // turn
  call_stack.top().node.ProceedPlay();
  call_stack.top().node.Deal();  // river
  call_stack.top().node.ProceedPlay();
  call_stack.top().node.AwardPot(start_state.same_stack_no_rake_);
  call_stack.top().value = call_stack.top().node.stack(0);
  // player 0, depth 5, flop seq 5, action 1
  tmp = call_stack.top().value;
  call_stack.pop();
  call_stack.top().action_values[1] = tmp;
  call_stack.top().value += call_stack.top().strategy[1] *
                            call_stack.top().action_values[1];
  // player 0, depth 5, flop seq 5
  cluster = info_abstraction.Cluster(call_stack.top().node, 0);
  REQUIRE(cluster == 1);
  regret = std::rint(0 + call_stack.top().action_values[0] -
                     call_stack.top().value);
  regret = std::max(regret, kRegretFloor);
  REQUIRE(regret == 4950);
  regret = std::rint(0 + call_stack.top().action_values[1] -
                     call_stack.top().value);
  regret = std::max(regret, kRegretFloor);
  REQUIRE(regret == -4950);
  // player 2, depth 4, flop seq 4
  tmp = call_stack.top().value;
  call_stack.pop();
  call_stack.top().value = tmp;
  // flop chance node
  tmp = call_stack.top().value;
  call_stack.pop();
  call_stack.top().value = tmp;
  // player 2, depth 3, preflop seq 9
  tmp = call_stack.top().value;
  call_stack.pop();
  call_stack.top().value = tmp;
  // player 1, depth 2, preflop seq 8
  tmp = call_stack.top().value;
  call_stack.pop();
  call_stack.top().value = tmp;
  // player 0, depth 1, preflop seq 0, action 2
  tmp = call_stack.top().value;
  call_stack.pop();
  call_stack.top().action_values[2] = tmp;
  call_stack.top().value += call_stack.top().strategy[2] *
                            call_stack.top().action_values[2];
  // player 0, depth 1, preflop seq 0
  cluster = info_abstraction.Cluster(call_stack.top().node, 0);
  REQUIRE(cluster == 0);
  regret = std::rint(0 + call_stack.top().action_values[0] -
                     call_stack.top().value);
  regret = std::max(regret, kRegretFloor);
  REQUIRE(regret == 5017);
  regret = std::rint(0 + call_stack.top().action_values[1] -
                     call_stack.top().value);
  regret = std::max(regret, kRegretFloor);
  REQUIRE(regret == -4983);
  regret = std::rint(0 + call_stack.top().action_values[2] -
                     call_stack.top().value);
  regret = std::max(regret, kRegretFloor);
  REQUIRE(regret == -33);
  // preflop chance node
  call_stack.pop();
  // Iteration 3, player 0, no pruning
  call_stack.pop();
  call_stack.pop();
  REQUIRE(call_stack.size() == 0);

  // Iteration 3, player 1
  sampled = sampler(rng());
  REQUIRE(sampled == 0.51527152553909528);
  // Iteration 3, player 1, no pruning
  call_stack.emplace(start_state, DoubleArray{0}, 0.0, DoubleArray{0});
  // preflop chance node
  call_stack.emplace(call_stack.top());
  call_stack.top().node.Deal();
  call_stack.top().node.ProceedPlay();
  call_stack.emplace(call_stack.top());
  // player 0, depth 1, preflop seq 0
  cluster = info_abstraction.Cluster(call_stack.top().node, 0);
  REQUIRE(cluster == 1);
  call_stack.top().strategy = DoubleArray{1.0/3.0, 1.0/3.0, 1.0/3.0};
  sampled = sampler(rng());
  REQUIRE(sampled == 0.29626728094292215);
  call_stack.emplace(call_stack.top());
  call_stack.top().node.Apply(fishbait::Action::kFold);
  // player 1, depth 2, preflop seq 1
  cluster = info_abstraction.Cluster(call_stack.top().node, 1);
  REQUIRE(cluster == 2);
  call_stack.top().strategy = DoubleArray{1.0/3.0, 1.0/3.0, 1.0/3.0};
  call_stack.top().value = 0.0;
  call_stack.top().action_values = DoubleArray{0.0, 0.0, 0.0};
  // player 1, depth 2, preflop seq 1, action 0
  call_stack.emplace(call_stack.top());
  call_stack.top().node.Apply(fishbait::Action::kFold);
  // terminal node, player 1 folded
  call_stack.top().node.AwardPot(start_state.same_stack_no_rake_);
  call_stack.top().value = call_stack.top().node.stack(1);
  // player 1, depth 2, preflop seq 1, action 0
  tmp = call_stack.top().value;
  call_stack.pop();
  call_stack.top().action_values[0] = tmp;
  REQUIRE(call_stack.top().action_values[0] == 9950);
  call_stack.top().value += call_stack.top().strategy[0] *
                            call_stack.top().action_values[0];
  // player 1, depth 2, preflop seq 1, action 1
  call_stack.emplace(call_stack.top());
  call_stack.top().node.Apply(fishbait::Action::kAllIn);
  // player 2, depth 3, preflop seq 2
  cluster = info_abstraction.Cluster(call_stack.top().node, 2);
  REQUIRE(cluster == 3);
  call_stack.top().strategy = DoubleArray{0.0, 1.0};
  sampled = sampler(rng());
  REQUIRE(sampled == 0.54871826148727043);
  call_stack.emplace(call_stack.top());
  call_stack.top().node.Apply(fishbait::Action::kAllIn);
  // terminal node
  call_stack.top().node.Deal();  // flop
  call_stack.top().node.ProceedPlay();
  call_stack.top().node.Deal();  // turn
  call_stack.top().node.ProceedPlay();
  call_stack.top().node.Deal();  // river
  call_stack.top().node.ProceedPlay();
  call_stack.top().node.AwardPot(start_state.same_stack_no_rake_);
  call_stack.top().value = call_stack.top().node.stack(1);
  // player 2, depth 3, preflop seq 2
  tmp = call_stack.top().value;
  call_stack.pop();
  call_stack.top().value = tmp;
  // player 1, depth 2, preflop seq 1, action 1
  tmp = call_stack.top().value;
  call_stack.pop();
  call_stack.top().action_values[1] = tmp;
  REQUIRE(call_stack.top().action_values[1] == 0.0);
  call_stack.top().value += call_stack.top().strategy[1] *
                            call_stack.top().action_values[1];
  // player 1, depth 2, preflop seq 1, action 2
  call_stack.emplace(call_stack.top());
  call_stack.top().node.Apply(fishbait::Action::kCheckCall);
  // player 2, depth 3, preflop seq 3
  cluster = info_abstraction.Cluster(call_stack.top().node, 2);
  REQUIRE(cluster == 3);
  call_stack.top().strategy = DoubleArray{0.5, 0.5};
  sampled = sampler(rng());
  REQUIRE(sampled == 0.87867058294745293);
  call_stack.emplace(call_stack.top());
  call_stack.top().node.Apply(fishbait::Action::kCheckCall);
  // flop chance node
  call_stack.top().node.Deal();
  call_stack.top().node.ProceedPlay();
  call_stack.emplace(call_stack.top());
  // player 1, depth 4, flop seq 0
  cluster = info_abstraction.Cluster(call_stack.top().node, 1);
  REQUIRE(cluster == 1);
  call_stack.top().strategy = DoubleArray{0.5, 0.5};
  call_stack.top().value = 0.0;
  call_stack.top().action_values = DoubleArray{0.0, 0.0};
  // player 1, depth 4, flop seq 0, action 0
  call_stack.emplace(call_stack.top());
  call_stack.top().node.Apply(fishbait::Action::kAllIn);
  // player 2, depth 5, flop seq 1
  cluster = info_abstraction.Cluster(call_stack.top().node, 2);
  REQUIRE(cluster == 2);
  call_stack.top().strategy = DoubleArray{0.5, 0.5};
  sampled = sampler(rng());
  REQUIRE(sampled == 0.20980855403838083);
  call_stack.emplace(call_stack.top());
  call_stack.top().node.Apply(fishbait::Action::kFold);
  // terminal node
  call_stack.top().node.AwardPot(start_state.same_stack_no_rake_);
  call_stack.top().value = call_stack.top().node.stack(1);
  // player 2, depth 5, flop seq 1
  tmp = call_stack.top().value;
  call_stack.pop();
  call_stack.top().value = tmp;
  // player 1, depth 4, flop seq 0, action 0
  tmp = call_stack.top().value;
  call_stack.pop();
  call_stack.top().action_values[0] = tmp;
  REQUIRE(call_stack.top().action_values[0] == 10100);
  call_stack.top().value += call_stack.top().strategy[0] *
                            call_stack.top().action_values[0];
  // player 1, depth 4, flop seq 0, action 1
  call_stack.emplace(call_stack.top());
  call_stack.top().node.Apply(fishbait::Action::kCheckCall);
  // player 2, depth 5, flop seq 2
  cluster = info_abstraction.Cluster(call_stack.top().node, 2);
  REQUIRE(cluster == 2);
  call_stack.top().strategy = DoubleArray{0.5, 0.5};
  sampled = sampler(rng());
  REQUIRE(sampled == 0.4843245717960375);
  call_stack.emplace(call_stack.top());
  call_stack.top().node.Apply(fishbait::Action::kAllIn);
  // player 1, depth 6, flop seq 3
  cluster = info_abstraction.Cluster(call_stack.top().node, 1);
  REQUIRE(cluster == 1);
  call_stack.top().strategy = DoubleArray{0.5, 0.5};
  call_stack.top().value = 0.0;
  call_stack.top().action_values = DoubleArray{0.0, 0.0};
  // player 1, depth 6, flop seq 3, action 0
  call_stack.emplace(call_stack.top());
  call_stack.top().node.Apply(fishbait::Action::kFold);
  // terminal node, player 1 folded
  call_stack.top().node.AwardPot(start_state.same_stack_no_rake_);
  call_stack.top().value = call_stack.top().node.stack(1);
  // player 1, depth 6, flop seq 3, action 0
  tmp = call_stack.top().value;
  call_stack.pop();
  call_stack.top().action_values[0] = tmp;
  REQUIRE(call_stack.top().action_values[0] == 9900);
  call_stack.top().value += call_stack.top().strategy[0] *
                            call_stack.top().action_values[0];
  // player 1, depth 6, flop seq 3, action 1
  call_stack.emplace(call_stack.top());
  call_stack.top().node.Apply(fishbait::Action::kAllIn);
  // terminal node
  call_stack.top().node.Deal();  // turn
  call_stack.top().node.ProceedPlay();
  call_stack.top().node.Deal();  // river
  call_stack.top().node.ProceedPlay();
  call_stack.top().node.AwardPot(start_state.same_stack_no_rake_);
  call_stack.top().value = call_stack.top().node.stack(1);
  // player 1, depth 6, flop seq 3, action 1
  tmp = call_stack.top().value;
  call_stack.pop();
  call_stack.top().action_values[1] = tmp;
  REQUIRE(call_stack.top().action_values[1] == 20000);
  call_stack.top().value += call_stack.top().strategy[1] *
                            call_stack.top().action_values[1];
  // player 1, depth 6, flop seq 3
  cluster = info_abstraction.Cluster(call_stack.top().node, 1);
  REQUIRE(cluster == 1);
  regret = std::rint(0 + call_stack.top().action_values[0] -
                     call_stack.top().value);
  regret = std::max(regret, kRegretFloor);
  REQUIRE(regret == -5050);
  regret = std::rint(0 + call_stack.top().action_values[1] -
                     call_stack.top().value);
  regret = std::max(regret, kRegretFloor);
  REQUIRE(regret == 5050);
  // player 2, depth 5, flop seq 2
  tmp = call_stack.top().value;
  call_stack.pop();
  call_stack.top().value = tmp;
  // player 1, depth 4, flop seq 0, action 1
  tmp = call_stack.top().value;
  call_stack.pop();
  call_stack.top().action_values[1] = tmp;
  REQUIRE(call_stack.top().action_values[1] == 14950);
  call_stack.top().value += call_stack.top().strategy[1] *
                            call_stack.top().action_values[1];
  // player 1, depth 4, flop seq 0
  cluster = info_abstraction.Cluster(call_stack.top().node, 1);
  REQUIRE(cluster == 1);
  regret = std::rint(0 + call_stack.top().action_values[0] -
                     call_stack.top().value);
  regret = std::max(regret, kRegretFloor);
  REQUIRE(regret == -2425);
  regret = std::rint(0 + call_stack.top().action_values[1] -
                     call_stack.top().value);
  regret = std::max(regret, kRegretFloor);
  REQUIRE(regret == 2425);
  // flop chance node
  tmp = call_stack.top().value;
  call_stack.pop();
  call_stack.top().value = tmp;
  // player 2, depth 3, preflop seq 3
  tmp = call_stack.top().value;
  call_stack.pop();
  call_stack.top().value = tmp;
  // player 1, depth 2, preflop seq 1, action 2
  tmp = call_stack.top().value;
  call_stack.pop();
  call_stack.top().action_values[2] = tmp;
  REQUIRE(call_stack.top().action_values[2] == 12525);
  call_stack.top().value += call_stack.top().strategy[2] *
                            call_stack.top().action_values[2];
  // player 1, depth 2, preflop seq 1
  cluster = info_abstraction.Cluster(call_stack.top().node, 1);
  REQUIRE(cluster == 2);
  regret = std::rint(0 + call_stack.top().action_values[0] -
                     call_stack.top().value);
  regret = std::max(regret, kRegretFloor);
  REQUIRE(regret == 2458);
  regret = std::rint(0 + call_stack.top().action_values[1] -
                     call_stack.top().value);
  regret = std::max(regret, kRegretFloor);
  REQUIRE(regret == -7492);
  regret = std::rint(0 + call_stack.top().action_values[2] -
                     call_stack.top().value);
  regret = std::max(regret, kRegretFloor);
  REQUIRE(regret == 5033);
  // player 0, depth 1, preflop seq 0
  call_stack.pop();
  // preflop chance node
  call_stack.pop();
  // Iteration 3, player 1, no pruning
  call_stack.pop();
  call_stack.pop();
  REQUIRE(call_stack.size() == 0);

  // Iteration 3, player 2
  sampled = sampler(rng());
  REQUIRE(sampled == 0.38261106951932172);
  // Iteration 3, player 2, with pruning
  call_stack.emplace(start_state, DoubleArray{0}, 0.0, DoubleArray{0});
  // preflop chance node
  call_stack.emplace(call_stack.top());
  call_stack.top().node.Deal();
  call_stack.top().node.ProceedPlay();
  call_stack.emplace(call_stack.top());
  // player 0, depth 1, preflop seq 0
  cluster = info_abstraction.Cluster(call_stack.top().node, 0);
  REQUIRE(cluster == 0);
  call_stack.top().strategy = DoubleArray{1.0, 0.0, 0.0};
  sampled = sampler(rng());
  REQUIRE(sampled == 0.6435783213666969);
  call_stack.emplace(call_stack.top());
  call_stack.top().node.Apply(fishbait::Action::kFold);
  // player 1, depth 2, preflop seq 1
  cluster = info_abstraction.Cluster(call_stack.top().node, 1);
  REQUIRE(cluster == 1);
  call_stack.top().strategy = DoubleArray{1.0/3.0, 1.0/3.0, 1.0/3.0};
  sampled = sampler(rng());
  REQUIRE(sampled == 0.77147336851021042);
  call_stack.emplace(call_stack.top());
  call_stack.top().node.Apply(fishbait::Action::kCheckCall);
  // player 2, depth 3, preflop seq 3
  cluster = info_abstraction.Cluster(call_stack.top().node, 2);
  REQUIRE(cluster == 2);
  call_stack.top().strategy = DoubleArray{0.5, 0.5};
  call_stack.top().value = 0.0;
  call_stack.top().action_values = DoubleArray{0.0, 0.0};
  // player 2, depth 3, preflop seq 3, action 0
  // pruned
  // player 2, depth 3, preflop seq 3, action 1
  // pruned
  // player 2, depth 3, preflop seq 3
  // action 0 pruned
  // action 1 pruned
  // player 1, depth 2, preflop seq 1
  call_stack.pop();
  // player 0, depth 1, preflop seq 0
  call_stack.pop();
  // preflop chance node
  call_stack.pop();
  // Iteration 3, player 2, with pruning
  call_stack.pop();
  call_stack.pop();
  REQUIRE(call_stack.size() == 0);

  // Iteration 4, player 0, update strategy
  call_stack.emplace(start_state, DoubleArray{0}, 0.0, DoubleArray{0});
  // preflop chance node
  call_stack.emplace(call_stack.top());
  call_stack.top().node.Deal();
  call_stack.top().node.ProceedPlay();
  call_stack.emplace(call_stack.top());
  // player 0, depth 1, preflop seq 0
  cluster = info_abstraction.Cluster(call_stack.top().node, 0);
  REQUIRE(cluster == 3);
  call_stack.top().strategy = DoubleArray{4166.0/11260, 4192.0/11260,
                                          2902.0/11260};
  sampled = sampler(rng());
  REQUIRE(sampled == 0.29716207412954404);
  action_count = 0 + 1;
  REQUIRE(action_count == 1);
  call_stack.emplace(call_stack.top());
  call_stack.top().node.Apply(fishbait::Action::kFold);
  // player 0 folded
  // player 0, depth 1, preflop seq 0
  call_stack.pop();
  // preflop chance node
  call_stack.pop();
  // Iteration 4, player 0, update strategy
  call_stack.pop();
  call_stack.pop();
  REQUIRE(call_stack.size() == 0);

  // Iteration 4, player 0
  sampled = sampler(rng());
  REQUIRE(sampled == 0.56581888123398771);
  // Iteration 4, player 0, no pruning
  call_stack.emplace(start_state, DoubleArray{0}, 0.0, DoubleArray{0});
  // preflop chance node
  call_stack.emplace(call_stack.top());
  call_stack.top().node.Deal();
  call_stack.top().node.ProceedPlay();
  call_stack.emplace(call_stack.top());
  // player 0, depth 1, preflop seq 0
  cluster = info_abstraction.Cluster(call_stack.top().node, 0);
  REQUIRE(cluster == 1);
  call_stack.top().strategy = DoubleArray{1.0/3.0, 1.0/3.0, 1.0/3.0};
  call_stack.top().value = 0.0;
  call_stack.top().action_values = DoubleArray{0.0, 0.0, 0.0};
  // player 0, depth 1, preflop seq 0, action 0
  call_stack.emplace(call_stack.top());
  call_stack.top().node.Apply(fishbait::Action::kFold);
  // player 1, depth 2, preflop seq 1, player 0 folded
  call_stack.emplace(call_stack.top());
  call_stack.top().node.Apply(fishbait::Action::kFold);
  // terminal node
  call_stack.top().node.AwardPot(start_state.same_stack_no_rake_);
  call_stack.top().value = call_stack.top().node.stack(0);
  // player 1, depth 2, preflop seq 1, player 0 folded
  tmp = call_stack.top().value;
  call_stack.pop();
  call_stack.top().value = tmp;
  // player 0, depth 1, preflop seq 0, action 0
  tmp = call_stack.top().value;
  call_stack.pop();
  call_stack.top().action_values[0] = tmp;
  call_stack.top().value += call_stack.top().strategy[0] *
                            call_stack.top().action_values[0];
  // player 0, depth 1, preflop seq 0, action 1
  call_stack.emplace(call_stack.top());
  call_stack.top().node.Apply(fishbait::Action::kAllIn);
  // player 1, depth 2, preflop seq 5
  cluster = info_abstraction.Cluster(call_stack.top().node, 1);
  REQUIRE(cluster == 1);
  call_stack.top().strategy = DoubleArray{0.5, 0.5};
  sampled = sampler(rng());
  REQUIRE(sampled == 0.50666665876433536);
  call_stack.emplace(call_stack.top());
  call_stack.top().node.Apply(fishbait::Action::kAllIn);
  // player 2, depth 3, preflop seq 7
  cluster = info_abstraction.Cluster(call_stack.top().node, 2);
  REQUIRE(cluster == 0);
  call_stack.top().strategy = DoubleArray{0.5, 0.5};
  sampled = sampler(rng());
  REQUIRE(sampled == 0.73587255159763665);
  call_stack.emplace(call_stack.top());
  call_stack.top().node.Apply(fishbait::Action::kAllIn);
  // terminal node
  call_stack.top().node.Deal();  // flop
  call_stack.top().node.ProceedPlay();
  call_stack.top().node.Deal();  // turn
  call_stack.top().node.ProceedPlay();
  call_stack.top().node.Deal();  // river
  call_stack.top().node.ProceedPlay();
  call_stack.top().node.AwardPot(start_state.same_stack_no_rake_);
  call_stack.top().value = call_stack.top().node.stack(0);
  // player 2, depth 3, preflop seq 7
  tmp = call_stack.top().value;
  call_stack.pop();
  call_stack.top().value = tmp;
  // player 1, depth 2, preflop seq 5
  tmp = call_stack.top().value;
  call_stack.pop();
  call_stack.top().value = tmp;
  // player 0, depth 1, preflop seq 0, action 1
  tmp = call_stack.top().value;
  call_stack.pop();
  call_stack.top().action_values[1] = tmp;
  call_stack.top().value += call_stack.top().strategy[1] *
                            call_stack.top().action_values[1];
  // player 0, depth 1, preflop seq 0, action 2
  call_stack.emplace(call_stack.top());
  call_stack.top().node.Apply(fishbait::Action::kCheckCall);
  // player 1, depth 2, preflop seq 8
  cluster = info_abstraction.Cluster(call_stack.top().node, 1);
  REQUIRE(cluster == 1);
  call_stack.top().strategy = DoubleArray{1.0/3.0, 1.0/3.0, 1.0/3.0};
  sampled = sampler(rng());
  REQUIRE(sampled == 0.62853888453221474);
  call_stack.emplace(call_stack.top());
  call_stack.top().node.Apply(fishbait::Action::kAllIn);
  // player 2, depth 3, preflop seq 11
  cluster = info_abstraction.Cluster(call_stack.top().node, 2);
  REQUIRE(cluster == 0);
  call_stack.top().strategy = DoubleArray{0.5, 0.5};
  sampled = sampler(rng());
  REQUIRE(sampled == 0.70245942943038309);
  call_stack.emplace(call_stack.top());
  call_stack.top().node.Apply(fishbait::Action::kAllIn);
  // player 0, depth 4, preflop seq 13
  cluster = info_abstraction.Cluster(call_stack.top().node, 0);
  REQUIRE(cluster == 1);
  call_stack.top().strategy = DoubleArray{0.5, 0.5};
  call_stack.top().value = 0.0;
  call_stack.top().action_values = DoubleArray{0.0, 0.0};
  // player 0, depth 4, preflop seq 13, action 0
  call_stack.emplace(call_stack.top());
  call_stack.top().node.Apply(fishbait::Action::kFold);
  // terminal node, player 0 folded
  call_stack.top().value = call_stack.top().node.stack(0);
  // player 0, depth 4, preflop seq 13, action 0
  tmp = call_stack.top().value;
  call_stack.pop();
  call_stack.top().action_values[0] = tmp;
  call_stack.top().value += call_stack.top().strategy[0] *
                            call_stack.top().action_values[0];
  // player 0, depth 4, preflop seq 13, action 1
  call_stack.emplace(call_stack.top());
  call_stack.top().node.Apply(fishbait::Action::kAllIn);
  // terminal node
  call_stack.top().node.Deal();  // flop
  call_stack.top().node.ProceedPlay();
  call_stack.top().node.Deal();  // turn
  call_stack.top().node.ProceedPlay();
  call_stack.top().node.Deal();  // river
  call_stack.top().node.ProceedPlay();
  call_stack.top().node.AwardPot(start_state.same_stack_no_rake_);
  call_stack.top().value = call_stack.top().node.stack(0);
  // player 0, depth 4, preflop seq 13, action 1
  tmp = call_stack.top().value;
  call_stack.pop();
  call_stack.top().action_values[1] = tmp;
  call_stack.top().value += call_stack.top().strategy[1] *
                            call_stack.top().action_values[1];
  // player 0, depth 4, preflop seq 13
  cluster = info_abstraction.Cluster(call_stack.top().node, 0);
  REQUIRE(cluster == 1);
  regret = std::rint(0 + call_stack.top().action_values[0] -
                     call_stack.top().value);
  regret = std::max(regret, kRegretFloor);
  REQUIRE(regret == -10050);
  regret = std::rint(0 + call_stack.top().action_values[1] -
                     call_stack.top().value);
  regret = std::max(regret, kRegretFloor);
  REQUIRE(regret == 10050);
  // player 2, depth 3, preflop seq 11
  tmp = call_stack.top().value;
  call_stack.pop();
  call_stack.top().value = tmp;
  // player 1, depth 2, preflop seq 8
  tmp = call_stack.top().value;
  call_stack.pop();
  call_stack.top().value = tmp;
  // player 0, depth 1, preflop seq 0, action 2
  tmp = call_stack.top().value;
  call_stack.pop();
  call_stack.top().action_values[2] = tmp;
  call_stack.top().value += call_stack.top().strategy[2] *
                            call_stack.top().action_values[2];
  // player 0, depth 1, preflop seq 0
  cluster = info_abstraction.Cluster(call_stack.top().node, 0);
  REQUIRE(cluster == 1);
  regret = std::rint(0 + call_stack.top().action_values[0] -
                     call_stack.top().value);
  regret = std::max(regret, kRegretFloor);
  REQUIRE(regret == -4983);
  regret = std::rint(0 + call_stack.top().action_values[1] -
                     call_stack.top().value);
  regret = std::max(regret, kRegretFloor);
  REQUIRE(regret == 17);
  regret = std::rint(0 + call_stack.top().action_values[2] -
                     call_stack.top().value);
  regret = std::max(regret, kRegretFloor);
  REQUIRE(regret == 4967);
  // preflop chance node
  call_stack.pop();
  // Iteration 4, player 0, no pruning
  call_stack.pop();
  call_stack.pop();
  REQUIRE(call_stack.size() == 0);

  // Iteration 4, player 1, update strategy
  call_stack.emplace(start_state, DoubleArray{0}, 0.0, DoubleArray{0});
  // preflop chance node
  call_stack.emplace(call_stack.top());
  call_stack.top().node.Deal();
  call_stack.top().node.ProceedPlay();
  call_stack.emplace(call_stack.top());
  // player 0, depth 1, preflop seq 0, action 0
  call_stack.emplace(call_stack.top());
  call_stack.top().node.Apply(fishbait::Action::kFold);
  // player 1, depth 2, preflop seq 1
  cluster = info_abstraction.Cluster(call_stack.top().node, 1);
  REQUIRE(cluster == 1);
  call_stack.top().strategy = DoubleArray{1.0/3.0, 1.0/3.0, 1.0/3.0};
  sampled = sampler(rng());
  REQUIRE(sampled == 0.2904261210561781);
  action_count = 0 + 1;
  REQUIRE(action_count == 1);
  call_stack.emplace(call_stack.top());
  call_stack.top().node.Apply(fishbait::Action::kFold);
  // terminal node, player 1 folded
  // player 1, depth 2, preflop seq 1
  call_stack.pop();
  // player 0, depth 1, preflop seq 0, action 0
  call_stack.pop();
  // player 0, depth 1, preflop seq 0, action 1
  call_stack.emplace(call_stack.top());
  call_stack.top().node.Apply(fishbait::Action::kAllIn);
  // player 1, depth 2, preflop seq 5
  cluster = info_abstraction.Cluster(call_stack.top().node, 1);
  REQUIRE(cluster == 1);
  call_stack.top().strategy = DoubleArray{0.5, 0.5};
  sampled = sampler(rng());
  REQUIRE(sampled == 0.08099916009361013);
  action_count = 0 + 1;
  REQUIRE(action_count == 1);
  call_stack.emplace(call_stack.top());
  call_stack.top().node.Apply(fishbait::Action::kFold);
  // player 2, depth 3, preflop seq 6, player 1 folded
  // player 1, depth 2, preflop seq 5
  call_stack.pop();
  // player 0, depth 1, preflop seq 0, action 1
  call_stack.pop();
  // player 0, depth 1, preflop seq 0, action 2
  call_stack.emplace(call_stack.top());
  call_stack.top().node.Apply(fishbait::Action::kCheckCall);
  // player 1, depth 2, preflop seq 8
  cluster = info_abstraction.Cluster(call_stack.top().node, 1);
  REQUIRE(cluster == 1);
  call_stack.top().strategy = DoubleArray{1.0/3.0, 1.0/3.0, 1.0/3.0};
  sampled = sampler(rng());
  REQUIRE(sampled == 0.48051278932377689);
  action_count = 0 + 1;
  REQUIRE(action_count == 1);
  call_stack.emplace(call_stack.top());
  call_stack.top().node.Apply(fishbait::Action::kAllIn);
  // player 2, depth 3, preflop seq 11, action 0
  call_stack.emplace(call_stack.top());
  call_stack.top().node.Apply(fishbait::Action::kFold);
  // player 0, depth 4, preflop seq 12, action 0
  call_stack.emplace(call_stack.top());
  call_stack.top().node.Apply(fishbait::Action::kFold);
  // terminal node
  // player 0, depth 4, preflop seq 12, action 0
  call_stack.pop();
  // player 0, depth 4, preflop seq 12, action 1
  call_stack.emplace(call_stack.top());
  call_stack.top().node.Apply(fishbait::Action::kAllIn);
  // terminal node
  // player 0, depth 4, preflop seq 12, action 1
  call_stack.pop();
  // player 2, depth 3, preflop seq 11, action 0
  call_stack.pop();
  // player 2, depth 3, preflop seq 11, action 1
  call_stack.emplace(call_stack.top());
  call_stack.top().node.Apply(fishbait::Action::kAllIn);
  // player 0, depth 4, preflop seq 13, action 0
  call_stack.emplace(call_stack.top());
  call_stack.top().node.Apply(fishbait::Action::kFold);
  // terminal node
  // player 0, depth 4, preflop seq 13, action 0
  call_stack.pop();
  // player 0, depth 4, preflop seq 13, action 1
  call_stack.emplace(call_stack.top());
  call_stack.top().node.Apply(fishbait::Action::kAllIn);
  // terminal node
  // player 0, depth 4, preflop seq 13, action 1
  call_stack.pop();
  // player 2, depth 3, preflop seq 11, action 1
  call_stack.pop();
  // player 1, depth 2, preflop seq 8
  call_stack.pop();
  // player 0, depth 1, preflop seq 0, action 2
  call_stack.pop();
  // preflop chance node
  call_stack.pop();
  // Iteration 4, player 1, update strategy
  call_stack.pop();
  call_stack.pop();
  REQUIRE(call_stack.size() == 0);

  // Iteration 4, player 1
  sampled = sampler(rng());
  REQUIRE(sampled == 0.27941979158259844);
  // Iteration 4, player 0, with pruning
  call_stack.emplace(start_state, DoubleArray{0}, 0.0, DoubleArray{0});
  // preflop chance node
  call_stack.emplace(call_stack.top());
  call_stack.top().node.Deal();
  call_stack.top().node.ProceedPlay();
  call_stack.emplace(call_stack.top());
  // player 0, depth 1, preflop seq 0
  cluster = info_abstraction.Cluster(call_stack.top().node, 0);
  REQUIRE(cluster == 0);
  call_stack.top().strategy = DoubleArray{1.0, 0.0, 0.0};
  sampled = sampler(rng());
  REQUIRE(sampled == 0.59665676881366425);
  call_stack.emplace(call_stack.top());
  call_stack.top().node.Apply(fishbait::Action::kFold);
  // player 1, depth 2, preflop seq 1
  cluster = info_abstraction.Cluster(call_stack.top().node, 1);
  REQUIRE(cluster == 1);
  call_stack.top().strategy = DoubleArray{1.0/3.0, 1.0/3.0, 1.0/3.0};
  call_stack.top().value = 0;
  call_stack.top().action_values = DoubleArray{0.0, 0.0, 0.0};
  // player 1, depth 2, preflop seq 1, action 0
  // pruned
  // player 1, depth 2, preflop seq 1, action 1
  // pruned
  // player 1, depth 2, preflop seq 1, action 2
  // pruned
  // player 1, depth 2, preflop seq 1
  // no explored actions
  // player 0, depth 1, preflop seq 0
  call_stack.pop();
  // preflop chance node
  call_stack.pop();
  // Iteration 4, player 1, with pruning
  call_stack.pop();
  call_stack.pop();
  REQUIRE(call_stack.size() == 0);

  // Iteration 4, player 2, update strategy
  call_stack.emplace(start_state, DoubleArray{0}, 0.0, DoubleArray{0});
  // preflop chance node
  call_stack.emplace(call_stack.top());
  call_stack.top().node.Deal();
  call_stack.top().node.ProceedPlay();
  call_stack.emplace(call_stack.top());
  // player 0, depth 1, preflop seq 0, action 0
  call_stack.emplace(call_stack.top());
  call_stack.top().node.Apply(fishbait::Action::kFold);
  // player 1, depth 2, preflop seq 1, action 0
  call_stack.emplace(call_stack.top());
  call_stack.top().node.Apply(fishbait::Action::kFold);
  // terminal node
  // player 1, depth 2, preflop seq 1, action 0
  call_stack.pop();
  // player 1, depth 2, preflop seq 1, action 1
  call_stack.emplace(call_stack.top());
  call_stack.top().node.Apply(fishbait::Action::kAllIn);
  // player 2, depth 3, preflop seq 2
  cluster = info_abstraction.Cluster(call_stack.top().node, 2);
  REQUIRE(cluster == 3);
  call_stack.top().strategy = DoubleArray{0.0, 1.0};
  sampled = sampler(rng());
  REQUIRE(sampled == 0.64214795516727508);
  action_count = 0 + 1;
  REQUIRE(action_count == 1);
  call_stack.emplace(call_stack.top());
  call_stack.top().node.Apply(fishbait::Action::kAllIn);
  // terminal node
  // player 2, depth 3, preflop seq 2
  call_stack.pop();
  // player 1, depth 2, preflop seq 1, action 1
  call_stack.pop();
  // player 1, depth 2, preflop seq 1, action 2
  call_stack.emplace(call_stack.top());
  call_stack.top().node.Apply(fishbait::Action::kCheckCall);
  // player 2, depth 3, preflop seq 3
  cluster = info_abstraction.Cluster(call_stack.top().node, 2);
  REQUIRE(cluster == 3);
  call_stack.top().strategy = DoubleArray{0.5, 0.5};
  sampled = sampler(rng());
  REQUIRE(sampled == 0.041529612154309184);
  action_count = 0 + 1;
  REQUIRE(action_count == 1);
  call_stack.emplace(call_stack.top());
  call_stack.top().node.Apply(fishbait::Action::kAllIn);
  // player 1, depth 4, preflop seq 4, action 0
  call_stack.emplace(call_stack.top());
  call_stack.top().node.Apply(fishbait::Action::kFold);
  // terminal node
  // player 1, depth 4, preflop seq 4, action 0
  call_stack.pop();
  // player 1, depth 4, preflop seq 4, action 1
  call_stack.emplace(call_stack.top());
  call_stack.top().node.Apply(fishbait::Action::kAllIn);
  // terminal node
  // player 1, depth 4, preflop seq 4, action 1
  call_stack.pop();
  // player 2, depth 3, preflop seq 3
  call_stack.pop();
  // player 1, depth 2, preflop seq 1, action 2
  call_stack.pop();
  // player 0, depth 1, preflop seq 0, action 0
  call_stack.pop();
  // player 0, depth 1, preflop seq 0, action 1
  call_stack.emplace(call_stack.top());
  call_stack.top().node.Apply(fishbait::Action::kAllIn);
  // player 1, depth 2, preflop seq 5, action 0
  call_stack.emplace(call_stack.top());
  call_stack.top().node.Apply(fishbait::Action::kFold);
  // player 2, depth 3, preflop seq 6
  cluster = info_abstraction.Cluster(call_stack.top().node, 2);
  REQUIRE(cluster == 3);
  call_stack.top().strategy = DoubleArray{0.5, 0.5};
  sampled = sampler(rng());
  REQUIRE(sampled == 0.339393545673126573);
  action_count = 0 + 1;
  REQUIRE(action_count == 1);
  call_stack.emplace(call_stack.top());
  call_stack.top().node.Apply(fishbait::Action::kFold);
  // terminal node, player 2 folded
  // player 2, depth 3, preflop seq 6
  call_stack.pop();
  // player 1, depth 2, preflop seq 5, action 0
  call_stack.pop();
  // player 1, depth 2, preflop seq 5, action 1
  call_stack.emplace(call_stack.top());
  call_stack.top().node.Apply(fishbait::Action::kAllIn);
  // player 2, depth 3, preflop seq 7
  cluster = info_abstraction.Cluster(call_stack.top().node, 2);
  REQUIRE(cluster == 3);
  call_stack.top().strategy = DoubleArray{0.5, 0.5};
  sampled = sampler(rng());
  REQUIRE(sampled == 0.127833803101753823);
  action_count = 0 + 1;
  REQUIRE(action_count == 1);
  call_stack.emplace(call_stack.top());
  call_stack.top().node.Apply(fishbait::Action::kFold);
  // terminal node, player 2 folded
  // player 2, depth 3, preflop seq 7
  call_stack.pop();
  // player 1, depth 2, preflop seq 5, action 1
  call_stack.pop();
  // player 0, depth 1, preflop seq 0, action 1
  call_stack.pop();
  // player 0, depth 1, preflop seq 0, action 2
  call_stack.emplace(call_stack.top());
  call_stack.top().node.Apply(fishbait::Action::kCheckCall);
  // player 1, depth 2, preflop seq 8, action 0
  call_stack.emplace(call_stack.top());
  call_stack.top().node.Apply(fishbait::Action::kFold);
  // player 2, depth 3, preflop seq 9
  cluster = info_abstraction.Cluster(call_stack.top().node, 2);
  REQUIRE(cluster == 3);
  call_stack.top().strategy = DoubleArray{0.5, 0.5};
  sampled = sampler(rng());
  REQUIRE(sampled == 0.003478278407055832);
  action_count = 0 + 1;
  REQUIRE(action_count == 1);
  call_stack.emplace(call_stack.top());
  call_stack.top().node.Apply(fishbait::Action::kAllIn);
  // player 0, depth 4, preflop seq 10, action 0
  call_stack.emplace(call_stack.top());
  call_stack.top().node.Apply(fishbait::Action::kFold);
  // terminal node
  // player 0, depth 4, preflop seq 10, action 0
  call_stack.pop();
  // player 0, depth 4, preflop seq 10, action 1
  call_stack.emplace(call_stack.top());
  call_stack.top().node.Apply(fishbait::Action::kAllIn);
  // terminal node
  // player 0, depth 4, preflop seq 10, action 1
  call_stack.pop();
  // player 2, depth 3, preflop seq 9
  call_stack.pop();
  // player 1, depth 2, preflop seq 8, action 0
  call_stack.pop();
  // player 1, depth 2, preflop seq 8, action 1
  call_stack.emplace(call_stack.top());
  call_stack.top().node.Apply(fishbait::Action::kAllIn);
  // player 2, depth 3, preflop seq 11
  cluster = info_abstraction.Cluster(call_stack.top().node, 2);
  REQUIRE(cluster == 3);
  call_stack.top().strategy = DoubleArray{0.5, 0.5};
  sampled = sampler(rng());
  REQUIRE(sampled == 0.176328510761320473);
  action_count = 0 + 1;
  REQUIRE(action_count == 1);
  call_stack.emplace(call_stack.top());
  call_stack.top().node.Apply(fishbait::Action::kFold);
  // player 0, depth 4, preflop seq 12, action 0, player 2 folded
  // player 2, depth 3, preflop seq 11
  call_stack.pop();
  // player 1, depth 2, preflop seq 8, action 1
  call_stack.pop();
  // player 1, depth 2, preflop seq 8, action 2
  call_stack.emplace(call_stack.top());
  call_stack.top().node.Apply(fishbait::Action::kCheckCall);
  // player 2, depth 3, preflop seq 14
  cluster = info_abstraction.Cluster(call_stack.top().node, 2);
  REQUIRE(cluster == 3);
  call_stack.top().strategy = DoubleArray{0.5, 0.5};
  sampled = sampler(rng());
  REQUIRE(sampled == 0.350764444230192263);
  action_count = 0 + 1;
  REQUIRE(action_count == 1);
  call_stack.emplace(call_stack.top());
  call_stack.top().node.Apply(fishbait::Action::kAllIn);
  // player 0, depth 4, preflop seq 15, action 0
  call_stack.emplace(call_stack.top());
  call_stack.top().node.Apply(fishbait::Action::kFold);
  // player 1, depth 5, preflop seq 16, action 0
  call_stack.emplace(call_stack.top());
  call_stack.top().node.Apply(fishbait::Action::kFold);
  // terminal node
  // player 1, depth 5, preflop seq 16, action 0
  call_stack.pop();
  // player 1, depth 5, preflop seq 16, action 1
  call_stack.emplace(call_stack.top());
  call_stack.top().node.Apply(fishbait::Action::kAllIn);
  // terminal node
  // player 1, depth 5, preflop seq 16, action 1
  call_stack.pop();
  // player 0, depth 4, preflop seq 15, action 0
  call_stack.pop();
  // player 0, depth 4, preflop seq 15, action 1
  call_stack.emplace(call_stack.top());
  call_stack.top().node.Apply(fishbait::Action::kAllIn);
  // player 1, depth 5, preflop seq 17, action 0
  call_stack.emplace(call_stack.top());
  call_stack.top().node.Apply(fishbait::Action::kFold);
  // terminal node
  // player 1, depth 5, preflop seq 17, action 0
  call_stack.pop();
  // player 1, depth 5, preflop seq 17, action 1
  call_stack.emplace(call_stack.top());
  call_stack.top().node.Apply(fishbait::Action::kAllIn);
  // terminal node
  // player 1, depth 5, preflop seq 17, action 1
  call_stack.pop();
  // player 0, depth 4, preflop seq 15, action 1
  call_stack.pop();
  // player 2, depth 3, preflop seq 14
  call_stack.pop();
  // player 1, depth 2, preflop seq 8, action 2
  call_stack.pop();
  // player 0, depth 1, preflop seq 0, action 2
  call_stack.pop();
  // preflop chance node
  call_stack.pop();
  // Iteration 4, player 2, update strategy
  call_stack.pop();
  call_stack.pop();
  REQUIRE(call_stack.size() == 0);

  // Iteration 4, player 2
  sampled = sampler(rng());
  REQUIRE(sampled == 0.865264597970383909);
  // Iteration 4, player 2, no pruning
  call_stack.emplace(start_state, DoubleArray{0}, 0.0, DoubleArray{0});
  // preflop chance node
  call_stack.emplace(call_stack.top());
  call_stack.top().node.Deal();
  call_stack.top().node.ProceedPlay();
  call_stack.emplace(call_stack.top());
  // player 0, depth 1, preflop seq 0
  cluster = info_abstraction.Cluster(call_stack.top().node, 0);
  REQUIRE(cluster == 2);
  call_stack.top().strategy = DoubleArray{1.0/3.0, 1.0/3.0, 1.0/3.0};
  sampled = sampler(rng());
  REQUIRE(sampled == 0.59846611387269677);
  call_stack.emplace(call_stack.top());
  call_stack.top().node.Apply(fishbait::Action::kAllIn);
  // player 1, depth 2, preflop seq 5
  cluster = info_abstraction.Cluster(call_stack.top().node, 1);
  REQUIRE(cluster == 3);
  call_stack.top().strategy = DoubleArray{0.0, 1.0};
  sampled = sampler(rng());
  REQUIRE(sampled == 0.01614554907543608);
  call_stack.emplace(call_stack.top());
  call_stack.top().node.Apply(fishbait::Action::kAllIn);
  // player 2, depth 3, preflop seq 7
  cluster = info_abstraction.Cluster(call_stack.top().node, 2);
  REQUIRE(cluster == 2);
  call_stack.top().strategy = DoubleArray{0.5, 0.5};
  call_stack.top().value = 0.0;
  call_stack.top().action_values = DoubleArray{0.0, 0.0};
  // player 2, depth 3, preflop seq 7, action 0
  call_stack.emplace(call_stack.top());
  call_stack.top().node.Apply(fishbait::Action::kFold);
  // terminal node, player 2 folded
  /* don't deal cards here because our MCCFR implementation would not and that
     would throw the node random number generator out of sync */
  call_stack.top().value = call_stack.top().node.stack(2);
  // player 2, depth 3, preflop seq 7, action 0
  tmp = call_stack.top().value;
  call_stack.pop();
  call_stack.top().action_values[0] = tmp;
  call_stack.top().value += call_stack.top().strategy[0] *
                            call_stack.top().action_values[0];
  // player 2, depth 3, preflop seq 7, action 1
  call_stack.emplace(call_stack.top());
  call_stack.top().node.Apply(fishbait::Action::kAllIn);
  // terminal node
  call_stack.top().node.Deal();  // flop
  call_stack.top().node.ProceedPlay();
  call_stack.top().node.Deal();  // turn
  call_stack.top().node.ProceedPlay();
  call_stack.top().node.Deal();  // river
  call_stack.top().node.ProceedPlay();
  call_stack.top().node.AwardPot(start_state.same_stack_no_rake_);
  call_stack.top().value = call_stack.top().node.stack(2);
  // player 2, depth 3, preflop seq 7, action 1
  tmp = call_stack.top().value;
  call_stack.pop();
  call_stack.top().action_values[1] = tmp;
  call_stack.top().value += call_stack.top().strategy[1] *
                            call_stack.top().action_values[1];
  // player 2, depth 3, preflop seq 7
  cluster = info_abstraction.Cluster(call_stack.top().node, 2);
  REQUIRE(cluster == 2);
  regret = std::rint(0 + call_stack.top().action_values[0] -
                     call_stack.top().value);
  regret = std::max(regret, kRegretFloor);
  REQUIRE(regret == 4950);
  regret = std::rint(0 + call_stack.top().action_values[1] -
                     call_stack.top().value);
  regret = std::max(regret, kRegretFloor);
  REQUIRE(regret == -4950);
  // player 1, depth 2, preflop seq 5
  call_stack.pop();
  // player 0, depth 1, preflop seq 0
  call_stack.pop();
  // preflop chance node
  call_stack.pop();
  // Iteration 4, player 2, no pruning
  call_stack.pop();
  call_stack.pop();
  REQUIRE(call_stack.size() == 0);

  // Iteration 5, player 0
  sampled = sampler(rng());
  REQUIRE(sampled == 0.352083241272808822);
  // Iteration 5, player 0, with pruning
  call_stack.emplace(start_state, DoubleArray{0}, 0.0, DoubleArray{0});
  // preflop chance node
  call_stack.emplace(call_stack.top());
  call_stack.top().node.Deal();
  call_stack.top().node.ProceedPlay();
  call_stack.emplace(call_stack.top());
  // player 0, depth 1, preflop seq 0
  cluster = info_abstraction.Cluster(call_stack.top().node, 0);
  REQUIRE(cluster == 2);
  call_stack.top().strategy = DoubleArray{1.0/3.0, 1.0/3.0, 1.0/3.0};
  call_stack.top().value = 0.0;
  call_stack.top().action_values = DoubleArray{0.0, 0.0, 0.0};
  // player 0, depth 1, preflop seq 0, action 0
  // action 0 pruned
  // player 0, depth 1, preflop seq 0, action 1
  // action 0 pruned
  // player 0, depth 1, preflop seq 0, action 2
  // action 0 pruned
  // player 0, depth 1, preflop seq 0
  // no actions explored
  // preflop chance node
  call_stack.pop();
  // Iteration 5, player 0, with pruning
  call_stack.pop();
  call_stack.pop();
  REQUIRE(call_stack.size() == 0);

  // Iteration 5, player 1
  sampled = sampler(rng());
  REQUIRE(sampled == 0.438323528738039536);
  // Iteration 5, player 1, with pruning
  call_stack.emplace(start_state, DoubleArray{0}, 0.0, DoubleArray{0});
  // preflop chance node
  call_stack.emplace(call_stack.top());
  call_stack.top().node.Deal();
  call_stack.top().node.ProceedPlay();
  call_stack.emplace(call_stack.top());
  // player 0, depth 1, preflop seq 0
  cluster = info_abstraction.Cluster(call_stack.top().node, 0);
  REQUIRE(cluster == 0);
  call_stack.top().strategy = DoubleArray{1.0, 0.0, 0.0};
  sampled = sampler(rng());
  REQUIRE(sampled == 0.150065580621439265);
  call_stack.emplace(call_stack.top());
  call_stack.top().node.Apply(fishbait::Action::kFold);
  // player 1, depth 2, preflop seq 1
  cluster = info_abstraction.Cluster(call_stack.top().node, 1);
  REQUIRE(cluster == 1);
  call_stack.top().strategy = DoubleArray{1.0/3.0, 1.0/3.0, 1.0/3.0};
  call_stack.top().value = 0;
  call_stack.top().action_values = DoubleArray{0.0, 0.0, 0.0};
  // player 1, depth 2, preflop seq 1, action 0
  // action 0 pruned
  // player 1, depth 2, preflop seq 1, action 1
  // action 1 pruned
  // player 1, depth 2, preflop seq 1, action 2
  // action 2 pruned
  // player 1, depth 2, preflop seq 1
  // no actions explored
  // player 0, depth 1, preflop seq 0
  call_stack.pop();
  // preflop chance node
  call_stack.pop();
  // Iteration 5, player 1, with pruning
  call_stack.pop();
  call_stack.pop();
  REQUIRE(call_stack.size() == 0);

  // Iteration 5, player 2
  sampled = sampler(rng());
  REQUIRE(sampled == 0.0009093491813846156);
  // Iteration 5, player 2, with pruning
  call_stack.emplace(start_state, DoubleArray{0}, 0.0, DoubleArray{0});
  // preflop chance node
  call_stack.emplace(call_stack.top());
  call_stack.top().node.Deal();
  call_stack.top().node.ProceedPlay();
  call_stack.emplace(call_stack.top());
  // player 0, depth 1, preflop seq 0
  cluster = info_abstraction.Cluster(call_stack.top().node, 0);
  REQUIRE(cluster == 0);
  call_stack.top().strategy = DoubleArray{1.0, 0.0, 0.0};
  sampled = sampler(rng());
  REQUIRE(sampled == 0.5539439465152324882);
  call_stack.emplace(call_stack.top());
  call_stack.top().node.Apply(fishbait::Action::kFold);
  // player 1, depth 2, preflop seq 1
  cluster = info_abstraction.Cluster(call_stack.top().node, 1);
  REQUIRE(cluster == 0);
  call_stack.top().strategy = DoubleArray{1.0/3.0, 1.0/3.0, 1.0/3.0};
  sampled = sampler(rng());
  REQUIRE(sampled == 0.9924161526832536717);
  call_stack.emplace(call_stack.top());
  call_stack.top().node.Apply(fishbait::Action::kCheckCall);
  // player 2, depth 3, preflop seq 3
  cluster = info_abstraction.Cluster(call_stack.top().node, 2);
  REQUIRE(cluster == 1);
  call_stack.top().strategy = DoubleArray{0.5, 0.5};
  call_stack.top().value = 0.0;
  call_stack.top().action_values = DoubleArray{0.0, 0.0};
  // player 2, depth 3, preflop seq 3, action 0
  // action 0 pruned
  // player 2, depth 3, preflop seq 3, action 1
  // action 1 pruned
  // player 2, depth 3, preflop seq 3
  // no actions explored
  // player 1, depth 2, preflop seq 1
  call_stack.pop();
  // player 0, depth 1, preflop seq 0
  call_stack.pop();
  // preflop chance node
  call_stack.pop();
  // Iteration 5, player 2, with pruning
  call_stack.pop();
  call_stack.pop();
  REQUIRE(call_stack.size() == 0);

  start_state.SetSeed(fishbait::Random::Seed{});
}  // TEST_CASE "mccfr test helper"
