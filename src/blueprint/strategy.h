// Copyright 2021 Marzuk Rashid and Emily Dale

#ifndef SRC_BLUEPRINT_STRATEGY_H_
#define SRC_BLUEPRINT_STRATEGY_H_

#include <algorithm>
#include <array>
#include <iostream>
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
    @param prune_probability The probability that we prune in Traverse-MCCFR.
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
           int strategy_interval, int prune_threshold, double prune_probability,
           Regret prune_constant, int LCFR_threshold, int discount_interval,
           Regret regret_floor, int snapshot_interval, int strategy_delay,
           bool verbose = false)
           : regret_floor_{regret_floor}, prune_constant_{prune_constant},
             info_abstraction_{verbose},
             action_abstraction_{actions, start_state},
             regrets_{InitRegretTable()}, action_counts_{
                 InitInfosetActionTable<ActionCount>(Round::kPreFlop)
             }, rng_() {
    MCCFR(start_state, iterations, strategy_interval, prune_threshold,
          prune_probability, LCFR_threshold, discount_interval,
          snapshot_interval, strategy_delay, verbose);
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

    @param start_state The starting state of the game.
    @param iterations The number of iterations to run mccfr.
    @param strategy_interval The number of iterations between each update of the
        average strategy.
    @param prune_threshold The number of iterations to wait before pruning.
    @param prune_probability The probability that we prune in Traverse-MCCFR.
    @param LCFR_threshold The number of iterations to apply LCFR.
    @param discount_interval The number of iterations between each LCFR
        discount.
    @param snapshot_interval The number of iterations between each snapshot.
    @param strategy_delay The number of iterations to wait before updating the
        average strategy and taking snapshots.
    @param verbose Whether to print debug information.
  */
  void MCCFR(const Node<kPlayers>& start_state, int iterations,
             int strategy_interval, int prune_threshold,
             double prune_probability, int LCFR_threshold,
             int discount_interval, [[maybe_unused]] int snapshot_interval,
             int strategy_delay, bool verbose) {
    if (verbose) std::cout << "Starting MCCFR" << std::endl;
    for (int t = 1; t <= iterations; ++t) {
      if (verbose) std::cout << "iteration: " << t << std::endl;
      for (PlayerId player = 0; player < kPlayers; ++player) {
        // update strategy after strategy_inveral iterations
        if (t > strategy_delay && t % strategy_interval == 0) {
          start_state.DealCards();
          UpdateStrategy(start_state,
                         info_abstraction_.Cluster(start_state, player), 0,
                         player);
        }

        // Set prune variable and traverse MCCFR
        bool prune = false;
        if (t > prune_threshold) {
          std::uniform_real_distribution<> uniform_distribution(0.0, 1.0);
          double random_prune = uniform_distribution(rng_());
          if (random_prune < prune_probability) {
            prune = true;
          }
        }
        start_state.DealCards();
        TraverseMCCFR(start_state, info_abstraction_.ClusterArray(start_state),
                      0, player, prune);
      }

      /* Discount all regrets and action counter every discount_interval until
         LCFR_threshold */
      if (t <= LCFR_threshold && t % discount_interval == 0) {
        double d = (t * 1.0 / discount_interval) /
                   (t * 1.0 / discount_interval + 1);
        for (RoundId r = 0; r < kNRounds; ++r) {
          std::for_each(regrets_[r].data(),
                        regrets_[r].data() + regrets_[r].size(),
                        [=](Regret& regret) {
                          regret = std::rint(regret * d);
                        });
        }
        std::for_each(action_counts_.data(),
                      action_counts_.data() + action_counts_.size(),
                      [=](ActionCount& count) {
                        count = std::rint(count * d);
                      });
      }
    }  // for t
  }  // MCCFR()

  /*
    @brief Returns the sum of all positive regrets at an infoset.

    @param card_bucket The card cluster id of the infoset.
    @param seq The sequence id of the infoset.
    @param round The betting round of the infoset.
  */
  Regret PositiveRegretSum(CardCluster card_bucket, SequenceId seq,
                           Round round) const {
    Regret sum = 0;
    for (nda::index_t action_id : regrets_[+round].k()) {
      sum += std::max(0, regrets_[+round](card_bucket, seq, action_id));
    }
    return sum;
  }

  /*
    @brief Computes the strategy at the given infoset from regrets.

    @param card_bucket The card cluster id of the infoset.
    @param seq The sequence id of the infoset.
    @param round The betting round of the infoset.

    @return An array with the computed strategy.
  */
  std::array<double, kActions> CalculateStrategy(CardCluster card_bucket,
                                                 SequenceId seq,
                                                 Round round) const {
    nda::size_t legal_actions = action_abstraction_.NumLegalActions(seq, round);
    Regret sum = PositiveRegretSum(card_bucket, seq, round);

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

    @param card_bucket The card cluster id of the infoset.
    @param seq The sequence id of the infoset.
    @param round The betting round of the infoset.

    @return The index of the sampled action.
  */
  nda::index_t SampleAction(CardCluster card_bucket, SequenceId seq,
                            Round round) {
    std::uniform_real_distribution<double> sampler(0, 1);
    double sampled = sampler(rng_());
    double bound = 0;
    Regret sum = PositiveRegretSum(card_bucket, seq, round);
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
    @param card_bucket The card cluster id of the infoset at the current
        recursive call.
    @param seq The sequence id of the infoset at the current recursive call.
    @param player The player whose strategy is being updated.
  */
  void UpdateStrategy(const Node<kPlayers>& state, CardCluster card_bucket,
                      SequenceId seq, PlayerId player);

  /*
    @brief Recursively updates the given player's cumulative regrets.

    @param state State of the game at the current recursive call.
    @param card_buckets The card cluster ids for each non folded player in the
        current round.
    @param seq The sequence id of the infoset at the current recursive call.
    @param player The player whose strategy is being updated.
    @param prune Whether to prune actions with regrets less than
        prunt_constant_.
  */
  void TraverseMCCFR(const Node<kPlayers>& state,
                     std::array<CardCluster, kPlayers> card_buckets,
                     SequenceId seq, PlayerId player, bool prune);
};  // class Strategy

}  // namespace fishbait

#endif  // SRC_BLUEPRINT_STRATEGY_H_
