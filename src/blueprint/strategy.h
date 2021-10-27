// Copyright 2021 Marzuk Rashid and Emily Dale

#ifndef SRC_BLUEPRINT_STRATEGY_H_
#define SRC_BLUEPRINT_STRATEGY_H_

#include <algorithm>
#include <array>
#include <cmath>
#include <filesystem>
#include <iostream>
#include <random>
#include <stdexcept>
#include <string>

#include "array/array.h"
#include "blueprint/definitions.h"
#include "blueprint/sequence_table.h"
#include "clustering/definitions.h"
#include "poker/definitions.h"
#include "poker/node.h"
#include "utils/cereal.h"
#include "utils/random.h"

namespace fishbait {

template <PlayerN kPlayers, std::size_t kActions, typename InfoAbstraction>
class Strategy {
 private:
  // Table of values for each legal action. Size card clusters * legal actions.
  using LegalActionsTableShape = nda::shape<nda::dim<>, nda::dense_dim<>>;
  template <typename T>
  using LegalActionsTable = nda::array<T, LegalActionsTableShape>;

  InfoAbstraction info_abstraction_;
  SequenceTable<kPlayers, kActions> action_abstraction_;

  // round * card clusters * legal actions
  std::array<LegalActionsTable<Regret>, kNRounds> regrets_;
  LegalActionsTable<ActionCount> action_counts_;

  Regret prune_constant_;     /* Actions with regret less than or equal to this
                                 constant are eligible to be pruned. */
  Regret regret_floor_;       /* Floor to cutoff negative regrets at. */

  Random rng_;

  /* Struct to represent the indexes of an action at an infoset. round_idx is
     the index numbered by counting all actions in a round. legal_idx is the
     index only counting the legal actions at the infoset. */
  struct ActionIndicies {
    std::size_t round_idx;
    std::size_t legal_idx;
  };

 public:
  /*
    @brief Constructor

    @param start_state The starting state of the game, must be compatible with 
        the Node::SameStackNoRake optimizations.
    @param actions The actions available in the abstracted game tree.
    @param info_abstraction An object that will abstract the card information.
    @param prune_constant Actions with regret less than or equal to this
        constant are eligible to be pruned.
    @param regret_floor Floor to cutoff negative regrets at.
    @param seed The seed to use for the random number generator used to sample
        actions and stochastically prune.
    @param verbose Whether to print debug information.
  */
  Strategy(const Node<kPlayers>& start_state,
           const std::array<AbstractAction, kActions>& actions,
           InfoAbstraction info_abstraction, Regret prune_constant,
           Regret regret_floor,
           Random::Seed seed = Random::Seed{}, bool verbose = false)
           : info_abstraction_{info_abstraction},
             action_abstraction_{actions, start_state},
             regrets_{InitRegretTable(verbose)}, action_counts_{
                 InitLegalActionsTable<ActionCount>(Round::kPreFlop)
             }, prune_constant_{prune_constant}, regret_floor_{regret_floor},
             rng_{seed} { }
  Strategy(const Strategy& other) = default;
  Strategy& operator=(const Strategy& other) = default;

  /* @brief Strategy serialize function. */
  template<class Archive>
  void serialize(Archive& archive) {
    archive(info_abstraction_, action_abstraction_, regrets_, action_counts_,
            prune_constant_, regret_floor_, rng_);
  }

  /*
    @brief Loads a Strategy snapshot from the given path on disk.

    @param path The path to the snapshot to load.
    @param verbose Whether to print debug information.

    @return The loaded Strategy.
  */
  static Strategy LoadSnapshot(const std::filesystem::path path,
                               bool verbose = false) {
    Strategy loaded;
    CerealLoad(path.string(), &loaded, verbose);
    return loaded;
  }

  /*
    @brief Updates the given player's average preflop strategy.

    @param player The player whose strategy is being updated.
  */
  void UpdateStrategy(PlayerId player) {
    Node<kPlayers> start_state_copy = action_abstraction_.start_state();
    UpdateStrategy(start_state_copy, 0, 0, player);
  }

  /*
    @brief Updates the given player's cumulative regrets.

    @param player The player whose strategy is being updated.
    @param prune Whether to prune actions with regrets less than
        prune_constant_.
  */
  void TraverseMCCFR(PlayerId player, bool prune) {
    Node<kPlayers> start_state_copy = action_abstraction_.start_state();
    std::array<CardCluster, kPlayers> card_buckets{};
    TraverseMCCFR(start_state_copy, card_buckets, 0, player, prune);
  }

  /*
    @brief Discounts the regrets and action counts by the given factor.
  */
  void Discount(double factor) {
    for (RoundId r = 0; r < kNRounds; ++r) {
      std::for_each(regrets_[r].data(),
                    regrets_[r].data() + regrets_[r].size(),
                    [=](Regret& regret) {
                      regret = std::rint(regret * factor);
                    });
    }
    std::for_each(action_counts_.data(),
                  action_counts_.data() + action_counts_.size(),
                  [=](ActionCount& count) {
                    count = std::rint(count * factor);
                  });
  }

  /*
    @brief Samples an action from the current strategy at the given infoset.

    @param round The betting round of the infoset.
    @param card_bucket The card cluster id of the infoset.
    @param seq The sequence id of the infoset.

    @return The indicies of the sampled action.
  */
  ActionIndicies SampleAction(Round round, CardCluster card_bucket,
                              SequenceId seq) {
    std::size_t offset = action_abstraction_.LegalOffset(round, seq);
    nda::size_t legal_actions = action_abstraction_.NumLegalActions(round, seq);
    Regret sum = PositiveRegretSum(round, card_bucket, offset, legal_actions);

    std::uniform_real_distribution<double> sampler(0, 1);
    double sampled = sampler(rng_());
    double bound = 0;

    nda::size_t round_actions = action_abstraction_.ActionCount(round);

    std::size_t legal_i = 0;
    for (std::size_t i = 0; i < round_actions; ++i) {
      if (action_abstraction_.Next(round, seq, i) == kIllegalId) continue;

      double action_prob = 0;
      if (sum > 0) {
        Regret action_reget = regrets_[+round](card_bucket, offset + legal_i);
        action_prob = std::max(0, action_reget);
        action_prob /= sum;
      } else {
        action_prob = 1.0 / legal_actions;
      }

      bound += action_prob;
      if (sampled < bound) {
        return {i, legal_i};
      }

      ++legal_i;
    }  // for i
    const std::string error = std::string(__func__) +
                              ": No action was selected.";
    throw std::runtime_error(error);
  }  // SampleAction()

  /* @brief action_abstraction_ getter function */
  const auto& action_abstraction() const { return action_abstraction_; }

  /* @brief regrets_ getter function */
  const auto& regrets() const { return regrets_; }

  /* @brief action_counts_ getter function */
  const auto& action_counts() const { return action_counts_; }

 private:
  /*
    @brief Initializes regret table.
  */
  std::array<LegalActionsTable<Regret>, kNRounds> InitRegretTable(
      bool verbose) {
    std::array<LegalActionsTable<Regret>, kNRounds> regret_table;
    for (RoundId r = 0; r < kNRounds; ++r) {
      regret_table[r] = InitLegalActionsTable<Regret>(Round{r});
      if (verbose) {
        std::cout << Round{r} << " regret table initialized." << std::endl;
      }
    }
    return regret_table;
  }

  /*
    @brief Initializes a card x sequence x action table for a given round.
  */
  template<typename T>
  LegalActionsTable<T> InitLegalActionsTable(Round r) {
    return LegalActionsTable<T>{{InfoAbstraction::NumClusters(r),
                                 action_abstraction_.NumLegalActions(r)}, 0};
  }

  /* @brief Barebones constructor to load a saved strategy. */
  Strategy() : action_abstraction_{std::array<AbstractAction, kActions>{},
                                   Node<kPlayers>{}} { }

  /*
    @brief Returns the sum of all positive regrets at an infoset.

    @param round The betting round of the infoset.
    @param card_bucket The card cluster id of the infoset.
    @param offset The sequence table legal offset of the infoset.
    @param legal_actions The number of legal actions at this infoset.
  */
  Regret PositiveRegretSum(Round round, CardCluster card_bucket,
                           std::size_t offset,
                           nda::size_t legal_actions) const {
    Regret sum = 0;
    for (std::size_t i = offset; i < offset + legal_actions; ++i) {
      sum += std::max(0, regrets_[+round](card_bucket, i));
    }
    return sum;
  }

  /*
    @brief Computes the strategy at the given infoset from regrets.

    @param round The betting round of the infoset.
    @param card_bucket The card cluster id of the infoset.
    @param offset The sequence table legal offset of the infoset.
    @param legal_actions The number of legal actions at this infoset.

    @return An array with the computed strategy. The ith element of the array is
        the probability of choosing the ith legal action.
  */
  std::array<double, kActions> CalculateStrategy(Round round,
      CardCluster card_bucket, std::size_t offset,
      nda::size_t legal_actions) const {
    Regret sum = PositiveRegretSum(round, card_bucket, offset, legal_actions);

    std::array<double, kActions> strategy = {0};
    for (std::size_t i = 0; i < legal_actions; ++i) {
      if (sum > 0) {
        strategy[i] = std::max(0, regrets_[+round](card_bucket, offset + i));
        strategy[i] /= sum;
      } else {
        strategy[i] = 1.0 / legal_actions;
      }
    }

    return strategy;
  }

  /*
    @brief Recursively updates the given player's average preflop strategy.

    @param state State of the game at the current recursive call.
    @param card_bucket The card cluster id of the infoset at the current
        recursive call.
    @param seq The sequence id of the infoset at the current recursive call.
    @param player The player whose strategy is being updated.
  */
  void UpdateStrategy(Node<kPlayers>& state, CardCluster card_bucket,
                      SequenceId seq, PlayerId player) {
    Round round = state.round();
    if (round > Round::kPreFlop || !state.in_progress() ||
        state.folded(player) || state.stack(player) == 0) {
      return;
    } else if (state.acting_player() == state.kChancePlayer) {
      state.Deal();
      state.ProceedPlay();
      return UpdateStrategy(state, info_abstraction_.Cluster(state, player),
                            seq, player);
    }
    nda::const_vector_ref<AbstractAction> actions =
        action_abstraction_.Actions(round);
    if (state.acting_player() == player) {
      ActionIndicies action_idxs = SampleAction(round, card_bucket, seq);
      AbstractAction action = actions(action_idxs.round_idx);
      state.Apply(action.play, state.ConvertBet(action.size));
      std::size_t offset = action_abstraction_.LegalOffset(round, seq);
      action_counts_(card_bucket, offset + action_idxs.legal_idx) += 1;
      SequenceId next_seq = action_abstraction_.Next(round, seq,
                                                     action_idxs.round_idx);
      return UpdateStrategy(state, card_bucket, next_seq, player);
    } else {
      for (nda::index_t action_index = 0; action_index < actions.width();
           ++action_index) {
        if (action_abstraction_.Next(round, seq, action_index) != kIllegalId) {
          Node<kPlayers> new_state = state;
          AbstractAction action = actions(action_index);
          new_state.Apply(action.play, new_state.ConvertBet(action.size));
          UpdateStrategy(new_state, card_bucket,
                         action_abstraction_.Next(round, seq, action_index),
                         player);
        }
      }
    }
  }  // UpdateStrategy()

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
  double TraverseMCCFR(Node<kPlayers>& state,
                       const std::array<CardCluster, kPlayers>& card_buckets,
                       SequenceId seq, PlayerId player, bool prune) {
    if (!state.in_progress()) {
      state.AwardPot(state.same_stack_no_rake_);
      return state.stack(player);
    } else if (state.folded(player)) {
      return state.stack(player);
    } else if (state.acting_player() == state.kChancePlayer) {
      state.Deal();
      state.ProceedPlay();
      return TraverseMCCFR(state, info_abstraction_.ClusterArray(state), seq,
                           player, prune);
    }

    Round round = state.round();
    PlayerId acting_player = state.acting_player();
    nda::const_vector_ref<AbstractAction> actions =
        action_abstraction_.Actions(round);
    std::size_t offset = action_abstraction_.LegalOffset(round, seq);
    nda::size_t legal_actions = action_abstraction_.NumLegalActions(round, seq);

    if (acting_player == player) {
      std::array<double, kActions> strategy = CalculateStrategy(round,
          card_buckets[player], offset, legal_actions);
      double value = 0;

      // These arrays are indexed by all round actions, not just legal ones
      std::array<double, kActions> action_values;
      std::array<bool, kActions> explored = {false};
      std::array<bool, kActions> legal = {false};

      std::size_t legal_i = 0;
      for (nda::index_t i = 0; i < actions.width(); ++i) {
        SequenceId next_seq = action_abstraction_.Next(round, seq, i);
        if (next_seq == kIllegalId) continue;
        legal[i] = true;

        Regret action_regret = regrets_[+round](card_buckets[player],
                                                offset + legal_i);
        if (!prune || action_regret > prune_constant_ ||
            round == Round::kRiver || next_seq == kLeafId) {
          AbstractAction action = actions(i);
          Node<kPlayers> new_state = state;
          new_state.Apply(action.play, new_state.ConvertBet(action.size));
          double action_value = TraverseMCCFR(new_state, card_buckets, next_seq,
                                              player, prune);
          action_values[i] = action_value;
          value += action_value * strategy[legal_i];
          explored[i] = true;
        }
        ++legal_i;
      }  // for i

      legal_i = 0;
      for (nda::index_t i = 0; i < actions.width(); ++i) {
        if (!legal[i]) continue;
        if (explored[i]) {
          Regret& infoset_regret = regrets_[+round](card_buckets[player],
                                                    offset + legal_i);
          Regret value_difference =
              static_cast<Regret>(std::rint(action_values[i] - value));
          infoset_regret = std::max(regret_floor_,
                                    infoset_regret + value_difference);
        }
        ++legal_i;
      }

      return value;

    // acting_player != player
    } else {
      nda::index_t action_index = SampleAction(round,
                                               card_buckets[acting_player],
                                               seq).round_idx;
      AbstractAction action = actions(action_index);
      state.Apply(action.play, state.ConvertBet(action.size));
      return TraverseMCCFR(state, card_buckets,
                           action_abstraction_.Next(round, seq, action_index),
                           player, prune);
    }  // else
  }  // TraverseMCCFR()
};  // class Strategy

}  // namespace fishbait

#endif  // SRC_BLUEPRINT_STRATEGY_H_
