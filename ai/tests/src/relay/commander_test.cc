#include <array>
#include <filesystem>
#include <random>
#include <utility>

#include "catch2/catch.hpp"
#include "clustering/test_clusters.h"
#include "mccfr/definitions.h"
#include "mccfr/strategy.h"
#include "poker/definitions.h"
#include "poker/node.h"
#include "relay/commander.h"
#include "utils/random.h"

TEST_CASE("commander throw test", "[relay][commander][.]") {
  constexpr fishbait::PlayerN kPlayers = 3;
  constexpr int kActions = 5;

  fishbait::Node<kPlayers> start_state;
  std::array<fishbait::AbstractAction, kActions> actions = {{
      {fishbait::Action::kFold},
      {fishbait::Action::kAllIn},
      {fishbait::Action::kCheckCall},

      {fishbait::Action::kBet, 2.0, 1, fishbait::Round::kTurn,
       fishbait::Round::kTurn, 2, 0},
      {fishbait::Action::kBet, 0.25, 1, fishbait::Round::kFlop,
       fishbait::Round::kRiver, 0, 10000}
  }};
  fishbait::TestClusters info_abstraction;
  int prune_constant = 0;
  int regret_floor = -10000;
  fishbait::Strategy s(start_state, actions, info_abstraction,
                       prune_constant, regret_floor);
  auto avg = s.InitialAverage();

  for (int i = 0; i < 10000; ++i) {
    for (fishbait::PlayerId p = 0; p < kPlayers; ++p) {
      s.TraverseMCCFR(p, false);
      s.UpdateStrategy(p);
    }
    avg += s;
  }
  avg.Normalize();

  std::filesystem::remove("out/tests/commander_test_strat.hdf");
  using CommanderT = fishbait::Commander<kPlayers, kActions,
                                         fishbait::TestClusters>;
  CommanderT napoleon(
      CommanderT::ScribeT{avg, "out/tests/commander_test_strat.hdf"});
  fishbait::Node<kPlayers> sim_game{200, 0, 2, 1};
  fishbait::PlayerId fishbait_seat = 0;
  napoleon.Reset(sim_game, fishbait_seat);
  const fishbait::Node<kPlayers>& state = napoleon.State();
  constexpr int kDummyActions = 9;
  std::array<fishbait::AbstractAction, kDummyActions> dummy_actions = {{
      {fishbait::Action::kFold},
      {fishbait::Action::kAllIn},
      {fishbait::Action::kCheckCall},

      {fishbait::Action::kBet, 3.0},
      {fishbait::Action::kBet, 2.0},
      {fishbait::Action::kBet, 1.5},
      {fishbait::Action::kBet, 1.0},
      {fishbait::Action::kBet, 0.5},
      {fishbait::Action::kBet, 0.25}
  }};
  fishbait::Random rng;
  auto random_action = [&]() -> fishbait::AbstractAction {
    fishbait::AbstractAction action;
    std::uniform_int_distribution sampler{0, kDummyActions - 1};
    do {
      int idx = sampler(rng());
      action = dummy_actions[idx];
    } while (!state.IsLegal(action));
    return action;
  };

  for (int i = 0; i < 1000000; ++i) {
    while (state.in_progress()) {
      if (state.acting_player() == state.kChancePlayer) {
        napoleon.Deal();
        napoleon.ProceedPlay();
      } else if (state.acting_player() == fishbait_seat) {
        napoleon.Query();
      } else {
        fishbait::AbstractAction action = random_action();
        fishbait::Chips bet_size = state.ProportionToChips(action.size);
        napoleon.Apply(action.play, bet_size);
      }
    }
    napoleon.AwardPot();
    napoleon.NewHand();
    for (fishbait::PlayerId p = 0; p < kPlayers; ++p) {
      if (state.stack(p) == 0) {
        napoleon.Reset(sim_game, fishbait_seat);
        break;
      }
    }
    if (i % 10000 == 0) std::cout << i << std::endl;
  }
}  // TEST_CASE "commander throw test"
