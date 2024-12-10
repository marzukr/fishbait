#include <array>
#include <atomic>
#include <filesystem>
#include <iostream>
#include <memory>
#include <ostream>
#include <string_view>

#include "clustering/cluster_table.h"
#include "mccfr/definitions.h"
#include "mccfr/hyperparameters.h"
#include "mccfr/sequence_table.h"
#include "mccfr/strategy.h"
#include "poker/node.h"
#include "utils/math.h"
#include "utils/random.h"
#include "utils/timer.h"

int main() {
  fishbait::Node<fishbait::hparam::kPlayers> start_state;
  fishbait::ClusterTable cluster_table(true);

  using Minutes = fishbait::Timer::Minutes;
  fishbait::Timer main_timer;
  double trained_time = 0.0;
  fishbait::Timer train_timer;
  fishbait::Timer discount_timer;
  fishbait::Timer snapshot_timer;

  auto log_fn = [&]() -> std::ostream& {
    std::cout << "[";
    main_timer.Check<Minutes>(std::cout);
    std::cout << " elapsed, " << trained_time << " min trained]: ";
    return std::cout;
  };

  log_fn() << "initializing strategy" << std::endl;
  fishbait::Strategy strategy(start_state, fishbait::hparam::kActionArr,
                              cluster_table, fishbait::hparam::kPruneConstant,
                              fishbait::hparam::kRegretFloor);
  using AverageT = decltype(strategy)::Average;
  log_fn() << "initializing last average" << std::endl;
  auto last_average = std::make_unique<AverageT>(strategy.InitialAverage());
  decltype(last_average) current_average = nullptr;

  thread_local fishbait::Random rng;
  bool check_prune = false;
  bool check_update = false;
  bool is_training = false;

  // If the training threads should continue
  static_assert(std::atomic<bool>::is_always_lock_free);
  std::atomic<bool> should_continue;
  should_continue.store(true, std::memory_order_release);

  auto train_fn = [&](bool thread_check_update, bool thread_check_prune) {
    int iteration = 0;
    while (should_continue.load(std::memory_order_acquire)) {
      for (fishbait::PlayerId player = 0; player < fishbait::hparam::kPlayers;
           ++player) {
        // update strategy after kStrategyInterval iterations
        if (thread_check_update &&
            iteration % fishbait::hparam::kStrategyInterval == 0) {
          strategy.UpdateStrategy(player);
        }

        // Set prune variable and traverse MCCFR
        bool prune = false;
        if (thread_check_prune) {
          std::uniform_real_distribution<> uniform_distribution(0.0, 1.0);
          double random_prune = uniform_distribution(rng());
          if (random_prune < fishbait::hparam::kPruneProbability) {
            prune = true;
          }
        }
        strategy.TraverseMCCFR(player, prune);
      }  // for player
      ++iteration;
    }  // while should_continue
  };  // train_fn()
  std::array<std::thread, fishbait::kThreads> threads;
  auto spawn_threads = [&]() {
    should_continue.store(true, std::memory_order_release);
    for (std::size_t i = 0; i < threads.size(); ++i) {
      threads[i] = std::thread(train_fn, check_update, check_prune);
    }
    train_timer.Start();
    discount_timer.Start();
    snapshot_timer.Start();
    is_training = true;
  };
  auto join_threads = [&]() {
    should_continue.store(false, std::memory_order_release);
    for (std::size_t i = 0; i < threads.size(); ++i) {
      threads[i].join();
    }
    train_timer.Stop();
    discount_timer.Stop();
    snapshot_timer.Stop();
    is_training = false;
  };

  log_fn() << "Starting MCCFR" << std::endl;
  spawn_threads();
  while (trained_time < fishbait::hparam::kTrainingTime) {
    if (!check_update && trained_time > fishbait::hparam::kStrategyDelay) {
      log_fn() << "Starting preflop average updates" << std::endl;
      if (is_training) join_threads();
      check_update = true;
    }

    if (!check_prune && trained_time > fishbait::hparam::kPruneThreshold) {
      log_fn() << "Starting prune" << std::endl;
      if (is_training) join_threads();
      check_prune = true;
    }

    /* Discount all regrets and action counter every kDiscountInterval until
       kLCFRThreshold */
    if (trained_time < fishbait::hparam::kLCFRThreshold &&
        discount_timer.Check<Minutes>() >=
            fishbait::hparam::kDiscountInterval) {
      if (is_training) join_threads();
      double d = (trained_time / fishbait::hparam::kDiscountInterval) /
                 (trained_time / fishbait::hparam::kDiscountInterval + 1);
      log_fn() << "Discounting by " << d << std::endl;
      strategy.Discount(d);
      discount_timer.Reset();
    }

    /* Save a snapshot of the strategy and update the average strategy every
       kSnapshotInterval minutes if it's been at least kStrategyDelay
       minutes. */
    if (trained_time >= fishbait::hparam::kStrategyDelay &&
        snapshot_timer.Check<Minutes>() >=
            fishbait::hparam::kSnapshotInterval) {
      if (is_training) join_threads();

      if (current_average == nullptr) {
        log_fn() << "Computing initial average" << std::endl;
        current_average = std::make_unique<AverageT>(strategy.InitialAverage());
      } else {
        log_fn() << "Updating old average" << std::endl;
        *last_average = *current_average;
        log_fn() << "Computing new average" << std::endl;
        *current_average += strategy;
      }

      log_fn() << "Evaluating new against previous averages" << std::endl;
      std::vector<double> means = current_average->BattleStats(
          *last_average, fishbait::hparam::kBattleMeans,
          fishbait::hparam::kBattleTrials);
      double avg_improvement = fishbait::Mean(means);
      double avg_improvement_std = fishbait::Std(means, avg_improvement);
      double avg_improvment_err = fishbait::CI95(means, avg_improvement_std);
      log_fn() << avg_improvement << " ± " << avg_improvment_err << std::endl;

      snapshot_timer.Reset();
    }

    if (!is_training) {
      log_fn() << "Restarting training" << std::endl;
      spawn_threads();
    }
    std::this_thread::sleep_for(fishbait::Timer::Seconds{60});
    trained_time = train_timer.Check<fishbait::Timer::Minutes>();
  }  // while trained_time < fishbait::hparam::kTrainingTime

  join_threads();
  log_fn() << "Completed MCCFR" << std::endl;

  std::filesystem::create_directories(fishbait::hparam::kSaveDir);

  log_fn() << "Normalizing average" << std::endl;
  current_average->Normalize();
  log_fn() << "Saving average" << std::endl;
  CerealSave(fishbait::hparam::kAvgPath.string(), current_average.get(), false);

  log_fn() << "Saving final strategy" << std::endl;
  CerealSave(fishbait::hparam::kFinalStrategyPath.string(), &strategy, false);
}
