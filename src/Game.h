#ifndef GAME
#define GAME

#include "hand_evaluator/Constants.h"
#include "Agent.h"
#include "GameState.h"

#define FLOP_1 12
#define FLOP_2 13
#define FLOP_3 14
#define TURN 15
#define RIVER 16
 
class Game
{
  private:
    int deck_ [DECK_SIZE];
    Agent** agents_;
    int small_blind_pos_;
    GameState game_state_;

    void ShuffleDeck();
    void AwardPot();
    void Preflop(int button_pos);
    void Flop();
    void Turn();
    void River();

  public:
    Game(char num_players, char num_rounds, float small_blind_multiplier, 
         double starting_bb_amounts);
    void Play(int button_pos);
};
 
#endif
