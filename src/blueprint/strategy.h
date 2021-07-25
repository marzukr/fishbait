// Copyright 2021 Marzuk Rashid and Emily Dale

#ifndef SRC_BLUEPRINT_STRATEGY_H_
#define SRC_BLUEPRINT_STRATEGY_H_

#include <algorithm>

#include "array/array.h"
#include "blueprint/definitions.h"
#include "blueprint/sequence_table.h"
#include "clustering/cluster_table.h"
#include "engine/definitions.h"
#include "engine/node.h"

namespace blueprint {

template <engine::PlayerN kPlayers, int kActions>
class Strategy {
 private:
  using InfosetActionTableShape = nda::shape<nda::dim<>, nda::dim<>,
                                             nda::dense_dim<>>;
  template <typename T>
  using InfosetActionTable = nda::array<T, InfosetActionTableShape>;

  const Regret regret_floor_;
  const Regret prune_constant_;

  clustering::ClusterTable info_abstraction_;
  SequenceTable<kPlayers, kActions> action_abstraction_;
  // Card Buckets x Sequences x Actions
  std::array<InfosetActionTable<Regret>, engine::kNRounds> regrets_;
  InfosetActionTable<ActionCount> action_counts_;

 public:
  /*
    @brief Constructor

    @param start_state The starting state of the game.
    @param actions The actions available in the abstracted game tree.
    @param iterations The number of iterations to run mccfr.
    @param strategy_interval The number of iterations between each update of the
        average strategy.
    @param prune_threshold The number of iterations to wait before pruning.
    @param prune_constant Actions with regret less than this constant are
        eligible to be pruned.
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
           const std::array<Action, kActions>& actions, int iterations,
           int strategy_interval, int prune_threshold, Regret prune_constant,
           int LCFR_threshold, int discount_interval, Regret regret_floor,
           int snapshot_interval, int strategy_delay, bool verbose = false);
  Strategy(const Strategy& other) = default;
  Strategy& operator=(const Strategy& other) = default;

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
             int strategy_delay, bool verbose);

  /*
    @brief Computes the strategy at the given infoset from regrets.

    @param seq The sequence id of the infoset.
    @param round The betting round of the infoset.
    @param card_bucket The card cluster id of the infoset.

    @return An array with the computed strategy.
  */
  std::array<double, kActions> CalculateStrategy(SequenceId seq,
      engine::Round round, clustering::CardCluster card_bucket) const {
    engine::RoundId round_id = engine::GetRoundId(round);

    Regret sum = 0;
    int legal_action_count = 0;
    for (nda::index_t action_id : regrets_[round_id].k()) {
      if (action_abstraction_.Next(seq, round_id, action_id) != kIllegalId) {
        ++legal_action_count;
      }
      sum += std::max(0, regrets_[round_id](card_bucket, seq, action_id));
    }

    std::array<double, kActions> strategy = {0};
    for (nda::index_t action_id : regrets_[round_id].k()) {
      if (sum > 0) {
        strategy[action_id] = std::max(0, regrets_[round_id](card_bucket, seq,
                                                             action_id));
        strategy[action_id] /= sum;
      } else {
        strategy[action_id] = 1.0 / legal_action_count;
      }
    }

    return strategy;
  }

  /*
    @brief Samples an action from the strategy at the given infoset.

    @param seq The sequence id of the infoset.
    @param round The betting round of the infoset.
    @param card_bucket The card cluster id of the infoset.

    @return The index of the sampled action.
  */
  int SampleAction(SequenceId seq, engine::Round round,
      clustering::CardCluster card_bucket) const;

  /*
    @brief Recursively updates the given player's average preflop strategy.

    @param state State of the game at the current recursive call.
    @param seq The sequence id of the infoset at the current recursive call.
    @param card_bucket The card cluster id of the infoset at the current
        recursive call.
    @param player The player whose strategy is being updated.
  */
  void UpdateStrategy(engine::Node<kPlayers>& state, SequenceId seq,
                      clustering::CardCluster card_bucket,
                      engine::PlayerId player);

  /*
    @brief Recursively updates the given player's cumulative regrets.

    @param state State of the game at the current recursive call.
    @param seq The sequence id of the infoset at the current recursive call.
    @param card_bucket The card cluster ids for each non folded player in the
        current round.
    @param player The player whose strategy is being updated.
    @param prune Whether to prune actions with regrets less than
        prunt_constant_.
  */
  void TraverseMCCFR(engine::Node<kPlayers>& state, SequenceId seq,
                     std::array<clustering::CardCluster, kPlayers> card_buckets,
                     engine::PlayerId player, bool prune);
};  // class Strategy

}  // namespace blueprint

#endif  // SRC_BLUEPRINT_STRATEGY_H_
