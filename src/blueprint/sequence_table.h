// Copyright 2021 Emily Dale and Marzuk Rashid

#ifndef SRC_BLUEPRINT_SEQUENCE_TABLE_H_
#define SRC_BLUEPRINT_SEQUENCE_TABLE_H_

#include <algorithm>
#include <array>
#include <cstdint>
#include <numeric>
#include <ostream>
#include <stdexcept>

#include "array/array.h"
#include "array/matrix.h"
#include "blueprint/definitions.h"
#include "engine/definitions.h"
#include "engine/node.h"

namespace blueprint {

template <engine::PlayerN kPlayers, std::size_t kActions>
class SequenceTable {
 private:
  using ActionArray = std::array<std::array<Action, kActions>,
                                 engine::kNRounds>;
  using NumActionsArray = std::array<std::size_t, engine::kNRounds>;
  using SequenceMatrix = std::array<nda::matrix<SequenceId>,
                                    engine::kNRounds>;
  using NumSequenceArray = std::array<SequenceN, engine::kNRounds>;

  ActionArray actions_;
  const engine::Node<kPlayers> start_state_;
  SequenceMatrix table_;

 public:
  SequenceTable(const std::array<Action, kActions>& actions,
                const engine::Node<kPlayers>& start_state)
                : actions_{}, start_state_{start_state}, table_{} {
    NumActionsArray action_counter;
    SortActions(actions, actions_, action_counter);
    NumSequenceArray row_counter = CountSorted(actions_, action_counter,
                                               start_state);
    for (engine::RoundId i = 0; i < engine::kNRounds; ++i) {
      table_[i] = nda::matrix<SequenceId>{{row_counter[i], action_counter[i]}};
    }

    row_counter.fill(0);
    Generate(start_state_, actions_, action_counter, 0, row_counter,
        [&](Sequence seq, nda::index_t action_col, SequenceId val) {
          engine::RoundId round_id = engine::GetRoundId(seq.round);
          table_[round_id](seq.id, action_col) = val;
        });
  }  // SequenceTable()

  SequenceTable(const SequenceTable& other) = default;
  SequenceTable& operator=(const SequenceTable& other) = default;

  SequenceId Next(Sequence current_node, nda::index_t action_idx) const {
    engine::RoundId round_id =
        engine::GetRoundId(current_node.round);
    return table_[round_id](current_node.id, action_idx);
  }

  SequenceN States(engine::RoundId round_id) const {
    return table_[round_id].rows();
  }
  SequenceN States(engine::Round round) const {
    engine::RoundId round_id = engine::GetRoundId(round);
    return States(round_id);
  }

  nda::size_t ActionCount(engine::RoundId round_id) const {
    return table_[round_id].columns();
  }
  nda::size_t ActionCount(engine::Round round) const {
    engine::RoundId round_id = engine::GetRoundId(round);
    return ActionCount(round_id);
  }

  nda::const_vector_ref<Action> Actions(engine::RoundId round_id) const {
    return nda::const_vector_ref<Action>(&actions_[round_id][0],
                                         ActionCount(round_id));
  }
  nda::const_vector_ref<Action> Actions(engine::Round round) const {
    engine::RoundId round_id = engine::GetRoundId(round);
    return Actions(round_id);
  }

  friend std::ostream& operator<<(std::ostream& os, const SequenceTable& s) {
    nda::size_t elements = std::accumulate(s.table_.begin(), s.table_.end(), 0,
        [](const nda::size_t sum, const nda::matrix<SequenceId>& round_table) {
          return sum + round_table.size();
        });
    std::size_t bytes = sizeof(SequenceId) * elements;
    double memory = bytes / 1073741824.0;
    os << "SequenceTable<" << +kPlayers << ", " << kActions << ">" << " { "
       << "preflop rows: " << s.States(engine::Round::kPreFlop) << "; "
       << "flop rows: " << s.States(engine::Round::kFlop) << "; "
       << "turn rows: " << s.States(engine::Round::kTurn) << "; "
       << "river rows: " << s.States(engine::Round::kRiver) << "; "
       << "memory: " << memory << " GB; "
       << "}";
    return os;
  }

  /*
    @brief Counts the number of rows in a hypothetical sequence table.

    @param actions The actions available in the abstracted game tree.
    @param start_state The game tree node to start the table at.

    @return Array with the number of rows in each round.
  */
  static NumSequenceArray Count(const std::array<Action, kActions>& actions,
                                const engine::Node<kPlayers>& start_state) {
    ActionArray sorted_actions;
    NumActionsArray num_actions;
    SortActions(actions, sorted_actions, num_actions);
    return CountSorted(sorted_actions, num_actions, start_state);
  }

 private:
  /*
    @brief Counts the number of rows in a hypothetical sequence table.

    This function assumes that the actions are already sorted by round.

    @param sorted_actions The actions available in the abstracted game tree.
    @param action_count The number of actions in each round.
    @param start_state The game tree node to start the table at.

    @return An array with the number of rows in each round.
  */
  static NumSequenceArray CountSorted(const ActionArray& sorted_actions,
      const NumActionsArray& num_actions,
      const engine::Node<kPlayers>& start_state) {
    NumSequenceArray row_counter;
    std::fill(row_counter.begin(), row_counter.end(), 0);
    Generate(start_state, sorted_actions, num_actions, 0, row_counter,
             [](Sequence, nda::index_t, SequenceId) {});
    return row_counter;
  }

  /*
    @brief Sorts and counts the actions into each round.

    @param actions All the actions availible at any round.
    @param sorted_actions Pointer to array to store the sorted actions.
    @param num_actions Pointer to array to save the number of actions at each
        round.
  */
  static void SortActions(const std::array<Action, kActions>& actions,
                          ActionArray& sorted_actions,
                          NumActionsArray& num_actions) {
    std::fill(num_actions.begin(), num_actions.end(), 0);
    for (std::size_t i = 0; i < kActions; ++i) {
      for (engine::RoundId round = 0; round < engine::kNRounds;
           ++round) {
        engine::RoundId action_round =
            engine::GetRoundId(actions[i].max_round);
        if (round <= action_round) {
          sorted_actions[round][num_actions[round]] = actions[i];
          ++num_actions[round];
        }
      }  // for round
    }  // for i
  }

  /*
    @brief Runs through the recursive sequence table construction algorithm.

    @param state The game tree node to generate a row for.
    @param actions The actions available in the abstracted game tree.
    @param num_actions Array of number of actions available at each round.
    @param seq The SequenceId corresponding to the game tree node.
    @param row_counter An array to a count of the number of rows added to the
        table so far in each round.
    @param row_marker A pointer to a function that will be called to mark a
        given value at the given Sequence and action in the table.
  */
  template <typename RowMarkFn>
  static void Generate(const engine::Node<kPlayers>& state,
                       const ActionArray& actions,
                       const NumActionsArray& num_actions, SequenceId seq,
                       NumSequenceArray& row_counter, RowMarkFn&& row_marker) {
    engine::RoundId round_id = engine::GetRoundId(state.round());
    if (row_counter[round_id] == kIllegalId) {
      throw std::overflow_error("Sequence table has too many rows.");
    }
    ++row_counter[round_id];
    for (std::size_t j = 0; j < num_actions[round_id]; ++j) {
      Action action = actions[round_id][j];
      engine::Chips chip_size = ActionSize(action, state);
      if (chip_size) {
        engine::Node<kPlayers> new_state = state;
        if (new_state.Apply(action.play, chip_size)) {
          engine::Round new_round = new_state.round();
          engine::RoundId new_round_id = engine::GetRoundId(new_round);
          row_marker({seq, state.round()}, j, row_counter[new_round_id]);
          Generate(new_state, actions, num_actions, row_counter[new_round_id],
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
  static engine::Chips ActionSize(const Action& action,
                                  const engine::Node<kPlayers>& state) {
    if ((action.max_rotation == 0 || state.Rotation() < action.max_rotation) &&
        (state.round() <= action.max_round)) {
      switch (action.play) {
        case engine::Action::kAllIn:
          return true;
        case engine::Action::kCheckCall:
          return state.CanCheckCall();
        case engine::Action::kFold:
          return state.CanFold();
        case engine::Action::kBet:
          engine::Chips size = state.ConvertBet(action.size);
          return state.CanBet(size) ? size : false;
      }
    }
    return false;
  }
};  // class SequenceTable

}  // namespace blueprint

#endif  // SRC_BLUEPRINT_SEQUENCE_TABLE_H_
