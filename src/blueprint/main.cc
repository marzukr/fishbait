// Copyright 2021 Marzuk Rashid

#include <array>
#include <iostream>

#include "blueprint/sequence_table.h"
#include "blueprint/strategy.h"
#include "clustering/cluster_table.h"
#include "poker/node.h"
#include "utils/random.h"

int main() {
  fishbait::Node<6> start_state;
  std::array<fishbait::AbstractAction, 23> actions = {{
      {fishbait::Action::kFold},
      {fishbait::Action::kCheckCall},
      {fishbait::Action::kAllIn},

      {fishbait::Action::kBet, 0.4, 1, fishbait::Round::kPreFlop,
       fishbait::Round::kPreFlop},
      {fishbait::Action::kBet, 0.5, 4, fishbait::Round::kPreFlop,
       fishbait::Round::kPreFlop},
      {fishbait::Action::kBet, 0.6, 3, fishbait::Round::kPreFlop,
       fishbait::Round::kPreFlop},
      {fishbait::Action::kBet, 0.75, 3, fishbait::Round::kPreFlop,
       fishbait::Round::kPreFlop},
      {fishbait::Action::kBet, 0.875, 3, fishbait::Round::kPreFlop,
       fishbait::Round::kPreFlop},
      {fishbait::Action::kBet, 1, 0, fishbait::Round::kPreFlop,
       fishbait::Round::kPreFlop},
      {fishbait::Action::kBet, 1.25, 3, fishbait::Round::kPreFlop,
       fishbait::Round::kPreFlop},
      {fishbait::Action::kBet, 1.5, 2, fishbait::Round::kPreFlop,
       fishbait::Round::kPreFlop},
      {fishbait::Action::kBet, 2, 0, fishbait::Round::kPreFlop,
       fishbait::Round::kPreFlop},
      {fishbait::Action::kBet, 2.5, 0, fishbait::Round::kPreFlop,
       fishbait::Round::kPreFlop},
      {fishbait::Action::kBet, 3, 0, fishbait::Round::kPreFlop,
       fishbait::Round::kPreFlop},
      {fishbait::Action::kBet, 4, 0, fishbait::Round::kPreFlop,
       fishbait::Round::kPreFlop},
      {fishbait::Action::kBet, 5, 0, fishbait::Round::kPreFlop,
       fishbait::Round::kPreFlop},
      {fishbait::Action::kBet, 6, 0, fishbait::Round::kPreFlop,
       fishbait::Round::kPreFlop},

      {fishbait::Action::kBet, 0.25, 1, fishbait::Round::kFlop,
       fishbait::Round::kFlop, 2, 2081},
      {fishbait::Action::kBet, 0.5, 3, fishbait::Round::kFlop,
       fishbait::Round::kFlop, 4},
      {fishbait::Action::kBet, 1, 0, fishbait::Round::kFlop,
       fishbait::Round::kFlop, 4},
      {fishbait::Action::kBet, 2, 0, fishbait::Round::kFlop,
       fishbait::Round::kFlop, 4},

      {fishbait::Action::kBet, 0.5, 1, fishbait::Round::kTurn,
       fishbait::Round::kRiver, 3},
      {fishbait::Action::kBet, 1, 0, fishbait::Round::kTurn,
       fishbait::Round::kRiver, 3},
  }};
  fishbait::ClusterTable cluster_table(true);
  constexpr int kStrategyInterval = 200;  // 200 minutes
  constexpr int kPruneThreshold = 200;  // 200 minutes
  constexpr double kPruneProbability = 0.95;
  constexpr fishbait::Regret kPruneConstant = -300000000;
  constexpr int kLCFRThreshold = 400;  // 400 minutes
  constexpr int kDiscountInterval = 10;  // 10 minutes
  constexpr fishbait::Regret kRegretFloor = -310000000;
  constexpr int kSnapshotInterval = 200;  // 200 minutes
  constexpr int kStrategyDelay = 800;  // 800 minutes
  fishbait::Strategy strategy(start_state, actions, cluster_table,
                              kStrategyInterval, kPruneThreshold,
                              kPruneProbability, kPruneConstant, kLCFRThreshold,
                              kDiscountInterval, kRegretFloor,
                              kSnapshotInterval, kStrategyDelay, "run_1",
                              fishbait::Random::Seed{}, true);
}
