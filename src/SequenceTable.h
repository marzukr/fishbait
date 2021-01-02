#ifndef SEQUENCETABLE
#define SEQUENCETABLE
#define NUM_RAISES 1
#define NUM_ROUNDS 4

#include "GameState.h"

class SequenceTable{

  private:
  void NumPossible(const GameState& state);
  void Update(const GameState& state, unsigned int orig_index);
  unsigned int current_index_;
  bool update_nums_;
  float raise_sizes_ [NUM_ROUNDS][NUM_RAISES];
  float num_raise_sizes_ [NUM_ROUNDS];
  unsigned int num_rows_[5];

  public:
  SequenceTable(float raise_sizes [NUM_ROUNDS][NUM_RAISES], int num_raise_sizes[NUM_ROUNDS], 
                   double starting_chips, float small_blind_multiplier, 
                   char num_players, char num_rounds);
  unsigned int** table_;
  unsigned int getTotalRows();
};

#endif
