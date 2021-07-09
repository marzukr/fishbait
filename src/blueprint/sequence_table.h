// Copyright 2021 Emily Dale and Marzuk Rashid

#ifndef SRC_BLUEPRINT_SEQUENCE_TABLE_H_
#define SRC_BLUEPRINT_SEQUENCE_TABLE_H_

#include <cstdint>
#include <limits>
#include <ostream>
#include <stdexcept>
#include <vector>

#include "array/array.h"
#include "array/matrix.h"
#include "poker_engine/node.h"
#include "utils/fraction.h"

namespace blueprint {

struct Action {
  poker_engine::Action play;

  // Bet size proportional to the pot. Only matters if play is a kBet.
  double size = 0;

  /* The largest rotation this action can be player. Not inclusive. 0 indicates
     the action can be player on any rotation. */
  uint8_t max_rotation = 0;

  // The latest round this action can be player. Inclusive.
  poker_engine::Round max_round = poker_engine::Round::kRiver;
};

template <uint8_t kPlayers>
class SequenceTable {
 public:
  using ActionIdT = uint32_t;

  SequenceTable(const std::vector<Action>& actions,
                const poker_engine::Node<kPlayers>& start_state)
                : actions_{actions}, start_state_{start_state}, table_{} {
    ActionIdT rows = Count(actions_, start_state_);
    table_ = nda::matrix<ActionIdT>{{rows, actions.size()}};
    rows = 0;
    Generate(start_state_, actions_, 0, &rows,
        [&](ActionIdT id, size_t action_col, ActionIdT val) {
          this->table_(id, action_col) = val;
        });
  }  // SequenceTable()

  SequenceTable(const SequenceTable& other) = default;
  SequenceTable& operator=(const SequenceTable& other) = default;

  ActionIdT Next(ActionIdT current_node, size_t action_idx) const {
    return table_(current_node, action_idx);
  }

  ActionIdT States() const {
    return table_.rows();
  }

  ActionIdT Actions() const {
    return table_.columns();
  }

  friend std::ostream& operator<<(std::ostream& os, const SequenceTable& s) {
    uint64_t elements = s.table_.rows() * s.table_.columns();
    size_t bytes = sizeof(ActionIdT) * elements;
    double memory = bytes / 1.0e+9;
    os << "SequenceTable<" << +kPlayers << ">" << " { "
       << "rows: " << s.States() << "; "
       << "memory: ~" << memory << " GB; "
       << "}";
    return os;
  }

  static ActionIdT Count(const std::vector<Action>& actions,
      const poker_engine::Node<kPlayers>& start_state) {
    ActionIdT rows = 0;
    Generate(start_state, actions, 0, &rows,
        [](ActionIdT, size_t, ActionIdT) {});
    return rows;
  }

  static constexpr ActionIdT kLeafId = 0;
  static constexpr ActionIdT kIllegalId = std::numeric_limits<ActionIdT>::max();

 private:
  template <typename RowMarkFn>
  static void Generate(const poker_engine::Node<kPlayers>& state,
                       const std::vector<Action>& actions, ActionIdT id,
                       ActionIdT* row_counter, RowMarkFn&& row_marker) {
    if (*row_counter == std::numeric_limits<ActionIdT>::max()) {
      throw std::overflow_error("Sequence table has too many rows.");
    }
    ++(*row_counter);
    for (size_t j = 0; j < actions.size(); ++j) {
      Action action = actions[j];
      uint32_t chip_size = ActionSize(action, state);
      if (chip_size) {
        poker_engine::Node<kPlayers> new_state = state;
        if (new_state.Apply(action.play, chip_size)) {
          row_marker(id, j, *row_counter);
          Generate(new_state, actions, *row_counter, row_counter, row_marker);
        } else {
          row_marker(id, j, kLeafId);
        }
      } else {
        row_marker(id, j, kIllegalId);
      }
    }
  }

  /*
    @brief Converts the pot proportion of action to a state Apply()-able size.

    @return The size of the action to play if it can be played, 0 otherwise.
  */
  static uint32_t ActionSize(const Action& action,
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
          uint32_t size = state.ConvertBet(action.size);
          return state.CanBet(size) ? size : false;
      }
    }
    return false;
  }

  const std::vector<Action> actions_;
  const poker_engine::Node<kPlayers> start_state_;
  nda::matrix<ActionIdT> table_;
};  // class SequenceTable

}  // namespace blueprint

#endif  // SRC_BLUEPRINT_SEQUENCE_TABLE_H_
