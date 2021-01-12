#include "sequence_table/SequenceTable.h"
#include <vector>
#include <stdint.h>

const int kDeckSize = 52;

namespace blueprint_strategy {

class Game {
  public:
    Game(char num_players, char num_rounds, int32_t small_blind,
       int32_t big_blind, int32_t starting_amounts, char small_blind_pos);
    poker_engine::GameState state_;
    char num_dealt;
    int deck_[kDeckSize];
    std::vector<int32_t> player_cards_;
    std::vector<int> board_cards_;
    std::vector<int32_t> GetUtility();
    uint16_t GetInfosetIndex(char player);
    bool IsChanceNode() {
      return (num_dealt != state_.current_round_);
    }
    void Deal();
}
}