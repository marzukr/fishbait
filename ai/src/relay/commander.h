#ifndef AI_SRC_RELAY_COMMANDER_H_
#define AI_SRC_RELAY_COMMANDER_H_

#include <algorithm>
#include <functional>
#include <optional>
#include <random>
#include <stdexcept>
#include <utility>
#include <tuple>
#include <memory>

#include "array/array.h"
#include "clustering/cluster_table.h"
#include "clustering/definitions.h"
#include "clustering/matchmaker.h"
#include "mccfr/definitions.h"
#include "mccfr/sequence_table.h"
#include "mccfr/strategy.h"
#include "poker/definitions.h"
#include "poker/node.h"
#include "relay/scribe.h"
#include "utils/math.h"
#include "utils/random.h"

namespace fishbait {

template <PlayerId kPlayers, std::size_t kActions, typename InfoAbstraction>
class Commander {
 public:
  using ScribeT = Scribe<kPlayers, kActions, InfoAbstraction>;

 private:
  // The state of the abstracted game used to determine the strategy
  Node<kPlayers> abstract_state_;

  // The state of the actual game. Also holds the truth about card information.
  Node<kPlayers> actual_state_;

  // The abstracted strategy to use
  ScribeT strategy_;

  // Class to index our cards for us
  Matchmaker info_abstraction_;

  // The sequence id of the current state of the abstract game
  SequenceId abstract_seq_;

  // FISHBAIT's player id
  PlayerId fishbait_seat_;

  // If there has been an action in this round yet
  bool first_round_action_;

  // Random number generator for sampling
  Random rng_;

 public:
  explicit Commander(Scribe<kPlayers, kActions, InfoAbstraction>&& strategy)
      : abstract_state_{strategy.StartState()}, actual_state_{abstract_state_},
        strategy_{strategy}, info_abstraction_{}, abstract_seq_{0},
        fishbait_seat_{0}, first_round_action_{true}, rng_{} {}

  /* @brief Resets the game with the given actual state and fishbait seat. */
  void Reset(Node<kPlayers> actual_start, PlayerId fishbait_seat) {
    actual_state_ = actual_start;
    abstract_state_ = strategy_.StartState();
    abstract_state_.Erase();
    abstract_state_.NewHand(actual_state_.button());
    abstract_seq_ = 0;
    fishbait_seat_ = fishbait_seat;
    first_round_action_ = true;
  }

  /* @brief Sets the given player's hand. */
  void SetHand(PlayerId player, const Hand<ISO_Card>& hand) {
    actual_state_.SetHand(player, hand);
  }

  /* @brief Sets the game's public card board. */
  void SetBoard(const BoardArray<ISO_Card>& board) {
    actual_state_.SetBoard(board);
  }

  /* @brief Deals the appropriate cards according to the round. */
  void Deal() {
    actual_state_.Deal();
  }

  /* @brief Proceeds play to the next round. */
  void ProceedPlay() {
    actual_state_.ProceedPlay();
    if (ShouldUpdateAbstract()) abstract_state_.ProceedPlay();
    first_round_action_ = true;
    AutoFoldCheckCall();
  }

  /* @brief Awards the pot at the end of the hand. */
  void AwardPot() {
    actual_state_.AwardPot(actual_state_.single_run_);
  }

  /* @brief Resets the states and starts a new game. */
  void NewHand() {
    abstract_state_.Erase();
    actual_state_.NewHand();
    abstract_state_.NewHand(actual_state_.button());
    abstract_seq_ = 0;
    first_round_action_ = true;
  }

  /*
    @brief Inform Fishbait of the current acting player's move.

    @param play The action the current acting player has played.
    @param size The number of chips the current player has bet, if applicable.
  */
  void Apply(Action play, Chips size = 0) {
    PlayerId player = actual_state_.acting_player();
    if (player == fishbait_seat_) {
      throw std::logic_error("Apply called when it is fishbait's turn to act.");
    }

    if (ShouldUpdateAbstract()) {
      if (play == Action::kFold) {
        if (abstract_state_.CanFold()) {
          ApplyAbstract(Action::kFold);
        } else {
          ApplyAbstract(Action::kCheckCall);
        }
      } else if (play == Action::kCheckCall) {
        if (abstract_state_.CanCheckCall()) {
          ApplyAbstract(Action::kCheckCall);
        } else {
          ApplyAbstract(Action::kAllIn);
        }
      } else if (play == Action::kAllIn) {
        Chips additional_bet = actual_state_.stack(player);
        Chips call_amount = actual_state_.NeededToCall();
        double bet_prop = actual_state_.ChipsToProportion(additional_bet);

        /* if the all in is a call */
        if (additional_bet <= call_amount) {
          Chips abstract_call = abstract_state_.NeededToCall();
          double call_proportion = (
            abstract_state_.ChipsToProportion(abstract_call)
          );

          /* if we can check, map the all in to a check. Otherwise, either map
             it to a fold or call with ps-har */
          if (!abstract_state_.CanFold()) {
            ApplyAbstract(Action::kCheckCall);
          } else if (MapToA(0, call_proportion, bet_prop)) {
            ApplyAbstract(Action::kFold);
          } else {
            /* if mapped to call and the previous bet in the abstract game was
               not all in, then call */
            if (abstract_state_.CanCheckCall()) {
              ApplyAbstract(Action::kCheckCall);

            /* if mapped to call and the previous bet in the abstract game was
               an all in, then call the all in */
            } else {
              ApplyAbstract(Action::kAllIn);
            }
          }

        /* if the all in is a bet, raise, or call + extra */
        } else {
          /* if there was not a previous all in bet in the abstract game, then
             map to the nearest sized bet with ps-har */
          if (abstract_state_.CanCheckCall()) {
            ApplyAbstract(Action::kBet, bet_prop);

          /* if the previous bet in the abstract game was an all in, then also
             map this bet to all in */
          } else {
            ApplyAbstract(Action::kAllIn);
          }
        }

      /* play == Action::kBet */
      } else {
        double bet_prop = actual_state_.ChipsToProportion(size);
        ApplyAbstract(Action::kBet, bet_prop);
      }
    }  // if ShouldUpdateAbstract()

    ApplyActual(play, size);
    first_round_action_ = false;
    AutoFoldCheckCall();
  }  // Apply()

  /*
    @brief Get the policy of Fishbait if it were choosing an action for the
        player acting in the current abstract state. All illegal actions in the
        abstract or real game are set to 0, and the normalized result is
        returned.
  */
  std::array<float, kActions> GetNormalizedLegalPolicy() {
    Round r = actual_state_.round();
    PlayerId acting_player = actual_state_.acting_player();
    auto access_fn = std::bind(&ScribeT::GetCluster, &strategy_,
                               std::placeholders::_1, std::placeholders::_2);
    CardCluster cc = info_abstraction_.Cluster(actual_state_, acting_player,
                                               access_fn);
    std::array policy = strategy_.Policy(r, cc, abstract_seq_);
    std::array actions = strategy_.Actions(r);
    hsize_t n_actions = strategy_.ActionCount(r);

    /* It is important to go through all items in policies, not just until
     * n_actions because otherwise some may be NaN which makes Normalize turn
     * everything into Nan */
    for (std::size_t i = 0; i < policy.size(); ++i) {
      /* Filter out all actions that are illegal in the actual game */
      if (!actual_state_.IsLegal(actions[i]) || i >= n_actions) {
        policy[i] = 0;
      }
    }
    Normalize(policy);
    return policy;
  }

  struct AvailableAction {
    Action play;
    double size;
    float policy;
    std::size_t action_idx = kIllegalId;

    bool operator==(const AvailableAction& other) const {
      return (
        std::tie(play, size, policy, action_idx) ==
        std::tie(other.play, other.size, other.policy, other.action_idx)
      );
    }
  };

  /*
    @brief Get the actions available to Fishbait and its policy if it were
        choosing an action for the player acting in the current abstract state.
  */
  std::array<AvailableAction, kActions> GetAvailableActions() {
    Round r = actual_state_.round();
    std::array actions = strategy_.Actions(r);
    hsize_t n_actions = strategy_.ActionCount(r);
    std::array<AvailableAction, kActions> ret_arr;
    std::array policy = GetNormalizedLegalPolicy();
    for (std::size_t i = 0; i < n_actions; ++i) {
      ret_arr[i] = {actions[i].play, actions[i].size, policy[i], i};
    }
    return ret_arr;
  }

  /*
    @brief Ask fishbait to make a move.

    @return A tuple detailing the action, chips, and action_idx
  */
  std::tuple<Action, Chips, std::size_t> Query() {
    if (actual_state_.acting_player() != fishbait_seat_) {
      throw std::logic_error("Query called when it's not fishbait's turn.");
    }

    // Sample the action
    Round r = actual_state_.round();
    std::array actions = strategy_.Actions(r);
    std::array policy = GetNormalizedLegalPolicy();
    std::size_t action_idx = Sample(policy, rng_);
    AbstractAction action = actions[action_idx];

    // Apply the action to the abstract game
    abstract_state_.Apply(action);
    abstract_seq_ = strategy_.Next(r, abstract_seq_, action_idx);
    first_round_action_ = false;

    // Apply the action to the actual game
    Chips actual_chips = actual_state_.ProportionToChips(action.size);
    actual_state_.Apply(action.play, actual_chips);

    AutoFoldCheckCall();
    return std::make_tuple(action.play, actual_chips, action_idx);
  }

  /* @brief Returns the current state of the game. */
  const Node<kPlayers>& State() const { return actual_state_; }

  /* @brief Returns fishbait's player id. */
  PlayerId fishbait_seat() const { return fishbait_seat_; }

  /* @brief Commander serialize function */
  template<class Archive>
  void save(Archive& archive) const {
    std::unique_ptr<ScribeT> archive_strategy = (
      std::make_unique<ScribeT>(strategy_)
    );
    archive(archive_strategy);
    archive(
      abstract_state_, actual_state_, abstract_seq_, fishbait_seat_,
      first_round_action_
    );
  }

  /* @brief Commander deserialize function */
  template <class Archive>
  static void load_and_construct(
    Archive& archive,
    cereal::construct<Commander<kPlayers, kActions, InfoAbstraction>>& construct
  ) {
    std::unique_ptr<ScribeT> archive_strategy;
    archive(archive_strategy);
    construct(std::move(*archive_strategy));
    archive(
      construct->abstract_state_, construct->actual_state_,
      construct->abstract_seq_, construct->fishbait_seat_,
      construct->first_round_action_
    );
  }

 private:
  /*
    @brief If we should update the abstract state.

    We should only update the abstract state if fishbait has further moves to
    make (if fishbait is not folded and not all in).
  */
  bool ShouldUpdateAbstract() {
    return actual_state_.in_progress() &&
           actual_state_.stack(fishbait_seat_) != 0 &&
           !actual_state_.folded(fishbait_seat_) &&
           actual_state_.players_left() - actual_state_.players_all_in() > 1;
  }

  /* @brief Applies the given Action to the actual state. */
  void ApplyActual(Action play, Chips size = 0) {
    actual_state_.Apply(play, size);
  }

  /* @brief Applies the given Action to the abstract state. */
  void ApplyAbstract(Action play,
                     std::optional<double> size = std::optional<double>{}) {
    Round round = abstract_state_.round();
    std::array actions = strategy_.Actions(round);
    hsize_t num_actions = strategy_.ActionCount(round);
    nda::index_t action_idx = -1;

    /* If the request action is a bet and there is a requested size given, then
       we need to find a close bet size in the abstraction to map it to. */
    if (play == Action::kBet && size) {
      /* Find the largest bet size smaller than the requested size and the
         smallest bet size larger than the requested size */
      int min_i = -1;
      double min_size = 0;
      int max_i = -1;
      double max_size = 0;
      for (std::size_t i = 0; i < num_actions; ++i) {
        if (strategy_.Next(round, abstract_seq_, i) == kIllegalId) {
          continue;
        }
        AbstractAction a = actions[i];
        double a_size = a.size;
        if (a.play == Action::kFold) {
          continue;
        } else if (a.play == Action::kCheckCall) {
          /* Allow mapping a bet to a check if it's the first action in the
             round */
          if (first_round_action_) {
            a_size = 0.0;
          } else {
            continue;
          }
        } else if (a.play == Action::kAllIn) {
          /* Find what pot porportion an all in bet would be */
          PlayerId player = abstract_state_.acting_player();
          Chips shove_size = abstract_state_.stack(player);
          a_size = abstract_state_.ChipsToProportion(shove_size);
        }
        if (a_size <= *size && (min_i == -1 || a_size > min_size)) {
          min_i = i;
          min_size = a_size;
        }
        if (a_size >= *size && (max_i == -1 || a_size < max_size)) {
          max_i = i;
          max_size = a_size;
        }
      }  // for i in actions

      /* In this case, the requested size is in the abstraction, so we play it
         directly. */
      if (min_i == max_i) {
        play = actions[min_i].play;
        size = min_size;
        action_idx = min_i;

      /* In this case, the requested size is smaller than any size in the
         abstraction, so we play the smallest size in the abstraction. */
      } else if (min_i == -1) {
        play = actions[max_i].play;
        size = max_size;
        action_idx = max_i;

      /* In this case, the requested size is larger than any size in the
         abstraction, so we play the largest size in the abstraction. */
      } else if (max_i == -1) {
        play = actions[min_i].play;
        size = min_size;
        action_idx = min_i;

      /* In this case, the requested size is in between two sizes in the
         abstraction, so we use the randomized pseudo harmonic mapping to pick
         one of them to play. */
      } else {
        if (MapToA(min_size, max_size, *size)) {
          play = actions[min_i].play;
          size = min_size;
          action_idx = min_i;
        } else {
          play = actions[max_i].play;
          size = max_size;
          action_idx = max_i;
        }
      }
    }  // if kBet and size

    /* Apply the selected play and size to the abstract game, and move to the
       corresponding next abstract sequence id. */
    Chips size_chips = abstract_state_.ProportionToChips(*size);
    abstract_state_.Apply(play, size_chips);
    for (std::size_t i = 0; i < num_actions && action_idx == -1; ++i) {
      const AbstractAction& a = actions[i];
      if (a.play == play && (a.play != Action::kBet || a.size == *size)) {
        action_idx = i;
        break;
      }
    }
    abstract_seq_ = strategy_.Next(round, abstract_seq_, action_idx);
  }  // ApplyAbstract()

  /*
    @brief Auto folds/calls/checks players who are out in the real game.

    Sometimes folds and all-ins are mapped to check/calls in the abstract game.
    For this reason, these players need to be auto folded or check/called on
    their next turn.
  */
  void AutoFoldCheckCall() {
    if (ShouldUpdateAbstract()) {
      while (abstract_state_.acting_player() != abstract_state_.kChancePlayer &&
             (actual_state_.stack(abstract_state_.acting_player()) == 0 ||
              actual_state_.folded(abstract_state_.acting_player()))) {
        if (abstract_state_.CanFold()) {
          ApplyAbstract(Action::kFold);
        } else {
          ApplyAbstract(Action::kCheckCall);
        }
      }
    }
  }  // AutoFoldCheckCall()

  /*
    @brief Randomly maps x to either a or b with the pseudo-harmonic mapping.

    @param a the lower abstraction bound as a proportion of the pot
    @param b the upper abstraction bound as a proportion of the pot
    @param x the actual bet size as a proportion of the pot
    @return True if map to a, false if map to b.
  */
  bool MapToA(double a, double b, double x) {
    double f = ((b - x) * (1.0 + a)) / ((b - a) * (1.0 + x));
    std::uniform_real_distribution<double> sampler(0, 1);
    double sampled = sampler(rng_());
    return sampled < f;
  }
};  // class Commander

}  // namespace fishbait

#endif  // AI_SRC_RELAY_COMMANDER_H_
