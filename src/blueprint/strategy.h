// Copyright 2021 Marzuk Rashid and Emily Dale

#ifndef SRC_BLUEPRINT_STRATEGY_H_
#define SRC_BLUEPRINT_STRATEGY_H_

#include <algorithm>
#include <array>
#include <iostream>
#include <random>
#include <stdexcept>
#include <string>

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

  std::array<Chips, kPlayers> starting_stacks_;

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
             }, rng_(), starting_stacks_{InitStartingStacks(start_state)} {
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

  /* @brief Initializes starting stacks array. */
  std::array<Chips, kPlayers> InitStartingStacks(const Node<kPlayers>&
                                                 start_state) {
    std::array<Chips, kPlayers> starting_stacks;
    for (PlayerId player = 0; player < kPlayers; ++player) {
      starting_stacks[player] = start_state.stack(player);
    }
    return starting_stacks;
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
  void MCCFR(Node<kPlayers> start_state, int iterations, int strategy_interval,
             int prune_threshold, double prune_probability, int LCFR_threshold,
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
      double action_prob = 0;
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
    const std::string error = std::string(__func__) +
                              ": No action was selected.";
    throw std::runtime_error(error);
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
                      SequenceId seq, PlayerId player) {
    Round round = state.round();
    if (round > Round::kPreFlop || !state.in_progress() ||
        state.folded(player) || state.stack(player) == 0) {
      return;
    }
    nda::const_vector_ref<AbstractAction> actions =
        action_abstraction_.Actions(round);
    if (state.acting_player() == player) {
      Node<kPlayers> new_state = state;
      nda::index_t action_index = SampleAction(card_bucket, seq, round);
      AbstractAction action = actions(action_index);
      new_state.Apply(action.play, new_state.ConvertBet(action.size));
      action_counts_(card_bucket, seq, action_index) += 1;
      UpdateStrategy(new_state, card_bucket,
                     action_abstraction_.Next(seq, round, action_index),
                     player);
    } else {
      for (nda::index_t action_index = 0; action_index < actions.width();
           ++action_index) {
        if (action_abstraction_.Next(seq, round, action_index) != kIllegalId) {
          Node<kPlayers> new_state = state;
          AbstractAction action = actions(action_index);
          new_state.Apply(action.play, new_state.ConvertBet(action.size));
          UpdateStrategy(new_state, card_bucket,
                         action_abstraction_.Next(seq, round, action_index),
                         player);
        }
      }
    }
  }

  /*
    @brief Recursively updates the given player's cumulative regrets.

    @param state State of the game at the current recursive call.
    @param card_buckets The card cluster ids for each non folded player in the
        current round.
    @param seq The sequence id of the infoset at the current recursive call.
    @param player The player whose strategy is being updated.
    @param prune Whether to prune actions with regrets less than
        prunt_constant_.
    
    @return The value of the node.
  */
  double TraverseMCCFR(const Node<kPlayers>& state,
                       std::array<CardCluster, kPlayers> card_buckets,
                       SequenceId seq, PlayerId player, bool prune) {
    if (!state.in_progress() || state.folded(player)) {
      Node<kPlayers> new_state = state;
      new_state.AwardPot(new_state.same_stack_no_rake_);
      return 1.0 * new_state.stack(player) - 1.0 * starting_stacks_[player];
    }
    Round round = state.round();
    PlayerId acting_player = state.acting_player();
    nda::const_vector_ref<AbstractAction> actions =
        action_abstraction_.Actions(round);
    if (acting_player == player) {
      std::array<double, kActions> strategy = CalculateStrategy(
          card_buckets[player], seq, round);
      double value = 0;
      std::array<double, kActions> action_values;
      std::array<bool, kActions> explored;
      for (nda::index_t action_index = 0; action_index < actions.width();
           ++action_index) {
        SequenceId next_seq = action_abstraction_.Next(seq, round,
                                                       action_index);
        if ((next_seq != kIllegalId) & (!prune ||
          regrets_[+round](card_buckets[player], seq, action_index) >
          prune_constant_)) {
          AbstractAction action = actions(action_index);
          Node<kPlayers> new_state = state;
          new_state.Apply(action.play, new_state.ConvertBet(action.size));
          std::array<CardCluster, kPlayers> new_card_buckets =
              info_abstraction_.ClusterArray(new_state);
          double action_value = TraverseMCCFR(new_state, new_card_buckets,
                                              next_seq, player, prune);
          action_values[action_index] = action_value;
          value += action_value * strategy[action_index];
          explored[action_index] = true;
        } else {
          explored[action_index] = false;
        }
      }
      for (nda::index_t action_index = 0; action_index < actions.width();
           ++action_index) {
        if (explored[action_index]) {
          regrets_[+round](card_buckets[player], seq, action_index) = std::max(
              regret_floor_,
              (regrets_[+round](card_buckets[player], seq, action_index) +
              (Regret) std::rint(action_values[action_index] - value)));
        }
      }
      return value;
    } else {
      nda::index_t action_index = SampleAction(card_buckets[acting_player],
                                                seq, round);
      Node<kPlayers> new_state = state;
      AbstractAction action = actions(action_index);
      new_state.Apply(action.play, new_state.ConvertBet(action.size));
      std::array<CardCluster, kPlayers> new_card_buckets =
              info_abstraction_.ClusterArray(new_state);
      return TraverseMCCFR(new_state, new_card_buckets,
                           action_abstraction_.Next(seq, round, action_index),
                           player, prune);
    }  // if (acting_player == player)
  }  // TraverseMCCFR()
};  // class Strategy

}  // namespace fishbait

#endif  // SRC_BLUEPRINT_STRATEGY_H_
