#ifndef AI_SRC_MCCFR_STRATEGY_H_
#define AI_SRC_MCCFR_STRATEGY_H_

#include <algorithm>
#include <array>
#include <cmath>
#include <filesystem>
#include <functional>
#include <iostream>
#include <iterator>
#include <numeric>
#include <random>
#include <stdexcept>
#include <string>
#include <thread>  // NOLINT(build/c++11)
#include <utility>
#include <vector>

#include "array/array.h"
#include "clustering/definitions.h"
#include "mccfr/definitions.h"
#include "mccfr/sequence_table.h"
#include "poker/definitions.h"
#include "poker/node.h"
#include "utils/cereal.h"
#include "utils/config.h"
#include "utils/math.h"
#include "utils/random.h"
#include "utils/thread.h"

namespace fishbait {

template <PlayerN kPlayers, std::size_t kActions, typename InfoAbstraction>
class Strategy {
 private:
  // Table of values for each legal action. Size card clusters * legal actions.
  using LegalActionsTableShape = nda::shape<nda::dim<>, nda::dense_dim<>>;
  template <typename T>
  using LegalActionsTable = nda::array<T, LegalActionsTableShape>;

  // Array of LegalActionsTable, one for each round
  template <typename T>
  using GameLegalActionsTable = std::array<LegalActionsTable<T>, kNRounds>;

  InfoAbstraction info_abstraction_;
  SequenceTable<kPlayers, kActions> action_abstraction_;

  // round * card clusters * legal actions
  GameLegalActionsTable<Regret> regrets_;
  LegalActionsTable<ActionCount> action_counts_;

  Regret prune_constant_;     /* Actions with regret less than or equal to this
                                 constant are eligible to be pruned. */
  Regret regret_floor_;       /* Floor to cutoff negative regrets at. */

  inline static thread_local Random rng_;

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
  */
  Strategy(const Node<kPlayers>& start_state,
           const std::array<AbstractAction, kActions>& actions,
           InfoAbstraction info_abstraction, Regret prune_constant,
           Regret regret_floor)
           : info_abstraction_{info_abstraction},
             action_abstraction_{actions, start_state},
             regrets_{InitGameLegalActionsTable<Regret>()}, action_counts_{
                 InitLegalActionsTable<ActionCount>(Round::kPreFlop)
             }, prune_constant_{prune_constant}, regret_floor_{regret_floor} { }
  Strategy(const Strategy& other) = default;
  Strategy& operator=(const Strategy& other) = default;

  /* @brief Strategy serialize function. */
  template<class Archive>
  void serialize(Archive& archive) {
    archive(info_abstraction_, action_abstraction_, regrets_, action_counts_,
            prune_constant_, regret_floor_);
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
    for (RoundId r_id = 0; r_id < kNRounds; ++r_id) {
      fishbait::Round r = Round{r_id};
      CardCluster n_clusters = InfoAbstraction::NumClusters(r);
      nda::size_t legal_actions = action_abstraction_.NumLegalActions(r);
      auto discount_clusters = [&, factor, r_id, legal_actions]
                               (CardCluster start, CardCluster end) {
        for (CardCluster cluster = start; cluster < end; ++cluster) {
          Regret* start = &regrets_[r_id](cluster, 0);
          Regret* end = start + legal_actions;
          std::for_each(start, end, [=](Regret& regret) {
            regret = std::rint(regret * factor);
          });
        }  // for cluster
      };  // discount_clusters()
      DivideWork(n_clusters, discount_clusters);
    }  // for r_id
    CardCluster n_clusters = InfoAbstraction::NumClusters(Round::kPreFlop);
    nda::size_t legal_actions =
        action_abstraction_.NumLegalActions(Round::kPreFlop);
    auto discount_act_count = [&, factor, legal_actions]
                              (CardCluster start, CardCluster end) {
      for (CardCluster cluster = start; cluster < end; ++cluster) {
        ActionCount* start = &action_counts_(cluster, 0);
        ActionCount* end = start + legal_actions;
        std::for_each(start, end, [=](ActionCount& count) {
          count = std::rint(count * factor);
        });
      }  // for cluster
    };  // discount_act_count()
    DivideWork(n_clusters, discount_act_count);
  }  // Discount()

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

    std::uniform_real_distribution<double> sampler(0, 1);
    nda::size_t round_actions = action_abstraction_.ActionCount(round);

    while (true) {
      Regret sum = PositiveRegretSum(round, card_bucket, offset, legal_actions);
      double sampled = sampler(rng_());
      double bound = 0;

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
    }
  }  // SampleAction()

  /* @brief action_abstraction_ getter function */
  const auto& action_abstraction() const { return action_abstraction_; }

  /* @brief regrets_ getter function */
  const auto& regrets() const { return regrets_; }

  /* @brief action_counts_ getter function */
  const auto& action_counts() const { return action_counts_; }

  /* 
    @brief Sets the seed of the random number generator used to sample actions.
  */
  static void SetSeed(Random::Seed seed) {
    rng_.seed(seed);
  }

 private:
  /*
    @brief Initializes an array LegalActionTables, one for each round.
  */
  template<typename DataT>
  GameLegalActionsTable<DataT> InitGameLegalActionsTable() {
    GameLegalActionsTable<DataT> regret_table;
    for (RoundId r = 0; r < kNRounds; ++r) {
      regret_table[r] = InitLegalActionsTable<DataT>(Round{r});
    }
    return regret_table;
  }

  /*
    @brief Initializes a card x sequence x action table for a given round.

    All entries are set to 0.
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
  template <typename FloatT = double>
  std::array<FloatT, kActions> CalculateStrategy(Round round,
      CardCluster card_bucket, std::size_t offset,
      nda::size_t legal_actions) const {
    Regret sum = PositiveRegretSum(round, card_bucket, offset, legal_actions);

    std::array<FloatT, kActions> strategy = {0};
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
      state.Apply(action.play, state.ProportionToChips(action.size));
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
          new_state.Apply(action.play,
                          new_state.ProportionToChips(action.size));
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
          new_state.Apply(action.play,
                          new_state.ProportionToChips(action.size));
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
      state.Apply(action.play, state.ProportionToChips(action.size));
      return TraverseMCCFR(state, card_buckets,
                           action_abstraction_.Next(round, seq, action_index),
                           player, prune);
    }  // else
  }  // TraverseMCCFR()

 public:
  /* Stores the average of multiple strategies. */
  class Average {
   private:
    GameLegalActionsTable<float> probabilities_;
    int n_;  // How many strategies are in this average.
    SequenceTable<kPlayers, kActions> action_abstraction_;
    InfoAbstraction info_abstraction_;
    inline static thread_local Random rng_;

    explicit Average(Strategy& ref)
        : probabilities_{ref.InitGameLegalActionsTable<float>()}, n_{0},
          action_abstraction_{ref.action_abstraction_},
          info_abstraction_{ref.info_abstraction_} {
      *this += ref;
    }

    /* @brief Barebones constructor to load a saved average. */
    Average() : action_abstraction_{std::array<AbstractAction, kActions>{},
                                    Node<kPlayers>{}},
                info_abstraction_{InfoAbstraction::BlankTable()} {}

   public:
    friend class Strategy;

    Average(const Average& other)
        : action_abstraction_{std::array<AbstractAction, kActions>{},
                              Node<kPlayers>{}},
          info_abstraction_{InfoAbstraction::BlankTable()} {
      *this = other;
    };
    Average(Average&& other)
        : action_abstraction_{std::array<AbstractAction, kActions>{},
                              Node<kPlayers>{}},
          info_abstraction_{InfoAbstraction::BlankTable()} {
      *this = std::move(other);
    };
    Average& operator=(const Average& other) {
      n_ = other.n_;
      action_abstraction_ = other.action_abstraction_;
      info_abstraction_ = other.info_abstraction_;
      for (RoundId r_id = 0; r_id < kNRounds; ++r_id) {
        probabilities_[r_id].reshape(other.probabilities_[r_id].shape());
        auto compute_clusters = [&, r_id](std::size_t start, std::size_t end) {
          const float* start_i = other.probabilities_[r_id].data() + start;
          const float* end_i = other.probabilities_[r_id].data() + end;
          float* start_o = probabilities_[r_id].data() + start;
          std::copy(start_i, end_i, start_o);
        };  // compute_clusters()
        DivideWork(probabilities_[r_id].size(), compute_clusters);
      }  // for round
      return *this;
    }
    Average& operator=(Average&& other) {
      n_ = other.n_;
      action_abstraction_ = std::move(other.action_abstraction_);
      info_abstraction_ = std::move(other.info_abstraction_);
      probabilities_ = std::move(other.probabilities_);
      return *this;
    }

    bool operator!=(const Average& other) const {
      return probabilities_ != other.probabilities_ || n_ != other.n_ ||
             action_abstraction_ != other.action_abstraction_ ||
             info_abstraction_ != other.info_abstraction_;
    }
    bool operator==(const Average& other) const { return !(*this != other); }

    /* @brief Average serialize function. */
    template<class Archive>
    void serialize(Archive& archive) {
      archive(probabilities_, n_, action_abstraction_, info_abstraction_);
    }

    /* @brief Loads an Average snapshot from the given path on disk. */
    static Average LoadAverage(const std::filesystem::path path,
                               bool verbose = false) {
      Average loaded;
      CerealLoad(path.string(), &loaded, verbose);
      return loaded;
    }

    /* @brief Adds the given strategy to this average */
    Average& operator+=(const Strategy& rhs) {
      /* For preflop, normalize action counts and overwrite since action counts
         already does averaging */
      {
        RoundId r_id = 0;
        fishbait::Round r = Round{r_id};
        SequenceN round_seqs = rhs.action_abstraction_.States(r);
        CardCluster n_clusters = InfoAbstraction::NumClusters(r);
        auto compute_clusters = [&, r_id, r, round_seqs]
                                (CardCluster start, CardCluster end) {
          for (CardCluster cluster = start; cluster < end; ++cluster) {
            std::size_t offset = 0;
            for (SequenceId seq = 0; seq < round_seqs; ++seq) {
              nda::size_t legal_actions =
                  rhs.action_abstraction_.NumLegalActions(r, seq);
              const ActionCount* i_begin = &rhs.action_counts_(cluster, offset);
              const ActionCount* i_end = i_begin + legal_actions;
              float* o_begin = &probabilities_[r_id](cluster, offset);
              float* o_end = o_begin + legal_actions;

              ActionCount sum = std::accumulate(i_begin, i_end, 0);
              if (sum > 0) {
                std::transform(i_begin, i_end, o_begin,
                    [=](const ActionCount& a) { return a * 1.0 / sum; });
              } else {
                std::fill(o_begin, o_end, 1.0 / legal_actions);
              }

              offset += legal_actions;
            }
          }
        };  // compute_clusters()
        DivideWork(n_clusters, compute_clusters);
      }  // preflop block

      for (RoundId r_id = 1; r_id < kNRounds; ++r_id) {
        fishbait::Round r = Round{r_id};
        SequenceN round_seqs = rhs.action_abstraction_.States(r);
        CardCluster n_clusters = InfoAbstraction::NumClusters(r);
        auto compute_clusters = [&, r_id, r, round_seqs]
                                (CardCluster start, CardCluster end) {
          for (CardCluster cluster = start; cluster < end; ++cluster) {
            std::size_t offset = 0;
            for (SequenceId seq = 0; seq < round_seqs; ++seq) {
              nda::size_t legal_actions =
                  rhs.action_abstraction_.NumLegalActions(r, seq);
              std::array<float, kActions> strategy =
                  rhs.template CalculateStrategy<float>(r, cluster, offset,
                                                        legal_actions);
              std::transform(strategy.begin(),
                             std::next(strategy.begin(), legal_actions),
                             &probabilities_[r_id](cluster, offset),
                             &probabilities_[r_id](cluster, offset),
                             std::plus<float>{});
              offset += legal_actions;
            }  // for seq
          }  // for cluster
        };  // compute_clusters()
        DivideWork(n_clusters, compute_clusters);
      }  // for round
      n_ += 1;
      return *this;
    }

    /* @brief Normalize the probabilities by dividing all of them by n_. */
    void Normalize() {
      // Preflop already normalized because of action counts
      if (n_ == 1) return;
      for (RoundId r_id = 1; r_id < kNRounds; ++r_id) {
        probabilities_[r_id].for_each_value([=](float& ref) {
          ref /= n_;
        });
      }
      n_ = 1;
    }

    /*
      @brief Samples an action from this average strategy at the given infoset.

      Assumes this average is normal (n_ = 1).

      @param round The betting round of the infoset.
      @param card_bucket The card cluster id of the infoset.
      @param seq The sequence id of the infoset.

      @return The indicies of the sampled action.
    */
    ActionIndicies SampleAction(Round round, CardCluster card_bucket,
                                SequenceId seq) {
      std::size_t offset = action_abstraction_.LegalOffset(round, seq);

      std::uniform_real_distribution<float> sampler(0, 1);
      nda::size_t round_actions = action_abstraction_.ActionCount(round);

      while (true) {
        float sampled = sampler(rng_());
        float bound = 0;

        std::size_t legal_i = 0;
        for (std::size_t i = 0; i < round_actions; ++i) {
          if (action_abstraction_.Next(round, seq, i) == kIllegalId) continue;

          if (round == Round::kPreFlop) {
            bound += probabilities_[+round](card_bucket, offset + legal_i);
          } else {
            bound += probabilities_[+round](card_bucket, offset + legal_i) / n_;
          }
          if (sampled < bound) {
            return {i, legal_i};
          }

          ++legal_i;
        }  // for i
      }
    }  // SampleAction()

    /*
      @brief Returns the strategy at the given infoset.

      @param round The betting round of the infoset.
      @param card_bucket The card cluster id of the infoset.
      @param seq The sequence id of the infoset.
    */
    std::array<float, kActions> Policy(Round round, CardCluster card_bucket,
                                       SequenceId seq) const {
      std::array<float, kActions> policy;
      std::size_t offset = action_abstraction_.LegalOffset(round, seq);
      nda::size_t round_actions = action_abstraction_.ActionCount(round);
      std::size_t legal_i = 0;
      for (std::size_t i = 0; i < round_actions; ++i) {
        if (action_abstraction_.Next(round, seq, i) == kIllegalId) {
          policy[i] = 0;
          continue;
        }

        if (round == Round::kPreFlop) {
          policy[i] = probabilities_[+round](card_bucket, offset + legal_i);
        } else {
          policy[i] = probabilities_[+round](card_bucket, offset + legal_i);
          policy[i] /= n_;
        }

        ++legal_i;
      }  // for i
      return policy;
    }  // Policy()

    /*
      @brief Test this average strategy vs the op average strategy.

      Both strategies must have the same action abstraction. Assumes all players
      start with the same amount of chips as the player on the button and there
      is no rake.

      @param op The opponent strategy to test against.
      @param means The number of times to run the trials.
      @param trials The number of trials to run per position.

      @return A vector of means from each trial.
    */
    std::vector<double> BattleStats(Average& op, int means = 100,
                                    int trials = 1000000) {
      std::vector<double> mean_ls(means);
      auto run_trials = [&](int start, int end) {
        for (int i = start; i < end; ++i) {
          std::vector<int> results = Battle(op, trials);
          mean_ls[i] = Mean(results);
        }
      };
      DivideWork(means, run_trials);
      return mean_ls;
    }  // BattleStats()

    const auto& probabilities() const { return probabilities_; }
    const auto& action_abstraction() const { return action_abstraction_; }
    const auto& info_abstraction() const { return info_abstraction_; }

   private:
    /*
      @brief Test this average strategy vs the op average strategy.

      Both strategies must have the same action abstraction. Assumes all players
      start with the same amount of chips as the player on the button and there
      is no rake.

      @param op The opponent strategy to test against.
      @param trials The number of trials to run per position.

      @return A vector of (kPlayers * trials) chip gains and losses for this
          average strategy.
    */
    std::vector<int> Battle(Average& op, int trials = 1000000) {
      if (action_abstraction_ != op.action_abstraction_) {
        throw std::invalid_argument("op average strategy does not have the "
                                    "same action abstraction.");
      }

      std::vector<int> results(kPlayers * trials);
      Chips default_stack = action_abstraction_.start_state().stack(0);
      for (PlayerId player = 0; player < kPlayers; ++player) {
        for (int i = 0; i < trials; ++i) {
          Node<kPlayers> state = action_abstraction_.start_state();
          SequenceId seq = 0;
          std::array<CardCluster, kPlayers> card_buckets = {0};
          while (state.in_progress()) {
            Round round = state.round();
            if (state.acting_player() == state.kChancePlayer) {
              state.Deal();
              state.ProceedPlay();
              card_buckets = op.info_abstraction_.ClusterArray(state);
              card_buckets[player] = info_abstraction_.Cluster(state, player);
            } else if (state.folded(player)) {
              break;
            } else if (state.acting_player() == player) {
              std::size_t act_idx =
                  SampleAction(round, card_buckets[player], seq).round_idx;
              seq = action_abstraction_.Next(round, seq, act_idx);
              AbstractAction action =
                  action_abstraction_.Actions(round)[act_idx];
              state.Apply(action.play, state.ProportionToChips(action.size));
            } else {
              std::size_t act_idx = op.SampleAction(round,
                  card_buckets[state.acting_player()], seq).round_idx;
              seq = op.action_abstraction_.Next(round, seq, act_idx);
              AbstractAction action =
                  op.action_abstraction_.Actions(round)[act_idx];
              state.Apply(action.play, state.ProportionToChips(action.size));
            }
          }  // while state.in_progress()
          if (!state.in_progress()) state.AwardPot(state.same_stack_no_rake_);
          results[player * trials + i] = state.stack(player) - default_stack;
        }  // for i
      }  // for player

      return results;
    }  // Battle()
  };  // class Average

  /* @brief Return an avg strategy where this strategy is the only datapoint. */
  Average InitialAverage() {
    return Average{*this};
  }
};  // class Strategy

}  // namespace fishbait

#endif  // AI_SRC_MCCFR_STRATEGY_H_
