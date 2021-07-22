// Copyright 2021 Marzuk Rashid

#include <array>
#include <ios>
#include <iostream>
#include <sstream>

#include "array/array.h"
#include "catch2/catch.hpp"
#include "blueprint/definitions.h"
#include "blueprint/sequence_table.h"
#include "engine/definitions.h"
#include "engine/node.h"

TEST_CASE("6 players all in or fold", "[blueprint][sequence_table]") {
  std::array<blueprint::Action, 2> actions = {{{engine::Action::kFold},
                                               {engine::Action::kAllIn}}};
  engine::Node<6> start_state;

  std::array row_counts = blueprint::SequenceTable<6, 2>::Count(actions,
                                                                start_state);
  REQUIRE((row_counts[0] == 62 && row_counts[1] == 0 && row_counts[2] == 0 &&
           row_counts[3] == 0));

  blueprint::SequenceTable seq{actions, start_state};

  std::stringstream ss;
  ss.precision(10);
  ss << std::fixed << seq;
  std::string seq_str = ss.str();
  REQUIRE(seq_str == "SequenceTable<6, 2> { preflop rows: 62; flop rows: 0; "
                     "turn rows: 0; river rows: 0; memory: 0.0000004619 GB; }");

  using blueprint::kLeafId;
  std::vector<blueprint::SequenceId> preflop_table = {1, 31, 2, 16, 3, 9, 4, 6,
      kLeafId, 5, kLeafId, kLeafId, 7, 8, kLeafId, kLeafId, kLeafId, kLeafId,
      10, 13, 11, 12, kLeafId, kLeafId, kLeafId, kLeafId, 14, 15, kLeafId,
      kLeafId, kLeafId, kLeafId, 17, 24, 18, 21, 19, 20, kLeafId, kLeafId,
      kLeafId, kLeafId, 22, 23, kLeafId, kLeafId, kLeafId, kLeafId, 25, 28, 26,
      27, kLeafId, kLeafId, kLeafId, kLeafId, 29, 30, kLeafId, kLeafId, kLeafId,
      kLeafId, 32, 47, 33, 40, 34, 37, 35, 36, kLeafId, kLeafId, kLeafId,
      kLeafId, 38, 39, kLeafId, kLeafId, kLeafId, kLeafId, 41, 44, 42, 43,
      kLeafId, kLeafId, kLeafId, kLeafId, 45, 46, kLeafId, kLeafId, kLeafId,
      kLeafId, 48, 55, 49, 52, 50, 51, kLeafId, kLeafId, kLeafId, kLeafId, 53,
      54, kLeafId, kLeafId, kLeafId, kLeafId, 56, 59, 57, 58, kLeafId, kLeafId,
      kLeafId, kLeafId, 60, 61, kLeafId, kLeafId, kLeafId, kLeafId};

  REQUIRE(seq.States(engine::Round::kPreFlop) == 62);
  REQUIRE(seq.States(engine::Round::kFlop) == 0);
  REQUIRE(seq.States(engine::Round::kTurn) == 0);
  REQUIRE(seq.States(engine::Round::kRiver) == 0);

  REQUIRE(seq.ActionCount(engine::Round::kPreFlop) == 2);
  REQUIRE(seq.ActionCount(engine::Round::kFlop) == 2);
  REQUIRE(seq.ActionCount(engine::Round::kTurn) == 2);
  REQUIRE(seq.ActionCount(engine::Round::kRiver) == 2);

  for (engine::RoundId i = 0; i < engine::kNRounds; ++i) {
    engine::Round round = engine::GetRound(i);
    nda::const_vector_ref<blueprint::Action> round_actions = seq.Actions(round);
    for (int i = 0; i < round_actions.width(); ++i) {
      REQUIRE(round_actions(i) == actions[i]);
    }
  }

  for (blueprint::SequenceId i = 0; i < 62; ++i) {
    for (nda::index_t j = 0; j < 2; ++j) {
      blueprint::Sequence s = {i, engine::Round::kPreFlop};
      REQUIRE(seq.Next(s, j) == preflop_table[i * 2 + j]);
    }
  }
}  // TEST_CASE "6 players all in or fold"

TEST_CASE("3 player all in fold check pot bet", "[blueprint][sequence_table]") {
  std::array<blueprint::Action, 4> actions = {{
      {engine::Action::kFold},
      {engine::Action::kAllIn},
      {engine::Action::kCheckCall},
      {engine::Action::kBet, 1.0, 1, engine::Round::kPreFlop}
  }};
  engine::Node<3> start_state;

  std::array row_counts = blueprint::SequenceTable<3, 4>::Count(actions,
                                                                start_state);
  REQUIRE((row_counts[0] == 98 && row_counts[1] == 180 &&
           row_counts[2] == 180 && row_counts[3] == 180));

  blueprint::SequenceTable seq{actions, start_state};

  std::stringstream ss;
  ss.precision(10);
  ss << std::fixed << seq;
  std::string seq_str = ss.str();
  REQUIRE(seq_str == "SequenceTable<3, 4> { preflop rows: 98; flop rows: 180; "
                     "turn rows: 180; river rows: 180; "
                     "memory: 0.0000074953 GB; }");

  using blueprint::kIllegalId;
  using blueprint::kLeafId;

  std::vector<blueprint::SequenceId> preflop_table = {1, 11, 14, 56, kLeafId, 2,
      3, 7, kLeafId, kLeafId, kIllegalId, kIllegalId, kIllegalId, 4, kLeafId, 5,
      kLeafId, kLeafId, kIllegalId, kIllegalId, kLeafId, 6, 4, kIllegalId,
      kLeafId, kLeafId, kIllegalId, kIllegalId, kLeafId, 8, 8, 9, kLeafId,
      kLeafId, kIllegalId, kIllegalId, kLeafId, 10, 12, kIllegalId, kLeafId,
      kLeafId, kIllegalId, kIllegalId, 12, 13, kIllegalId, kIllegalId, kLeafId,
      kLeafId, kIllegalId, kIllegalId, kLeafId, kLeafId, kIllegalId, kIllegalId,
      15, 19, 22, 36, kIllegalId, 16, 16, 17, kLeafId, kLeafId, kIllegalId,
      kIllegalId, kLeafId, 18, 20, kIllegalId, kLeafId, kLeafId, kIllegalId,
      kIllegalId, 20, 21, kIllegalId, kIllegalId, kLeafId, kLeafId, kIllegalId,
      kIllegalId, kLeafId, kLeafId, kIllegalId, kIllegalId, kIllegalId, 23, 24,
      26, 24, 25, kIllegalId, kIllegalId, kLeafId, kLeafId, kIllegalId,
      kIllegalId, kLeafId, kLeafId, kIllegalId, kIllegalId, 27, 29, 32,
      kIllegalId, kLeafId, 28, 36, kIllegalId, kLeafId, kLeafId, kIllegalId,
      kIllegalId, 30, 31, kIllegalId, kIllegalId, kLeafId, kLeafId, kIllegalId,
      kIllegalId, kLeafId, kLeafId, kIllegalId, kIllegalId, 40, 33, 44,
      kIllegalId, 34, 35, kIllegalId, kIllegalId, kLeafId, kLeafId, kIllegalId,
      kIllegalId, kLeafId, kLeafId, kIllegalId, kIllegalId, 37, 39, 42, 46,
      kLeafId, 38, 56, kIllegalId, kLeafId, kLeafId, kIllegalId, kIllegalId, 40,
      41, kIllegalId, kIllegalId, kLeafId, kLeafId, kIllegalId, kIllegalId,
      kLeafId, kLeafId, kIllegalId, kIllegalId, 60, 43, 64, kIllegalId, 44, 45,
      kIllegalId, kIllegalId, kLeafId, kLeafId, kIllegalId, kIllegalId, kLeafId,
      kLeafId, kIllegalId, kIllegalId, 47, 49, 52, kIllegalId, kLeafId, 48, 76,
      kIllegalId, kLeafId, kLeafId, kIllegalId, kIllegalId, 50, 51, kIllegalId,
      kIllegalId, kLeafId, kLeafId, kIllegalId, kIllegalId, kLeafId, kLeafId,
      kIllegalId, kIllegalId, 80, 53, 84, kIllegalId, 54, 55, kIllegalId,
      kIllegalId, kLeafId, kLeafId, kIllegalId, kIllegalId, kLeafId, kLeafId,
      kIllegalId, kIllegalId, 57, 61, 64, 78, kLeafId, 58, 96, 59, kLeafId,
      kLeafId, kIllegalId, kIllegalId, kLeafId, 60, 100, kIllegalId, kLeafId,
      kLeafId, kIllegalId, kIllegalId, 62, 63, kIllegalId, kIllegalId, kLeafId,
      kLeafId, kIllegalId, kIllegalId, kLeafId, kLeafId, kIllegalId, kIllegalId,
      104, 65, 108, 68, 66, 67, kIllegalId, kIllegalId, kLeafId, kLeafId,
      kIllegalId, kIllegalId, kLeafId, kLeafId, kIllegalId, kIllegalId, 69, 71,
      74, kIllegalId, kLeafId, 70, 120, kIllegalId, kLeafId, kLeafId,
      kIllegalId, kIllegalId, 72, 73, kIllegalId, kIllegalId, kLeafId, kLeafId,
      kIllegalId, kIllegalId, kLeafId, kLeafId, kIllegalId, kIllegalId, 124, 75,
      128, kIllegalId, 76, 77, kIllegalId, kIllegalId, kLeafId, kLeafId,
      kIllegalId, kIllegalId, kLeafId, kLeafId, kIllegalId, kIllegalId, 79, 81,
      84, 88, kLeafId, 80, 140, kIllegalId, kLeafId, kLeafId, kIllegalId,
      kIllegalId, 82, 83, kIllegalId, kIllegalId, kLeafId, kLeafId, kIllegalId,
      kIllegalId, kLeafId, kLeafId, kIllegalId, kIllegalId, 144, 85, 148,
      kIllegalId, 86, 87, kIllegalId, kIllegalId, kLeafId, kLeafId, kIllegalId,
      kIllegalId, kLeafId, kLeafId, kIllegalId, kIllegalId, 89, 91, 94,
      kIllegalId, kLeafId, 90, 160, kIllegalId, kLeafId, kLeafId, kIllegalId,
      kIllegalId, 92, 93, kIllegalId, kIllegalId, kLeafId, kLeafId, kIllegalId,
      kIllegalId, kLeafId, kLeafId, kIllegalId, kIllegalId, 164, 95, 168,
      kIllegalId, 96, 97, kIllegalId, kIllegalId, kLeafId, kLeafId, kIllegalId,
      kIllegalId, kLeafId, kLeafId, kIllegalId, kIllegalId};
  REQUIRE(seq.States(engine::Round::kPreFlop) == 98);
  REQUIRE(seq.ActionCount(engine::Round::kPreFlop) == 4);
  nda::const_vector_ref<blueprint::Action> round_actions =
      seq.Actions(engine::Round::kPreFlop);
  for (int i = 0; i < round_actions.width(); ++i) {
    REQUIRE(round_actions(i) == actions[i]);
  }
  for (blueprint::SequenceId i = 0; i < 98; ++i) {
    for (nda::index_t j = 0; j < 4; ++j) {
      blueprint::Sequence s = {i, engine::Round::kPreFlop};
      REQUIRE(seq.Next(s, j) == preflop_table[i * 4 + j]);
    }
  }

  std::vector<blueprint::SequenceId> flop_table = {kIllegalId, 1, 2, kLeafId,
      kLeafId, kIllegalId, kIllegalId, 3, kLeafId, kLeafId, kLeafId, kIllegalId,
      kIllegalId, 5, 6, kLeafId, kLeafId, kIllegalId, kIllegalId, 7, 4, kLeafId,
      kLeafId, kIllegalId, kIllegalId, 9, 10, kLeafId, kLeafId, kIllegalId,
      kIllegalId, 11, 8, kLeafId, kLeafId, kIllegalId, kIllegalId, 13, 14,
      kLeafId, kLeafId, kIllegalId, kIllegalId, 15, 12, kLeafId, kLeafId,
      kIllegalId, kIllegalId, 17, 18, kLeafId, kLeafId, kIllegalId, kIllegalId,
      19, 16, kLeafId, kLeafId, kIllegalId, kIllegalId, 21, 22, kLeafId,
      kLeafId, kIllegalId, kIllegalId, 23, 20, kLeafId, kLeafId, kIllegalId,
      kIllegalId, 25, 28, 26, 27, kIllegalId, kLeafId, kLeafId, kIllegalId,
      kLeafId, kLeafId, kIllegalId, kIllegalId, 29, 32, 30, 31, kIllegalId,
      kLeafId, kLeafId, kIllegalId, kLeafId, kLeafId, kIllegalId, kIllegalId,
      33, 24, 34, 35, kIllegalId, kLeafId, kLeafId, kIllegalId, kLeafId,
      kLeafId, kIllegalId, kIllegalId, 37, 38, kLeafId, kLeafId, kIllegalId,
      kIllegalId, 39, 36, kLeafId, kLeafId, kIllegalId, kIllegalId, 41, 42,
      kLeafId, kLeafId, kIllegalId, kIllegalId, 43, 40, kLeafId, kLeafId,
      kIllegalId, kIllegalId, 45, 48, 46, 47, kIllegalId, kLeafId, kLeafId,
      kIllegalId, kLeafId, kLeafId, kIllegalId, kIllegalId, 49, 52, 50, 51,
      kIllegalId, kLeafId, kLeafId, kIllegalId, kLeafId, kLeafId, kIllegalId,
      kIllegalId, 53, 44, 54, 55, kIllegalId, kLeafId, kLeafId, kIllegalId,
      kLeafId, kLeafId, kIllegalId, kIllegalId, 57, 58, kLeafId, kLeafId,
      kIllegalId, kIllegalId, 59, 56, kLeafId, kLeafId, kIllegalId, kIllegalId,
      61, 62, kLeafId, kLeafId, kIllegalId, kIllegalId, 63, 60, kLeafId,
      kLeafId, kIllegalId, kIllegalId, 65, 68, 66, 67, kIllegalId, kLeafId,
      kLeafId, kIllegalId, kLeafId, kLeafId, kIllegalId, kIllegalId, 69, 72, 70,
      71, kIllegalId, kLeafId, kLeafId, kIllegalId, kLeafId, kLeafId,
      kIllegalId, kIllegalId, 73, 64, 74, 75, kIllegalId, kLeafId, kLeafId,
      kIllegalId, kLeafId, kLeafId, kIllegalId, kIllegalId, 77, 78, kLeafId,
      kLeafId, kIllegalId, kIllegalId, 79, 76, kLeafId, kLeafId, kIllegalId,
      kIllegalId, 81, 82, kLeafId, kLeafId, kIllegalId, kIllegalId, 83, 80,
      kLeafId, kLeafId, kIllegalId, kIllegalId, 85, 88, 86, 87, kIllegalId,
      kLeafId, kLeafId, kIllegalId, kLeafId, kLeafId, kIllegalId, kIllegalId,
      89, 92, 90, 91, kIllegalId, kLeafId, kLeafId, kIllegalId, kLeafId,
      kLeafId, kIllegalId, kIllegalId, 93, 84, 94, 95, kIllegalId, kLeafId,
      kLeafId, kIllegalId, kLeafId, kLeafId, kIllegalId, kIllegalId, 97, 98,
      kLeafId, kLeafId, kIllegalId, kIllegalId, 99, 96, kLeafId, kLeafId,
      kIllegalId, kIllegalId, 101, 102, kLeafId, kLeafId, kIllegalId,
      kIllegalId, 103, 100, kLeafId, kLeafId, kIllegalId, kIllegalId, 105, 106,
      kLeafId, kLeafId, kIllegalId, kIllegalId, 107, 104, kLeafId, kLeafId,
      kIllegalId, kIllegalId, 109, 112, 110, 111, kIllegalId, kLeafId, kLeafId,
      kIllegalId, kLeafId, kLeafId, kIllegalId, kIllegalId, 113, 116, 114, 115,
      kIllegalId, kLeafId, kLeafId, kIllegalId, kLeafId, kLeafId, kIllegalId,
      kIllegalId, 117, 108, 118, 119, kIllegalId, kLeafId, kLeafId, kIllegalId,
      kLeafId, kLeafId, kIllegalId, kIllegalId, 121, 122, kLeafId, kLeafId,
      kIllegalId, kIllegalId, 123, 120, kLeafId, kLeafId, kIllegalId,
      kIllegalId, 125, 126, kLeafId, kLeafId, kIllegalId, kIllegalId, 127, 124,
      kLeafId, kLeafId, kIllegalId, kIllegalId, 129, 132, 130, 131, kIllegalId,
      kLeafId, kLeafId, kIllegalId, kLeafId, kLeafId, kIllegalId, kIllegalId,
      133, 136, 134, 135, kIllegalId, kLeafId, kLeafId, kIllegalId, kLeafId,
      kLeafId, kIllegalId, kIllegalId, 137, 128, 138, 139, kIllegalId, kLeafId,
      kLeafId, kIllegalId, kLeafId, kLeafId, kIllegalId, kIllegalId, 141, 142,
      kLeafId, kLeafId, kIllegalId, kIllegalId, 143, 140, kLeafId, kLeafId,
      kIllegalId, kIllegalId, 145, 146, kLeafId, kLeafId, kIllegalId,
      kIllegalId, 147, 144, kLeafId, kLeafId, kIllegalId, kIllegalId, 149, 152,
      150, 151, kIllegalId, kLeafId, kLeafId, kIllegalId, kLeafId, kLeafId,
      kIllegalId, kIllegalId, 153, 156, 154, 155, kIllegalId, kLeafId, kLeafId,
      kIllegalId, kLeafId, kLeafId, kIllegalId, kIllegalId, 157, 148, 158, 159,
      kIllegalId, kLeafId, kLeafId, kIllegalId, kLeafId, kLeafId, kIllegalId,
      kIllegalId, 161, 162, kLeafId, kLeafId, kIllegalId, kIllegalId, 163, 160,
      kLeafId, kLeafId, kIllegalId, kIllegalId, 165, 166, kLeafId, kLeafId,
      kIllegalId, kIllegalId, 167, 164, kLeafId, kLeafId, kIllegalId,
      kIllegalId, 169, 172, 170, 171, kIllegalId, kLeafId, kLeafId, kIllegalId,
      kLeafId, kLeafId, kIllegalId, kIllegalId, 173, 176, 174, 175, kIllegalId,
      kLeafId, kLeafId, kIllegalId, kLeafId, kLeafId, kIllegalId, kIllegalId,
      177, 168, 178, 179, kIllegalId, kLeafId, kLeafId, kIllegalId, kLeafId,
      kLeafId, kIllegalId};
  REQUIRE(seq.States(engine::Round::kFlop) == 180);
  REQUIRE(seq.ActionCount(engine::Round::kFlop) == 3);
  round_actions = seq.Actions(engine::Round::kFlop);
  for (int i = 0; i < round_actions.width(); ++i) {
    REQUIRE(round_actions(i) == actions[i]);
  }
  for (blueprint::SequenceId i = 0; i < 180; ++i) {
    for (nda::index_t j = 0; j < 3; ++j) {
      blueprint::Sequence s = {i, engine::Round::kFlop};
      REQUIRE(seq.Next(s, j) == flop_table[i * 3 + j]);
    }
  }

  std::vector<blueprint::SequenceId> turn_table = {kIllegalId, 1, 2, kLeafId,
      kLeafId, kIllegalId, kIllegalId, 3, kLeafId, kLeafId, kLeafId, kIllegalId,
      kIllegalId, 5, 6, kLeafId, kLeafId, kIllegalId, kIllegalId, 7, 4, kLeafId,
      kLeafId, kIllegalId, kIllegalId, 9, 10, kLeafId, kLeafId, kIllegalId,
      kIllegalId, 11, 8, kLeafId, kLeafId, kIllegalId, kIllegalId, 13, 14,
      kLeafId, kLeafId, kIllegalId, kIllegalId, 15, 12, kLeafId, kLeafId,
      kIllegalId, kIllegalId, 17, 18, kLeafId, kLeafId, kIllegalId, kIllegalId,
      19, 16, kLeafId, kLeafId, kIllegalId, kIllegalId, 21, 22, kLeafId,
      kLeafId, kIllegalId, kIllegalId, 23, 20, kLeafId, kLeafId, kIllegalId,
      kIllegalId, 25, 28, 26, 27, kIllegalId, kLeafId, kLeafId, kIllegalId,
      kLeafId, kLeafId, kIllegalId, kIllegalId, 29, 32, 30, 31, kIllegalId,
      kLeafId, kLeafId, kIllegalId, kLeafId, kLeafId, kIllegalId, kIllegalId,
      33, 24, 34, 35, kIllegalId, kLeafId, kLeafId, kIllegalId, kLeafId,
      kLeafId, kIllegalId, kIllegalId, 37, 38, kLeafId, kLeafId, kIllegalId,
      kIllegalId, 39, 36, kLeafId, kLeafId, kIllegalId, kIllegalId, 41, 42,
      kLeafId, kLeafId, kIllegalId, kIllegalId, 43, 40, kLeafId, kLeafId,
      kIllegalId, kIllegalId, 45, 48, 46, 47, kIllegalId, kLeafId, kLeafId,
      kIllegalId, kLeafId, kLeafId, kIllegalId, kIllegalId, 49, 52, 50, 51,
      kIllegalId, kLeafId, kLeafId, kIllegalId, kLeafId, kLeafId, kIllegalId,
      kIllegalId, 53, 44, 54, 55, kIllegalId, kLeafId, kLeafId, kIllegalId,
      kLeafId, kLeafId, kIllegalId, kIllegalId, 57, 58, kLeafId, kLeafId,
      kIllegalId, kIllegalId, 59, 56, kLeafId, kLeafId, kIllegalId, kIllegalId,
      61, 62, kLeafId, kLeafId, kIllegalId, kIllegalId, 63, 60, kLeafId,
      kLeafId, kIllegalId, kIllegalId, 65, 68, 66, 67, kIllegalId, kLeafId,
      kLeafId, kIllegalId, kLeafId, kLeafId, kIllegalId, kIllegalId, 69, 72, 70,
      71, kIllegalId, kLeafId, kLeafId, kIllegalId, kLeafId, kLeafId,
      kIllegalId, kIllegalId, 73, 64, 74, 75, kIllegalId, kLeafId, kLeafId,
      kIllegalId, kLeafId, kLeafId, kIllegalId, kIllegalId, 77, 78, kLeafId,
      kLeafId, kIllegalId, kIllegalId, 79, 76, kLeafId, kLeafId, kIllegalId,
      kIllegalId, 81, 82, kLeafId, kLeafId, kIllegalId, kIllegalId, 83, 80,
      kLeafId, kLeafId, kIllegalId, kIllegalId, 85, 88, 86, 87, kIllegalId,
      kLeafId, kLeafId, kIllegalId, kLeafId, kLeafId, kIllegalId, kIllegalId,
      89, 92, 90, 91, kIllegalId, kLeafId, kLeafId, kIllegalId, kLeafId,
      kLeafId, kIllegalId, kIllegalId, 93, 84, 94, 95, kIllegalId, kLeafId,
      kLeafId, kIllegalId, kLeafId, kLeafId, kIllegalId, kIllegalId, 97, 98,
      kLeafId, kLeafId, kIllegalId, kIllegalId, 99, 96, kLeafId, kLeafId,
      kIllegalId, kIllegalId, 101, 102, kLeafId, kLeafId, kIllegalId,
      kIllegalId, 103, 100, kLeafId, kLeafId, kIllegalId, kIllegalId, 105, 106,
      kLeafId, kLeafId, kIllegalId, kIllegalId, 107, 104, kLeafId, kLeafId,
      kIllegalId, kIllegalId, 109, 112, 110, 111, kIllegalId, kLeafId, kLeafId,
      kIllegalId, kLeafId, kLeafId, kIllegalId, kIllegalId, 113, 116, 114, 115,
      kIllegalId, kLeafId, kLeafId, kIllegalId, kLeafId, kLeafId, kIllegalId,
      kIllegalId, 117, 108, 118, 119, kIllegalId, kLeafId, kLeafId, kIllegalId,
      kLeafId, kLeafId, kIllegalId, kIllegalId, 121, 122, kLeafId, kLeafId,
      kIllegalId, kIllegalId, 123, 120, kLeafId, kLeafId, kIllegalId,
      kIllegalId, 125, 126, kLeafId, kLeafId, kIllegalId, kIllegalId, 127, 124,
      kLeafId, kLeafId, kIllegalId, kIllegalId, 129, 132, 130, 131, kIllegalId,
      kLeafId, kLeafId, kIllegalId, kLeafId, kLeafId, kIllegalId, kIllegalId,
      133, 136, 134, 135, kIllegalId, kLeafId, kLeafId, kIllegalId, kLeafId,
      kLeafId, kIllegalId, kIllegalId, 137, 128, 138, 139, kIllegalId, kLeafId,
      kLeafId, kIllegalId, kLeafId, kLeafId, kIllegalId, kIllegalId, 141, 142,
      kLeafId, kLeafId, kIllegalId, kIllegalId, 143, 140, kLeafId, kLeafId,
      kIllegalId, kIllegalId, 145, 146, kLeafId, kLeafId, kIllegalId,
      kIllegalId, 147, 144, kLeafId, kLeafId, kIllegalId, kIllegalId, 149, 152,
      150, 151, kIllegalId, kLeafId, kLeafId, kIllegalId, kLeafId, kLeafId,
      kIllegalId, kIllegalId, 153, 156, 154, 155, kIllegalId, kLeafId, kLeafId,
      kIllegalId, kLeafId, kLeafId, kIllegalId, kIllegalId, 157, 148, 158, 159,
      kIllegalId, kLeafId, kLeafId, kIllegalId, kLeafId, kLeafId, kIllegalId,
      kIllegalId, 161, 162, kLeafId, kLeafId, kIllegalId, kIllegalId, 163, 160,
      kLeafId, kLeafId, kIllegalId, kIllegalId, 165, 166, kLeafId, kLeafId,
      kIllegalId, kIllegalId, 167, 164, kLeafId, kLeafId, kIllegalId,
      kIllegalId, 169, 172, 170, 171, kIllegalId, kLeafId, kLeafId, kIllegalId,
      kLeafId, kLeafId, kIllegalId, kIllegalId, 173, 176, 174, 175, kIllegalId,
      kLeafId, kLeafId, kIllegalId, kLeafId, kLeafId, kIllegalId, kIllegalId,
      177, 168, 178, 179, kIllegalId, kLeafId, kLeafId, kIllegalId, kLeafId,
      kLeafId, kIllegalId};
  REQUIRE(seq.States(engine::Round::kTurn) == 180);
  REQUIRE(seq.ActionCount(engine::Round::kTurn) == 3);
  round_actions = seq.Actions(engine::Round::kTurn);
  for (int i = 0; i < round_actions.width(); ++i) {
    REQUIRE(round_actions(i) == actions[i]);
  }
  for (blueprint::SequenceId i = 0; i < 180; ++i) {
    for (nda::index_t j = 0; j < 3; ++j) {
      blueprint::Sequence s = {i, engine::Round::kTurn};
      REQUIRE(seq.Next(s, j) == turn_table[i * 3 + j]);
    }
  }

  std::vector<blueprint::SequenceId> river_table = {kIllegalId, 1, 2, kLeafId,
      kLeafId, kIllegalId, kIllegalId, 3, kLeafId, kLeafId, kLeafId, kIllegalId,
      kIllegalId, 5, 6, kLeafId, kLeafId, kIllegalId, kIllegalId, 7, kLeafId,
      kLeafId, kLeafId, kIllegalId, kIllegalId, 9, 10, kLeafId, kLeafId,
      kIllegalId, kIllegalId, 11, kLeafId, kLeafId, kLeafId, kIllegalId,
      kIllegalId, 13, 14, kLeafId, kLeafId, kIllegalId, kIllegalId, 15, kLeafId,
      kLeafId, kLeafId, kIllegalId, kIllegalId, 17, 18, kLeafId, kLeafId,
      kIllegalId, kIllegalId, 19, kLeafId, kLeafId, kLeafId, kIllegalId,
      kIllegalId, 21, 22, kLeafId, kLeafId, kIllegalId, kIllegalId, 23, kLeafId,
      kLeafId, kLeafId, kIllegalId, kIllegalId, 25, 28, 26, 27, kIllegalId,
      kLeafId, kLeafId, kIllegalId, kLeafId, kLeafId, kIllegalId, kIllegalId,
      29, 32, 30, 31, kIllegalId, kLeafId, kLeafId, kIllegalId, kLeafId,
      kLeafId, kIllegalId, kIllegalId, 33, kLeafId, 34, 35, kIllegalId, kLeafId,
      kLeafId, kIllegalId, kLeafId, kLeafId, kIllegalId, kIllegalId, 37, 38,
      kLeafId, kLeafId, kIllegalId, kIllegalId, 39, kLeafId, kLeafId, kLeafId,
      kIllegalId, kIllegalId, 41, 42, kLeafId, kLeafId, kIllegalId, kIllegalId,
      43, kLeafId, kLeafId, kLeafId, kIllegalId, kIllegalId, 45, 48, 46, 47,
      kIllegalId, kLeafId, kLeafId, kIllegalId, kLeafId, kLeafId, kIllegalId,
      kIllegalId, 49, 52, 50, 51, kIllegalId, kLeafId, kLeafId, kIllegalId,
      kLeafId, kLeafId, kIllegalId, kIllegalId, 53, kLeafId, 54, 55, kIllegalId,
      kLeafId, kLeafId, kIllegalId, kLeafId, kLeafId, kIllegalId, kIllegalId,
      57, 58, kLeafId, kLeafId, kIllegalId, kIllegalId, 59, kLeafId, kLeafId,
      kLeafId, kIllegalId, kIllegalId, 61, 62, kLeafId, kLeafId, kIllegalId,
      kIllegalId, 63, kLeafId, kLeafId, kLeafId, kIllegalId, kIllegalId, 65, 68,
      66, 67, kIllegalId, kLeafId, kLeafId, kIllegalId, kLeafId, kLeafId,
      kIllegalId, kIllegalId, 69, 72, 70, 71, kIllegalId, kLeafId, kLeafId,
      kIllegalId, kLeafId, kLeafId, kIllegalId, kIllegalId, 73, kLeafId, 74, 75,
      kIllegalId, kLeafId, kLeafId, kIllegalId, kLeafId, kLeafId, kIllegalId,
      kIllegalId, 77, 78, kLeafId, kLeafId, kIllegalId, kIllegalId, 79, kLeafId,
      kLeafId, kLeafId, kIllegalId, kIllegalId, 81, 82, kLeafId, kLeafId,
      kIllegalId, kIllegalId, 83, kLeafId, kLeafId, kLeafId, kIllegalId,
      kIllegalId, 85, 88, 86, 87, kIllegalId, kLeafId, kLeafId, kIllegalId,
      kLeafId, kLeafId, kIllegalId, kIllegalId, 89, 92, 90, 91, kIllegalId,
      kLeafId, kLeafId, kIllegalId, kLeafId, kLeafId, kIllegalId, kIllegalId,
      93, kLeafId, 94, 95, kIllegalId, kLeafId, kLeafId, kIllegalId, kLeafId,
      kLeafId, kIllegalId, kIllegalId, 97, 98, kLeafId, kLeafId, kIllegalId,
      kIllegalId, 99, kLeafId, kLeafId, kLeafId, kIllegalId, kIllegalId, 101,
      102, kLeafId, kLeafId, kIllegalId, kIllegalId, 103, kLeafId, kLeafId,
      kLeafId, kIllegalId, kIllegalId, 105, 106, kLeafId, kLeafId, kIllegalId,
      kIllegalId, 107, kLeafId, kLeafId, kLeafId, kIllegalId, kIllegalId, 109,
      112, 110, 111, kIllegalId, kLeafId, kLeafId, kIllegalId, kLeafId, kLeafId,
      kIllegalId, kIllegalId, 113, 116, 114, 115, kIllegalId, kLeafId, kLeafId,
      kIllegalId, kLeafId, kLeafId, kIllegalId, kIllegalId, 117, kLeafId, 118,
      119, kIllegalId, kLeafId, kLeafId, kIllegalId, kLeafId, kLeafId,
      kIllegalId, kIllegalId, 121, 122, kLeafId, kLeafId, kIllegalId,
      kIllegalId, 123, kLeafId, kLeafId, kLeafId, kIllegalId, kIllegalId, 125,
      126, kLeafId, kLeafId, kIllegalId, kIllegalId, 127, kLeafId, kLeafId,
      kLeafId, kIllegalId, kIllegalId, 129, 132, 130, 131, kIllegalId, kLeafId,
      kLeafId, kIllegalId, kLeafId, kLeafId, kIllegalId, kIllegalId, 133, 136,
      134, 135, kIllegalId, kLeafId, kLeafId, kIllegalId, kLeafId, kLeafId,
      kIllegalId, kIllegalId, 137, kLeafId, 138, 139, kIllegalId, kLeafId,
      kLeafId, kIllegalId, kLeafId, kLeafId, kIllegalId, kIllegalId, 141, 142,
      kLeafId, kLeafId, kIllegalId, kIllegalId, 143, kLeafId, kLeafId, kLeafId,
      kIllegalId, kIllegalId, 145, 146, kLeafId, kLeafId, kIllegalId,
      kIllegalId, 147, kLeafId, kLeafId, kLeafId, kIllegalId, kIllegalId, 149,
      152, 150, 151, kIllegalId, kLeafId, kLeafId, kIllegalId, kLeafId, kLeafId,
      kIllegalId, kIllegalId, 153, 156, 154, 155, kIllegalId, kLeafId, kLeafId,
      kIllegalId, kLeafId, kLeafId, kIllegalId, kIllegalId, 157, kLeafId, 158,
      159, kIllegalId, kLeafId, kLeafId, kIllegalId, kLeafId, kLeafId,
      kIllegalId, kIllegalId, 161, 162, kLeafId, kLeafId, kIllegalId,
      kIllegalId, 163, kLeafId, kLeafId, kLeafId, kIllegalId, kIllegalId, 165,
      166, kLeafId, kLeafId, kIllegalId, kIllegalId, 167, kLeafId, kLeafId,
      kLeafId, kIllegalId, kIllegalId, 169, 172, 170, 171, kIllegalId, kLeafId,
      kLeafId, kIllegalId, kLeafId, kLeafId, kIllegalId, kIllegalId, 173, 176,
      174, 175, kIllegalId, kLeafId, kLeafId, kIllegalId, kLeafId, kLeafId,
      kIllegalId, kIllegalId, 177, kLeafId, 178, 179, kIllegalId, kLeafId,
      kLeafId, kIllegalId, kLeafId, kLeafId, kIllegalId};
  REQUIRE(seq.States(engine::Round::kRiver) == 180);
  REQUIRE(seq.ActionCount(engine::Round::kRiver) == 3);
  round_actions = seq.Actions(engine::Round::kRiver);
  for (int i = 0; i < round_actions.width(); ++i) {
    REQUIRE(round_actions(i) == actions[i]);
  }
  for (blueprint::SequenceId i = 0; i < 180; ++i) {
    for (nda::index_t j = 0; j < 3; ++j) {
      blueprint::Sequence s = {i, engine::Round::kRiver};
      REQUIRE(seq.Next(s, j) == river_table[i * 3 + j]);
    }
  }
}  // TEST_CASE "3 player all in fold check pot bet"
