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
#include "poker/definitions.h"
#include "poker/node.h"

namespace fishbait {

template <PlayerN kPlayers, std::size_t kActions>
class SequenceTable {
 private:
  using ActionArray = std::array<std::array<AbstractAction, kActions>,
                                 kNRounds>;
  using NumActionsArray = std::array<std::size_t, kNRounds>;
  using SequenceMatrix = std::array<nda::matrix<SequenceId>, kNRounds>;

  ActionArray actions_;
  const Node<kPlayers> start_state_;
  SequenceMatrix table_;

 public:
  /*
    @brief Constructs a sequence table.

    @param actions The actions available in the abstracted game tree.
    @param start_state The game tree node to start the table at.
  */
  SequenceTable(const std::array<AbstractAction, kActions>& actions,
                const Node<kPlayers>& start_state)
                : actions_{}, start_state_{start_state}, table_{} {
    NumActionsArray action_counter;
    SortActions(actions, actions_, action_counter);
    NumNodesArray node_counter = CountSorted(actions_, action_counter,
                                               start_state);
    for (RoundId i = 0; i < kNRounds; ++i) {
      table_[i] = nda::matrix<SequenceId>{{node_counter[i].internal_nodes,
                                           action_counter[i]}};
    }

    node_counter.fill({0, 0});
    Node new_state = start_state_;
    Generate(new_state, actions_, action_counter, 0, node_counter,
        [&](SequenceId seq, Round round, nda::index_t action_col,
            SequenceId val) {
          table_[+round](seq, action_col) = val;
        });
  }  // SequenceTable()
  SequenceTable(const SequenceTable& other) = default;
  SequenceTable& operator=(const SequenceTable& other) = default;

  /*
    @brief Returns the sequence id reached by taking the given action.

    @param current_node Sequence of the state where the action is to be taken.
    @param round The betting round that the current_node is in.
    @param action_idx The action to take.
  */
  SequenceId Next(SequenceId current_node, Round round,
                  nda::index_t action_idx) const {
    return table_[+round](current_node, action_idx);
  }

  /*
    @brief Returns the number of sequences in the given round.
  */
  SequenceN States(Round round) const {
    return table_[+round].rows();
  }

  /*
    @brief Returns the number of legal actions available in the given infoset.
  */
  nda::size_t NumLegalActions(SequenceId seq, Round round) const {
    nda::size_t count = 0;
    for (nda::index_t action_id : table_[+round].j()) {
      if (table_[+round](seq, action_id) != kIllegalId) {
        ++count;
      }
    }
    return count;
  }

  /*
    @brief Returns the total number of actions available in the given round.
  */
  nda::size_t ActionCount(Round round) const {
    return table_[+round].columns();
  }

  /*
    @brief Returns a vector_ref of the actions available in the given round.
  */
  nda::const_vector_ref<AbstractAction> Actions(Round round) const {
    return nda::const_vector_ref<AbstractAction>(&actions_[+round][0],
                                                 ActionCount(round));
  }

  /*
    @brief Returns the start state for the sequence table.
  */
  const Node<kPlayers>& start_state() {
    return start_state_;
  }


  friend std::ostream& operator<<(std::ostream& os, const SequenceTable& s) {
    nda::size_t elements = std::accumulate(s.table_.begin(), s.table_.end(), 0,
        [](const nda::size_t sum, const nda::matrix<SequenceId>& round_table) {
          return sum + round_table.size();
        });
    std::size_t bytes = sizeof(SequenceId) * elements;
    double memory = bytes / 1073741824.0;
    os << "SequenceTable<" << +kPlayers << ", " << kActions << ">" << " { "
       << "preflop rows: " << s.States(Round::kPreFlop) << "; "
       << "flop rows: " << s.States(Round::kFlop) << "; "
       << "turn rows: " << s.States(Round::kTurn) << "; "
       << "river rows: " << s.States(Round::kRiver) << "; "
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
  static NumNodesArray Count(
      const std::array<AbstractAction, kActions>& actions,
      const Node<kPlayers>& start_state) {
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
  static NumNodesArray CountSorted(const ActionArray& sorted_actions,
                                      const NumActionsArray& num_actions,
                                      const Node<kPlayers>& start_state) {
    NumNodesArray node_counter;
    node_counter.fill({0, 0});
    Node new_state = start_state;
    Generate(new_state, sorted_actions, num_actions, 0, node_counter,
             [](SequenceId, Round, nda::index_t, SequenceId) {});
    return node_counter;
  }

  /*
    @brief Sorts and counts the actions into each round.

    @param actions All the actions availible at any round.
    @param sorted_actions Pointer to array to store the sorted actions.
    @param num_actions Pointer to array to save the number of actions at each
        round.
  */
  static void SortActions(const std::array<AbstractAction, kActions>& actions,
                          ActionArray& sorted_actions,
                          NumActionsArray& num_actions) {
    std::fill(num_actions.begin(), num_actions.end(), 0);
    for (std::size_t i = 0; i < kActions; ++i) {
      for (RoundId round = 0; round < kNRounds; ++round) {
        RoundId action_round = +actions[i].max_round;
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
    @param node_counter An array to a count of the number of rows added to the
        table so far in each round.
    @param row_marker A pointer to a function that will be called to mark a
        given value at the given Sequence and action in the table.

    @returns The id of the state if a row was created. Otherwise kLeafId for
        terminal states.
  */
  template <typename RowMarkFn>
  static SequenceId Generate(Node<kPlayers>& state, const ActionArray& actions,
                             const NumActionsArray& num_actions, SequenceId seq,
                             NumNodesArray& node_counter,
                             RowMarkFn&& row_marker) {
    if (node_counter[+state.round()].internal_nodes == kIllegalId) {
      throw std::overflow_error("Sequence table has too many rows.");
    } else if (!state.in_progress()) {
      ++node_counter[+state.round()].leaf_nodes;
      return kLeafId;
    } else if (state.acting_player() == state.kChancePlayer) {
      state.ProceedPlay();
      return Generate(state, actions, num_actions, seq, node_counter,
                      row_marker);
    }

    ++node_counter[+state.round()].internal_nodes;
    for (std::size_t j = 0; j < num_actions[+state.round()]; ++j) {
      AbstractAction action = actions[+state.round()][j];
      Chips chip_size = ActionSize(action, state);
      if (chip_size) {
        Node<kPlayers> new_state = state;
        new_state.Apply(action.play, chip_size);
        Round new_round = new_state.round();
        SequenceId new_state_id = Generate(new_state, actions, num_actions,
            node_counter[+new_round].internal_nodes, node_counter, row_marker);
        row_marker(seq, state.round(), j, new_state_id);
      } else {
        row_marker(seq, state.round(), j, kIllegalId);
      }
    }  // for j
    return seq;
  }  // Generate()

  /*
    @brief Converts the pot proportion of action to a state Apply()-able size.

    @return The size of the action to play if it can be played, 0 otherwise.
  */
  static Chips ActionSize(const AbstractAction& action,
                          const Node<kPlayers>& state) {
    if ((action.max_rotation == 0 || state.Rotation() < action.max_rotation) &&
        (state.round() <= action.max_round)) {
      switch (action.play) {
        case Action::kAllIn:
          return true;
        case Action::kCheckCall:
          return state.CanCheckCall();
        case Action::kFold:
          return state.CanFold();
        case Action::kBet:
          Chips size = state.ConvertBet(action.size);
          return state.CanBet(size) ? size : false;
      }
    }
    return false;
  }
};  // class SequenceTable

}  // namespace fishbait

#endif  // SRC_BLUEPRINT_SEQUENCE_TABLE_H_
