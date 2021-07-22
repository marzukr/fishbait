// Copyright 2021 Marzuk Rashid

#include <array>
#include <iostream>

#include "blueprint/sequence_table.h"
#include "engine/node.h"

int main() {
  // std::vector<blueprint::Action> actions =
  //     {{engine::Action::kFold},
  //      {engine::Action::kCheckCall},
  //      {engine::Action::kBet, 0.25, 1, engine::Round::kFlop},
  //      {engine::Action::kBet, 1.0/3.0, 1, engine::Round::kPreFlop},
  //      {engine::Action::kBet, 0.5, 1},
  //      {engine::Action::kBet, 2.0/3.0, 1, engine::Round::kPreFlop},
  //      {engine::Action::kBet, 0.75, 1, engine::Round::kFlop},
  //      {engine::Action::kBet, 1.0, 1},
  //      {engine::Action::kBet, 1.25, 1, engine::Round::kPreFlop},
  //      {engine::Action::kBet, 4.0/3.0, 1, engine::Round::kPreFlop},
  //      {engine::Action::kBet, 1.5, 1, engine::Round::kFlop},
  //      {engine::Action::kBet, 5.0/3.0, 1, engine::Round::kPreFlop},
  //      {engine::Action::kBet, 1.75, 1, engine::Round::kPreFlop},
  //      {engine::Action::kBet, 2.0, 1, engine::Round::kFlop},
  //      {engine::Action::kBet, 3.0, 1, engine::Round::kPreFlop},
  //      {engine::Action::kAllIn}};
  std::array<blueprint::Action, 5> actions = {{
      {engine::Action::kFold}, {engine::Action::kCheckCall},
      {engine::Action::kBet, 0.5, 1}, {engine::Action::kBet, 1.0},
      {engine::Action::kAllIn}
  }};
  engine::Node<6> start_state;
  std::array row_counts = blueprint::SequenceTable<6, 5>::Count(actions,
                                                                start_state);
  std::cout << row_counts[0] << std::endl;
  std::cout << row_counts[1] << std::endl;
  std::cout << row_counts[2] << std::endl;
  std::cout << row_counts[3] << std::endl;
}
