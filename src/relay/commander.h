// Copyright 2021 Marzuk Rashid

#ifndef SRC_RELAY_COMMANDER_H_
#define SRC_RELAY_COMMANDER_H_

#include <algorithm>
#include <functional>
#include <optional>
#include <random>
#include <stdexcept>
#include <utility>

#include "array/array.h"
#include "blueprint/definitions.h"
#include "blueprint/sequence_table.h"
#include "blueprint/strategy.h"
#include "clustering/cluster_table.h"
#include "clustering/definitions.h"
#include "clustering/matchmaker.h"
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
          Chips abstract_size = abstract_state_.ProportionToChips(bet_prop);
          Chips abstract_call = abstract_state_.NeededToCall();

          /* if we can check, map the all in to a check. Otherwise, either map
             it to a fold or call with ps-har */
          if (!abstract_state_.CanFold()) {
            ApplyAbstract(Action::kCheckCall);
          } else if (MapToA(0, abstract_call, abstract_size)) {
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

  struct AvailableAction {
    Action play;
    double size;
    float policy;
    std::size_t action_idx = kIllegalId;
  };

  /*
    @brief Get the actions available to Fishbait and its policy if it were
        choosing an action for the player acting in the current abstract state.
  */
  std::array<AvailableAction, kActions> GetAvailableActions() {
    Round r = actual_state_.round();
    PlayerId acting_player = actual_state_.acting_player();

    auto access_fn = std::bind(&ScribeT::GetCluster, &strategy_,
                               std::placeholders::_1, std::placeholders::_2);
    CardCluster cc = info_abstraction_.Cluster(actual_state_, acting_player,
                                               access_fn);
    std::array policy = strategy_.Policy(r, cc, abstract_seq_);
    std::array actions = strategy_.Actions(r);
    hsize_t n_actions = strategy_.ActionCount(r);

    std::array<AvailableAction, kActions> ret_arr;
    for (std::size_t i = 0; i < n_actions; ++i) {
      SequenceId next_seq = strategy_.Next(r, abstract_seq_, i);
      /* Filter out all actions that are illegal in the actual game */
      if (!actual_state_.IsLegal(actions[i]) || next_seq == kIllegalId) {
        policy[i] = 0;
        continue;
      }
      ret_arr[i] = { actions[i].play, actions[i].size, policy[i], i };
    }
    Normalize(policy);
    for (std::size_t i = 0; i < n_actions; ++i) {
      ret_arr[i].policy = policy[i];
    }
    return ret_arr;
  }

  /*
    @brief Ask fishbait to make a move.

    @return A pair of an Action and an amount of Chips. The Action is the action
        that fishbait has decided to take. The Chips are how many Chips fishbait
        is betting if the Action is kBet.
  */
  std::pair<Action, Chips> Query() {
    if (actual_state_.acting_player() != fishbait_seat_) {
      throw std::logic_error("Query called when it's not fishbait's turn.");
    }

    // Sample the action
    Round r = actual_state_.round();
    auto access_fn = std::bind(&ScribeT::GetCluster, &strategy_,
                               std::placeholders::_1, std::placeholders::_2);
    CardCluster cc = info_abstraction_.Cluster(actual_state_, fishbait_seat_,
                                               access_fn);
    std::array policy = strategy_.Policy(r, cc, abstract_seq_);
    std::array actions = strategy_.Actions(r);
    hsize_t n_actions = strategy_.ActionCount(r);
    for (nda::size_t i = 0; i < policy.size(); ++i) {
      /* Filter out all actions that are illegal in the actual game */
      if (!actual_state_.IsLegal(actions[i]) || i >= n_actions) {
        policy[i] = 0;
      }
    }
    Normalize(policy);
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
    return std::make_pair(action.play, actual_chips);
  }

  /* @brief Returns the current state of the game. */
  const Node<kPlayers>& State() const { return actual_state_; }

  /* @brief Returns fishbait's player id. */
  PlayerId fishbait_seat() const { return fishbait_seat_; }

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
         abstraction, so we convert the pot sizes to actual chips in the
         abstract game, and then use the randomized pseudo harmonic mapping to
         pick one of them to play. */
      } else {
        Chips a_chips = abstract_state_.ProportionToChips(min_size);
        Chips b_chips = abstract_state_.ProportionToChips(max_size);
        Chips x_chips = abstract_state_.ProportionToChips(*size);
        if (MapToA(a_chips, b_chips, x_chips)) {
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

    @return True if map to a, false if map to b.
  */
  bool MapToA(Chips a, Chips b, Chips x) {
    double f = ((b - x) * (1.0 + a)) / ((b - a) * (1.0 + x));
    std::uniform_real_distribution<double> sampler(0, 1);
    double sampled = sampler(rng_());
    return sampled < f;
  }
};  // class Commander

}  // namespace fishbait

#endif  // SRC_RELAY_COMMANDER_H_
