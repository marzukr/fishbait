// Copyright 2021 Marzuk Rashid

#include <array>
#include <iostream>

#include "blueprint/sequence_table.h"
#include "blueprint/strategy.h"
#include "clustering/cluster_table.h"
#include "poker/node.h"
#include "utils/random.h"

int main() {
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
       fishbait::Round::kRiver,  3},
  }};
  fishbait::Node<6> start_state;
  std::array row_counts = fishbait::SequenceTable<6, 23>::Count(actions,
                                                                start_state);
  std::cout << row_counts << std::endl;
  fishbait::ClusterTable cluster_table(true);
  fishbait::Strategy strategy(start_state, actions, cluster_table, 1, 1, 1, 1,
                              1, 1, 1, 1, 1, 1, "placeholder",
                              fishbait::Random::Seed{}, true);
}
