#include "sequence_table/SequenceTable.h"
#include <vector>
#include <stdint.h>
#include "sequence_table/Game.h"
namespace blueprint_strategy{
class Regret {
 public:

  SequenceTable action_table_;
  int32_t** regret_table_;
  float** strategy_table_;
  uint32_t** action_counter_;
  uint32_t action_counter_rows_;
  uint32_t num_infosets_;
  uint32_t regret_table_rows_;
  char num_players_;
  char num_rounds_; 
  int32_t small_blind_;
  int32_t big_blind_; 
  int32_t starting_amounts_;
  char small_blind_pos_;
  std::vector<float> CalculateStrategy(uint32_t regret_index, uint16_t infoset_index, poker_engine::GameState& state);
  void UpdateStrategy(uint32_t action_index, Game& game, char player);
  int32_t TraverseMCCFR(uint32_t action_index, Game& game, char player, bool prune);
  void MCCFRP();
}
}