#ifndef SEQUENCETABLE
#define SEQUENCETABLE
#define kNumRaises 5
#define kNumRounds 4

#include "GameState.h"

class SequenceTable{

  private:
  void Update(const GameState& state, unsigned int orig_index);
  void AllocateRow(unsigned int index, char current_round);
  unsigned int current_index_;
  bool update_nums_;
  float raise_sizes_ [kNumRounds][kNumRaises];
  float num_raise_sizes_ [kNumRounds];
  unsigned int num_rows_[kNumRounds];
  unsigned int total_rows_;

  public:
  SequenceTable(float raise_sizes [kNumRounds][kNumRaises], 
                int num_raise_sizes[kNumRounds], 
                double starting_chips, float small_blind_multiplier, 
                char num_players, char num_rounds);
  ~SequenceTable();
  unsigned int** table_;
  unsigned int GetTotalRows();
  unsigned int GetPreflopRows();
  unsigned int GetFlopRows();
  unsigned int GetTurnRows();
  unsigned int GetRiverRows();
  void PrintTable(bool full_table);

};

#endif
