#include <array>
#include <ios>
#include <sstream>

#include "array/array.h"
#include "catch2/catch.hpp"
#include "mccfr/definitions.h"
#include "mccfr/sequence_table.h"
#include "poker/definitions.h"
#include "poker/node.h"

TEST_CASE("6 players all in or fold", "[mccfr][sequence_table]") {
  std::array<fishbait::AbstractAction, 2> actions = {{
      {fishbait::Action::kFold},
      {fishbait::Action::kAllIn},
  }};
  fishbait::Node<6> start_state;

  std::array row_counts = fishbait::SequenceTable<6, 2>::Count(actions,
                                                               start_state);
  CHECK(row_counts[0].internal_nodes == 62);
  CHECK(row_counts[0].leaf_nodes == 6);
  CHECK(row_counts[0].illegal_nodes == 0);
  CHECK(row_counts[0].legal_actions == 124);

  CHECK(row_counts[1].internal_nodes == 0);
  CHECK(row_counts[1].leaf_nodes == 0);
  CHECK(row_counts[1].illegal_nodes == 0);
  CHECK(row_counts[1].legal_actions == 0);

  CHECK(row_counts[2].internal_nodes == 0);
  CHECK(row_counts[2].leaf_nodes == 0);
  CHECK(row_counts[2].illegal_nodes == 0);
  CHECK(row_counts[2].legal_actions == 0);

  CHECK(row_counts[3].internal_nodes == 0);
  CHECK(row_counts[3].leaf_nodes == 57);
  CHECK(row_counts[3].illegal_nodes == 0);
  CHECK(row_counts[3].legal_actions == 0);

  fishbait::SequenceTable seq{actions, start_state};

  std::stringstream ss;
  ss.precision(10);
  ss << std::fixed << seq;
  std::string seq_str = ss.str();
  CHECK(seq_str == "SequenceTable<6, 2> { preflop rows: 62; flop rows: 0; "
                   "turn rows: 0; river rows: 0; memory: 0.0000004619 GB; }");

  REQUIRE(seq.start_state() == start_state);

  using fishbait::kLeafId;
  std::vector<fishbait::SequenceId> preflop_table = {1, 31, 2, 16, 3, 9, 4, 6,
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
  std::vector<std::size_t> preflop_offsets = {0, 2, 4, 6, 8, 10, 12, 14, 16, 18,
      20, 22, 24, 26, 28, 30, 32, 34, 36, 38, 40, 42, 44, 46, 48, 50, 52, 54,
      56, 58, 60, 62, 64, 66, 68, 70, 72, 74, 76, 78, 80, 82, 84, 86, 88, 90,
      92, 94, 96, 98, 100, 102, 104, 106, 108, 110, 112, 114, 116, 118, 120,
      122};

  REQUIRE(seq.States(fishbait::Round::kPreFlop) == 62);
  REQUIRE(seq.States(fishbait::Round::kFlop) == 0);
  REQUIRE(seq.States(fishbait::Round::kTurn) == 0);
  REQUIRE(seq.States(fishbait::Round::kRiver) == 0);

  REQUIRE(seq.ActionCount(fishbait::Round::kPreFlop) == 2);
  REQUIRE(seq.ActionCount(fishbait::Round::kFlop) == 2);
  REQUIRE(seq.ActionCount(fishbait::Round::kTurn) == 2);
  REQUIRE(seq.ActionCount(fishbait::Round::kRiver) == 2);

  REQUIRE(seq.NumLegalActions(fishbait::Round::kPreFlop) == 124);

  for (fishbait::RoundId i = 0; i < fishbait::kNRounds; ++i) {
    fishbait::Round round = fishbait::Round{i};
    nda::const_vector_ref<fishbait::AbstractAction> round_actions =
        seq.Actions(round);
    for (int j = 0; j < round_actions.width(); ++j) {
      REQUIRE(round_actions(j) == actions[j]);
    }
  }

  for (fishbait::SequenceId i = 0; i < 62; ++i) {
    REQUIRE(seq.NumLegalActions(fishbait::Round::kPreFlop, i) == 2);
    REQUIRE(seq.LegalOffset(fishbait::Round::kPreFlop, i) ==
            preflop_offsets[i]);
    for (nda::index_t j = 0; j < 2; ++j) {
      REQUIRE(seq.Next(fishbait::Round::kPreFlop, i, j) ==
              preflop_table[i * 2 + j]);
    }
  }
}  // TEST_CASE "6 players all in or fold"

TEST_CASE("3 player extensive test", "[mccfr][sequence_table]") {
  std::array<fishbait::AbstractAction, 5> actions = {{
      {fishbait::Action::kFold},
      {fishbait::Action::kAllIn},
      {fishbait::Action::kCheckCall},

      {fishbait::Action::kBet, 2.0, 1, fishbait::Round::kTurn,
       fishbait::Round::kTurn, 2, 0},
      {fishbait::Action::kBet, 0.25, 1, fishbait::Round::kFlop,
       fishbait::Round::kRiver, 0, 10000}
  }};
  fishbait::Node<3> start_state;

  std::array row_counts = fishbait::SequenceTable<3, 5>::Count(actions,
                                                               start_state);

  CHECK(row_counts[0].internal_nodes == 18);
  CHECK(row_counts[0].leaf_nodes == 7);
  CHECK(row_counts[0].illegal_nodes == 15);
  CHECK(row_counts[0].legal_actions == 39);

  CHECK(row_counts[1].internal_nodes == 20);
  CHECK(row_counts[1].leaf_nodes == 7);
  CHECK(row_counts[1].illegal_nodes == 40);
  CHECK(row_counts[1].legal_actions == 40);

  CHECK(row_counts[2].internal_nodes == 28);
  CHECK(row_counts[2].leaf_nodes == 15);
  CHECK(row_counts[2].illegal_nodes == 76);
  CHECK(row_counts[2].legal_actions == 64);

  CHECK(row_counts[3].internal_nodes == 36);
  CHECK(row_counts[3].leaf_nodes == 85);
  CHECK(row_counts[3].illegal_nodes == 72);
  CHECK(row_counts[3].legal_actions == 72);

  fishbait::SequenceTable seq{actions, start_state};

  std::stringstream ss;
  ss.precision(10);
  ss << std::fixed << seq;
  std::string seq_str = ss.str();
  REQUIRE(seq_str == "SequenceTable<3, 5> { preflop rows: 18; flop rows: 20; "
                     "turn rows: 28; river rows: 36; "
                     "memory: 0.0000015572 GB; }");

  REQUIRE(seq.start_state() == start_state);

  using fishbait::kIllegalId;
  using fishbait::kLeafId;

  std::vector<fishbait::SequenceId> preflop_table = {1, 5, 8, kLeafId, 2, 3,
      kLeafId, kLeafId, kIllegalId, kIllegalId, 4, 0, kLeafId, kLeafId,
      kIllegalId, 6, 7, kIllegalId, kLeafId, kLeafId, kIllegalId, kLeafId,
      kLeafId, kIllegalId, 9, 11, 14, kIllegalId, 10, 4, kLeafId, kLeafId,
      kIllegalId, 12, 13, kIllegalId, kLeafId, kLeafId, kIllegalId, kLeafId,
      kLeafId, kIllegalId, kIllegalId, 15, 8, 16, 17, kIllegalId, kLeafId,
      kLeafId, kIllegalId, kLeafId, kLeafId, kIllegalId};
  std::vector<nda::size_t> preflop_legal_actions = {3, 3, 2, 2, 2, 2, 2, 2, 3,
      2, 2, 2, 2, 2, 2, 2, 2, 2};
  std::vector<std::size_t> preflop_offsets = {0, 3, 6, 8, 10, 12, 14, 16, 18,
      21, 23, 25, 27, 29, 31, 33, 35, 37};
  std::array<fishbait::AbstractAction, 3> preflop_actions = {{
      {fishbait::Action::kFold},
      {fishbait::Action::kAllIn},
      {fishbait::Action::kCheckCall},
  }};
  REQUIRE(seq.States(fishbait::Round::kPreFlop) == 18);
  REQUIRE(seq.ActionCount(fishbait::Round::kPreFlop) == 3);
  REQUIRE(seq.NumLegalActions(fishbait::Round::kPreFlop) == 39);
  nda::const_vector_ref<fishbait::AbstractAction> round_actions =
      seq.Actions(fishbait::Round::kPreFlop);
  for (int i = 0; i < round_actions.width(); ++i) {
    REQUIRE(round_actions(i) == preflop_actions[i]);
  }
  for (fishbait::SequenceId i = 0; i < 18; ++i) {
    REQUIRE(seq.NumLegalActions(fishbait::Round::kPreFlop, i) ==
            preflop_legal_actions[i]);
    REQUIRE(seq.LegalOffset(fishbait::Round::kPreFlop, i) ==
            preflop_offsets[i]);
    for (nda::index_t j = 0; j < 3; ++j) {
      REQUIRE(seq.Next(fishbait::Round::kPreFlop, i, j) ==
              preflop_table[i * 3 + j]);
    }
  }

  std::vector<fishbait::SequenceId> flop_table = {kIllegalId, 1, 2,
      kIllegalId, kLeafId, kLeafId, kIllegalId, kIllegalId, kIllegalId, 3, 0,
      kIllegalId, kLeafId, kLeafId, kIllegalId, kIllegalId, kIllegalId, 5, 6,
      kIllegalId, kLeafId, kLeafId, kIllegalId, kIllegalId, kIllegalId, 7, 8,
      kIllegalId, kLeafId, kLeafId, kIllegalId, kIllegalId, kIllegalId, 9, 12,
      kIllegalId, 10, 11, kIllegalId, kIllegalId, kLeafId, kLeafId, kIllegalId,
      kIllegalId, kLeafId, kLeafId, kIllegalId, kIllegalId, kIllegalId, 13, 16,
      kIllegalId, 14, 15, kIllegalId, kIllegalId, kLeafId, kLeafId, kIllegalId,
      kIllegalId, kLeafId, kLeafId, kIllegalId, kIllegalId, kIllegalId, 17, 16,
      kIllegalId, 18, 19, kIllegalId, kIllegalId, kLeafId, kLeafId, kIllegalId,
      kIllegalId, kLeafId, kLeafId, kIllegalId, kIllegalId};
  std::vector<nda::size_t> flop_legal_actions = {2, 2, 2, 2, 2, 2, 2, 2, 2,
      2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2};
  std::vector<std::size_t> flop_offsets = {0, 2, 4, 6, 8, 10, 12, 14, 16, 18,
      20, 22, 24, 26, 28, 30, 32, 34, 36, 38};
  std::array<fishbait::AbstractAction, 4> flop_actions = {{
      {fishbait::Action::kFold},
      {fishbait::Action::kAllIn},
      {fishbait::Action::kCheckCall},
      {fishbait::Action::kBet, 0.25, 1, fishbait::Round::kFlop,
       fishbait::Round::kRiver, 0, 10000}
  }};
  REQUIRE(seq.States(fishbait::Round::kFlop) == 20);
  REQUIRE(seq.ActionCount(fishbait::Round::kFlop) == 4);
  REQUIRE(seq.NumLegalActions(fishbait::Round::kFlop) == 40);
  round_actions = seq.Actions(fishbait::Round::kFlop);
  for (int i = 0; i < round_actions.width(); ++i) {
    REQUIRE(round_actions(i) == flop_actions[i]);
  }
  for (fishbait::SequenceId i = 0; i < 20; ++i) {
    REQUIRE(seq.NumLegalActions(fishbait::Round::kFlop, i) ==
            flop_legal_actions[i]);
    REQUIRE(seq.LegalOffset(fishbait::Round::kFlop, i) ==
            flop_offsets[i]);
    for (nda::index_t j = 0; j < 3; ++j) {
      REQUIRE(seq.Next(fishbait::Round::kFlop, i, j) == flop_table[i * 4 + j]);
    }
  }

  std::vector<fishbait::SequenceId> turn_table = {kIllegalId, 1, 2, 6,
      kIllegalId, kLeafId, kLeafId, kIllegalId, kIllegalId, kIllegalId,
      kIllegalId, 3, 0, 4, kIllegalId, kLeafId, kLeafId, kIllegalId, kIllegalId,
      kIllegalId, kLeafId, 5, 4, kIllegalId, kIllegalId, kLeafId, kLeafId,
      kIllegalId, kIllegalId, kIllegalId, kLeafId, 7, 8, kIllegalId, kIllegalId,
      kLeafId, kLeafId, kIllegalId, kIllegalId, kIllegalId, kIllegalId, 9, 10,
      14, kIllegalId, kLeafId, kLeafId, kIllegalId, kIllegalId, kIllegalId,
      kIllegalId, 11, 12, 12, kIllegalId, kLeafId, kLeafId, kIllegalId,
      kIllegalId, kIllegalId, kLeafId, 13, 16, kIllegalId, kIllegalId, kLeafId,
      kLeafId, kIllegalId, kIllegalId, kIllegalId, kLeafId, 15, 20, kIllegalId,
      kIllegalId, kLeafId, kLeafId, kIllegalId, kIllegalId, kIllegalId,
      kIllegalId, 17, 20, kIllegalId, kIllegalId, 18, 19, kIllegalId,
      kIllegalId, kIllegalId, kLeafId, kLeafId, kIllegalId, kIllegalId,
      kIllegalId, kLeafId, kLeafId, kIllegalId, kIllegalId, kIllegalId,
      kIllegalId, 21, 24, kIllegalId, kIllegalId, 22, 23, kIllegalId,
      kIllegalId, kIllegalId, kLeafId, kLeafId, kIllegalId, kIllegalId,
      kIllegalId, kLeafId, kLeafId, kIllegalId, kIllegalId, kIllegalId,
      kIllegalId, 25, 24, kIllegalId, kIllegalId, 26, 27, kIllegalId,
      kIllegalId, kIllegalId, kLeafId, kLeafId, kIllegalId, kIllegalId,
      kIllegalId, kLeafId, kLeafId, kIllegalId, kIllegalId, kIllegalId};
  std::vector<nda::size_t> turn_legal_actions = {3, 2, 3, 2, 3, 2, 3, 2, 3,
      2, 3, 2, 3, 2, 3, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2};
  std::vector<std::size_t> turn_offsets = {0, 3, 5, 8, 10, 13, 15, 18, 20, 23,
      25, 28, 30, 33, 35, 38, 40, 42, 44, 46, 48, 50, 52, 54, 56, 58, 60, 62};
  std::array<fishbait::AbstractAction, 5> turn_actions = {{
      {fishbait::Action::kFold},
      {fishbait::Action::kAllIn},
      {fishbait::Action::kCheckCall},

      {fishbait::Action::kBet, 2.0, 1, fishbait::Round::kTurn,
       fishbait::Round::kTurn, 2, 0},
      {fishbait::Action::kBet, 0.25, 1, fishbait::Round::kFlop,
       fishbait::Round::kRiver, 0, 10000}
  }};
  REQUIRE(seq.States(fishbait::Round::kTurn) == 28);
  REQUIRE(seq.ActionCount(fishbait::Round::kTurn) == 5);
  REQUIRE(seq.NumLegalActions(fishbait::Round::kTurn) == 64);
  round_actions = seq.Actions(fishbait::Round::kTurn);
  for (int i = 0; i < round_actions.width(); ++i) {
    REQUIRE(round_actions(i) == turn_actions[i]);
  }
  for (fishbait::SequenceId i = 0; i < 28; ++i) {
    REQUIRE(seq.NumLegalActions(fishbait::Round::kTurn, i) ==
            turn_legal_actions[i]);
    REQUIRE(seq.LegalOffset(fishbait::Round::kTurn, i) ==
            turn_offsets[i]);
    for (nda::index_t j = 0; j < 5; ++j) {
      REQUIRE(seq.Next(fishbait::Round::kTurn, i, j) == turn_table[i * 5 + j]);
    }
  }

  std::vector<fishbait::SequenceId> river_table = {kIllegalId, 1, 2, kIllegalId,
      kLeafId, kLeafId, kIllegalId, kIllegalId, kIllegalId, 3, kLeafId,
      kIllegalId, kLeafId, kLeafId, kIllegalId, kIllegalId, kIllegalId, 5, 6,
      kIllegalId, kLeafId, kLeafId, kIllegalId, kIllegalId, kIllegalId, 7,
      kLeafId, kIllegalId, kLeafId, kLeafId, kIllegalId, kIllegalId, kIllegalId,
      9, 10, kIllegalId, kLeafId, kLeafId, kIllegalId, kIllegalId, kIllegalId,
      11, kLeafId, kIllegalId, kLeafId, kLeafId, kIllegalId, kIllegalId,
      kIllegalId, 13, 14, kIllegalId, kLeafId, kLeafId, kIllegalId, kIllegalId,
      kIllegalId, 15, kLeafId, kIllegalId, kLeafId, kLeafId, kIllegalId,
      kIllegalId, kIllegalId, 17, 18, kIllegalId, kLeafId, kLeafId, kIllegalId,
      kIllegalId, kIllegalId, 19, kLeafId, kIllegalId, kLeafId, kLeafId,
      kIllegalId, kIllegalId, kIllegalId, 21, 22, kIllegalId, kLeafId, kLeafId,
      kIllegalId, kIllegalId, kIllegalId, 23, kLeafId, kIllegalId, kLeafId,
      kLeafId, kIllegalId, kIllegalId, kIllegalId, 25, 28, kIllegalId, 26, 27,
      kIllegalId, kIllegalId, kLeafId, kLeafId, kIllegalId, kIllegalId, kLeafId,
      kLeafId, kIllegalId, kIllegalId, kIllegalId, 29, 32, kIllegalId, 30, 31,
      kIllegalId, kIllegalId, kLeafId, kLeafId, kIllegalId, kIllegalId, kLeafId,
      kLeafId, kIllegalId, kIllegalId, kIllegalId, 33, kLeafId, kIllegalId, 34,
      35, kIllegalId, kIllegalId, kLeafId, kLeafId, kIllegalId, kIllegalId,
      kLeafId, kLeafId, kIllegalId, kIllegalId};
  std::vector<nda::size_t> river_legal_actions = {2, 2, 2, 2, 2, 2, 2, 2, 2,
      2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
      2, 2};
  std::vector<std::size_t> river_offsets = {0, 2, 4, 6, 8, 10, 12, 14, 16, 18,
      20, 22, 24, 26, 28, 30, 32, 34, 36, 38, 40, 42, 44, 46, 48, 50, 52, 54,
      56, 58, 60, 62, 64, 66, 68, 70};
  std::array<fishbait::AbstractAction, 4> river_actions = {{
      {fishbait::Action::kFold},
      {fishbait::Action::kAllIn},
      {fishbait::Action::kCheckCall},

      {fishbait::Action::kBet, 0.25, 1, fishbait::Round::kFlop,
       fishbait::Round::kRiver, 0, 10000}
  }};
  REQUIRE(seq.States(fishbait::Round::kRiver) == 36);
  REQUIRE(seq.ActionCount(fishbait::Round::kRiver) == 4);
  REQUIRE(seq.NumLegalActions(fishbait::Round::kRiver) == 72);
  round_actions = seq.Actions(fishbait::Round::kRiver);
  for (int i = 0; i < round_actions.width(); ++i) {
    REQUIRE(round_actions(i) == river_actions[i]);
  }
  for (fishbait::SequenceId i = 0; i < 36; ++i) {
    REQUIRE(seq.NumLegalActions(fishbait::Round::kRiver, i) ==
            river_legal_actions[i]);
    REQUIRE(seq.LegalOffset(fishbait::Round::kRiver, i) ==
            river_offsets[i]);
    for (nda::index_t j = 0; j < 4; ++j) {
      REQUIRE((seq.Next(fishbait::Round::kRiver, i, j) ==
               river_table[i * 4 + j]));
    }
  }
}  // TEST_CASE "3 player extensive test"

TEST_CASE("equality test", "[mccfr][sequence_table]") {
  std::array<fishbait::AbstractAction, 5> actions_1 = {{
      {fishbait::Action::kFold},
      {fishbait::Action::kAllIn},
      {fishbait::Action::kCheckCall},

      {fishbait::Action::kBet, 2.0, 1, fishbait::Round::kTurn,
       fishbait::Round::kTurn, 2, 0},
      {fishbait::Action::kBet, 0.25, 1, fishbait::Round::kFlop,
       fishbait::Round::kRiver, 0, 10000}
  }};
  fishbait::Node<3> start_state_1;
  fishbait::SequenceTable tab1{actions_1, start_state_1};
  fishbait::SequenceTable tab4{actions_1, start_state_1};
  REQUIRE(tab1 == tab4);
  REQUIRE(!(tab1 != tab4));

  std::array<fishbait::AbstractAction, 5> actions_2 = {{
      {fishbait::Action::kFold},
      {fishbait::Action::kAllIn},
      {fishbait::Action::kCheckCall},

      {fishbait::Action::kBet, 2.0, 1, fishbait::Round::kTurn,
       fishbait::Round::kTurn, 2, 0},
      {fishbait::Action::kBet, 0.25, 1, fishbait::Round::kRiver,
       fishbait::Round::kRiver, 0, 10000}
  }};
  fishbait::SequenceTable tab2{actions_2, start_state_1};
  REQUIRE(tab2 != tab1);
  REQUIRE(!(tab2 == tab1));

  fishbait::Node<3> start_state_2;
  start_state_2.Deal();
  fishbait::SequenceTable tab3{actions_1, start_state_2};
  REQUIRE(tab3 != tab1);
  REQUIRE(!(tab3 == tab1));

  tab2 = fishbait::SequenceTable{actions_1, start_state_1};
  REQUIRE(!(tab2 != tab1));
  REQUIRE(tab2 == tab1);
}  // TEST_CASE "equality test"
