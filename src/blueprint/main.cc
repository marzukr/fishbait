// Copyright 2021 Marzuk Rashid

#include <array>
#include <iostream>

#include "blueprint/sequence_table.h"
#include "blueprint/strategy.h"
#include "clustering/cluster_table.h"
#include "poker/node.h"
#include "utils/random.h"

int main() {
  std::array<fishbait::AbstractAction, 34> actions = {{
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

      {fishbait::Action::kBet, 0.4, 1, fishbait::Round::kPreFlop},
      {fishbait::Action::kBet, 0.5, 4, fishbait::Round::kPreFlop},
      {fishbait::Action::kBet, 0.6, 3, fishbait::Round::kPreFlop},
      {fishbait::Action::kBet, 0.75, 3, fishbait::Round::kPreFlop},
      {fishbait::Action::kBet, 0.875, 3, fishbait::Round::kPreFlop},
      {fishbait::Action::kBet, 1, 0, fishbait::Round::kPreFlop},
      {fishbait::Action::kBet, 1.25, 3, fishbait::Round::kPreFlop},
      {fishbait::Action::kBet, 1.5, 2, fishbait::Round::kPreFlop},
      {fishbait::Action::kBet, 2, 0, fishbait::Round::kPreFlop},
      {fishbait::Action::kBet, 2.5, 0, fishbait::Round::kPreFlop},
      {fishbait::Action::kBet, 3, 0, fishbait::Round::kPreFlop},
      {fishbait::Action::kBet, 4, 0, fishbait::Round::kPreFlop},
      {fishbait::Action::kBet, 5, 0, fishbait::Round::kPreFlop},
      {fishbait::Action::kBet, 6, 0, fishbait::Round::kPreFlop},

      {fishbait::Action::kBet, 0.25, 1, fishbait::Round::kFlop},
      {fishbait::Action::kBet, 0.5, 3, fishbait::Round::kFlop},
      {fishbait::Action::kBet, 1, 0, fishbait::Round::kFlop},
      {fishbait::Action::kBet, 2, 0, fishbait::Round::kFlop},

      {fishbait::Action::kBet, 0.5, 1, fishbait::Round::kTurn},
      {fishbait::Action::kBet, 1, 0, fishbait::Round::kTurn},

      {fishbait::Action::kBet, 0.5, 1, fishbait::Round::kRiver},
      {fishbait::Action::kBet, 1, 0, fishbait::Round::kRiver},
  }};
  fishbait::Node<6> start_state;
  std::array row_counts = fishbait::SequenceTable<6, 34>::Count(actions,
                                                                start_state);
  std::cout << row_counts << std::endl;
  // fishbait::Strategy<6, 29> strategy(start_state, actions, 1, 1, 1, 1, 1, 1,
  //                                    1, 1, 1, 1, "placeholder", true);
}
