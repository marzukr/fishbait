// Copyright 2021 Marzuk Rashid

#include <iostream>
#include <vector>

#include "blueprint/sequence_table.h"
#include "poker_engine/node.h"

int main() {
// std::vector<blueprint::Action> actions =
//     {{poker_engine::Action::kFold},
//      {poker_engine::Action::kCheckCall},
//      {poker_engine::Action::kBet, 0.25, 1, poker_engine::Round::kFlop},
//      {poker_engine::Action::kBet, 1.0/3.0, 1, poker_engine::Round::kPreFlop},
//      {poker_engine::Action::kBet, 0.5, 1},
//      {poker_engine::Action::kBet, 2.0/3.0, 1, poker_engine::Round::kPreFlop},
//      {poker_engine::Action::kBet, 0.75, 1, poker_engine::Round::kFlop},
//      {poker_engine::Action::kBet, 1.0, 1},
//      {poker_engine::Action::kBet, 1.25, 1, poker_engine::Round::kPreFlop},
//      {poker_engine::Action::kBet, 4.0/3.0, 1, poker_engine::Round::kPreFlop},
//      {poker_engine::Action::kBet, 1.5, 1, poker_engine::Round::kFlop},
//      {poker_engine::Action::kBet, 5.0/3.0, 1, poker_engine::Round::kPreFlop},
//      {poker_engine::Action::kBet, 1.75, 1, poker_engine::Round::kPreFlop},
//      {poker_engine::Action::kBet, 2.0, 1, poker_engine::Round::kFlop},
//      {poker_engine::Action::kBet, 3.0, 1, poker_engine::Round::kPreFlop},
//      {poker_engine::Action::kAllIn}};
  blueprint::Action actions[5] =
      {{poker_engine::Action::kFold},
       {poker_engine::Action::kCheckCall},
       {poker_engine::Action::kBet, 0.5, 1},
       {poker_engine::Action::kBet, 1.0},
       {poker_engine::Action::kAllIn}};
  poker_engine::Node<6> start_state;
  blueprint::SequenceN row_counter[poker_engine::kNRounds];
  blueprint::SequenceTable<6, 5>::Count(actions, start_state, row_counter);
  std::cout << row_counter[0] << std::endl;
  std::cout << row_counter[1] << std::endl;
  std::cout << row_counter[2] << std::endl;
  std::cout << row_counter[3] << std::endl;
}
