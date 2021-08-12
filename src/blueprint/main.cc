// Copyright 2021 Marzuk Rashid

#include <array>
#include <iostream>

#include "blueprint/sequence_table.h"
#include "blueprint/strategy.h"
#include "poker/node.h"

int main() {
  // std::vector<fishbait::AbstractAction> actions = {
  //      {fishbait::Action::kFold},
  //      {fishbait::Action::kCheckCall},
  //      {fishbait::Action::kBet, 0.25, 1, fishbait::Round::kFlop},
  //      {fishbait::Action::kBet, 1.0/3.0, 1, fishbait::Round::kPreFlop},
  //      {fishbait::Action::kBet, 0.5, 1},
  //      {fishbait::Action::kBet, 2.0/3.0, 1, fishbait::Round::kPreFlop},
  //      {fishbait::Action::kBet, 0.75, 1, fishbait::Round::kFlop},
  //      {fishbait::Action::kBet, 1.0, 1},
  //      {fishbait::Action::kBet, 1.25, 1, fishbait::Round::kPreFlop},
  //      {fishbait::Action::kBet, 4.0/3.0, 1, fishbait::Round::kPreFlop},
  //      {fishbait::Action::kBet, 1.5, 1, fishbait::Round::kFlop},
  //      {fishbait::Action::kBet, 5.0/3.0, 1, fishbait::Round::kPreFlop},
  //      {fishbait::Action::kBet, 1.75, 1, fishbait::Round::kPreFlop},
  //      {fishbait::Action::kBet, 2.0, 1, fishbait::Round::kFlop},
  //      {fishbait::Action::kBet, 3.0, 1, fishbait::Round::kPreFlop},
  //      {fishbait::Action::kAllIn}};
  std::array<fishbait::AbstractAction, 5> actions = {{
      {fishbait::Action::kFold}, {fishbait::Action::kCheckCall},
      {fishbait::Action::kBet, 0.5, 1}, {fishbait::Action::kBet, 1.0},
      {fishbait::Action::kAllIn}
  }};
  fishbait::Node<6> start_state;
  std::array row_counts = fishbait::SequenceTable<6, 5>::Count(actions,
                                                               start_state);
  std::cout << row_counts[0] << std::endl;
  std::cout << row_counts[1] << std::endl;
  std::cout << row_counts[2] << std::endl;
  std::cout << row_counts[3] << std::endl;
  fishbait::Strategy<6, 5> strategy(start_state, actions, 1, 1, 1, 1, 1, 1, 1,
                                    1, 1, 1, "placeholder", true);
}
