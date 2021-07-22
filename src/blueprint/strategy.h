// Copyright 2021 Marzuk Rashid and Emily Dale

#ifndef SRC_BLUEPRINT_STRATEGY_H_
#define SRC_BLUEPRINT_STRATEGY_H_

#include "array/matrix.h"
#include "blueprint/definitions.h"
#include "blueprint/sequence_table.h"
#include "engine/definitions.h"
#include "engine/node.h"

namespace blueprint {

template <engine::PlayerN kPlayers, int kActions>
class Strategy {
 public:
  /*
    @brief Constructor

    @param start_state The starting state of the game.
    @param actions The actions available in the abstracted game tree.
    @param iterations The number of iterations to run mccfr.
    @param strategy_interval The number of iterations between each update of the
        average strategy.
    @param prune_threshold The number of iterations to wait before pruning.
    @param LCFR_threshold The number of iterations to apply LCFR.
    @param discount_interval The number of iterations between each LCFR
        discount.
    @param regret_floor Floor to cutoff negative regrets at.
    @param snapshot_interval The number of iterations between each snapshot.
    @param strategy_delay The number of iterations to wait before updating the
        average strategy and taking snapshots.
    @param verbose Whether to print debug information.
  */
  Strategy(const engine::Node<kPlayers>& start_state,
           const Action actions[kActions], int iterations,
           int strategy_interval, int prune_threshold, int LCFR_threshold,
           int discount_interval, Regret regret_floor, int snapshot_interval,
           int strategy_delay, bool verbose = false);

 private:
  /*
    @brief Computes the strategy using the MCCFR algorithm.

    @param iterations The number of iterations to run mccfr.
    @param strategy_interval The number of iterations between each update of the
        average strategy.
    @param prune_threshold The number of iterations to wait before pruning.
    @param LCFR_threshold The number of iterations to apply LCFR.
    @param discount_interval The number of iterations between each LCFR
        discount.
    @param snapshot_interval The number of iterations between each snapshot.
    @param strategy_delay The number of iterations to wait before updating the
        average strategy and taking snapshots.
    @param verbose Whether to print debug information.
  */
  void MCCFR(int iterations, int strategy_interval, int prune_threshold,
             int LCFR_threshold, int discount_interval, int snapshot_interval,
             int strategy_delay, bool verbose) const;

  /*
    @brief Computes the strategy at the given infoset from regrets.

    @param i The infoset to compute the strategy for.
    @param strategy Array to store the computed strategy.
  */
  void CalculateStrategy(Infoset i, double strategy[kActions]) const;

  /*
    @brief Samples an action from the strategy at the given infoset.

    @param i The infoset to sample an action from.

    @return The index of the sampled action.
  */
  int SampleAction(Infoset i) const;

  /*
    @brief Recursively iterates through actions and card dealings and thus 
           sequenceID/cardID combinations

    @param sequenceID ID of current action sequence
    @param cardID ID of card infoset
    @param index of current player
  */
  void UpdateStrategy(blueprint::sequenceID sequenceID, uint32_t cardID,
                      uint8_t player);

  /*
    @brief Recursively iterates over game tree (by sampling action from current 
           regrets) and updates regrets table with outcome 

    @param sequenceID ID of current action sequence
    @param cardID ID of card infoset
    @param index of current player
    @param whether to prune extreme regrets
  */
  void TraverseMCCFR(blueprint::sequenceID sequenceID, uint32_t cardID,
                      uint8_t player, bool prune);

  const Regret regret_floor_;
  SequenceTable<kPlayers, kActions> sequences_;
  nda::matrix<Regret> regrets_[engine::kNRounds];
  nda::matrix<ActionCount> action_counts_;
};  // class Strategy

}  // namespace blueprint

#endif  // SRC_BLUEPRINT_STRATEGY_H_
