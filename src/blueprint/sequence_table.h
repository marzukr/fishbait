// Copyright 2021 Emily Dale and Marzuk Rashid

#ifndef SRC_BLUEPRINT_SEQUENCE_TABLE_H_
#define SRC_BLUEPRINT_SEQUENCE_TABLE_H_

#include <algorithm>
#include <cstdint>
#include <ostream>
#include <stdexcept>
#include <vector>

#include "array/array.h"
#include "array/matrix.h"
#include "blueprint/definitions.h"
#include "poker_engine/definitions.h"
#include "poker_engine/node.h"
#include "utils/fraction.h"

namespace blueprint {

template <poker_engine::PlayerN kPlayers, int kActions>
class SequenceTable {
 public:
  SequenceTable(const Action actions[kActions],
                const poker_engine::Node<kPlayers>& start_state)
                : start_state_{start_state}, table_{} {
    int action_counter[poker_engine::kNRounds];
    SortActions(actions, actions_, action_counter);

    SequenceN row_counter[poker_engine::kNRounds];
    CountSorted(actions_, action_counter, start_state, row_counter);

    for (poker_engine::RoundId i = 0; i < poker_engine::kNRounds; ++i) {
      table_[i] = nda::matrix<SequenceId>{{row_counter[i], action_counter[i]}};
    }

    std::fill(row_counter, row_counter + poker_engine::kNRounds, 0);
    Generate(start_state_, actions_, action_counter, 0, row_counter,
        [&](Sequence seq, nda::index_t action_col, SequenceId val) {
          poker_engine::RoundId round_id = poker_engine::GetRoundId(seq.round);
          table_[round_id](seq.id, action_col) = val;
        });
  }  // SequenceTable()

  SequenceTable(const SequenceTable& other) = default;
  SequenceTable& operator=(const SequenceTable& other) = default;

  SequenceId Next(Sequence current_node, nda::index_t action_idx) const {
    poker_engine::RoundId round_id =
        poker_engine::GetRoundId(current_node.round);
    return table_[round_id](current_node.id, action_idx);
  }

  SequenceN States(poker_engine::RoundId round_id) const {
    return table_[round_id].rows();
  }
  SequenceN States(poker_engine::Round round) const {
    poker_engine::RoundId round_id = poker_engine::GetRoundId(round);
    return States(round_id);
  }

  nda::size_t ActionCount(poker_engine::RoundId round_id) const {
    return table_[round_id].columns();
  }
  nda::size_t ActionCount(poker_engine::Round round) const {
    poker_engine::RoundId round_id = poker_engine::GetRoundId(round);
    return ActionCount(round_id);
  }

  nda::const_vector_ref<Action> Actions(poker_engine::RoundId round_id) const {
    return nda::const_vector_ref<Action>(&actions_[round_id][0],
                                         ActionCount(round_id));
  }
  nda::const_vector_ref<Action> Actions(poker_engine::Round round) const {
    poker_engine::RoundId round_id = poker_engine::GetRoundId(round);
    return Actions(round_id);
  }

  friend std::ostream& operator<<(std::ostream& os, const SequenceTable& s) {
    nda::size_t elements = s.table_.rows() * s.table_.columns();
    size_t bytes = sizeof(SequenceId) * elements;
    double memory = bytes / 1.0e+9;
    os << "SequenceTable<" << +kPlayers << ">" << " { "
       << "rows: " << s.States() << "; "
       << "memory: ~" << memory << " GB; "
       << "}";
    return os;
  }

  /*
    @brief Counts the number of rows in a hypothetical sequence table.

    @param actions The actions available in the abstracted game tree.
    @param start_state The game tree node to start the table at.
    @param row_counter The number of rows in each round.
  */
  static void Count(const Action actions[kActions],
                    const poker_engine::Node<kPlayers>& start_state,
                    SequenceN row_counter[poker_engine::kNRounds]) {
    Action sorted_actions[poker_engine::kNRounds][kActions];
    int action_counter[poker_engine::kNRounds];
    SortActions(actions, sorted_actions, action_counter);
    CountSorted(sorted_actions, action_counter, start_state, row_counter);
  }

 private:
  /*
    @brief Counts the number of rows in a hypothetical sequence table.

    This function assumes that the actions are already sorted by round.

    @param sorted_actions The actions available in the abstracted game tree.
    @param action_count The number of actions in each round.
    @param start_state The game tree node to start the table at.
    @param row_counter The number of rows in each round.
  */
  static void CountSorted(
      const Action sorted_actions[poker_engine::kNRounds][kActions],
      int action_count[poker_engine::kNRounds],
      const poker_engine::Node<kPlayers>& start_state,
      SequenceN row_counter[poker_engine::kNRounds]) {
    std::fill(row_counter, row_counter + poker_engine::kNRounds, 0);
    Generate(start_state, sorted_actions, action_count, 0, row_counter,
        [](Sequence, nda::index_t, SequenceId) {});
  }

  /*
    @brief Sorts and counts the actions into each round.

    @param actions All the actions availible at any round.
    @param sorted_actions Array to store the sorted actions.
    @param action_counter Array to save the number of actions at each round.
  */
  static void SortActions(const Action actions[kActions],
      Action sorted_actions[poker_engine::kNRounds][kActions],
      int action_counter[poker_engine::kNRounds]) {
    std::fill(action_counter, action_counter + poker_engine::kNRounds, 0);
    for (int i = 0; i < kActions; ++i) {
      for (poker_engine::RoundId round = 0; round < poker_engine::kNRounds;
           ++round) {
        poker_engine::RoundId action_round =
            poker_engine::GetRoundId(actions[i].max_round);
        if (round <= action_round) {
          sorted_actions[round][action_counter[round]] = actions[i];
          ++action_counter[round];
        }
      }  // for round
    }  // for action
  }

  /*
    @brief Runs through the recursive sequence table construction algorithm.

    @param state The game tree node to generate a row for.
    @param actions The actions available in the abstracted game tree.
    @param action_count Array of number of actions available at each round.
    @param seq The SequenceId corresponding to the game tree node.
    @param row_counter An array to a count of the number of rows added to the
        table so far in each round.
    @param row_marker A pointer to a function that will be called to mark a
        given value at the given Sequence and action in the table.
  */
  template <typename RowMarkFn>
  static void Generate(const poker_engine::Node<kPlayers>& state,
                       const Action actions[poker_engine::kNRounds][kActions],
                       const int action_count[poker_engine::kNRounds],
                       const SequenceId seq,
                       SequenceN row_counter[poker_engine::kNRounds],
                       RowMarkFn&& row_marker) {
    poker_engine::RoundId round_id = poker_engine::GetRoundId(state.round());
    if (row_counter[round_id] == kIllegalId) {
      throw std::overflow_error("Sequence table has too many rows.");
    }
    ++row_counter[round_id];
    for (int j = 0; j < action_count[round_id]; ++j) {
      Action action = actions[round_id][j];
      poker_engine::Chips chip_size = ActionSize(action, state);
      if (chip_size) {
        poker_engine::Node<kPlayers> new_state = state;
        if (new_state.Apply(action.play, chip_size)) {
          poker_engine::Round new_round = new_state.round();
          poker_engine::RoundId new_round_id =
              poker_engine::GetRoundId(new_round);
          row_marker({seq, state.round()}, j, row_counter[new_round_id]);
          Generate(new_state, actions, action_count, row_counter[new_round_id],
                   row_counter, row_marker);
        } else {
          row_marker({seq, state.round()}, j, kLeafId);
        }
      } else {
        row_marker({seq, state.round()}, j, kIllegalId);
      }
    }  // for j
  }

  /*
    @brief Converts the pot proportion of action to a state Apply()-able size.

    @return The size of the action to play if it can be played, 0 otherwise.
  */
  static poker_engine::Chips ActionSize(const Action& action,
      const poker_engine::Node<kPlayers>& state) {
    if ((action.max_rotation == 0 || state.Rotation() < action.max_rotation) &&
        (state.round() <= action.max_round)) {
      switch (action.play) {
        case poker_engine::Action::kAllIn:
          return true;
        case poker_engine::Action::kCheckCall:
          return state.CanCheckCall();
        case poker_engine::Action::kFold:
          return state.CanFold();
        case poker_engine::Action::kBet:
          poker_engine::Chips size = state.ConvertBet(action.size);
          return state.CanBet(size) ? size : false;
      }
    }
    return false;
  }

  Action actions_[poker_engine::kNRounds][kActions];
  const poker_engine::Node<kPlayers> start_state_;
  nda::matrix<SequenceId> table_[poker_engine::kNRounds];
};  // class SequenceTable

}  // namespace blueprint

#endif  // SRC_BLUEPRINT_SEQUENCE_TABLE_H_
