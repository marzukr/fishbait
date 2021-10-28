// Copyright 2021 Marzuk Rashid

#include <array>
#include <filesystem>
#include <iostream>

#include "blueprint/definitions.h"
#include "blueprint/sequence_table.h"
#include "blueprint/strategy.h"
#include "clustering/cluster_table.h"
#include "poker/node.h"
#include "utils/random.h"
#include "utils/timer.h"

int main() {
  constexpr fishbait::PlayerN kPlayers = 6;
  fishbait::Node<kPlayers> start_state;
  /* The following bet sizes are what we believe are close to what was used in
     pluribus to a reasonable degree of accuracy. It results in 664,842,258
     action sequences. */
  std::array<fishbait::AbstractAction, 23> actions = {{
      {fishbait::Action::kFold},
      {fishbait::Action::kCheckCall},
      {fishbait::Action::kAllIn},

      {fishbait::Action::kBet, 0.4, 1, fishbait::Round::kPreFlop,
       fishbait::Round::kPreFlop},
      {fishbait::Action::kBet, 0.5, 4, fishbait::Round::kPreFlop,
       fishbait::Round::kPreFlop},
      {fishbait::Action::kBet, 0.6, 3, fishbait::Round::kPreFlop,
       fishbait::Round::kPreFlop},
      {fishbait::Action::kBet, 0.75, 3, fishbait::Round::kPreFlop,
       fishbait::Round::kPreFlop},
      {fishbait::Action::kBet, 0.875, 3, fishbait::Round::kPreFlop,
       fishbait::Round::kPreFlop},
      {fishbait::Action::kBet, 1, 0, fishbait::Round::kPreFlop,
       fishbait::Round::kPreFlop},
      {fishbait::Action::kBet, 1.25, 3, fishbait::Round::kPreFlop,
       fishbait::Round::kPreFlop},
      {fishbait::Action::kBet, 1.5, 2, fishbait::Round::kPreFlop,
       fishbait::Round::kPreFlop},
      {fishbait::Action::kBet, 2, 0, fishbait::Round::kPreFlop,
       fishbait::Round::kPreFlop},
      {fishbait::Action::kBet, 2.5, 0, fishbait::Round::kPreFlop,
       fishbait::Round::kPreFlop},
      {fishbait::Action::kBet, 3, 0, fishbait::Round::kPreFlop,
       fishbait::Round::kPreFlop},
      {fishbait::Action::kBet, 4, 0, fishbait::Round::kPreFlop,
       fishbait::Round::kPreFlop},
      {fishbait::Action::kBet, 5, 0, fishbait::Round::kPreFlop,
       fishbait::Round::kPreFlop},
      {fishbait::Action::kBet, 6, 0, fishbait::Round::kPreFlop,
       fishbait::Round::kPreFlop},

      {fishbait::Action::kBet, 0.25, 1, fishbait::Round::kFlop,
       fishbait::Round::kFlop, 2, 2081},
      {fishbait::Action::kBet, 0.5, 3, fishbait::Round::kFlop,
       fishbait::Round::kFlop, 4},
      {fishbait::Action::kBet, 1, 0, fishbait::Round::kFlop,
       fishbait::Round::kFlop, 4},
      {fishbait::Action::kBet, 2, 0, fishbait::Round::kFlop,
       fishbait::Round::kFlop, 4},

      {fishbait::Action::kBet, 0.5, 1, fishbait::Round::kTurn,
       fishbait::Round::kRiver, 3},
      {fishbait::Action::kBet, 1, 0, fishbait::Round::kTurn,
       fishbait::Round::kRiver, 3},
  }};
  fishbait::ClusterTable cluster_table(true);

  /* The total time to run training in minutes. 11625 minutes = ~8 days */
  constexpr double kTrainingTime = 11625;

  /* The number of minutes between each update of the average strategy. */
  constexpr int kStrategyInterval = 200;

  /* The number of minutes to wait before pruning. */
  constexpr int kPruneThreshold = 200;

  /* The probability that we prune in Traverse-MCCFR. */
  constexpr double kPruneProbability = 0.95;

  /* Actions with regret less than or equal to this constant are eligible to be
     pruned. */
  constexpr fishbait::Regret kPruneConstant = -300000000;

  /* The number of minutes to apply LCFR. */
  constexpr int kLCFRThreshold = 400;

  /* The number of minutes between each LCFR discount. */
  constexpr int kDiscountInterval = 10;  // 10 minutes

  /* Floor to cutoff negative regrets at. */
  constexpr fishbait::Regret kRegretFloor = -310000000;

  /* The number of minutes between each snapshot. */
  constexpr int kSnapshotInterval = 200;

  /* The number of minutes to wait before updating the average strategy and
     taking snapshots. */
  constexpr int kStrategyDelay = 800;

  fishbait::Strategy strategy(start_state, actions, cluster_table,
                              kPruneConstant, kRegretFloor,
                              fishbait::Random::Seed{});

  std::filesystem::path base_path("out/blueprint");
  std::filesystem::path save_path = base_path / "run_1";
  std::filesystem::create_directory(save_path);

  using Minutes = fishbait::Timer::Minutes;
  fishbait::Timer main_timer;
  fishbait::Timer iteration_timer;
  fishbait::Timer update_strategy_timer;
  fishbait::Timer discount_timer;
  fishbait::Timer snapshot_timer;
  fishbait::Random rng;
  int iteration = 0;
  double elapsed_time = 0;
  std::cout << "Starting MCCFR" << std::endl;
  while (elapsed_time < kTrainingTime) {
    std::cout << "iteration " << iteration << " :";
    iteration_timer.Reset(std::cout) << std::endl;
    for (fishbait::PlayerId player = 0; player < kPlayers; ++player) {
      // update strategy after kStrategyInterval iterations
      if (elapsed_time > kStrategyDelay &&
          update_strategy_timer.Check<Minutes>() >= kStrategyInterval) {
        update_strategy_timer.Reset();
        strategy.UpdateStrategy(player);
      }

      // Set prune variable and traverse MCCFR
      bool prune = false;
      if (elapsed_time > kPruneThreshold) {
        std::uniform_real_distribution<> uniform_distribution(0.0, 1.0);
        double random_prune = uniform_distribution(rng());
        if (random_prune < kPruneProbability) {
          prune = true;
        }
      }
      strategy.TraverseMCCFR(player, prune);
    }  // for player

    /* Discount all regrets and action counter every kDiscountInterval until
       kLCFRThreshold */
    if (elapsed_time < kLCFRThreshold &&
        discount_timer.Check<Minutes>() >= kDiscountInterval) {
      discount_timer.Reset();
      double d = (elapsed_time / kDiscountInterval) /
                 (elapsed_time / kDiscountInterval + 1);
      strategy.Discount(d);
    }

    /* Save a snapshot of the strategy and store to disk every kSnapshotInterval
       minutes if it's been at least kStrategyDelay minutes. */
    if (elapsed_time > kStrategyDelay &&
        snapshot_timer.Check<Minutes>() >= kSnapshotInterval) {
      snapshot_timer.Reset();
      std::stringstream strategy_ss;
      strategy_ss << "strategy_" << static_cast<int>(elapsed_time) << ".cereal";
      std::filesystem::path strategy_path = save_path / strategy_ss.str();
      CerealSave(strategy_path.string(), &strategy, true);
    }

    ++iteration;
    elapsed_time = main_timer.Check<fishbait::Timer::Minutes>();
  }  // while elapsed_time < kTrainingTime

  std::filesystem::path strategy_path = save_path / "strategy_final.cereal";
  CerealSave(strategy_path.string(), &strategy, true);
}
