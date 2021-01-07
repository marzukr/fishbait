#ifndef SEQUENCETABLE
#define SEQUENCETABLE

#include <stdint.h>
#include "poker_engine/GameState.h"
#include "utils/Matrix.h"

const char kNumRaises = 3;
const char kNumRounds = 4;
const uint32_t kIllegalActionVal = 0;

namespace blueprint_strategy {

class SequenceTable { 
 public:
  SequenceTable(float raise_sizes_2d[kNumRounds][kNumRaises], 
                char num_raise_sizes[kNumRounds], 
                long starting_chips, long small_blind, long big_blind, 
                char num_players, char num_rounds, bool only_nums);
  ~SequenceTable();
  // utils::Matrix<uint32_t>* table_[kNumRounds];
  // utils::Matrix<uint32_t>* table_;
  // uint32_t** table_[kNumRounds];
  uint32_t** table_;
  uint32_t GetTotalRows();
  uint32_t GetPreflopRows();
  uint32_t GetFlopRows();
  uint32_t GetTurnRows();
  uint32_t GetRiverRows();
  // overload << instead
  void PrintTable(bool full_table, char num_rounds);
  void AllocateRow(uint32_t index, char current_round, char num_raise_sizes[kNumRounds]);
  double IndexToAction(uint32_t action_index, poker_engine::GameState state);

 private:
  void Update(poker_engine::GameState& state, uint32_t& orig_index, 
                           float raise_sizes [kNumRounds*(kNumRaises+3)], int raise_size_index[kNumRounds+1],
                           char num_raise_sizes[kNumRounds], bool& update_nums);
  long LegalAction(double& action, long& call, float& raise_mult, 
                     poker_engine::GameState& state);
  //get rid of unneccessary member variables
  uint32_t total_rows_;
  // uint32_t terminal_rows_[kNumRounds];
  char num_rounds_;
  float raise_sizes_ [kNumRounds][kNumRaises];
  char num_raise_sizes_ [kNumRounds];
  long LegalActionFold (long& call) {
    return call == 0 ? -2 : -1;
  }
  long LegalActionCall (long& call, poker_engine::GameState& state) {
    return call < state.chip_amounts_[state.acting_player_] ? call : -2;
  }
  uint32_t num_rows_[kNumRounds];
  uint32_t terminal_rows_;

};  // class SequenceTable

}  // namespace blueprint_strategy

#endif  // SEQUENCETABLE
