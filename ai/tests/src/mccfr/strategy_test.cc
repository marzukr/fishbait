#include <algorithm>
#include <array>
#include <cmath>
#include <filesystem>
#include <functional>
#include <numeric>
#include <stack>
#include <string>
#include <vector>

#include "catch2/catch.hpp"
#include "clustering/cluster_table.h"
#include "clustering/definitions.h"
#include "clustering/test_clusters.h"
#include "mccfr/sequence_table.h"
#include "mccfr/strategy.h"
#include "poker/definitions.h"
#include "poker/indexer.h"
#include "poker/node.h"
#include "utils/cereal.h"
#include "utils/math.h"
#include "utils/print.h"

TEST_CASE("mccfr test", "[mccfr][strategy]") {
  constexpr fishbait::PlayerN kPlayers = 3;
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
  fishbait::TestClusters info_abstraction;
  int prune_constant = 0;
  int regret_floor = -10000;
  fishbait::Strategy s(start_state, actions, info_abstraction,
                       prune_constant, regret_floor);
  s.SetSeed(fishbait::Random::Seed{68});

  std::filesystem::path base_path("out/tests/mccfr");
  std::filesystem::create_directories(base_path);

  // iteration 1
  s.TraverseMCCFR(0, false);
  s.TraverseMCCFR(1, false);
  s.TraverseMCCFR(2, false);

  // iteration 2
  s.SampleAction(fishbait::Round::kPreFlop, 0, 0);  /* just to run the rng like
                                                       we would if we were
                                                       checking for prune */
  s.TraverseMCCFR(0, false);
  s.SampleAction(fishbait::Round::kPreFlop, 0, 0);
  s.TraverseMCCFR(1, true);
  s.SampleAction(fishbait::Round::kPreFlop, 0, 0);
  s.TraverseMCCFR(2, true);
  s.Discount(0.5);

  // iteration 3
  s.SampleAction(fishbait::Round::kPreFlop, 0, 0);
  s.TraverseMCCFR(0, false);
  s.SampleAction(fishbait::Round::kPreFlop, 0, 0);
  s.TraverseMCCFR(1, false);
  s.SampleAction(fishbait::Round::kPreFlop, 0, 0);
  s.TraverseMCCFR(2, true);

  // iteration 4
  s.UpdateStrategy(0);
  s.SampleAction(fishbait::Round::kPreFlop, 0, 0);
  s.TraverseMCCFR(0, false);
  s.UpdateStrategy(1);
  s.SampleAction(fishbait::Round::kPreFlop, 0, 0);
  s.TraverseMCCFR(1, true);
  s.UpdateStrategy(2);
  s.SampleAction(fishbait::Round::kPreFlop, 0, 0);
  s.TraverseMCCFR(2, false);
  std::filesystem::path save_path = base_path / "strategy_4.cereal";
  CerealSave(save_path.string(), &s, false);

  // iteration 5
  s.SampleAction(fishbait::Round::kPreFlop, 0, 0);
  s.TraverseMCCFR(0, true);
  s.SampleAction(fishbait::Round::kPreFlop, 0, 0);
  s.TraverseMCCFR(1, true);
  s.SampleAction(fishbait::Round::kPreFlop, 0, 0);
  s.TraverseMCCFR(2, true);

  // iteration 6
  s.UpdateStrategy(0);
  s.SampleAction(fishbait::Round::kPreFlop, 0, 0);
  s.TraverseMCCFR(0, false);
  s.UpdateStrategy(1);
  s.SampleAction(fishbait::Round::kPreFlop, 0, 0);
  s.TraverseMCCFR(1, false);
  s.UpdateStrategy(2);
  s.SampleAction(fishbait::Round::kPreFlop, 0, 0);
  s.TraverseMCCFR(2, false);
  save_path = base_path / "strategy_6.cereal";
  CerealSave(save_path.string(), &s, false);

  auto strategy_4 =
      fishbait::Strategy<kPlayers, kActions, fishbait::TestClusters>
              ::LoadSnapshot("out/tests/mccfr/strategy_4.cereal");
  std::size_t legal_actions;

  std::vector<fishbait::Regret> it_4_regrets_preflop = {5017, -4983, -33, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, -4983, 17, 4967, 6633, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -10000, 10050, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 2458, -7492, 5033, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
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

  auto strategy_6 =
      fishbait::Strategy<kPlayers, kActions, fishbait::TestClusters>
              ::LoadSnapshot("out/tests/mccfr/strategy_6.cereal");

  std::vector<fishbait::Regret> it_6_regrets_preflop = {5017, -4983, -33, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 49, 5199, 4949, 6633, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 4108, -5842, 1733, 0, 0, 0, 0, 0, 0, 0, 0, -10000,
      10050, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2458, -7492, 5033, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 4950, -4950, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 4166, 4192, 2902, 0, 0, 0, -2525, 2525, 0, 0, 0, 0,
      -2538, 2538, 0, 0, 0, 0, 0, 0, 0, 1733, -1733, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0};
  legal_actions = strategy_6.action_abstraction()
                            .NumLegalActions(fishbait::Round::kPreFlop);
  for (std::size_t i = 0; i < it_6_regrets_preflop.size(); ++i) {
    fishbait::CardCluster cluster = i / legal_actions;
    std::size_t offset = i % legal_actions;
    REQUIRE(strategy_6.regrets()[+fishbait::Round::kPreFlop](cluster, offset) ==
            it_6_regrets_preflop[i]);
  }

  std::vector<fishbait::Regret> it_6_regrets_flop = {0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, -1186, 1186, 0, 0, 0, 0, 0, 0, 4950, -4950, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -2425, 2425, 0, 0, 0, 0, -5050, 5050,
      3467, -3467, 4950, -4950, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2475,
      -2475, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2625,
      -2625, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4950, -4950, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0};
  legal_actions = strategy_6.action_abstraction()
                            .NumLegalActions(fishbait::Round::kFlop);
  for (std::size_t i = 0; i < it_6_regrets_flop.size(); ++i) {
    fishbait::CardCluster cluster = i / legal_actions;
    std::size_t offset = i % legal_actions;
    REQUIRE(strategy_6.regrets()[+fishbait::Round::kFlop](cluster, offset) ==
            it_6_regrets_flop[i]);
  }

  std::vector<fishbait::Regret> it_6_regrets_turn = {0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, -2496, 2579, -84, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, -3217, 1733, 1483, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      4700, -4700, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0};
  legal_actions = strategy_6.action_abstraction()
                            .NumLegalActions(fishbait::Round::kTurn);
  for (std::size_t i = 0; i < it_6_regrets_turn.size(); ++i) {
    fishbait::CardCluster cluster = i / legal_actions;
    std::size_t offset = i % legal_actions;
    REQUIRE(strategy_6.regrets()[+fishbait::Round::kTurn](cluster, offset) ==
            it_6_regrets_turn[i]);
  }

  std::vector<fishbait::Regret> it_6_regrets_river = {0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, -2662, 2662, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -4950, 4950, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  legal_actions = strategy_6.action_abstraction()
                            .NumLegalActions(fishbait::Round::kRiver);
  for (std::size_t i = 0; i < it_6_regrets_river.size(); ++i) {
    fishbait::CardCluster cluster = i / legal_actions;
    std::size_t offset = i % legal_actions;
    REQUIRE(strategy_6.regrets()[+fishbait::Round::kRiver](cluster, offset) ==
            it_6_regrets_river[i]);
  }

  std::vector<fishbait::ActionCount> it_6_action_counts = {0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0,
      0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0,
      0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 1, 1, 0, 0, 0, 0, 1, 1,
      0, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 0};
  legal_actions = strategy_6.action_abstraction()
                            .NumLegalActions(fishbait::Round::kPreFlop);
  for (std::size_t i = 0; i < it_6_action_counts.size(); ++i) {
    fishbait::CardCluster cluster = i / legal_actions;
    std::size_t offset = i % legal_actions;
    REQUIRE(strategy_6.action_counts()(cluster, offset) ==
            it_6_action_counts[i]);
  }

  auto prob_table = strategy_6.InitialAverage();
  auto prob_table_dup = s.InitialAverage();
  auto prob_table_cp = prob_table;
  auto prob_table_cp2 = strategy_4.InitialAverage();
  prob_table_cp2 = prob_table_cp;
  REQUIRE(prob_table == prob_table_dup);
  REQUIRE(prob_table == prob_table_cp);
  REQUIRE(prob_table_cp2 == prob_table);
  prob_table_cp2 = strategy_6.InitialAverage();
  REQUIRE(prob_table_cp2 == prob_table_cp);
  REQUIRE(prob_table.action_abstraction() == strategy_6.action_abstraction());
  prob_table = prob_table_cp2;
  auto& probabilities = prob_table.probabilities();

  std::vector<float> prob_table_preflop = {0.333333333, 0.333333333,
      0.333333333, 0.333333333, 0.333333333, 0.333333333, 0.5, 0.5, 0.5, 0.5,
      0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.333333333, 0.333333333,
      0.333333333, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5,
      0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0, 0, 1, 1, 0, 0, 0.5, 0.5, 0.5, 0.5, 0.5,
      0.5, 1, 0, 0.5, 0.5, 0.5, 0.5, 0, 1, 0, 0.5, 0.5, 1, 0, 0.5, 0.5, 0, 1, 0,
      1, 0.5, 0.5, 1, 0, 0.5, 0.5, 0.5, 0.5, 0.333333333, 0.333333333,
      0.333333333, 0.333333333, 0.333333333, 0.333333333, 1, 0, 1, 0, 0.5, 0.5,
      0.5, 0.5, 0, 1, 1, 0, 0.333333333, 0.333333333, 0.333333333, 1, 0, 0.5,
      0.5, 1, 0, 0.5, 0.5, 0.5, 0.5, 0, 1, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 1, 0,
      0, 1, 0, 0, 0, 1, 1, 0, 0.5, 0.5, 0, 1, 1, 0, 1, 0, 0, 0, 1, 1, 0, 0.5,
      0.5, 1, 0, 0.5, 0.5, 0.5, 0.5, 1, 0, 0.5, 0.5, 1, 0, 1, 0};
  legal_actions = strategy_6.action_abstraction()
                            .NumLegalActions(fishbait::Round::kPreFlop);
  for (std::size_t i = 0; i < prob_table_preflop.size(); ++i) {
    fishbait::CardCluster cluster = i / legal_actions;
    std::size_t offset = i % legal_actions;
    REQUIRE(probabilities[+fishbait::Round::kPreFlop](cluster, offset) ==
            prob_table_preflop[i]);
  }

  std::vector<float> prob_table_flop = {0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5,
      0.5, 0.5, 0.5, 0.5, 0, 1, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 1, 0, 0.5, 0.5,
      0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5,
      0.5, 0, 1, 0.5, 0.5, 0.5, 0.5, 0, 1, 1, 0, 1, 0, 0.5, 0.5, 0.5, 0.5, 0.5,
      0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5,
      0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5,
      0.5, 0.5, 0.5, 1, 0, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5,
      0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5,
      0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5,
      0.5, 0.5, 0.5, 0.5, 1, 0, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5,
      0.5, 1, 0, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5};
  legal_actions = strategy_6.action_abstraction()
                            .NumLegalActions(fishbait::Round::kFlop);
  for (std::size_t i = 0; i < prob_table_flop.size(); ++i) {
    fishbait::CardCluster cluster = i / legal_actions;
    std::size_t offset = i % legal_actions;
    REQUIRE(probabilities[+fishbait::Round::kFlop](cluster, offset) ==
            prob_table_flop[i]);
  }

  std::vector<float> prob_table_turn = {0.333333333, 0.333333333, 0.333333333,
      0.5, 0.5, 0.333333333, 0.333333333, 0.333333333, 0.5, 0.5, 0.333333333,
      0.333333333, 0.333333333, 0.5, 0.5, 0.333333333, 0.333333333, 0.333333333,
      0.5, 0.5, 0.333333333, 0.333333333, 0.333333333, 0.5, 0.5, 0.333333333,
      0.333333333, 0.333333333, 0.5, 0.5, 0.333333333, 0.333333333, 0.333333333,
      0.5, 0.5, 0, 1, 0, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5,
      0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5,
      0.333333333, 0.333333333, 0.333333333, 0.5, 0.5, 0.333333333, 0.333333333,
      0.333333333, 0.5, 0.5, 0.333333333, 0.333333333, 0.333333333, 0.5, 0.5,
      0.333333333, 0.333333333, 0.333333333, 0.5, 0.5, 0, 0.538868159,
      0.461131841, 0.5, 0.5, 0.333333333, 0.333333333, 0.333333333, 0.5, 0.5,
      0.333333333, 0.333333333, 0.333333333, 0.5, 0.5, 0.333333333, 0.333333333,
      0.333333333, 1, 0, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5,
      0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5,
      0.333333333, 0.333333333, 0.333333333, 0.5, 0.5, 0.333333333, 0.333333333,
      0.333333333, 0.5, 0.5, 0.333333333, 0.333333333, 0.333333333, 0.5, 0.5,
      0.333333333, 0.333333333, 0.333333333, 0.5, 0.5, 0.333333333, 0.333333333,
      0.333333333, 0.5, 0.5, 0.333333333, 0.333333333, 0.333333333, 0.5, 0.5,
      0.333333333, 0.333333333, 0.333333333, 0.5, 0.5, 0.333333333, 0.333333333,
      0.333333333, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5,
      0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5,
      0.333333333, 0.333333333, 0.333333333, 0.5, 0.5, 0.333333333, 0.333333333,
      0.333333333, 0.5, 0.5, 0.333333333, 0.333333333, 0.333333333, 0.5, 0.5,
      0.333333333, 0.333333333, 0.333333333, 0.5, 0.5, 0.333333333, 0.333333333,
      0.333333333, 0.5, 0.5, 0.333333333, 0.333333333, 0.333333333, 0.5, 0.5,
      0.333333333, 0.333333333, 0.333333333, 0.5, 0.5, 0.333333333, 0.333333333,
      0.333333333, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5,
      0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5};
  legal_actions = strategy_6.action_abstraction()
                            .NumLegalActions(fishbait::Round::kTurn);
  for (std::size_t i = 0; i < prob_table_turn.size(); ++i) {
    fishbait::CardCluster cluster = i / legal_actions;
    std::size_t offset = i % legal_actions;
    REQUIRE(probabilities[+fishbait::Round::kTurn](cluster, offset) ==
            Approx(prob_table_turn[i]).margin(0.000000001));
  }

  std::vector<float> prob_table_river = {0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5,
      0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5,
      0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5,
      0.5, 0.5, 0.5, 0.5, 0, 1, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5,
      0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5,
      0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5,
      0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5,
      0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5,
      0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5,
      0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5,
      0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5,
      0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0, 1, 0.5, 0.5, 0.5,
      0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5,
      0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5,
      0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5,
      0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5,
      0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5,
      0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5,
      0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5,
      0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5};
  legal_actions = strategy_6.action_abstraction()
                            .NumLegalActions(fishbait::Round::kRiver);
  for (std::size_t i = 0; i < prob_table_river.size(); ++i) {
    fishbait::CardCluster cluster = i / legal_actions;
    std::size_t offset = i % legal_actions;
    REQUIRE(probabilities[+fishbait::Round::kRiver](cluster, offset) ==
            Approx(prob_table_river[i]).margin(0.000000001));
  }

  /* Adding strategy_4 to strategy_6, so the table will have strategy_4's
     preflop strategy from action_counts */
  prob_table += strategy_4;
  prob_table.Normalize();
  REQUIRE(prob_table != prob_table_cp);
  REQUIRE(prob_table != prob_table_dup);
  save_path = base_path / "avg_table.cereal";
  CerealSave(save_path.string(), &prob_table, false);
  auto load_prob_table =
      decltype(strategy_4)::Average::LoadAverage(save_path, false);
  REQUIRE(load_prob_table.action_abstraction() ==
          strategy_4.action_abstraction());
  auto& avg_probs = load_prob_table.probabilities();

  std::vector<float> avg_table_preflop = {0.333333333, 0.333333333, 0.333333333,
      0.333333333, 0.333333333, 0.333333333, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5,
      0.5, 0.5, 0.5, 0.5, 0.5, 0.333333333, 0.333333333, 0.333333333, 0.5, 0.5,
      0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5,
      0.5, 0.333333333, 0.333333333, 0.333333333, 1, 0, 0, 0.5, 0.5, 0.5, 0.5,
      0.5, 0.5, 1, 0, 0.5, 0.5, 0.5, 0.5, 0, 1, 0, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5,
      0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.333333333,
      0.333333333, 0.333333333, 0.333333333, 0.333333333, 0.333333333, 0.5, 0.5,
      0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.333333333,
      0.333333333, 0.333333333, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5,
      0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 1, 0, 0, 0.333333333,
      0.333333333, 0.333333333, 0, 1, 1, 0, 0.5, 0.5, 0.5, 0.5, 1, 0, 1, 0,
      0.333333333, 0.333333333, 0.333333333, 1, 0, 0.5, 0.5, 1, 0, 0.5, 0.5,
      0.5, 0.5, 1, 0, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5};
  legal_actions = strategy_6.action_abstraction()
                            .NumLegalActions(fishbait::Round::kPreFlop);
  for (std::size_t i = 0; i < avg_table_preflop.size(); ++i) {
    fishbait::CardCluster cluster = i / legal_actions;
    std::size_t offset = i % legal_actions;
    REQUIRE(avg_probs[+fishbait::Round::kPreFlop](cluster, offset) ==
            Approx(avg_table_preflop[i]).margin(0.000000001));
  }

  std::vector<float> avg_table_flop = {0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5,
      0.5, 0.5, 0.5, 0.5, 0, 1, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.75, 0.25, 0.5,
      0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5,
      0.5, 0.5, 0, 1, 0.5, 0.5, 0.5, 0.5, 0, 1, 0.75, 0.25, 1, 0, 0.5, 0.5, 0.5,
      0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5,
      0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5,
      0.5, 0.5, 0.5, 0.5, 0.5, 1, 0, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5,
      0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5,
      0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5,
      0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.75, 0.25, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5,
      0.5, 0.5, 0.5, 0.5, 0.75, 0.25, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5,
      0.5, 0.5};
  legal_actions = strategy_6.action_abstraction()
                            .NumLegalActions(fishbait::Round::kFlop);
  for (std::size_t i = 0; i < avg_table_flop.size(); ++i) {
    fishbait::CardCluster cluster = i / legal_actions;
    std::size_t offset = i % legal_actions;
    REQUIRE(avg_probs[+fishbait::Round::kFlop](cluster, offset) ==
            Approx(avg_table_flop[i]).margin(0.000000001));
  }

  std::vector<float> avg_table_turn = {0.333333333, 0.333333333, 0.333333333,
      0.5, 0.5, 0.333333333, 0.333333333, 0.333333333, 0.5, 0.5, 0.333333333,
      0.333333333, 0.333333333, 0.5, 0.5, 0.333333333, 0.333333333, 0.333333333,
      0.5, 0.5, 0.333333333, 0.333333333, 0.333333333, 0.5, 0.5, 0.333333333,
      0.333333333, 0.333333333, 0.5, 0.5, 0.333333333, 0.333333333, 0.333333333,
      0.5, 0.5, 0, 1, 0, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5,
      0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5,
      0.333333333, 0.333333333, 0.333333333, 0.5, 0.5, 0.333333333, 0.333333333,
      0.333333333, 0.5, 0.5, 0.333333333, 0.333333333, 0.333333333, 0.5, 0.5,
      0.333333333, 0.333333333, 0.333333333, 0.5, 0.5, 0.166666667, 0.436100746,
      0.397232587, 0.5, 0.5, 0.333333333, 0.333333333, 0.333333333, 0.5, 0.5,
      0.333333333, 0.333333333, 0.333333333, 0.5, 0.5, 0.333333333, 0.333333333,
      0.333333333, 0.75, 0.25, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5,
      0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5,
      0.333333333, 0.333333333, 0.333333333, 0.5, 0.5, 0.333333333, 0.333333333,
      0.333333333, 0.5, 0.5, 0.333333333, 0.333333333, 0.333333333, 0.5, 0.5,
      0.333333333, 0.333333333, 0.333333333, 0.5, 0.5, 0.333333333, 0.333333333,
      0.333333333, 0.5, 0.5, 0.333333333, 0.333333333, 0.333333333, 0.5, 0.5,
      0.333333333, 0.333333333, 0.333333333, 0.5, 0.5, 0.333333333, 0.333333333,
      0.333333333, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5,
      0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5,
      0.333333333, 0.333333333, 0.333333333, 0.5, 0.5, 0.333333333, 0.333333333,
      0.333333333, 0.5, 0.5, 0.333333333, 0.333333333, 0.333333333, 0.5, 0.5,
      0.333333333, 0.333333333, 0.333333333, 0.5, 0.5, 0.333333333, 0.333333333,
      0.333333333, 0.5, 0.5, 0.333333333, 0.333333333, 0.333333333, 0.5, 0.5,
      0.333333333, 0.333333333, 0.333333333, 0.5, 0.5, 0.333333333, 0.333333333,
      0.333333333, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5,
      0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5};
  legal_actions = strategy_6.action_abstraction()
                            .NumLegalActions(fishbait::Round::kTurn);
  for (std::size_t i = 0; i < avg_table_turn.size(); ++i) {
    fishbait::CardCluster cluster = i / legal_actions;
    std::size_t offset = i % legal_actions;
    REQUIRE(avg_probs[+fishbait::Round::kTurn](cluster, offset) ==
            Approx(avg_table_turn[i]).margin(0.000000001));
  }

  std::vector<float> avg_table_river = {0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5,
      0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5,
      0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5,
      0.5, 0.5, 0.5, 0.5, 0, 1, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5,
      0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5,
      0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5,
      0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5,
      0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5,
      0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5,
      0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5,
      0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5,
      0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.25, 0.75, 0.5, 0.5,
      0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5,
      0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5,
      0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5,
      0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5,
      0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5,
      0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5,
      0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5,
      0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5};
  legal_actions = strategy_6.action_abstraction()
                            .NumLegalActions(fishbait::Round::kRiver);
  for (std::size_t i = 0; i < avg_table_river.size(); ++i) {
    fishbait::CardCluster cluster = i / legal_actions;
    std::size_t offset = i % legal_actions;
    REQUIRE(avg_probs[+fishbait::Round::kRiver](cluster, offset) ==
            Approx(avg_table_river[i]).margin(0.000000001));
  }

  for (int i = 0; i < 10000; ++i) {
    for (fishbait::PlayerId p = 0; p < kPlayers; ++p) {
      s.UpdateStrategy(p);
      s.TraverseMCCFR(p, false);
    }
  }
  s.Discount(1.0/3.0);

  std::vector<fishbait::Regret> discount_regrets_preflop = {4477, 184706, 809,
      9529, 61900, 20124, 31929, 26387, 27131, 19265, -3333, 41694, 55150,
      89188, 51948, 38990, 70557, 32219, 46480, 68960, 38846, 21951, 9535,
      49112, 19650, 28344, 40928, 32458, 42358, 67096, 48749, 21649, 29293,
      36075, 56444, 25286, 17556, 25246, 43460, 60738, 144745, 56995, 13695,
      65008, 26992, 33278, 27055, -1427, 32944, -3333, 41561, 102669, 16590,
      54782, 26398, 51917, 57175, 64545, 42526, 39729, 23704, 5154, 6584, 50527,
      33605, 36586, 28006, 45346, 44961, 73063, 38234, 6324, 45695, 46973,
      20360, 24499, 5288, 48646, 35350, 156439, 68335, 5253, 23856, 63075,
      18745, 33479, 1188, 32421, 3430, 42687, 110330, 17842, 62666, 7189, 49580,
      64477, 25421, 71306, 37664, 1776, 22658, 20783, 48745, -3245, 46158, 3740,
      53747, 68714, 56542, 35082, 444, 43304, 42380, 22786, 19034, 30985, 32939,
      123523, 94031, 24815, 33832, 46372, 38987, 18440, 37023, 19955, 18538,
      17048, 38444, 63790, 83622, 64877, -3333, 64946, 42919, 16621, 71547,
      46906, 20382, 20866, 45067, 32837, 40874, 20058, 41699, 33193, 80628,
      42446, 35358, 6068, 48318, 41109, 3267, 25908, 31108, 32485};
  legal_actions = s.action_abstraction()
                   .NumLegalActions(fishbait::Round::kPreFlop);
  for (std::size_t i = 0; i < discount_regrets_preflop.size(); ++i) {
    fishbait::CardCluster cluster = i / legal_actions;
    std::size_t offset = i % legal_actions;
    REQUIRE(s.regrets()[+fishbait::Round::kPreFlop](cluster, offset) ==
            discount_regrets_preflop[i]);
  }

  std::vector<fishbait::Regret> discount_regrets_flop = {32899, 9142, 3587,
      20054, 14709, 6908, 20928, 23520, 7414, 19299, 19143, 20637, 13748, 10205,
      17582, 13251, 12975, 42111, 23964, 8097, -1127, 12931, 3267, 25462, 7417,
      22627, 20360, 7781, 8156, 18366, -33, 25821, 18037, 4358, 13973, 6375,
      1769, 7386, -3333, 13362, 28345, 24147, 10049, 19870, 8355, 18964, 25967,
      19373, 23584, 17460, 21958, -3333, 11144, 11498, -3333, 23240, 30508,
      25114, 6380, 21311, 5540, 8991, 21071, 4406, 1482, 20516, 7949, 16752,
      3571, 15547, 18119, -3333, 13340, 3719, 1392, 17742, 5863, 8380, 9031,
      -1862, 27276, 19316, 23449, 987, 19737, 6115, 17838, 27633, 19997, 13860,
      24024, 3007, 10487, 12007, 18182, 9989, 12047, 29951, 15059, 13877, 7419,
      7740, 10633, 21190, 6864, 21162, 12854, 13722, 10501, 14268, 16939, 6588,
      17945, -3333, 8403, 14824, 5804, 1624, 4937, 1721, 22363, 34119, 21054,
      8570, 11781, -2056, 3630, 33278, 24113, 9085, -33, 21102, 7393, 14901,
      8894, 20057, 38915, 10678, 23447, 588, 8876, 8901, 20586, 1510, 14018,
      11850, 18125, 3367, -3333, 16281, 23272, 8765, 1308, 12265, 14139, 67,
      4812, 11095, 13423, 5723};
  legal_actions = s.action_abstraction()
                   .NumLegalActions(fishbait::Round::kFlop);
  for (std::size_t i = 0; i < discount_regrets_flop.size(); ++i) {
    fishbait::CardCluster cluster = i / legal_actions;
    std::size_t offset = i % legal_actions;
    REQUIRE(s.regrets()[+fishbait::Round::kFlop](cluster, offset) ==
            discount_regrets_flop[i]);
  }

  std::vector<fishbait::Regret> discount_regrets_turn = {11975, 20082, 25790,
      13947, 3298, -3113, 7243, 6030, 4344, 13406, 9152, 18607, 760, -3333,
      1750, 5247, 3947, 9592, 3345, 18227, 1698, 3540, 10687, 12156, 3820, 6226,
      2424, 2451, 9656, 6217, 5323, -3333, 4552, -200, 3550, 7767, -374, 1821,
      8032, 1698, 14385, 6475, -3333, 11891, 3267, 8300, 6595, 1812, 6827, 5378,
      5221, 3854, -3333, 5120, 5218, 3367, 5200, 1765, 8280, 1938, 1600, 1700,
      -3333, 3350, 25111, 7925, 15437, 288, 13340, -3333, 2151, 5631, 17203,
      990, 13103, -2859, -1268, 3856, -2864, 6717, 3760, 8453, 17207, 2168,
      11445, 7007, 10106, 10295, 10112, 4163, -3220, -3333, 2085, 12801, -33,
      5114, 80, 7701, 1009, 9973, 5103, 5430, -3333, 10786, 10797, 7242, -3333,
      12277, 5136, 3769, 5620, 11037, -3333, 8482, -100, 3400, 1778, -3105,
      5790, -3333, -2917, 3483, 5483, 4100, -3333, 1700, 1650, 3367, 7911,
      21319, 13617, 7132, 11198, 10937, -1218, 3333, 10280, 6163, 4762, 11077,
      12724, -3333, 1750, 10923, -3333, 4334, 11602, 14945, 4437, 7528, 10903,
      5896, 9070, 4778, -3333, -3333, 6691, -3333, -3333, 1821, -3333, 5661,
      -2906, 3731, 10337, 1333, 9148, -3333, 15892, 4786, 5342, 10519, -1887,
      6980, -3333, 10958, 14330, 2858, 5724, -3333, -3333, 1700, -33, 9511,
      1466, 7376, 7350, 8200, 1600, 1700, 1650, -3333, 17673, -1569, 29490,
      12051, 9004, 10141, 8724, 2590, 9221, 10620, 15068, 5521, 9324, 4852, 602,
      6513, -3045, 4378, 11980, 11316, 13792, 7917, 5252, 7823, 7084, 6759,
      2424, 8645, 4177, 8510, 3090, 3757, 906, -200, 8565, 4225, 7333, 7025,
      6387, 9873, 5390, 16054, 10944, 3032, -3333, 1700, 2591, 7439, -3333, 800,
      5790, -3333, 3267, 3400, 4469, 11552, 117, 1647, 4849, 1749, 1600, 1700,
      1650, -1650};
  legal_actions = s.action_abstraction()
                   .NumLegalActions(fishbait::Round::kTurn);
  for (std::size_t i = 0; i < discount_regrets_turn.size(); ++i) {
    fishbait::CardCluster cluster = i / legal_actions;
    std::size_t offset = i % legal_actions;
    REQUIRE(s.regrets()[+fishbait::Round::kTurn](cluster, offset) ==
            discount_regrets_turn[i]);
  }

  std::vector<fishbait::Regret> discount_regrets_river = {-3333, 7357, -3333,
      9600, 33, -3333, -3333, 13615, 9049, 3822, 5416, 3749, 1583, -3333, 8819,
      6752, 6571, 3618, -167, 6698, 1583, -167, 2833, 6454, 4998, 5879, 8730,
      -753, -1483, 1650, 11940, -1437, 208, 2933, 1567, -3333, 0, 0, -3333,
      3550, 887, -1562, 3164, 11306, 224, -809, -3333, 9236, 8150, 5342, -33,
      5001, 3489, 389, 3865, 2281, 6869, 69, 1650, -3333, 1650, 67, 1650, 3367,
      -3333, 1650, 3267, 3350, 3200, 3400, 0, 0, 6605, 7228, 9875, -3333, 33,
      -33, 17548, 4498, 12700, 6561, 1093, 5593, 1583, 1583, 10389, 10255, 5934,
      -3333, 4975, 423, 4164, 3497, 9398, 4397, 6680, -303, 3287, 50, 42, -3333,
      7683, 6733, -679, 679, 0, 0, 0, 0, -1775, 1775, 1037, 2187, 8608, 4206,
      208, -3333, 2933, 1775, 3317, 5006, 1650, -3333, 1650, 67, 1650, -1650,
      50, -3333, -33, 3350, -3333, 1700, -3333, 8821, 1650, -3333, 1600, 1700,
      -1700, 1700, 0, 0, 8408, -3333, 3961, 6561, -3333, 1650, 8999, 14276,
      -1029, 12152, 1583, -1583, 3167, 0, 7610, 8101, 958, -3333, 1601, 3382,
      4955, -2712, 7020, -3333, -3333, 4752, 6787, 387, 3300, 3225, -3333, 4506,
      413, 3863, 0, 0, 0, 0, 1567, -3333, -3333, 3311, 11744, 4002, 4753, -39,
      1947, -3333, -3333, 5498, 3489, -3333, -1750, 1700, 1650, 3367, 17, 825,
      1778, -3333, 1600, 1700, -33, 3350, -1550, 1650, 3397, -3333, 0, 0, 1650,
      50, 7906, 3017, 6220, 8437, 33, -3333, 15860, 8344, 5609, 7443, -3333,
      1750, 2980, -3333, 8607, 5614, -2708, 2065, 6397, -2919, -3333, 3167,
      8848, 3098, 3775, 3697, 4031, 6065, 3299, 3141, -3333, 8344, 4635, 301,
      1567, -1567, 1567, -1567, 3501, -2349, 4178, -2947, -3333, 4237, 208,
      -3333, -1490, 7902, 3191, -3333, -247, 6653, 3349, 149, 0, 0, 0, 0, -3333,
      3350, -100, 3400, 5790, 3367, 1650, -133, -3333, 3350, 0, 0, 0, 0};
  legal_actions = s.action_abstraction()
                   .NumLegalActions(fishbait::Round::kRiver);
  for (std::size_t i = 0; i < discount_regrets_river.size(); ++i) {
    fishbait::CardCluster cluster = i / legal_actions;
    std::size_t offset = i % legal_actions;
    REQUIRE(s.regrets()[+fishbait::Round::kRiver](cluster, offset) ==
            discount_regrets_river[i]);
  }

  std::vector<fishbait::ActionCount> discount_action_counts = {163, 465, 231,
      294, 392, 165, 377, 461, 716, 122, 69, 95, 396, 455, 414, 424, 409, 429,
      217, 465, 169, 573, 264, 83, 148, 170, 668, 84, 147, 70, 161, 653, 185,
      93, 138, 112, 57, 57, 112, 165, 479, 194, 71, 580, 174, 313, 537, 338,
      512, 42, 131, 489, 336, 386, 465, 297, 554, 393, 302, 131, 644, 207, 105,
      90, 445, 405, 90, 104, 55, 139, 689, 162, 106, 89, 49, 81, 43, 88, 182,
      464, 173, 241, 177, 392, 346, 477, 187, 636, 112, 280, 591, 218, 458, 365,
      261, 562, 202, 290, 317, 585, 238, 94, 79, 318, 505, 54, 118, 78, 95, 471,
      352, 84, 88, 72, 246, 189, 129, 391, 244, 183, 216, 386, 248, 427, 396,
      240, 582, 89, 159, 380, 469, 272, 551, 587, 235, 156, 440, 253, 517, 305,
      81, 102, 477, 345, 63, 121, 124, 60, 458, 364, 95, 89, 43, 210, 107, 146};
  legal_actions = s.action_abstraction()
                   .NumLegalActions(fishbait::Round::kPreFlop);
  for (std::size_t i = 0; i < discount_action_counts.size(); ++i) {
    fishbait::CardCluster cluster = i / legal_actions;
    std::size_t offset = i % legal_actions;
    REQUIRE(s.action_counts()(cluster, offset) == discount_action_counts[i]);
  }

  start_state.SetSeed(fishbait::Random::Seed{});
  s.SetSeed(fishbait::Random::Seed{});
}  // TEST_CASE "mccfr test"

TEST_CASE("mccfr test helper", "[mccfr][strategy]") {
  Catch::StringMaker<double>::precision = 19;
  constexpr int kNumActions = 5;
  constexpr int kPlayers = 3;
  constexpr int kRegretFloor = -10000;
  constexpr int kDiscountInterval = 2;

  fishbait::TestClusters info_abstraction;

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
  chips = call_stack.top().node.ProportionToChips(2.0);
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
  // Iteration 2, player 2, with pruning
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
  // Iteration 2, player 2, with pruning
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
  REQUIRE(regret == -10000);
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
  // Iteration 4, player 1, with pruning
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
  call_stack.emplace(call_stack.top());
  call_stack.top().node.Apply(fishbait::Action::kFold);
  // terminal node, player 1 folded
  call_stack.top().node.AwardPot(start_state.same_stack_no_rake_);
  call_stack.top().value = call_stack.top().node.stack(1);
  // player 1, depth 2, preflop seq 1, action 0
  tmp = call_stack.top().value;
  call_stack.pop();
  call_stack.top().action_values[0] = tmp;
  call_stack.top().value += call_stack.top().strategy[0] *
                            call_stack.top().action_values[0];
  // player 1, depth 2, preflop seq 1, action 1
  // pruned
  // player 1, depth 2, preflop seq 1, action 2
  // pruned
  // player 1, depth 2, preflop seq 1
  cluster = info_abstraction.Cluster(call_stack.top().node, 1);
  REQUIRE(cluster == 1);
  regret = std::rint(0 + call_stack.top().action_values[0] -
                     call_stack.top().value);
  regret = std::max(regret, kRegretFloor);
  REQUIRE(regret == 6633);
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
  call_stack.top().strategy = DoubleArray{1.0, 0.0, 0.0};
  call_stack.top().value = 0;
  call_stack.top().action_values = DoubleArray{0.0, 0.0, 0.0};
  // player 1, depth 2, preflop seq 1, action 0
  call_stack.emplace(call_stack.top());
  call_stack.top().node.Apply(fishbait::Action::kFold);
  // terminal node
  call_stack.top().node.AwardPot(start_state.same_stack_no_rake_);
  call_stack.top().value = call_stack.top().node.stack(1);
  // player 1, depth 2, preflop seq 1, action 0
  tmp = call_stack.top().value;
  call_stack.pop();
  call_stack.top().action_values[0] = tmp;
  call_stack.top().value += call_stack.top().strategy[0] *
                            call_stack.top().action_values[0];
  // player 1, depth 2, preflop seq 1, action 1
  // action 1 pruned
  // player 1, depth 2, preflop seq 1, action 2
  // action 2 pruned
  // player 1, depth 2, preflop seq 1
  cluster = info_abstraction.Cluster(call_stack.top().node, 1);
  REQUIRE(cluster == 1);
  regret = std::rint(6633 + call_stack.top().action_values[0] -
                     call_stack.top().value);
  regret = std::max(regret, kRegretFloor);
  REQUIRE(regret == 6633);
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

  // Iteration 6, player 0, update strategy
  call_stack.emplace(start_state, DoubleArray{0}, 0.0, DoubleArray{0});
  // preflop chance node
  call_stack.emplace(call_stack.top());
  call_stack.top().node.Deal();
  call_stack.top().node.ProceedPlay();
  call_stack.emplace(call_stack.top());
  // player 0, depth 1, preflop seq 0
  cluster = info_abstraction.Cluster(call_stack.top().node, 0);
  REQUIRE(cluster == 1);
  call_stack.top().strategy = DoubleArray{0.0, 17.0/4984, 4967.0/4984};
  sampled = sampler(rng());
  REQUIRE(sampled == 0.9827827818400232118);
  action_count = 0 + 1;
  REQUIRE(action_count == 1);
  call_stack.emplace(call_stack.top());
  call_stack.top().node.Apply(fishbait::Action::kCheckCall);
  // player 1, depth 2, preflop seq 8, action 0
  call_stack.emplace(call_stack.top());
  call_stack.top().node.Apply(fishbait::Action::kFold);
  // player 2, depth 3, preflop seq 9, action 0
  call_stack.emplace(call_stack.top());
  call_stack.top().node.Apply(fishbait::Action::kAllIn);
  // player 0, depth 4, preflop seq 10
  cluster = info_abstraction.Cluster(call_stack.top().node, 0);
  REQUIRE(cluster == 1);
  call_stack.top().strategy = DoubleArray{0.5, 0.5};
  sampled = sampler(rng());
  REQUIRE(sampled == 0.0026918964651754598);
  action_count = 0 + 1;
  REQUIRE(action_count == 1);
  call_stack.emplace(call_stack.top());
  call_stack.top().node.Apply(fishbait::Action::kFold);
  // terminal node, player 0 folded
  // player 0, depth 4, preflop seq 10
  call_stack.pop();
  // player 2, depth 3, preflop seq 9, action 0
  call_stack.pop();
  // player 2, depth 3, preflop seq 9, action 1
  call_stack.emplace(call_stack.top());
  call_stack.top().node.Apply(fishbait::Action::kCheckCall);
  // flop
  // player 2, depth 3, preflop seq 9, action 1
  call_stack.pop();
  // player 1, depth 2, preflop seq 8, action 0
  call_stack.pop();
  // player 1, depth 2, preflop seq 8, action 1
  call_stack.emplace(call_stack.top());
  call_stack.top().node.Apply(fishbait::Action::kAllIn);
  // player 2, depth 3, preflop seq 11, action 0
  call_stack.emplace(call_stack.top());
  call_stack.top().node.Apply(fishbait::Action::kFold);
  // player 0, depth 4, preflop seq 12
  cluster = info_abstraction.Cluster(call_stack.top().node, 0);
  REQUIRE(cluster == 1);
  call_stack.top().strategy = DoubleArray{0.5, 0.5};
  sampled = sampler(rng());
  REQUIRE(sampled == 0.9672540226219451531);
  action_count = 0 + 1;
  REQUIRE(action_count == 1);
  call_stack.emplace(call_stack.top());
  call_stack.top().node.Apply(fishbait::Action::kAllIn);
  // terminal node
  // player 0, depth 4, preflop seq 12
  call_stack.pop();
  // player 2, depth 3, preflop seq 11, action 0
  call_stack.pop();
  // player 2, depth 3, preflop seq 11, action 1
  call_stack.emplace(call_stack.top());
  call_stack.top().node.Apply(fishbait::Action::kAllIn);
  // player 0, depth 4, preflop seq 13
  cluster = info_abstraction.Cluster(call_stack.top().node, 0);
  REQUIRE(cluster == 1);
  call_stack.top().strategy = DoubleArray{0.0, 1.0};
  sampled = sampler(rng());
  REQUIRE(sampled == 0.9248623149258298204);
  action_count = 0 + 1;
  REQUIRE(action_count == 1);
  call_stack.emplace(call_stack.top());
  call_stack.top().node.Apply(fishbait::Action::kAllIn);
  // terminal node
  // player 0, depth 4, preflop seq 13
  call_stack.pop();
  // player 2, depth 3, preflop seq 11, action 1
  call_stack.pop();
  // player 1, depth 2, preflop seq 8, action 1
  call_stack.pop();
  // player 1, depth 2, preflop seq 8, action 2
  call_stack.emplace(call_stack.top());
  call_stack.top().node.Apply(fishbait::Action::kCheckCall);
  // player 2, depth 3, preflop seq 14, action 0
  call_stack.emplace(call_stack.top());
  call_stack.top().node.Apply(fishbait::Action::kAllIn);
  // player 0, depth 4, preflop seq 15
  cluster = info_abstraction.Cluster(call_stack.top().node, 0);
  REQUIRE(cluster == 1);
  call_stack.top().strategy = DoubleArray{0.5, 0.5};
  sampled = sampler(rng());
  REQUIRE(sampled == 0.3397989509912353046);
  action_count = 0 + 1;
  REQUIRE(action_count == 1);
  call_stack.emplace(call_stack.top());
  call_stack.top().node.Apply(fishbait::Action::kFold);
  // player 1, depth 5, preflop seq 16, player 0 folded
  // player 0, depth 4, preflop seq 15
  call_stack.pop();
  // player 2, depth 3, preflop seq 14, action 0
  call_stack.pop();
  // player 2, depth 3, preflop seq 14, action 1
  call_stack.emplace(call_stack.top());
  call_stack.top().node.Apply(fishbait::Action::kCheckCall);
  // flop
  // player 2, depth 3, preflop seq 14, action 1
  call_stack.pop();
  // player 1, depth 2, preflop seq 8, action 2
  call_stack.pop();
  // player 0, depth 1, preflop seq 0
  call_stack.pop();
  // preflop chance node
  call_stack.pop();
  // Iteration 6, player 0, update strategy
  call_stack.pop();
  call_stack.pop();
  REQUIRE(call_stack.size() == 0);

  // Iteration 6, player 0
  sampled = sampler(rng());
  REQUIRE(sampled == 0.7036939186008208669);
  // Iteration 6, player 0, no pruning
  call_stack.emplace(start_state, DoubleArray{0}, 0.0, DoubleArray{0});
  // preflop chance node
  call_stack.emplace(call_stack.top());
  call_stack.top().node.Deal();
  call_stack.top().node.ProceedPlay();
  call_stack.emplace(call_stack.top());
  // player 0, depth 1, preflop seq 0
  cluster = info_abstraction.Cluster(call_stack.top().node, 0);
  REQUIRE(cluster == 1);
  call_stack.top().strategy = DoubleArray{0.0, 17.0/4984, 4967.0/4984};
  call_stack.top().value = 0.0;
  call_stack.top().action_values = DoubleArray{0.0, 0.0};
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
  REQUIRE(sampled == 0.3586705672004562029);
  call_stack.emplace(call_stack.top());
  call_stack.top().node.Apply(fishbait::Action::kFold);
  // player 2, depth 3, preflop seq 6
  cluster = info_abstraction.Cluster(call_stack.top().node, 2);
  REQUIRE(cluster == 3);
  call_stack.top().strategy = DoubleArray{0.5, 0.5};
  sampled = sampler(rng());
  REQUIRE(sampled == 0.3235911491481722191);
  call_stack.emplace(call_stack.top());
  call_stack.top().node.Apply(fishbait::Action::kFold);
  // terminal node
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
  REQUIRE(sampled == 0.8791671949781932893);
  call_stack.emplace(call_stack.top());
  call_stack.top().node.Apply(fishbait::Action::kCheckCall);
  // player 2, depth 3, preflop seq 14
  cluster = info_abstraction.Cluster(call_stack.top().node, 2);
  REQUIRE(cluster == 3);
  call_stack.top().strategy = DoubleArray{0.5, 0.5};
  sampled = sampler(rng());
  REQUIRE(sampled == 0.5039169491183892458);
  call_stack.emplace(call_stack.top());
  call_stack.top().node.Apply(fishbait::Action::kCheckCall);
  // flop chance node
  call_stack.emplace(call_stack.top());
  call_stack.top().node.Deal();
  call_stack.top().node.ProceedPlay();
  // player 1, depth 4, flop seq 8
  cluster = info_abstraction.Cluster(call_stack.top().node, 1);
  REQUIRE(cluster == 3);
  call_stack.top().strategy = DoubleArray{0.5, 0.5};
  sampled = sampler(rng());
  REQUIRE(sampled == 0.3091730971334085143);
  call_stack.emplace(call_stack.top());
  call_stack.top().node.Apply(fishbait::Action::kAllIn);
  // player 2, depth 5, flop seq 9
  cluster = info_abstraction.Cluster(call_stack.top().node, 2);
  REQUIRE(cluster == 0);
  call_stack.top().strategy = DoubleArray{0.5, 0.5};
  sampled = sampler(rng());
  REQUIRE(sampled == 0.3027904453053825162);
  call_stack.emplace(call_stack.top());
  call_stack.top().node.Apply(fishbait::Action::kFold);
  // player 0, depth 6, flop seq 10
  cluster = info_abstraction.Cluster(call_stack.top().node, 0);
  REQUIRE(cluster == 0);
  call_stack.top().strategy = DoubleArray{0.5, 0.5};
  call_stack.top().value = 0.0;
  call_stack.top().action_values = DoubleArray{0.0, 0.0};
  // player 0, depth 6, flop seq 10, action 0
  call_stack.emplace(call_stack.top());
  call_stack.top().node.Apply(fishbait::Action::kFold);
  // terminal node, player 0 folded
  /* don't deal cards here because our MCCFR implementation would not and that
     would throw the node random number generator out of sync */
  call_stack.top().value = call_stack.top().node.stack(0);
  // player 0, depth 6, flop seq 10, action 0
  tmp = call_stack.top().value;
  call_stack.pop();
  call_stack.top().action_values[0] = tmp;
  call_stack.top().value += call_stack.top().strategy[0] *
                            call_stack.top().action_values[0];
  // player 0, depth 6, flop seq 10, action 1
  call_stack.emplace(call_stack.top());
  call_stack.top().node.Apply(fishbait::Action::kAllIn);
  // terminal node
  call_stack.top().node.Deal();  // turn
  call_stack.top().node.ProceedPlay();
  call_stack.top().node.Deal();  // river
  call_stack.top().node.ProceedPlay();
  call_stack.top().node.AwardPot(start_state.same_stack_no_rake_);
  call_stack.top().value = call_stack.top().node.stack(0);
  // player 0, depth 6, flop seq 10, action 1
  tmp = call_stack.top().value;
  call_stack.pop();
  call_stack.top().action_values[1] = tmp;
  call_stack.top().value += call_stack.top().strategy[1] *
                            call_stack.top().action_values[1];
  // player 0, depth 6, flop seq 10
  cluster = info_abstraction.Cluster(call_stack.top().node, 0);
  REQUIRE(cluster == 0);
  regret = std::rint(0 + call_stack.top().action_values[0] -
                     call_stack.top().value);
  regret = std::max(regret, kRegretFloor);
  REQUIRE(regret == 4950);
  regret = std::rint(0 + call_stack.top().action_values[1] -
                     call_stack.top().value);
  regret = std::max(regret, kRegretFloor);
  REQUIRE(regret == -4950);
  // player 2, depth 5, flop seq 9
  tmp = call_stack.top().value;
  call_stack.pop();
  call_stack.top().value = tmp;
  // player 1, depth 4, flop seq 8
  tmp = call_stack.top().value;
  call_stack.pop();
  call_stack.top().value = tmp;
  // flop chance node
  tmp = call_stack.top().value;
  call_stack.pop();
  call_stack.top().value = tmp;
  // player 2, depth 3, preflop seq 14
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
  regret = std::rint(-4983 + call_stack.top().action_values[0] -
                     call_stack.top().value);
  regret = std::max(regret, kRegretFloor);
  REQUIRE(regret == 49);
  regret = std::rint(17 + call_stack.top().action_values[1] -
                     call_stack.top().value);
  regret = std::max(regret, kRegretFloor);
  REQUIRE(regret == 5199);
  regret = std::rint(4967 + call_stack.top().action_values[2] -
                     call_stack.top().value);
  regret = std::max(regret, kRegretFloor);
  REQUIRE(regret == 4949);
  // preflop chance node
  call_stack.pop();
  // Iteration 6, player 0, no pruning
  call_stack.pop();
  call_stack.pop();
  REQUIRE(call_stack.size() == 0);

  // Iteration 6, player 1, update strategy
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
  REQUIRE(cluster == 3);
  call_stack.top().strategy = DoubleArray{1.0/3.0, 1.0/3.0, 1.0/3.0};
  sampled = sampler(rng());
  REQUIRE(sampled == 0.0616434378431672883);
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
  REQUIRE(cluster == 3);
  call_stack.top().strategy = DoubleArray{0.0, 1.0};
  sampled = sampler(rng());
  REQUIRE(sampled == 0.893250337262604388);
  action_count = 0 + 1;
  REQUIRE(action_count == 1);
  call_stack.emplace(call_stack.top());
  call_stack.top().node.Apply(fishbait::Action::kAllIn);
  // player 2, depth 3, preflop seq 7, action 0
  call_stack.emplace(call_stack.top());
  call_stack.top().node.Apply(fishbait::Action::kFold);
  // terminal node
  // player 2, depth 3, preflop seq 7, action 0
  call_stack.pop();
  // player 2, depth 3, preflop seq 7, action 1
  call_stack.emplace(call_stack.top());
  call_stack.top().node.Apply(fishbait::Action::kAllIn);
  // terminal node
  // player 2, depth 3, preflop seq 7, action 1
  call_stack.pop();
  // player 1, depth 2, preflop seq 5
  call_stack.pop();
  // player 0, depth 1, preflop seq 0, action 1
  call_stack.pop();
  // player 0, depth 1, preflop seq 0, action 2
  call_stack.emplace(call_stack.top());
  call_stack.top().node.Apply(fishbait::Action::kCheckCall);
  // player 1, depth 2, preflop seq 8
  cluster = info_abstraction.Cluster(call_stack.top().node, 1);
  REQUIRE(cluster == 3);
  call_stack.top().strategy = DoubleArray{1.0/3.0, 1.0/3.0, 1.0/3.0};
  sampled = sampler(rng());
  REQUIRE(sampled == 0.9796718639843456922);
  action_count = 0 + 1;
  REQUIRE(action_count == 1);
  call_stack.emplace(call_stack.top());
  call_stack.top().node.Apply(fishbait::Action::kCheckCall);
  // player 2, depth 3, preflop seq 14, action 0
  call_stack.emplace(call_stack.top());
  call_stack.top().node.Apply(fishbait::Action::kAllIn);
  // player 0, depth 4, preflop seq 15, action 0
  call_stack.emplace(call_stack.top());
  call_stack.top().node.Apply(fishbait::Action::kFold);
  // player 1, depth 5, preflop seq 16
  cluster = info_abstraction.Cluster(call_stack.top().node, 1);
  REQUIRE(cluster == 3);
  call_stack.top().strategy = DoubleArray{0.5, 0.5};
  sampled = sampler(rng());
  REQUIRE(sampled == 0.1265465722927495984);
  action_count = 0 + 1;
  REQUIRE(action_count == 1);
  call_stack.emplace(call_stack.top());
  call_stack.top().node.Apply(fishbait::Action::kFold);
  // terminal node, player 1 folded
  // player 1, depth 5, preflop seq 16
  call_stack.pop();
  // player 0, depth 4, preflop seq 15, action 0
  call_stack.pop();
  // player 0, depth 4, preflop seq 15, action 1
  call_stack.emplace(call_stack.top());
  call_stack.top().node.Apply(fishbait::Action::kAllIn);
  // player 1, depth 5, preflop seq 17
  cluster = info_abstraction.Cluster(call_stack.top().node, 1);
  REQUIRE(cluster == 3);
  call_stack.top().strategy = DoubleArray{0.5, 0.5};
  sampled = sampler(rng());
  REQUIRE(sampled == 0.3603100969958108468);
  action_count = 0 + 1;
  REQUIRE(action_count == 1);
  call_stack.emplace(call_stack.top());
  call_stack.top().node.Apply(fishbait::Action::kFold);
  // terminal node, player 1 folded
  // player 1, depth 5, preflop seq 17
  call_stack.pop();
  // player 0, depth 4, preflop seq 15, action 1
  call_stack.pop();
  // player 2, depth 3, preflop seq 14, action 0
  call_stack.pop();
  // player 2, depth 3, preflop seq 14, action 1
  call_stack.emplace(call_stack.top());
  call_stack.top().node.Apply(fishbait::Action::kCheckCall);
  // flop
  // player 2, depth 3, preflop seq 14, action 1
  call_stack.pop();
  // player 1, depth 2, preflop seq 8
  call_stack.pop();
  // player 0, depth 1, preflop seq 0, action 2
  call_stack.pop();
  // preflop chance node
  call_stack.pop();
  // Iteration 6, player 1, update strategy
  call_stack.pop();
  call_stack.pop();
  REQUIRE(call_stack.size() == 0);

  // Iteration 6, player 1
  sampled = sampler(rng());
  REQUIRE(sampled == 0.6141347594965277379);
  // Iteration 6, player 1, no pruning
  call_stack.emplace(start_state, DoubleArray{0}, 0.0, DoubleArray{0});
  // preflop chance node
  call_stack.emplace(call_stack.top());
  call_stack.top().node.Deal();
  call_stack.top().node.ProceedPlay();
  call_stack.emplace(call_stack.top());
  // player 0, depth 1, preflop seq 0
  cluster = info_abstraction.Cluster(call_stack.top().node, 0);
  REQUIRE(cluster == 1);
  call_stack.top().strategy = DoubleArray{49.0/10197, 5199.0/10197,
                                          4949.0/10197};
  sampled = sampler(rng());
  REQUIRE(sampled == 0.5325965163399615232);
  call_stack.emplace(call_stack.top());
  call_stack.top().node.Apply(fishbait::Action::kCheckCall);
  // player 1, depth 2, preflop seq 8
  cluster = info_abstraction.Cluster(call_stack.top().node, 1);
  REQUIRE(cluster == 1);
  call_stack.top().strategy = DoubleArray{1.0/3.0, 1.0/3.0, 1.0/3.0};
  call_stack.top().value = 0.0;
  call_stack.top().action_values = DoubleArray{0.0, 0.0, 0.0};
  // player 1, depth 2, preflop seq 8, action 0
  call_stack.emplace(call_stack.top());
  call_stack.top().node.Apply(fishbait::Action::kFold);
  // player 2, depth 3, preflop seq 9, player 1 folded
  call_stack.emplace(call_stack.top());
  call_stack.top().node.Apply(fishbait::Action::kAllIn);
  // player 0, depth 4, preflop seq 10, player 1 folded
  call_stack.emplace(call_stack.top());
  call_stack.top().node.Apply(fishbait::Action::kFold);
  // terminal node
  call_stack.top().node.AwardPot(start_state.same_stack_no_rake_);
  call_stack.top().value = call_stack.top().node.stack(1);
  // player 0, depth 4, preflop seq 10, player 1 folded
  tmp = call_stack.top().value;
  call_stack.pop();
  call_stack.top().value = tmp;
  // player 2, depth 3, preflop seq 9, player 1 folded
  tmp = call_stack.top().value;
  call_stack.pop();
  call_stack.top().value = tmp;
  // player 1, depth 2, preflop seq 8, action 0
  tmp = call_stack.top().value;
  call_stack.pop();
  call_stack.top().action_values[0] = tmp;
  call_stack.top().value += call_stack.top().strategy[0] *
                            call_stack.top().action_values[0];
  // player 1, depth 2, preflop seq 8, action 1
  call_stack.emplace(call_stack.top());
  call_stack.top().node.Apply(fishbait::Action::kAllIn);
  // player 2, depth 3, preflop seq 11
  cluster = info_abstraction.Cluster(call_stack.top().node, 2);
  REQUIRE(cluster == 3);
  call_stack.top().strategy = DoubleArray{0.5, 0.5};
  sampled = sampler(rng());
  REQUIRE(sampled == 0.2910827638247552307);
  call_stack.emplace(call_stack.top());
  call_stack.top().node.Apply(fishbait::Action::kFold);
  // player 0, depth 4, preflop seq 12
  cluster = info_abstraction.Cluster(call_stack.top().node, 0);
  REQUIRE(cluster == 1);
  call_stack.top().strategy = DoubleArray{0.5, 0.5};
  sampled = sampler(rng());
  REQUIRE(sampled == 0.6142515188228341305);
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
  // player 0, depth 4, preflop seq 12
  tmp = call_stack.top().value;
  call_stack.pop();
  call_stack.top().value = tmp;
  // player 2, depth 3, preflop seq 11
  tmp = call_stack.top().value;
  call_stack.pop();
  call_stack.top().value = tmp;
  // player 1, depth 2, preflop seq 8, action 1
  tmp = call_stack.top().value;
  call_stack.pop();
  call_stack.top().action_values[1] = tmp;
  call_stack.top().value += call_stack.top().strategy[1] *
                            call_stack.top().action_values[1];
  // player 1, depth 2, preflop seq 8, action 2
  call_stack.emplace(call_stack.top());
  call_stack.top().node.Apply(fishbait::Action::kCheckCall);
  // player 2, depth 3, preflop seq 14
  cluster = info_abstraction.Cluster(call_stack.top().node, 2);
  REQUIRE(cluster == 3);
  call_stack.top().strategy = DoubleArray{0.5, 0.5};
  sampled = sampler(rng());
  REQUIRE(sampled == 0.8413639921690855283);
  call_stack.emplace(call_stack.top());
  call_stack.top().node.Apply(fishbait::Action::kCheckCall);
  // flop chance node
  call_stack.emplace(call_stack.top());
  call_stack.top().node.Deal();
  call_stack.top().node.ProceedPlay();
  // player 1, depth 4, flop seq 8
  cluster = info_abstraction.Cluster(call_stack.top().node, 1);
  REQUIRE(cluster == 3);
  call_stack.top().strategy = DoubleArray{0.5, 0.5};
  call_stack.top().value = 0.0;
  call_stack.top().action_values = DoubleArray{0.0, 0.0};
  // player 1, depth 4, flop seq 8, action 0
  call_stack.emplace(call_stack.top());
  call_stack.top().node.Apply(fishbait::Action::kAllIn);
  // player 2, depth 5, flop seq 9
  cluster = info_abstraction.Cluster(call_stack.top().node, 2);
  REQUIRE(cluster == 2);
  call_stack.top().strategy = DoubleArray{0.5, 0.5};
  sampled = sampler(rng());
  REQUIRE(sampled == 0.2274679782801657946);
  call_stack.emplace(call_stack.top());
  call_stack.top().node.Apply(fishbait::Action::kFold);
  // player 0, depth 6, flop seq 10
  cluster = info_abstraction.Cluster(call_stack.top().node, 0);
  REQUIRE(cluster == 2);
  call_stack.top().strategy = DoubleArray{0.5, 0.5};
  sampled = sampler(rng());
  REQUIRE(sampled == 0.381294864234608788);
  call_stack.emplace(call_stack.top());
  call_stack.top().node.Apply(fishbait::Action::kFold);
  // terminal node
  call_stack.top().node.AwardPot(start_state.same_stack_no_rake_);
  call_stack.top().value = call_stack.top().node.stack(1);
  // player 0, depth 6, flop seq 10
  tmp = call_stack.top().value;
  call_stack.pop();
  call_stack.top().value = tmp;
  // player 2, depth 5, flop seq 9
  tmp = call_stack.top().value;
  call_stack.pop();
  call_stack.top().value = tmp;
  // player 1, depth 4, flop seq 8, action 0
  tmp = call_stack.top().value;
  call_stack.pop();
  call_stack.top().action_values[0] = tmp;
  call_stack.top().value += call_stack.top().strategy[0] *
                            call_stack.top().action_values[0];
  // player 1, depth 4, flop seq 8, action 1
  call_stack.emplace(call_stack.top());
  call_stack.top().node.Apply(fishbait::Action::kCheckCall);
  // player 2, depth 5, flop seq 12
  cluster = info_abstraction.Cluster(call_stack.top().node, 2);
  REQUIRE(cluster == 2);
  call_stack.top().strategy = DoubleArray{0.5, 0.5};
  sampled = sampler(rng());
  REQUIRE(sampled == 0.2128166464564201776);
  call_stack.emplace(call_stack.top());
  call_stack.top().node.Apply(fishbait::Action::kAllIn);
  // player 0, depth 6, flop seq 13
  cluster = info_abstraction.Cluster(call_stack.top().node, 0);
  REQUIRE(cluster == 2);
  call_stack.top().strategy = DoubleArray{0.5, 0.5};
  sampled = sampler(rng());
  REQUIRE(sampled == 0.3359570485139959928);
  call_stack.emplace(call_stack.top());
  call_stack.top().node.Apply(fishbait::Action::kFold);
  // player 1, depth 7, flop seq 14
  cluster = info_abstraction.Cluster(call_stack.top().node, 1);
  REQUIRE(cluster == 3);
  call_stack.top().strategy = DoubleArray{0.5, 0.5};
  call_stack.top().value = 0.0;
  call_stack.top().action_values = DoubleArray{0.0, 0.0};
  // player 1, depth 7, flop seq 14, action 0
  call_stack.emplace(call_stack.top());
  call_stack.top().node.Apply(fishbait::Action::kFold);
  // terminal node
  call_stack.top().node.AwardPot(start_state.same_stack_no_rake_);
  call_stack.top().value = call_stack.top().node.stack(1);
  // player 1, depth 7, flop seq 14, action 0
  tmp = call_stack.top().value;
  call_stack.pop();
  call_stack.top().action_values[0] = tmp;
  call_stack.top().value += call_stack.top().strategy[0] *
                            call_stack.top().action_values[0];
  // player 1, depth 7, flop seq 14, action 1
  call_stack.emplace(call_stack.top());
  call_stack.top().node.Apply(fishbait::Action::kAllIn);
  // terminal node
  call_stack.top().node.Deal();  // turn
  call_stack.top().node.ProceedPlay();
  call_stack.top().node.Deal();  // river
  call_stack.top().node.ProceedPlay();
  call_stack.top().node.AwardPot(start_state.same_stack_no_rake_);
  call_stack.top().value = call_stack.top().node.stack(1);
  // player 1, depth 7, flop seq 14, action 1
  tmp = call_stack.top().value;
  call_stack.pop();
  call_stack.top().action_values[1] = tmp;
  call_stack.top().value += call_stack.top().strategy[1] *
                            call_stack.top().action_values[1];
  // player 1, depth 7, flop seq 14
  cluster = info_abstraction.Cluster(call_stack.top().node, 1);
  REQUIRE(cluster == 3);
  regret = std::rint(0 + call_stack.top().action_values[0] -
                     call_stack.top().value);
  regret = std::max(regret, kRegretFloor);
  REQUIRE(regret == 4950);
  regret = std::rint(0 + call_stack.top().action_values[1] -
                     call_stack.top().value);
  regret = std::max(regret, kRegretFloor);
  REQUIRE(regret == -4950);
  // player 0, depth 6, flop seq 13
  tmp = call_stack.top().value;
  call_stack.pop();
  call_stack.top().value = tmp;
  // player 2, depth 5, flop seq 12
  tmp = call_stack.top().value;
  call_stack.pop();
  call_stack.top().value = tmp;
  // player 1, depth 4, flop seq 8, action 1
  tmp = call_stack.top().value;
  call_stack.pop();
  call_stack.top().action_values[1] = tmp;
  call_stack.top().value += call_stack.top().strategy[1] *
                            call_stack.top().action_values[1];
  // player 1, depth 4, flop seq 8
  cluster = info_abstraction.Cluster(call_stack.top().node, 1);
  REQUIRE(cluster == 3);
  regret = std::rint(0 + call_stack.top().action_values[0] -
                     call_stack.top().value);
  regret = std::max(regret, kRegretFloor);
  REQUIRE(regret == 2625);
  regret = std::rint(0 + call_stack.top().action_values[1] -
                     call_stack.top().value);
  regret = std::max(regret, kRegretFloor);
  REQUIRE(regret == -2625);
  // flop chance node
  tmp = call_stack.top().value;
  call_stack.pop();
  call_stack.top().value = tmp;
  // player 2, depth 3, preflop seq 14
  tmp = call_stack.top().value;
  call_stack.pop();
  call_stack.top().value = tmp;
  // player 1, depth 2, preflop seq 8, action 2
  tmp = call_stack.top().value;
  call_stack.pop();
  call_stack.top().action_values[2] = tmp;
  call_stack.top().value += call_stack.top().strategy[2] *
                            call_stack.top().action_values[2];
  // player 1, depth 2, preflop seq 8
  cluster = info_abstraction.Cluster(call_stack.top().node, 1);
  REQUIRE(cluster == 1);
  regret = std::rint(0 + call_stack.top().action_values[0] -
                     call_stack.top().value);
  regret = std::max(regret, kRegretFloor);
  REQUIRE(regret == 4108);
  regret = std::rint(0 + call_stack.top().action_values[1] -
                     call_stack.top().value);
  regret = std::max(regret, kRegretFloor);
  REQUIRE(regret == -5842);
  regret = std::rint(0 + call_stack.top().action_values[2] -
                     call_stack.top().value);
  regret = std::max(regret, kRegretFloor);
  REQUIRE(regret == 1733);
  // player 0, depth 1, preflop seq 0
  tmp = call_stack.top().value;
  call_stack.pop();
  call_stack.top().value = tmp;
  // preflop chance node
  tmp = call_stack.top().value;
  call_stack.pop();
  call_stack.top().value = tmp;
  // Iteration 6, player 1, no pruning
  call_stack.pop();
  call_stack.pop();
  REQUIRE(call_stack.size() == 0);

  // Iteration 6, player 2, update strategy
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
  REQUIRE(cluster == 2);
  call_stack.top().strategy = DoubleArray{0.5, 0.5};
  sampled = sampler(rng());
  REQUIRE(sampled == 0.3413161061499349613);
  action_count = 0 + 1;
  REQUIRE(action_count == 1);
  call_stack.emplace(call_stack.top());
  call_stack.top().node.Apply(fishbait::Action::kFold);
  // terminal node, player 2 folded
  // player 2, depth 3, preflop seq 2
  call_stack.pop();
  // player 1, depth 2, preflop seq 1, action 1
  call_stack.pop();
  // player 1, depth 2, preflop seq 1, action 2
  call_stack.emplace(call_stack.top());
  call_stack.top().node.Apply(fishbait::Action::kCheckCall);
  // player 2, depth 3, preflop seq 3
  cluster = info_abstraction.Cluster(call_stack.top().node, 2);
  REQUIRE(cluster == 2);
  call_stack.top().strategy = DoubleArray{0.5, 0.5};
  sampled = sampler(rng());
  REQUIRE(sampled == 0.1585656263789323173);
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
  REQUIRE(cluster == 2);
  call_stack.top().strategy = DoubleArray{0.5, 0.5};
  sampled = sampler(rng());
  REQUIRE(sampled == 0.7643833236095329831);
  action_count = 0 + 1;
  REQUIRE(action_count == 1);
  call_stack.emplace(call_stack.top());
  call_stack.top().node.Apply(fishbait::Action::kAllIn);
  // terminal node
  // player 2, depth 3, preflop seq 6
  call_stack.pop();
  // player 1, depth 2, preflop seq 5, action 0
  call_stack.pop();
  // player 1, depth 2, preflop seq 5, action 1
  call_stack.emplace(call_stack.top());
  call_stack.top().node.Apply(fishbait::Action::kAllIn);
  // player 2, depth 3, preflop seq 7
  cluster = info_abstraction.Cluster(call_stack.top().node, 2);
  REQUIRE(cluster == 2);
  call_stack.top().strategy = DoubleArray{1.0, 0.0};
  sampled = sampler(rng());
  REQUIRE(sampled == 0.4739050638179094221);
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
  REQUIRE(cluster == 2);
  call_stack.top().strategy = DoubleArray{0.5, 0.5};
  sampled = sampler(rng());
  REQUIRE(sampled == 0.4944473652773088701);
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
  REQUIRE(cluster == 2);
  call_stack.top().strategy = DoubleArray{0.5, 0.5};
  sampled = sampler(rng());
  REQUIRE(sampled == 0.1515516488681908913);
  action_count = 0 + 1;
  REQUIRE(action_count == 1);
  call_stack.emplace(call_stack.top());
  call_stack.top().node.Apply(fishbait::Action::kFold);
  // player 0, depth 4, preflop seq 12, player 2 folded
  // player 2, depth 3, preflop seq 11
  call_stack.pop();
  // player 1, depth 2, preflop seq 8, action 1
  call_stack.pop();
  // player 1, depth 2, preflop seq 8, action 2
  call_stack.emplace(call_stack.top());
  call_stack.top().node.Apply(fishbait::Action::kCheckCall);
  // player 2, depth 3, preflop seq 14
  cluster = info_abstraction.Cluster(call_stack.top().node, 2);
  REQUIRE(cluster == 2);
  call_stack.top().strategy = DoubleArray{0.5, 0.5};
  sampled = sampler(rng());
  REQUIRE(sampled == 0.6790400851928969361);
  action_count = 0 + 1;
  REQUIRE(action_count == 1);
  call_stack.emplace(call_stack.top());
  call_stack.top().node.Apply(fishbait::Action::kCheckCall);
  // flop chance node
  // player 2, depth 3, preflop seq 14
  call_stack.pop();
  // player 1, depth 2, preflop seq 8, action 2
  call_stack.pop();
  // player 0, depth 1, preflop seq 0, action 2
  call_stack.pop();
  // preflop chance node
  call_stack.pop();
  // Iteration 6, player 2, update strategy
  call_stack.pop();
  call_stack.pop();
  REQUIRE(call_stack.size() == 0);

  // Iteration 6, player 2
  sampled = sampler(rng());
  REQUIRE(sampled == 0.8313797765959332065);
  // Iteration 6, player 2, no pruning
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
  REQUIRE(sampled == 0.8033923351202154617);
  call_stack.emplace(call_stack.top());
  call_stack.top().node.Apply(fishbait::Action::kCheckCall);
  // player 1, depth 2, preflop seq 8
  cluster = info_abstraction.Cluster(call_stack.top().node, 1);
  REQUIRE(cluster == 3);
  call_stack.top().strategy = DoubleArray{1.0/3.0, 1.0/3.0, 1.0/3.0};
  sampled = sampler(rng());
  REQUIRE(sampled == 0.2694162696746394414);
  call_stack.emplace(call_stack.top());
  call_stack.top().node.Apply(fishbait::Action::kFold);
  // player 2, depth 3, preflop seq 9
  cluster = info_abstraction.Cluster(call_stack.top().node, 2);
  REQUIRE(cluster == 3);
  call_stack.top().strategy = DoubleArray{0.5/1.0, 0.5/1.0};
  call_stack.top().value = 0.0;
  call_stack.top().action_values = DoubleArray{0.0, 0.0};
  // player 2, depth 3, preflop seq 9, action 0
  call_stack.emplace(call_stack.top());
  call_stack.top().node.Apply(fishbait::Action::kAllIn);
  // player 0, depth 4, preflop seq 10
  cluster = info_abstraction.Cluster(call_stack.top().node, 0);
  REQUIRE(cluster == 2);
  call_stack.top().strategy = DoubleArray{0.5, 0.5};
  sampled = sampler(rng());
  REQUIRE(sampled == 0.3714345650860906844);
  call_stack.emplace(call_stack.top());
  call_stack.top().node.Apply(fishbait::Action::kFold);
  // terminal node
  call_stack.top().node.AwardPot(start_state.same_stack_no_rake_);
  call_stack.top().value = call_stack.top().node.stack(2);
  // player 0, depth 4, preflop seq 10
  tmp = call_stack.top().value;
  call_stack.pop();
  call_stack.top().value = tmp;
  // player 2, depth 3, preflop seq 9, action 0
  tmp = call_stack.top().value;
  call_stack.pop();
  call_stack.top().action_values[0] = tmp;
  call_stack.top().value += call_stack.top().strategy[0] *
                            call_stack.top().action_values[0];
  // player 2, depth 3, preflop seq 9, action 1
  call_stack.emplace(call_stack.top());
  call_stack.top().node.Apply(fishbait::Action::kCheckCall);
  // flop chance node
  call_stack.top().node.Deal();
  call_stack.top().node.ProceedPlay();
  call_stack.emplace(call_stack.top());
  // player 2, depth 4, flop seq 4
  cluster = info_abstraction.Cluster(call_stack.top().node, 2);
  REQUIRE(cluster == 1);
  call_stack.top().strategy = DoubleArray{0.5/1.0, 0.5/1.0};
  call_stack.top().value = 0.0;
  call_stack.top().action_values = DoubleArray{0.0, 0.0};
  // player 2, depth 4, flop seq 4, action 0
  call_stack.emplace(call_stack.top());
  call_stack.top().node.Apply(fishbait::Action::kAllIn);
  // player 0, depth 5, flop seq 5
  cluster = info_abstraction.Cluster(call_stack.top().node, 0);
  REQUIRE(cluster == 0);
  call_stack.top().strategy = DoubleArray{0.5, 0.5};
  sampled = sampler(rng());
  REQUIRE(sampled == 0.4188424311526526833);
  call_stack.emplace(call_stack.top());
  call_stack.top().node.Apply(fishbait::Action::kFold);
  // terminal node
  call_stack.top().node.AwardPot(start_state.same_stack_no_rake_);
  call_stack.top().value = call_stack.top().node.stack(2);
  // player 0, depth 5, flop seq 5
  tmp = call_stack.top().value;
  call_stack.pop();
  call_stack.top().value = tmp;
  // player 2, depth 4, flop seq 4, action 0
  tmp = call_stack.top().value;
  call_stack.pop();
  call_stack.top().action_values[0] = tmp;
  call_stack.top().value = call_stack.top().strategy[0] *
                           call_stack.top().action_values[0];
  // player 2, depth 4, flop seq 4, action 1
  call_stack.emplace(call_stack.top());
  call_stack.top().node.Apply(fishbait::Action::kCheckCall);
  // player 0, depth 5, flop seq 6
  cluster = info_abstraction.Cluster(call_stack.top().node, 0);
  REQUIRE(cluster == 0);
  call_stack.top().strategy = DoubleArray{0.0, 1.0};
  sampled = sampler(rng());
  REQUIRE(sampled == 0.1588379735398281756);
  call_stack.emplace(call_stack.top());
  call_stack.top().node.Apply(fishbait::Action::kCheckCall);
  // turn chance node
  call_stack.emplace(call_stack.top());
  call_stack.top().node.Deal();
  call_stack.top().node.ProceedPlay();
  // player 2, depth 6, turn seq 8
  cluster = info_abstraction.Cluster(call_stack.top().node, 2);
  REQUIRE(cluster == 1);
  call_stack.top().strategy = DoubleArray{1.0/3.0, 1.0/3.0, 1.0/3.0};
  call_stack.top().value = 0.0;
  call_stack.top().action_values = DoubleArray{0.0, 0.0, 0.0};
  // player 2, depth 6, turn seq 8, action 0
  call_stack.emplace(call_stack.top());
  call_stack.top().node.Apply(fishbait::Action::kAllIn);
  // player 0, depth 7, turn seq 9
  cluster = info_abstraction.Cluster(call_stack.top().node, 0);
  REQUIRE(cluster == 1);
  call_stack.top().strategy = DoubleArray{0.5, 0.5};
  sampled = sampler(rng());
  REQUIRE(sampled == 0.5213763138023881361);
  call_stack.emplace(call_stack.top());
  call_stack.top().node.Apply(fishbait::Action::kAllIn);
  // terminal node
  call_stack.top().node.Deal();  // river
  call_stack.top().node.ProceedPlay();
  call_stack.top().node.AwardPot(start_state.same_stack_no_rake_);
  call_stack.top().value = call_stack.top().node.stack(2);
  // player 0, depth 7, turn seq 9
  tmp = call_stack.top().value;
  call_stack.pop();
  call_stack.top().value = tmp;
  // player 2, depth 6, turn seq 8, action 0
  tmp = call_stack.top().value;
  call_stack.pop();
  call_stack.top().action_values[0] = tmp;
  call_stack.top().value = call_stack.top().strategy[0] *
                           call_stack.top().action_values[0];
  // player 2, depth 6, turn seq 8, action 1
  call_stack.emplace(call_stack.top());
  call_stack.top().node.Apply(fishbait::Action::kCheckCall);
  // player 0, depth 7, turn seq 10
  cluster = info_abstraction.Cluster(call_stack.top().node, 0);
  REQUIRE(cluster == 1);
  call_stack.top().strategy = DoubleArray{1.0/3.0, 1.0/3.0, 1.0/3.0};
  sampled = sampler(rng());
  REQUIRE(sampled == 0.4512195846711998803);
  call_stack.emplace(call_stack.top());
  call_stack.top().node.Apply(fishbait::Action::kCheckCall);
  // river chance node
  call_stack.emplace(call_stack.top());
  call_stack.top().node.Deal();
  call_stack.top().node.ProceedPlay();
  // player 2, depth 8, river seq 12
  cluster = info_abstraction.Cluster(call_stack.top().node, 0);
  REQUIRE(cluster == 1);
  call_stack.top().strategy = DoubleArray{0.5, 0.5};
  call_stack.top().value = 0.0;
  call_stack.top().action_values = DoubleArray{0.0, 0.0};
  // player 2, depth 8, river seq 12, action 0
  call_stack.emplace(call_stack.top());
  call_stack.top().node.Apply(fishbait::Action::kAllIn);
  // player 0, depth 9, river seq 13
  cluster = info_abstraction.Cluster(call_stack.top().node, 0);
  REQUIRE(cluster == 1);
  call_stack.top().strategy = DoubleArray{0.5, 0.5};
  sampled = sampler(rng());
  REQUIRE(sampled == 0.7552982789088162896);
  call_stack.emplace(call_stack.top());
  call_stack.top().node.Apply(fishbait::Action::kAllIn);
  // terminal node
  call_stack.top().node.AwardPot(start_state.same_stack_no_rake_);
  call_stack.top().value = call_stack.top().node.stack(2);
  // player 0, depth 9, river seq 13
  tmp = call_stack.top().value;
  call_stack.pop();
  call_stack.top().value = tmp;
  // player 2, depth 8, river seq 12, action 0
  tmp = call_stack.top().value;
  call_stack.pop();
  call_stack.top().action_values[0] = tmp;
  call_stack.top().value += call_stack.top().strategy[0] *
                            call_stack.top().action_values[0];
  // player 2, depth 8, river seq 12, action 1
  call_stack.emplace(call_stack.top());
  call_stack.top().node.Apply(fishbait::Action::kCheckCall);
  // player 0, depth 9, river seq 14
  cluster = info_abstraction.Cluster(call_stack.top().node, 0);
  REQUIRE(cluster == 1);
  call_stack.top().strategy = DoubleArray{0.5, 0.5};
  sampled = sampler(rng());
  REQUIRE(sampled == 0.5674347814893229147);
  call_stack.emplace(call_stack.top());
  call_stack.top().node.Apply(fishbait::Action::kCheckCall);
  // terminal node
  call_stack.top().node.AwardPot(start_state.same_stack_no_rake_);
  call_stack.top().value = call_stack.top().node.stack(2);
  // player 0, depth 9, river seq 14
  tmp = call_stack.top().value;
  call_stack.pop();
  call_stack.top().value = tmp;
  // player 2, depth 8, river seq 12, action 1
  tmp = call_stack.top().value;
  call_stack.pop();
  call_stack.top().action_values[1] = tmp;
  call_stack.top().value += call_stack.top().strategy[1] *
                            call_stack.top().action_values[1];
  // player 2, depth 8, river seq 12
  cluster = info_abstraction.Cluster(call_stack.top().node, 2);
  REQUIRE(cluster == 2);
  regret = std::rint(0 + call_stack.top().action_values[0] -
                     call_stack.top().value);
  regret = std::max(regret, kRegretFloor);
  REQUIRE(regret == -4950);
  regret = std::rint(0 + call_stack.top().action_values[1] -
                     call_stack.top().value);
  regret = std::max(regret, kRegretFloor);
  REQUIRE(regret == 4950);
  // river chance node
  tmp = call_stack.top().value;
  call_stack.pop();
  call_stack.top().value = tmp;
  // player 0, depth 7, turn seq 10
  tmp = call_stack.top().value;
  call_stack.pop();
  call_stack.top().value = tmp;
  // player 2, depth 6, turn seq 8, action 1
  tmp = call_stack.top().value;
  call_stack.pop();
  call_stack.top().action_values[1] = tmp;
  call_stack.top().value += call_stack.top().strategy[1] *
                            call_stack.top().action_values[1];
  // player 2, depth 6, turn seq 8, action 2
  call_stack.emplace(call_stack.top());
  chips = call_stack.top().node.ProportionToChips(2.0);
  call_stack.top().node.Apply(fishbait::Action::kBet, chips);
  // player 0, depth 7, turn seq 14
  cluster = info_abstraction.Cluster(call_stack.top().node, 0);
  REQUIRE(cluster == 1);
  call_stack.top().strategy = DoubleArray{1.0/3.0, 1.0/3.0, 1.0/3.0};
  sampled = sampler(rng());
  REQUIRE(sampled == 0.6622614741327468302);
  call_stack.emplace(call_stack.top());
  call_stack.top().node.Apply(fishbait::Action::kAllIn);
  // player 2, depth 8, turn seq 15
  cluster = info_abstraction.Cluster(call_stack.top().node, 2);
  REQUIRE(cluster == 1);
  call_stack.top().strategy = DoubleArray{0.5, 0.5};
  call_stack.top().value = 0.0;
  call_stack.top().action_values = DoubleArray{0.0, 0.0};
  // player 2, depth 8, turn seq 15, action 0
  call_stack.emplace(call_stack.top());
  call_stack.top().node.Apply(fishbait::Action::kFold);
  // terminal node, player 2 folded
  call_stack.top().node.AwardPot(start_state.same_stack_no_rake_);
  call_stack.top().value = call_stack.top().node.stack(2);
  // player 2, depth 8, turn seq 15, action 0
  tmp = call_stack.top().value;
  call_stack.pop();
  call_stack.top().action_values[0] = tmp;
  call_stack.top().value += call_stack.top().strategy[0] *
                            call_stack.top().action_values[0];
  // player 2, depth 8, turn seq 15, action 1
  call_stack.emplace(call_stack.top());
  call_stack.top().node.Apply(fishbait::Action::kAllIn);
  // terminal node, player 2 folded
  call_stack.top().node.Deal();  // river
  call_stack.top().node.ProceedPlay();
  call_stack.top().node.AwardPot(start_state.same_stack_no_rake_);
  call_stack.top().value = call_stack.top().node.stack(2);
  // player 2, depth 8, turn seq 15, action 1
  tmp = call_stack.top().value;
  call_stack.pop();
  call_stack.top().action_values[1] = tmp;
  call_stack.top().value += call_stack.top().strategy[1] *
                            call_stack.top().action_values[1];
  // player 2, depth 8, turn seq 15
  cluster = info_abstraction.Cluster(call_stack.top().node, 2);
  REQUIRE(cluster == 1);
  regret = std::rint(0 + call_stack.top().action_values[0] -
                     call_stack.top().value);
  regret = std::max(regret, kRegretFloor);
  REQUIRE(regret == 4700);
  regret = std::rint(0 + call_stack.top().action_values[1] -
                     call_stack.top().value);
  regret = std::max(regret, kRegretFloor);
  REQUIRE(regret == -4700);
  // player 0, depth 7, turn seq 14
  tmp = call_stack.top().value;
  call_stack.pop();
  call_stack.top().value = tmp;
  // player 2, depth 6, turn seq 8, action 2
  tmp = call_stack.top().value;
  call_stack.pop();
  call_stack.top().action_values[2] = tmp;
  call_stack.top().value += call_stack.top().strategy[2] *
                            call_stack.top().action_values[2];
  // player 2, depth 6, turn seq 8
  cluster = info_abstraction.Cluster(call_stack.top().node, 2);
  REQUIRE(cluster == 1);
  regret = std::rint(0 + call_stack.top().action_values[0] -
                     call_stack.top().value);
  regret = std::max(regret, kRegretFloor);
  REQUIRE(regret == -3217);
  regret = std::rint(0 + call_stack.top().action_values[1] -
                     call_stack.top().value);
  regret = std::max(regret, kRegretFloor);
  REQUIRE(regret == 1733);
  regret = std::rint(0 + call_stack.top().action_values[2] -
                     call_stack.top().value);
  regret = std::max(regret, kRegretFloor);
  REQUIRE(regret == 1483);
  // turn chance node
  tmp = call_stack.top().value;
  call_stack.pop();
  call_stack.top().value = tmp;
  // player 0, depth 5, flop seq 6
  tmp = call_stack.top().value;
  call_stack.pop();
  call_stack.top().value = tmp;
  // player 2, depth 4, flop seq 4, action 1
  tmp = call_stack.top().value;
  call_stack.pop();
  call_stack.top().action_values[1] = tmp;
  call_stack.top().value += call_stack.top().strategy[1] *
                            call_stack.top().action_values[1];
  // player 2, depth 4, flop seq 4
  cluster = info_abstraction.Cluster(call_stack.top().node, 2);
  REQUIRE(cluster == 1);
  regret = std::rint(0 + call_stack.top().action_values[0] -
                     call_stack.top().value);
  regret = std::max(regret, kRegretFloor);
  REQUIRE(regret == 3467);
  regret = std::rint(0 + call_stack.top().action_values[1] -
                     call_stack.top().value);
  regret = std::max(regret, kRegretFloor);
  REQUIRE(regret == -3467);
  // flop chance node
  tmp = call_stack.top().value;
  call_stack.pop();
  call_stack.top().value = tmp;
  // player 2, depth 3, preflop seq 9, action 1
  tmp = call_stack.top().value;
  call_stack.pop();
  call_stack.top().action_values[1] = tmp;
  call_stack.top().value += call_stack.top().strategy[1] *
                            call_stack.top().action_values[1];
  // player 2, depth 3, preflop seq 9
  cluster = info_abstraction.Cluster(call_stack.top().node, 2);
  REQUIRE(cluster == 3);
  regret = std::rint(0 + call_stack.top().action_values[0] -
                     call_stack.top().value);
  regret = std::max(regret, kRegretFloor);
  REQUIRE(regret == 1733);
  regret = std::rint(0 + call_stack.top().action_values[1] -
                     call_stack.top().value);
  regret = std::max(regret, kRegretFloor);
  REQUIRE(regret == -1733);
  // player 1, depth 2, preflop seq 8
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
  // Iteration 6, player 2, no pruning
  call_stack.pop();
  call_stack.pop();
  REQUIRE(call_stack.size() == 0);

  start_state.SetSeed(fishbait::Random::Seed{});
}  // TEST_CASE "mccfr test helper"

TEST_CASE("sample action test", "[mccfr][strategy]") {
  constexpr fishbait::PlayerN kPlayers = 3;
  constexpr int kActions = 5;
  constexpr int kTrials = 60000;

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
  fishbait::TestClusters info_abstraction;
  int prune_constant = 0;
  int regret_floor = -10000;
  fishbait::Strategy s(start_state, actions, info_abstraction,
                       prune_constant, regret_floor);

  std::array<int, 3> observed = {0, 0, 0};
  for (int i = 0; i < kTrials; ++i) {
    std::size_t sampled =
        s.SampleAction(fishbait::Round::kPreFlop, 0, 0).legal_idx;
    observed[sampled] += 1;
  }

  // Chi-Squared test
  std::array<double, 3> expected = {kTrials/3.0, kTrials/3.0, kTrials/3.0};
  REQUIRE(fishbait::ChiSqTestStat(observed, expected) <=
          5.991);  // 0.05 significance level, 2 degrees of freedom

  auto s_avg = s.InitialAverage();
  observed = {0, 0, 0};
  for (int i = 0; i < kTrials; ++i) {
    std::size_t sampled =
        s_avg.SampleAction(fishbait::Round::kPreFlop, 0, 0).legal_idx;
    observed[sampled] += 1;
  }
  std::array policy = s_avg.Policy(fishbait::Round::kPreFlop, 0, 0);
  for (int i = 0; i < 3; ++i) {
    REQUIRE(policy[i] == Approx(1.0/3.0));
  }
  REQUIRE(fishbait::ChiSqTestStat(observed, expected) <= 5.991);
}

TEST_CASE("battle test", "[mccfr][strategy][.]") {
  constexpr fishbait::PlayerN kPlayers = 3;
  constexpr int kActions = 6;
  constexpr int kMeans = 100;
  constexpr int kTrials = 20000;
  constexpr int kTrainTime = 10000;
  constexpr int kAverageInterval = 1000;

  fishbait::Node<kPlayers> start_state;
  std::array<fishbait::AbstractAction, kActions> actions = {{
      {fishbait::Action::kFold},
      {fishbait::Action::kAllIn},
      {fishbait::Action::kCheckCall},

      {fishbait::Action::kBet, 3.0},
      {fishbait::Action::kBet, 2.0},
      {fishbait::Action::kBet, 1.0}
  }};
  fishbait::ClusterTable cluster_table(false);
  int prune_constant = -300000000;
  int regret_floor = -310000000;
  fishbait::Strategy s1(start_state, actions, cluster_table,
                        prune_constant, regret_floor);
  auto s1_in_avg = s1.InitialAverage();
  fishbait::Strategy s2(start_state, actions, cluster_table,
                        prune_constant, regret_floor);
  auto s2_in_avg = s2.InitialAverage();
  REQUIRE(s1_in_avg == s2_in_avg);

  std::vector<double> initial_means = s1_in_avg.BattleStats(s2_in_avg, kMeans,
                                                            kTrials);
  double initial_mean = fishbait::Mean(initial_means);
  double initial_std = fishbait::Std(initial_means, initial_mean);
  double initial_ci = fishbait::CI95(initial_means, initial_std);
  REQUIRE(0 >= initial_mean - initial_ci);
  REQUIRE(0 <= initial_mean + initial_ci);
  for (int i = 0; i < kTrainTime; ++i) {
    for (fishbait::PlayerId p = 0; p < kPlayers; ++p) {
      s1.TraverseMCCFR(p, false);
    }
    if ((i + 1) % kAverageInterval == 0) s1_in_avg += s1;
  }
  auto s1_in_avg_unnormal = s1_in_avg;
  REQUIRE(s1_in_avg != s2_in_avg);
  s1_in_avg.Normalize();
  REQUIRE(s1_in_avg != s1_in_avg_unnormal);
  REQUIRE(s1_in_avg != s2_in_avg);
  REQUIRE(s1_in_avg_unnormal != s2_in_avg);
  std::vector<double> trained_means = s1_in_avg.BattleStats(s2_in_avg, kMeans,
                                                            kTrials);
  double trained_mean = fishbait::Mean(trained_means);
  double trained_std = fishbait::Std(trained_means, trained_mean);
  double trained_ci = fishbait::CI95(trained_means, trained_std);
  REQUIRE(0 < trained_mean - trained_ci);
}  // TEST_CASE "battle test"
