#ifndef SEQUENCETABLE
#define SEQUENCETABLE
#define kNumRaises 3
#define kNumRounds 4
const unsigned int kIllegalActionVal = 0;

#include "GameState.h"

class SequenceTable{
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
  double IndexToAction(unsigned int action_index, GameState state);
  unsigned int total_rows_;


  private:
  void Update(GameState& state, unsigned int orig_index);
  void AllocateRow(unsigned int index, char current_round);
  bool IsLegalAction(double action, double raise_size, GameState state);
  unsigned int current_index_;
  unsigned int terminal_index_;
  bool update_nums_;
  char num_rounds_;
  float raise_sizes_ [kNumRounds][kNumRaises];
  float num_raise_sizes_ [kNumRounds];
  unsigned int num_rows_[kNumRounds];

};

#endif
