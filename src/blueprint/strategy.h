// Copyright 2021 Marzuk Rashid and Emily Dale

#ifndef SRC_BLUEPRINT_STRATEGY_H_
#define SRC_BLUEPRINT_STRATEGY_H_

#include <algorithm>
#include <array>
#include <random>

#include "array/array.h"
#include "blueprint/definitions.h"
#include "blueprint/sequence_table.h"
#include "clustering/cluster_table.h"
#include "poker/definitions.h"
#include "poker/node.h"
#include "utils/random.h"

namespace fishbait {

template <PlayerN kPlayers, int kActions>
class Strategy {
 private:
  using InfosetActionTableShape = nda::shape<nda::dim<>, nda::dim<>,
                                             nda::dense_dim<>>;
  template <typename T>
  using InfosetActionTable = nda::array<T, InfosetActionTableShape>;

  const Regret regret_floor_;
  const Regret prune_constant_;

  ClusterTable info_abstraction_;
  SequenceTable<kPlayers, kActions> action_abstraction_;
  // Card Buckets x Sequences x Actions
  std::array<InfosetActionTable<Regret>, kNRounds> regrets_;
  InfosetActionTable<ActionCount> action_counts_;
  Random rng_;

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
  Strategy(const Node<kPlayers>& start_state,
           const std::array<AbstractAction, kActions>& actions, int iterations,
           int strategy_interval, int prune_threshold, Regret prune_constant,
           int LCFR_threshold, int discount_interval, Regret regret_floor,
           int snapshot_interval, int strategy_delay, bool verbose = false)
           : regret_floor_{regret_floor}, prune_constant_{prune_constant},
             info_abstraction_{verbose},
             action_abstraction_{actions, start_state},
             regrets_{InitRegretTable()}, action_counts_{
                 InitInfosetActionTable<ActionCount>(+Round::kPreFlop)
             }, rng_{} {
    MCCFR(iterations, strategy_interval, prune_threshold, LCFR_threshold,
          discount_interval, snapshot_interval, strategy_delay, verbose);
  }
  Strategy(const Strategy& other) = default;
  Strategy& operator=(const Strategy& other) = default;

 private:
  /*
    @brief Initializes regret table.
  */
  std::array<InfosetActionTable<Regret>, kNRounds> InitRegretTable() {
    std::array<InfosetActionTable<Regret>, kNRounds> regret_table;
    for (RoundId r = 0; r < kNRounds; ++r) {
      regret_table[r] = InitInfosetActionTable<Regret>(Round{r});
    }
    return regret_table;
  }

  /*
    @brief Initializes a card x sequence x action table for a given round.
  */
  template<typename T>
  InfosetActionTable<T> InitInfosetActionTable(Round r) {
    return InfosetActionTable<T>{{NumClusters(r), action_abstraction_.States(r),
                                  action_abstraction_.ActionCount(r)}, 0};
  }

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
    @brief Returns the sum of all positive regrets at an infoset.

    @param seq The sequence id of the infoset.
    @param round The betting round of the infoset.
    @param card_bucket The card cluster id of the infoset.
  */
  Regret PositiveRegretSum(SequenceId seq, Round round,
                           CardCluster card_bucket) const {
    Regret sum = 0;
    for (nda::index_t action_id : regrets_[+round].k()) {
      sum += std::max(0, regrets_[+round](card_bucket, seq, action_id));
    }
    return sum;
  }

  /*
    @brief Computes the strategy at the given infoset from regrets.

    @param seq The sequence id of the infoset.
    @param round The betting round of the infoset.
    @param card_bucket The card cluster id of the infoset.

    @return An array with the computed strategy.
  */
  std::array<double, kActions> CalculateStrategy(SequenceId seq, Round round,
      CardCluster card_bucket) const {
    nda::size_t legal_actions = action_abstraction_.NumLegalActions(seq, round);
    Regret sum = PositiveRegretSum(seq, round, card_bucket);

    std::array<double, kActions> strategy = {0};
    for (nda::index_t action_id : regrets_[+round].k()) {
      if (sum > 0) {
        /* We don't need to check if the action is legal here because illegal
           actions always have a regret of 0 and thus will be assigned a
           probability of 0 in the strategy. */
        strategy[action_id] = std::max(0, regrets_[+round](card_bucket, seq,
                                                           action_id));
        strategy[action_id] /= sum;
      } else if (action_abstraction_.Next(seq, round, action_id) !=
                 kIllegalId) {
        strategy[action_id] = 1.0 / legal_actions;
      }
    }

    return strategy;
  }

  /*
    @brief Samples an action from the current strategy at the given infoset.

    @param seq The sequence id of the infoset.
    @param round The betting round of the infoset.
    @param card_bucket The card cluster id of the infoset.

    @return The index of the sampled action.
  */
  nda::index_t SampleAction(SequenceId seq, Round round,
                            CardCluster card_bucket) {
    std::uniform_real_distribution<double> sampler(0, 1);
    double sampled = sampler(rng_());
    double bound = 0;
    Regret sum = PositiveRegretSum(seq, round, card_bucket);
    nda::size_t legal_actions = action_abstraction_.NumLegalActions(seq, round);
    for (nda::index_t action_id : regrets_[+round].k()) {
      double action_prob;
      if (sum > 0) {
        /* We don't need to check if the action is legal here because illegal
           actions always have a regret of 0 and thus will be assigned an
           action_prob of 0. */
        action_prob = std::max(0, regrets_[+round](card_bucket, seq,
                                                   action_id));
        action_prob /= sum;
      } else if (action_abstraction_.Next(seq, round, action_id) !=
                 kIllegalId) {
        action_prob = 1.0 / legal_actions;
      }

      bound += action_prob;
      if (sampled < bound) {
        return action_id;
      }
    }  // for action_id
  }  // SampleAction()

  /*
    @brief Recursively updates the given player's average preflop strategy.

    @param state State of the game at the current recursive call.
    @param seq The sequence id of the infoset at the current recursive call.
    @param card_bucket The card cluster id of the infoset at the current
        recursive call.
    @param player The player whose strategy is being updated.
  */
  void UpdateStrategy(Node<kPlayers>& state, SequenceId seq,
                      CardCluster card_bucket, PlayerId player);

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
  void TraverseMCCFR(Node<kPlayers>& state, SequenceId seq,
                     std::array<CardCluster, kPlayers> card_buckets,
                     PlayerId player, bool prune);
};  // class Strategy

}  // namespace fishbait

#endif  // SRC_BLUEPRINT_STRATEGY_H_
