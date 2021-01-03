#ifndef SEQUENCETABLE
#define SEQUENCETABLE
#define NUM_RAISES 3
#define NUM_ROUNDS 4

#include "GameState.h"

class SequenceTable{

  private:
  void Update(const GameState& state, unsigned int orig_index);
  void AllocateRow(unsigned int index, char current_round);
  unsigned int current_index_;
  bool update_nums_;
  float raise_sizes_ [NUM_ROUNDS][NUM_RAISES];
  float num_raise_sizes_ [NUM_ROUNDS];
  unsigned int num_rows_[5];
  unsigned int total_rows_;

  public:
  SequenceTable(float raise_sizes [NUM_ROUNDS][NUM_RAISES], int num_raise_sizes[NUM_ROUNDS], 
                   double starting_chips, float small_blind_multiplier, 
                   char num_players, char num_rounds);
  unsigned int** table_;
  unsigned int GetTotalRows();
  unsigned int GetPreflopRows();
  unsigned int GetFlopRows();
  unsigned int GetTurnRows();
  unsigned int GetRiverRows();
  void PrintTable();

};

#endif
