// Copyright 2021 Marzuk Rashid

#include "hand_strengths/lut_generators.h"

#include <algorithm>
#include <cstdint>
#include <iostream>
#include <ostream>
#include <vector>

#include "array/array.h"
#include "array/matrix.h"
#include "SKPokerEval/src/SevenEval.h"
#include "engine/card_combinations.h"
#include "engine/card_utils.h"
#include "engine/definitions.h"
#include "engine/indexer.h"
#include "hand_strengths/definitions.h"
#include "hand_strengths/ochs.h"
#include "utils/combination_matrix.h"
#include "utils/timer.h"

namespace hand_strengths {

std::vector<ShowdownStrength> ShowdownLUT(const bool verbose) {
  if (verbose) {
    std::cout << "Generating Showdown LUT..." << std::endl;
  }

  utils::CombinationMatrix<OCHS_Id> op_clusters = SKClusterLUT();

  constexpr hand_index_t one_hand_approx =
      engine::ImperfectRecallHands(engine::Round::kRiver) /
      engine::kUniqueHands;

  std::vector<ShowdownStrength> showdown_lut(
    engine::ImperfectRecallHands(engine::Round::kRiver));

  engine::Indexer<2, 5> isocalc;
  std::array<engine::Card, 7> rollout;
  engine::CardCombinations op_hands(2);

  hand_index_t sd_count = 0;
  utils::Timer t;
  for (hand_index_t idx = 0;
       idx < engine::ImperfectRecallHands(engine::Round::kRiver); ++idx) {
    rollout = isocalc.Unindex<1>(idx);
    std::transform(rollout.begin(), rollout.end(), rollout.begin(),
                   engine::ConvertISOtoSK);

    for (op_hands.Reset(rollout); !op_hands.is_done(); ++op_hands) {
      SevenEval::Rank hero_rank = SevenEval::GetRank(rollout[0], rollout[1],
          rollout[2], rollout[3], rollout[4], rollout[5], rollout[6]);
      SevenEval::Rank villan_rank = SevenEval::GetRank(op_hands(0), op_hands(1),
          rollout[2], rollout[3], rollout[4], rollout[5], rollout[6]);

      double win_value = 0;
      if (hero_rank > villan_rank) {
        win_value = 1;
      } else if (hero_rank == villan_rank) {
        win_value = 0.5;
      }

      OCHS_Id cluster = op_clusters(op_hands(0), op_hands(1));
      showdown_lut[idx].ehs += win_value;
      showdown_lut[idx].ochs_wins[cluster] += win_value;
      showdown_lut[idx].ochs_totals[cluster] += 1;
    }  // for op_hands

    showdown_lut[idx].ehs /= engine::kOpHandsN;

    sd_count += 1;
    if (verbose && sd_count % one_hand_approx == 0) {
      std::cout << 100.0 * sd_count /
                   engine::ImperfectRecallHands(engine::Round::kRiver)
                << "%" << std::endl;
      t.StopAndReset("iteration time");
    }
  }  // for idx

  return showdown_lut;
}  // ShowdownLUT()

template <hand_index_t kLUTSize, HistBucketN kBuckets,
          engine::CardN kSimulationCards, engine::RoundN kISORound,
          typename IndexerT>
nda::matrix<HistCount> EHS_LUT(IndexerT& isocalc,
    const std::vector<ShowdownStrength>& showdown_lut, const bool verbose) {

  const hand_index_t simulation_size = utils::N_Choose_K(
      engine::kDeckSize - (engine::kPlayerCards - kSimulationCards),
      kSimulationCards);
  const hand_index_t one_percent_approx = kLUTSize * simulation_size / 100;
  const double bucket_size = 1.0 / kBuckets;

  nda::matrix<HistCount> ehs_lut(nda::matrix_shape<>(kLUTSize, kBuckets), 0);

  engine::Indexer<2, 5> showdown_calc;
  std::array<engine::Card, engine::kPlayerCards> rollout;
  std::array<hand_index_t, 2> indicies;
  engine::CardCombinations simulations(kSimulationCards);
  constexpr engine::CardN kNotSimulated = engine::kPlayerCards -
                                          kSimulationCards;


  hand_index_t sd_count = 0;
  utils::Timer t;
  for (hand_index_t idx = 0; idx < kLUTSize; ++idx) {
    std::array<engine::ISO_Card, kNotSimulated> hand =
        isocalc.template Unindex<kISORound>(idx);
    std::copy_n(hand.begin(), kNotSimulated, rollout.begin());

    for (simulations.Reset(hand); !simulations.is_done(); ++simulations) {
      for (engine::CardN j = 0; j < kSimulationCards; ++j) {
        rollout[kNotSimulated + j] = simulations(j);
      }

      // Get the appropriate showdown LUT index
      indicies = showdown_calc.Index(rollout);

      HistBucketId bucket_unbounded = showdown_lut[indicies[1]].ehs /
                                      bucket_size;
      HistBucketId bucket = std::min(bucket_unbounded, kBuckets - 1);

      ehs_lut(idx, bucket) += 1;

      sd_count += 1;
      if (verbose && sd_count % one_percent_approx == 0) {
        std::cout << 100.0 * sd_count / (kLUTSize * simulation_size)
                  << "%" << std::endl;
        t.StopAndReset("iteration time");
      }
    }  // for simulations
  }  // for idx

  return ehs_lut;
}  // EHS_LUT()

nda::matrix<HistCount> PreflopLUT(
    const std::vector<ShowdownStrength>& showdown_lut, const bool verbose) {
  if (verbose) {
    std::cout << "Generating Preflop LUT..." << std::endl;
  }

  constexpr hand_index_t kLUTSize =
      engine::ImperfectRecallHands(engine::Round::kPreFlop);
  constexpr HistBucketN kBuckets = 50;
  constexpr engine::CardN kSimulationSize = 5;
  constexpr engine::RoundN kISORound = 0;
  using IndexerT = engine::Indexer<2>;
  IndexerT isocalc;
  return EHS_LUT<kLUTSize, kBuckets, kSimulationSize, kISORound, IndexerT>(
      isocalc, showdown_lut, verbose);
}

nda::matrix<HistCount> FlopLUT(
    const std::vector<ShowdownStrength>& showdown_lut, const bool verbose) {
  if (verbose) {
    std::cout << "Generating Flop LUT..." << std::endl;
  }

  constexpr hand_index_t kLUTSize =
      engine::ImperfectRecallHands(engine::Round::kFlop);
  constexpr HistBucketN kBuckets = 50;
  constexpr engine::CardN kSimulationSize = 2;
  constexpr engine::RoundN kISORound = 1;
  using IndexerT = engine::Indexer<2, 3>;
  IndexerT isocalc;
  return EHS_LUT<kLUTSize, kBuckets, kSimulationSize, kISORound, IndexerT>(
      isocalc, showdown_lut, verbose);
}

nda::matrix<HistCount> TurnLUT(
    const std::vector<ShowdownStrength>& showdown_lut, const bool verbose) {
  if (verbose) {
    std::cout << "Generating Turn LUT..." << std::endl;
  }

  constexpr hand_index_t kLUTSize =
      engine::ImperfectRecallHands(engine::Round::kTurn);
  constexpr HistBucketN kBuckets = 50;
  constexpr engine::CardN kSimulationSize = 1;
  constexpr engine::RoundN kISORound = 1;
  using IndexerT = engine::Indexer<2, 4>;
  IndexerT isocalc;
  return EHS_LUT<kLUTSize, kBuckets, kSimulationSize, kISORound, IndexerT>(
      isocalc, showdown_lut, verbose);
}

nda::matrix<double> RiverLUT(
    const std::vector<ShowdownStrength>& showdown_lut, const bool verbose) {
  if (verbose) {
    std::cout << "Generating River LUT..." << std::endl;
  }

  constexpr hand_index_t one_percent_approx =
      engine::ImperfectRecallHands(engine::Round::kRiver) / 100;
  nda::matrix<double> river_lut(nda::matrix_shape<>{
    engine::ImperfectRecallHands(engine::Round::kRiver), kOCHS_N}, 0);

  utils::Timer t;
  for (hand_index_t idx = 0;
       idx < engine::ImperfectRecallHands(engine::Round::kRiver); ++idx) {
    for (OCHS_Id j = 0; j < kOCHS_N; ++j) {
      river_lut(idx, j) = showdown_lut[idx].ochs_wins[j] /
                          showdown_lut[idx].ochs_totals[j];
    }

    if (verbose && idx % one_percent_approx == 0) {
      std::cout << 100.0 * idx /
                   (engine::ImperfectRecallHands(engine::Round::kRiver) *
                    1.0)
                << "%" << std::endl;
      t.StopAndReset("iteration time");
    }
  }  // for idx
  return river_lut;
}  // RiverLUT()

nda::matrix<double> OCHS_PreflopLUT(
    const std::vector<ShowdownStrength>& showdown_lut, const bool verbose) {
  if (verbose) {
    std::cout << "Generating OCHS Preflop LUT..." << std::endl;
  }

  const hand_index_t simulation_size = utils::N_Choose_K(50, 5);
  const hand_index_t one_percent_approx =
      engine::kUniqueHands*simulation_size / 100;

  nda::matrix<double> ochs_preflop_lut(
      nda::matrix_shape<>(engine::kUniqueHands, kOCHS_N), 0);

  engine::Indexer<2, 5> showdown_calc;
  std::array<engine::ISO_Card, 7> rollout;
  std::array<hand_index_t, 2> indicies;
  std::array<HistCount, kOCHS_N> sim_totals;
  engine::CardCombinations simulations(5);

  hand_index_t sd_count = 0;
  utils::Timer t;
  for (hand_index_t idx = 0; idx < engine::kUniqueHands; ++idx) {
    std::array hand = showdown_calc.template Unindex<0>(idx);
    std::copy_n(hand.begin(), 2, rollout.begin());
    std::fill(sim_totals.begin(), sim_totals.end(), 0);

    for (simulations.Reset(hand); !simulations.is_done(); ++simulations) {
      for (engine::CardN j = 0; j < 5; ++j) {
        rollout[2 + j] = simulations(j);
      }

      // Get the appropriate showdown LUT index
      indicies = showdown_calc.Index(rollout);

      for (OCHS_Id k = 0; k < kOCHS_N; ++k) {
        ochs_preflop_lut(idx, k) += showdown_lut[indicies[1]].ochs_wins[k];
        sim_totals[k] += showdown_lut[indicies[1]].ochs_totals[k];
      }

      sd_count += 1;
      if (verbose && sd_count % one_percent_approx == 0) {
        std::cout << 100.0 * sd_count / (engine::kUniqueHands * simulation_size)
                  << "%" << std::endl;
        t.StopAndReset("iteration time");
      }
    }  // for simulations

    for (OCHS_Id k = 0; k < kOCHS_N; ++k) {
      ochs_preflop_lut(idx, k) /= sim_totals[k];
    }
  }  // for idx

  return ochs_preflop_lut;
}  // OCHS_PreflopLUT

}  // namespace hand_strengths
