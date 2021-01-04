#ifndef SEQUENCETABLE
#define SEQUENCETABLE

#include <stdint.h>
#include "GameState.h"

const char kNumRaises = 3;
const char kNumRounds = 4;
const uint32_t kIllegalActionVal = 0;

namespace blueprint_strategy {

class SequenceTable { 
 public:
  SequenceTable(float raise_sizes[kNumRounds][kNumRaises], 
                char num_raise_sizes[kNumRounds], 
                double starting_chips, float small_blind_multiplier, 
                char num_players, char num_rounds);
  ~SequenceTable();

  // switch to fast array not **
  uint32_t** table_;
  uint32_t GetTotalRows();
  uint32_t GetPreflopRows();
  uint32_t GetFlopRows();
  uint32_t GetTurnRows();
  uint32_t GetRiverRows();
  // overload << instead
  void PrintTable(bool full_table);
  double IndexToAction(uint32_t action_index, game_engine::GameState state);
  uint32_t total_rows_;

 private:
  void Update(game_engine::GameState& state, uint32_t orig_index);
  void AllocateRow(uint32_t index, char current_round);
  double LegalAction(double action, double call, float raise_mult, 
                     game_engine::GameState state);
  uint32_t current_index_;
  uint32_t terminal_index_;
  // get rid of unneccessary member variables
  bool update_nums_;
  char num_rounds_;
  float raise_sizes_ [kNumRounds][kNumRaises];
  char num_raise_sizes_ [kNumRounds];
  uint32_t num_rows_[kNumRounds];

};  // class SequenceTable

}  // namespace blueprint_strategy

#endif  // SEQUENCETABLE
