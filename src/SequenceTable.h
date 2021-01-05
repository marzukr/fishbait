#ifndef SEQUENCETABLE
#define SEQUENCETABLE

#include <stdint.h>
#include "GameState.h"
#include "Matrix.h"

const char kNumRaises = 2;
const char kNumRounds = 4;
const uint32_t kIllegalActionVal = 0;

namespace blueprint_strategy {

class SequenceTable { 
 public:
  SequenceTable(float raise_sizes_2d[kNumRounds][kNumRaises], 
                char num_raise_sizes[kNumRounds], 
                double starting_chips, float small_blind_multiplier, 
                char num_players, char num_rounds);
  ~SequenceTable();
  clustering::Matrix<uint32_t>* table_[kNumRounds];
  uint32_t GetTotalRows();
  uint32_t GetPreflopRows();
  uint32_t GetFlopRows();
  uint32_t GetTurnRows();
  uint32_t GetRiverRows();
  // overload << instead
  void PrintTable(bool full_table, char num_rounds);
  double IndexToAction(uint32_t action_index, game_engine::GameState state);

 private:
  void Update(game_engine::GameState& state, uint32_t orig_index, 
                           float raise_sizes [kNumRounds*kNumRaises], int raise_size_index[kNumRounds+1],
                           char num_raise_sizes[kNumRounds], bool update_nums);
  double LegalAction(double action, double call, float raise_mult, 
                     game_engine::GameState state);
  // get rid of unneccessary member variables
  uint32_t total_rows_;
  uint32_t terminal_rows_[kNumRounds];
  char num_rounds_;
  float raise_sizes_ [kNumRounds][kNumRaises];
  char num_raise_sizes_ [kNumRounds];
  uint32_t num_rows_[kNumRounds];

};  // class SequenceTable

}  // namespace blueprint_strategy

#endif  // SEQUENCETABLE
