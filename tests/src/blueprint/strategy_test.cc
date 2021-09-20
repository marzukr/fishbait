// Copyright 2021 Marzuk Rashid

#include <algorithm>
#include <array>
#include <cmath>
#include <iostream>
#include <stack>
#include <string>

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
  fishbait::Node<3> start_state;
  start_state.SetSeed(fishbait::Random::Seed(7));
  std::array<fishbait::AbstractAction, 5> actions = {{
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
  fishbait::Strategy s(start_state, actions, info_abstraction, iterations,
                       strategy_interval, prune_threshold, prune_probability,
                       prune_constant, LCFR_threshold, discount_interval,
                       regret_floor, snapshot_interval, strategy_delay,
                       save_dir, fishbait::Random::Seed{68}, verbose);
  start_state.SetSeed(fishbait::Random::Seed{});
}  // TEST_CASE "mccfr test"

TEST_CASE("mccfr test helper", "[blueprint][strategy][.]") {
  Catch::StringMaker<double>::precision = 17;
  constexpr int kNumActions = 5;
  constexpr int kPlayers = 3;
  constexpr int kRegretFloor = -15000;

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
  regret = std::rint(0 + call_stack.top().action_values[0] -
                     call_stack.top().value);
  regret = std::max(regret, kRegretFloor);
  REQUIRE(regret == 10000);
  regret = std::rint(0 + call_stack.top().action_values[1] -
                     call_stack.top().value);
  regret = std::max(regret, kRegretFloor);
  REQUIRE(regret == 0);
  regret = std::rint(0 + call_stack.top().action_values[2] -
                     call_stack.top().value);
  regret = std::max(regret, kRegretFloor);
  REQUIRE(regret == 12521);
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

  start_state.SetSeed(fishbait::Random::Seed{});
}  // TEST_CASE "mccfr test helper"
