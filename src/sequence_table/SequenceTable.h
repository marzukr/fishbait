// Copyright Emily Dale 2021

#ifndef SRC_SEQUENCE_TABLE_SEQUENCETABLE_H_
#define SRC_SEQUENCE_TABLE_SEQUENCETABLE_H_

#include <stdint.h>
#include <vector>
#include "poker_engine/GameState.h"
#include "utils/Matrix.h"

const uint32_t kIllegalActionVal = 0;

namespace blueprint_strategy {

class SequenceTable {
 public:
  SequenceTable(const std::vector<float> &raise_sizes,
                const std::vector<char> &num_raise_sizes,
                int32_t starting_chips, int32_t small_blind, int32_t big_blind,
                char num_players, char num_rounds, bool only_nums,
                char multi_player_max, char num_raises_limit,
                char round_limit_multi,
                const std::vector<float> pre_late_raise_sizes);
  ~SequenceTable();
  uint32_t** table_;
  char num_rounds_;
  std::vector<uint32_t> num_rows_;
  std::vector<char> num_raise_sizes_;
  std::vector<int> raise_size_index_;
  uint64_t terminal_rows_;
  uint32_t GetTotalRows();
  uint32_t GetPreflopRows();
  uint32_t GetFlopRows();
  uint32_t GetTurnRows();
  uint32_t GetRiverRows();
  friend std::ostream& operator<<(std::ostream&, const SequenceTable& s);
  void AllocateRow(const uint32_t& index, const char& current_round);
  std::vector<int32_t> GetAllActions(uint32_t row, poker_engine::GameState state);
  int32_t IndexToAction(uint32_t row, uint32_t col, poker_engine::GameState state);
  std::vector<uint32_t> GetAllActionIndicies(uint32_t row, poker_engine::GameState state);

 private:
  void Update(poker_engine::GameState& state,   // NOLINT (runtime/references)
              const uint32_t& orig_index,
              const bool& update_nums);
  int32_t LegalAction(const double& action, const int32_t& call,
                      const float& raise_mult, const int32_t& prev,
                      const poker_engine::GameState& state);
  float IndexToRaise(int index);
  uint32_t total_rows_;
  uint32_t num_illegal_;
  char multi_player_max_;
  char num_raises_limit_;
  char round_limit_multi_;
  bool only_nums_;
  std::vector<float> raise_sizes_;
  std::vector<float> pre_late_raise_sizes_;
  int32_t LegalActionFold(const int32_t& call) {
    if (call == 0) {
      num_illegal_ += 1;
      return -2;
    } else {
      return -1;
    }
  }
  int32_t LegalActionCall(const int32_t& call,
                          const poker_engine::GameState& state) {
    if (call < state.chip_amounts_[state.acting_player_]) {
      return call;
    } else {
      num_illegal_ += 1;
      return -2;
    }
  }
};  // class SequenceTable

}  // namespace blueprint_strategy

#endif  // SRC_SEQUENCE_TABLE_SEQUENCETABLE_H_
