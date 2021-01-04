#ifndef REGRET
#define REGRET

#include "SequenceTable.h"
#include "GameState.h"
#include "CardInfoset.h"

class Regret{
  private:
  SequenceTable sequence_table_;
  unsigned int non_leaf_;
  int** regret_table_;
  int** ev_table_;

  public:
  Regret(SequenceTable sequence_table);
  unsigned int TraverseMCCFR(unsigned int h, char p, CardInfoset infoset, GameState game_state);

};
#endif