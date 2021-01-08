#ifndef SEQUENCETABLEMULTARRAY
#define SEQUENCETABLEMULTARRAY

#include <stdint.h>
#include <vector>
#include "poker_engine/GameState.h"
#include "utils/Matrix.h"

const uint32_t kIllegalActionVal = 0;

namespace blueprint_strategy {

class SequenceTableMultArray { 
 public:
  SequenceTable(std::vector<float> &raise_sizes, 
                std::vector<char> &num_raise_sizes, 
                long starting_chips, long small_blind, long big_blind, 
                char num_players, char num_rounds, bool only_nums);
  ~SequenceTable();
  uint32_t** table_[4];
  uint32_t GetTotalRows();
  uint32_t GetPreflopRows();
  uint32_t GetFlopRows();
  uint32_t GetTurnRows();
  uint32_t GetRiverRows();
  // overload << instead
  void PrintTable(bool full_table, char num_rounds);
  void AllocateRow(uint32_t& index, char& current_round, std::vector<char> &num_raise_sizes);
  // double IndexToAction(uint32_t action_index, poker_engine::GameState state);

 private:
  void Update(poker_engine::GameState& state, uint32_t& orig_index, 
                           std::vector<float> &raise_sizes, std::vector<int> &raise_size_index,
                           std::vector<char> &num_raise_sizes, bool& update_nums);
  long LegalAction(double& action, long& call, float& raise_mult, 
                     poker_engine::GameState& state);
  //get rid of unneccessary member variables
  uint32_t total_rows_;
  char num_rounds_;
  std::vector<char> num_raise_sizes_;
  long LegalActionFold (long& call) {
    return call == 0 ? -2 : -1;
  }
  long LegalActionCall (long& call, poker_engine::GameState& state) {
    return call < state.chip_amounts_[state.acting_player_] ? call : -2;
  }
  std::vector<uint32_t> num_rows_;
  std::vector<uint32_t> terminal_rows_;

};  // class SequenceTableMultARRAY

}  // namespace blueprint_strategy

#endif  // SEQUENCETABLEMULTARRAY