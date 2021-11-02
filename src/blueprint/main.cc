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
  /*
    The following bet sizes are what we believe are close to what was used in
    pluribus to a reasonable degree of accuracy. It results in a sequence table
    with the following counts:

    preflop actions: 17
    flop actions: 7
    turn actions: 5
    river actions: 5

    illegal nodes: 325846945, 333211308, 294466390, 369101348
    total illegal nodes: 1322625991
    internal nodes: 22002506, 68725968, 100425836, 126042938
    total internal nodes: 317197248
    legal actions: 48195657, 147870468, 207662790, 261113342
    total legal actions: 664842257
    leaf nodes: 4190677, 20042089, 30712047, 292700197
    total leaf nodes: 347645010

    regrets = (preflop legal actions * preflop clusters) +
              (flop legal actions * flop clusters) +
              (turn legal actions * turn clusters) +
              (river legal actions * river clusters)
            = (48195657 * 169) + (147870468 * 200) + (207662790 * 200) +
              (261113342 * 200)
            = 131474386033
    regret table size = regrets * size of regret
                      = 131474386033 * size of int32_t
                      = 664842258 * 4 bytes
                      = 525897544132 bytes
                      = 489.7803 GiB
    
    action counts size = preflop legal actions * preflop clusters *
                         size of action count
                       = 48195657 * 169 * size of uint32_t
                       = 48195657 * 169 * 4 bytes
                       = 32580264132 bytes
                       = 30.34274 GiB

    sequence table table entries = (preflop internal nodes * preflop actions) +
                                   (flop internal nodes * flop actions) +
                                   (turn internal nodes * turn actions) +
                                   (river internal nodes * river actions)
                                 = (22002506 * 17) + (68725968 * 7) +
                                   (100425836 * 5) + (126042938 * 5)
                                 = 1987468248
    sequence table table size = sequence table table entries *
                                size of SequenceId
                              = 1987468248 * size of uint32_t
                              = 1987468248 * 4 bytes
                              = 7949872992 bytes
                              = 7.403896 GiB
    sequence table offsets size = total internal nodes * size of offset
                                = 317197248 * size of std::size_t
                                = 317197248 * (up to 8 bytes)
                                = 2537577984 bytes
                                = 2.363304 GiB
    action abstraction size = sequence table table size +
                              sequence table offsets size
                            = 7.403896 GiB + 2.363304 GiB
                            = 9.7672 GiB

    info abstraction size = (preflop hands + flop hands + turn hands +
                             river hands) * size of CardCluster
                          = (169 + 1286792 + 13960050 + 123156254) *
                            size of uint32_t
                          = 138403265 * 4 bytes
                          = 553613060 bytes
                          = 0.5155923 GiB
    
    strategy size = info abstraction size + action abstraction size +
                    regret table size + action counts size
                  = 0.5155923 GiB + 9.7672 GiB + 489.7803 GiB + 30.34274 GiB
                  = 530.4058323 GiB

    strategy average size = regrets * size of float
                          = 131474386033 * 4 bytes
                          = 525897544132 bytes
                          = 489.7803 GiB
  */
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
  std::filesystem::path avg_path;

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

    /* Save a snapshot of the strategy and update the average strategy every
       kSnapshotInterval minutes if it's been at least kStrategyDelay
       minutes. */
    if (elapsed_time > kStrategyDelay &&
        snapshot_timer.Check<Minutes>() >= kSnapshotInterval) {
      snapshot_timer.Reset();

      if (avg_path.empty()) {
        avg_path = save_path / "avg_table.cereal";
        auto strat_table = strategy.InitialAverage();
        CerealSave(avg_path.string(), &strat_table, true);
      } else {
        auto strat_table =
            decltype(strategy)::Average::LoadAverage(avg_path, true);
        strat_table += strategy;
        CerealSave(avg_path.string(), &strat_table, true);
      }

      std::stringstream strategy_ss;
      strategy_ss << "strategy_" << static_cast<int>(elapsed_time) << ".cereal";
      std::filesystem::path strategy_path = save_path / strategy_ss.str();
      CerealSave(strategy_path.string(), &strategy, true);
    }

    ++iteration;
    elapsed_time = main_timer.Check<fishbait::Timer::Minutes>();
  }  // while elapsed_time < kTrainingTime

  auto strat_table = decltype(strategy)::Average::LoadAverage(avg_path, true);
  strat_table.Normalize();
  CerealSave(avg_path.string(), &strat_table, true);

  std::filesystem::path strategy_path = save_path / "strategy_final.cereal";
  CerealSave(strategy_path.string(), &strategy, true);
}
