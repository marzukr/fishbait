#ifndef AI_SRC_MCCFR_SEQUENCE_TABLE_H_
#define AI_SRC_MCCFR_SEQUENCE_TABLE_H_

#include <algorithm>
#include <array>
#include <cstdint>
#include <numeric>
#include <ostream>
#include <stdexcept>
#include <utility>
#include <vector>

#include "array/array.h"
#include "array/matrix.h"
#include "mccfr/definitions.h"
#include "poker/definitions.h"
#include "poker/node.h"
#include "utils/cereal.h"

namespace fishbait {

template <PlayerN kPlayers, std::size_t kActions>
class SequenceTable {
 private:
  using ActionArray = std::array<std::array<AbstractAction, kActions>,
                                 kNRounds>;
  using NumActionsArray = std::array<std::size_t, kNRounds>;
  using SequenceMatrix = std::array<nda::matrix<SequenceId>, kNRounds>;

  // Array containing the available actions for each round.
  ActionArray actions_;

  // Starting state of the table.
  Node<kPlayers> start_state_;

  /* An array of SequenceTables for each round. For each sequence table, the
     entry at row i column j represents the new sequence reached from taking
     action j at sequence i. The value is kIllegalId if the action is illegal,
     or kLeafId if the action leads to a terminal node. */
  SequenceMatrix table_;

  /* For each round, the entry at index i of the vector represents the total
     number legal actions in all sequences preceding sequence i. */
  std::array<std::vector<std::size_t>, kNRounds> legal_offsets_;

 public:
  /*
    @brief Constructs a sequence table.

    @param actions The actions available in the abstracted game tree.
    @param start_state The game tree node to start the table at.
  */
  SequenceTable(const std::array<AbstractAction, kActions>& actions,
                const Node<kPlayers>& start_state) : actions_{},
                                                     start_state_{start_state},
                                                     table_{},
                                                     legal_offsets_{} {
    NumActionsArray action_counter;
    SortActions(actions, actions_, action_counter);
    NumNodesArray node_counter = CountSorted(actions_, action_counter,
                                             start_state);
    for (RoundId i = 0; i < kNRounds; ++i) {
      table_[i] = nda::matrix<SequenceId>{{node_counter[i].internal_nodes,
                                           action_counter[i]}};
    }

    node_counter.fill({0, 0, 0, 0});
    Node new_state = start_state_;
    Generate(new_state, 0, actions_, action_counter, 0, node_counter,
        [&](SequenceId seq, Round round, nda::index_t action_col,
            SequenceId val) {
          table_[+round](seq, action_col) = val;
        });
    ComputeLegalOffsets();
  }  // SequenceTable()
  SequenceTable(const SequenceTable& other) = default;
  SequenceTable(SequenceTable&& other) {
    *this = std::move(other);
  }
  SequenceTable& operator=(const SequenceTable& other) = default;
  SequenceTable& operator=(SequenceTable&& other) {
    actions_ = other.actions_;
    start_state_ = other.start_state_;
    table_ = std::move(other.table_);
    legal_offsets_ = std::move(other.legal_offsets_);
    return *this;
  }

  /* @brief SequenceTable serialize function */
  template<class Archive>
  void serialize(Archive& archive) {
    archive(actions_, start_state_, table_, legal_offsets_);
  }

  /*
    @brief Returns the sequence id reached by taking the given action.

    @param round The betting round that the current_node is in.
    @param current_node Sequence of the state where the action is to be taken.
    @param action_idx The action to take.
  */
  SequenceId Next(Round round, SequenceId current_node,
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

    legal_offsets_ must already be computed for this to work (this is done in
    the constuctor)
  */
  nda::size_t NumLegalActions(Round round, SequenceId seq) const {
    if (seq < legal_offsets_[+round].size() - 1) {
      return legal_offsets_[+round][seq + 1] - legal_offsets_[+round][seq];
    }
    return ComputeLegalActions(round, seq);
  }

  /*
    @brief Returns the total number of legal actions in the given round.
  */
  std::size_t NumLegalActions(Round round) const {
    nda::index_t last_seq = legal_offsets_[+round].size() - 1;
    return legal_offsets_[+round][last_seq] + NumLegalActions(round, last_seq);
  }

  /*
    @brief Returns the number of legal actions preceding the given sequence.
  */
  std::size_t LegalOffset(Round round, SequenceId seq) const {
    return legal_offsets_[+round][seq];
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
  const Node<kPlayers>& start_state() const {
    return start_state_;
  }

  /*
    @brief Returns the table for the sequence table.
  */
  const SequenceMatrix& table() const {
    return table_;
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

  /* @brief Checks if two SequenceTables are not the same */
  bool operator!=(const SequenceTable& other) const {
    return actions_ != other.actions_ || start_state_ != other.start_state_ ||
           table_ != other.table_ || legal_offsets_ != other.legal_offsets_;
  }

  bool operator==(const SequenceTable& other) const {
    return !(*this != other);
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
    node_counter.fill({0, 0, 0, 0});
    Node new_state = start_state;
    Generate(new_state, 0, sorted_actions, num_actions, 0, node_counter,
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
      RoundId min_round = +actions[i].min_round;
      RoundId max_round = +actions[i].max_round;
      for (RoundId round = min_round; round <= max_round; ++round) {
        sorted_actions[round][num_actions[round]] = actions[i];
        ++num_actions[round];
      }
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
  static SequenceId Generate(Node<kPlayers>& state, int num_raises,
                             const ActionArray& actions,
                             const NumActionsArray& num_actions, SequenceId seq,
                             NumNodesArray& node_counter,
                             RowMarkFn&& row_marker) {
    if (node_counter[+state.round()].internal_nodes == kLeafId ||
        node_counter[+state.round()].leaf_nodes == kLeafId ||
        node_counter[+state.round()].illegal_nodes == kLeafId) {
      throw std::overflow_error("Sequence table has too many rows.");
    } else if (!state.in_progress()) {
      ++node_counter[+state.round()].leaf_nodes;
      return kLeafId;
    } else if (state.acting_player() == state.kChancePlayer) {
      state.ProceedPlay();
      return Generate(state, 0, actions, num_actions, seq, node_counter,
                      row_marker);
    }

    ++node_counter[+state.round()].internal_nodes;
    for (std::size_t j = 0; j < num_actions[+state.round()]; ++j) {
      AbstractAction action = actions[+state.round()][j];
      Chips chip_size = ActionSize(action, state, num_raises);
      if (chip_size) {
        ++node_counter[+state.round()].legal_actions;
        Node<kPlayers> new_state = state;
        new_state.Apply(action.play, chip_size);
        Round new_round = new_state.round();
        int new_raise_num = num_raises;
        if (action.play == Action::kBet) ++new_raise_num;
        SequenceId new_state_id = Generate(new_state, new_raise_num, actions,
            num_actions, node_counter[+new_round].internal_nodes, node_counter,
            row_marker);
        row_marker(seq, state.round(), j, new_state_id);
      } else {
        ++node_counter[+state.round()].illegal_nodes;
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
                          const Node<kPlayers>& state, int num_raises) {
    if ((action.max_raise_num == 0) || (num_raises < action.max_raise_num)) {
      switch (action.play) {
        case Action::kAllIn:
          return true;
        case Action::kCheckCall:
          return state.CanCheckCall();
        case Action::kFold:
          return state.CanFold();
        case Action::kBet:
          Chips size = state.ProportionToChips(action.size);
          if (state.pot() < action.min_pot) {
            return false;
          }
          if (action.max_players != 0 &&
              state.players_left() > action.max_players) {
            return false;
          }
          if (1.0 * size / state.stack(state.acting_player()) >
              kPotCommittedThreshold) {
            return false;
          }
          return state.CanBet(size) ? size : false;
      }
    }
    return false;
  }

  /*
    @brief Computes the number of legal actions available in the given infoset.
  */
  nda::size_t ComputeLegalActions(Round round, SequenceId seq) const {
    nda::size_t count = 0;
    for (nda::index_t action_id : table_[+round].j()) {
      if (table_[+round](seq, action_id) != kIllegalId) {
        ++count;
      }
    }
    return count;
  }

  /*
    @brief Computes the legal_offsets_ array. 
  */
  void ComputeLegalOffsets() {
    for (RoundId i = 0; i < kNRounds; ++i) {
      legal_offsets_[i].resize(States(Round{i}));
      std::fill(legal_offsets_[i].begin(), legal_offsets_[i].end(), 0);
      for (SequenceId j = 1; j < legal_offsets_[i].size(); ++j) {
        legal_offsets_[i][j] = legal_offsets_[i][j - 1] +
                               ComputeLegalActions(Round{i}, j - 1);
      }
    }
  }
};  // class SequenceTable

}  // namespace fishbait

#endif  // AI_SRC_MCCFR_SEQUENCE_TABLE_H_
