// Copyright 2021 Marzuk Rashid and Emily Dale

#ifndef SRC_BLUEPRINT_STRATEGY_H_
#define SRC_BLUEPRINT_STRATEGY_H_

#include <algorithm>
#include <array>
#include <cmath>
#include <filesystem>
#include <iomanip>
#include <iostream>
#include <random>
#include <sstream>
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
  using InfosetActionTableShape = nda::shape<nda::dim<>, nda::dim<>,
                                             nda::dense_dim<>>;
  template <typename T>
  using InfosetActionTable = nda::array<T, InfosetActionTableShape>;

  Regret regret_floor_;
  Regret prune_constant_;

  InfoAbstraction info_abstraction_;
  SequenceTable<kPlayers, kActions> action_abstraction_;

  // Indexed Card Buckets x Sequences x Actions
  std::array<InfosetActionTable<Regret>, kNRounds> regrets_;
  InfosetActionTable<ActionCount> action_counts_;

  Random rng_;
  std::filesystem::path save_path_;

 public:
  /*
    @brief Constructor

    @param start_state The starting state of the game, must be compatible with 
        the Node::SameStackNoRake optimizations.
    @param actions The actions available in the abstracted game tree.
    @param info_abstraction An object that will abstract the card information.
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
    @param save_dir The name of the subdirectory relative to kBlueprintSaveDir
        to save the final average strategy and snapshots to. The subdirectory
        will be created if it does not exist.
    @param seed The seed to use for the random number generator used to sample
        actions and stochastically prune.
    @param verbose Whether to print debug information.
  */
  Strategy(const Node<kPlayers>& start_state,
           const std::array<AbstractAction, kActions>& actions,
           InfoAbstraction info_abstraction, int iterations,
           int strategy_interval, int prune_threshold, double prune_probability,
           Regret prune_constant, int LCFR_threshold, int discount_interval,
           Regret regret_floor, int snapshot_interval, int strategy_delay,
           std::string_view save_dir, Random::Seed seed = Random::Seed{},
           bool verbose = false)
           : regret_floor_{regret_floor}, prune_constant_{prune_constant},
             info_abstraction_{info_abstraction},
             action_abstraction_{actions, start_state},
             regrets_{InitRegretTable()}, action_counts_{
                 InitInfosetActionTable<ActionCount>(Round::kPreFlop)
             }, rng_{seed}, save_path_{CreateSaveDir(save_dir)} {
    MCCFR(iterations, strategy_interval, prune_threshold, prune_probability,
          LCFR_threshold, discount_interval, snapshot_interval, strategy_delay,
          verbose);
  }
  Strategy(const Strategy& other) = default;
  Strategy& operator=(const Strategy& other) = default;

  /* @brief Strategy serialize function. */
  template<class Archive>
  void serialize(Archive& archive) const {
    archive(regret_floor_, prune_constant_, info_abstraction_,
            action_abstraction_, regrets_, action_counts_, save_path_.string());
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
    return InfosetActionTable<T>{{InfoAbstraction::NumClusters(r),
                                  action_abstraction_.States(r),
                                  action_abstraction_.ActionCount(r)}, 0};
  }

  /*
    @brief Creates a directory called save_dir in kBlueprintSaveDir.
    
    Used to save the final average strategy and snapshots. The directory is only
    created if it does not already exist.

    @return The path to the created directory.
  */
  std::filesystem::path CreateSaveDir(std::string_view save_dir) {
    std::filesystem::path base_path(kBlueprintSaveDir);
    std::filesystem::path save_path = base_path / save_dir;
    std::filesystem::create_directory(save_path);
    return save_path;
  }

  /* @brief Barebones constructor to load a saved strategy. */
  Strategy() : regret_floor_{}, prune_constant_{}, info_abstraction_{},
               action_abstraction_{Node<kPlayers>{},
                                   std::array<AbstractAction, kActions>{}},
               regrets_{}, action_counts_{}, rng_{}, save_path_{} { }

  /*
    @brief Saves a snapshot of the strategy to the save_path_.

    @param iteration The current mccfr iteration number.
    @param total_iterations The total number of mccfr iterations that will be
        performed.
    @param verbose Whether to print debug information.
  */
  void TakeSnapshot(int iteration, int total_iterations, bool verbose) {
    int total_digits = std::floor(std::log10(total_iterations)) + 1;
    std::stringstream iter_ss;
    iter_ss.fill('0');
    iter_ss << std::setw(total_digits) << iteration;

    std::stringstream strategy_ss;
    strategy_ss << "strategy_" << iter_ss.str() << ".cereal";
    std::filesystem::path strategy_path = save_path_ / strategy_ss.str();
    CerealSave(strategy_path.string(), this, verbose);
  }

  /*
    @brief Computes the strategy using the MCCFR algorithm.

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
  void MCCFR(int iterations, int strategy_interval, int prune_threshold,
             double prune_probability, int LCFR_threshold,
             int discount_interval, int snapshot_interval, int strategy_delay,
             bool verbose) {
    if (verbose) std::cout << "Starting MCCFR" << std::endl;
    for (int t = 1; t <= iterations; ++t) {
      if (verbose) std::cout << "iteration: " << t << std::endl;
      for (PlayerId player = 0; player < kPlayers; ++player) {
        // update strategy after strategy_inveral iterations
        if (t > strategy_delay && t % strategy_interval == 0) {
          UpdateStrategy(player);
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
        TraverseMCCFR(player, prune);
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
      if ((t > strategy_delay && t % snapshot_interval == 0) ||
          t == iterations) {
        TakeSnapshot(t, iterations, verbose);
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
    @brief Updates the given player's average preflop strategy.

    @param player The player whose strategy is being updated.
  */
  void UpdateStrategy(PlayerId player) {
    Node<kPlayers> start_state_copy = action_abstraction_.start_state();
    UpdateStrategy(start_state_copy, 0, 0, player);
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
      nda::index_t action_index = SampleAction(card_bucket, seq, round);
      AbstractAction action = actions(action_index);
      state.Apply(action.play, state.ConvertBet(action.size));
      action_counts_(card_bucket, seq, action_index) += 1;
      return UpdateStrategy(state, card_bucket,
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
  }  // UpdateStrategy()

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
        Regret action_regret = regrets_[+round](card_buckets[player], seq,
                                                action_index);
        if ((next_seq != kIllegalId) &&
            (!prune || action_regret > prune_constant_)) {
          AbstractAction action = actions(action_index);
          Node<kPlayers> new_state = state;
          new_state.Apply(action.play, new_state.ConvertBet(action.size));
          double action_value = TraverseMCCFR(new_state, card_buckets, next_seq,
                                              player, prune);
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
          Regret infoset_regret = regrets_[+round](card_buckets[player], seq,
                                                   action_index);
          Regret value_difference = static_cast<Regret>(std::rint(
              action_values[action_index] - value));
          regrets_[+round](card_buckets[player], seq, action_index) = std::max(
              regret_floor_, infoset_regret + value_difference);
        }
      }
      return value;
    } else {
      nda::index_t action_index = SampleAction(card_buckets[acting_player], seq,
                                               round);
      AbstractAction action = actions(action_index);
      state.Apply(action.play, state.ConvertBet(action.size));
      return TraverseMCCFR(state, card_buckets,
                           action_abstraction_.Next(seq, round, action_index),
                           player, prune);
    }
  }  // TraverseMCCFR()
};  // class Strategy

}  // namespace fishbait

#endif  // SRC_BLUEPRINT_STRATEGY_H_
