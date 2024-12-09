#include "hand_strengths/lut_generators.h"

#include <algorithm>
#include <cstdint>
#include <iostream>
#include <ostream>
#include <vector>

#include "array/array.h"
#include "array/matrix.h"
#include "hand_strengths/card_combinations.h"
#include "hand_strengths/definitions.h"
#include "hand_strengths/ochs.h"
#include "poker/card_utils.h"
#include "poker/definitions.h"
#include "poker/indexer.h"
#include "SKPokerEval/src/SevenEval.h"
#include "utils/combination_matrix.h"
#include "utils/timer.h"

namespace fishbait {

std::vector<ShowdownStrength> ShowdownLUT(const bool verbose) {
  if (verbose) {
    std::cout << "Generating Showdown LUT..." << std::endl;
  }

  CombinationMatrix<OCHS_Id> op_clusters = SKClusterLUT();

  constexpr hand_index_t one_hand_approx = ImperfectRecallHands(Round::kRiver) /
                                           kUniqueHands;

  std::vector<ShowdownStrength> showdown_lut(
      ImperfectRecallHands(Round::kRiver));

  Indexer<2, 5> isocalc;
  std::array<Card, 7> rollout;
  CardCombinations op_hands(2);

  hand_index_t sd_count = 0;
  Timer t;
  for (hand_index_t idx = 0; idx < ImperfectRecallHands(Round::kRiver); ++idx) {
    rollout = isocalc.Unindex<1>(idx);
    std::transform(rollout.begin(), rollout.end(), rollout.begin(),
                   ConvertISOtoSK);

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

    showdown_lut[idx].ehs /= kOpHandsN;

    sd_count += 1;
    if (verbose && sd_count % one_hand_approx == 0) {
      std::cout << 100.0 * sd_count / ImperfectRecallHands(Round::kRiver)
                << "%" << std::endl;
      t.Reset(std::cout << "iteration time: ") << std::endl;
    }
  }  // for idx

  return showdown_lut;
}  // ShowdownLUT()

template <hand_index_t kLUTSize, HistBucketN kBuckets, CardN kSimulationCards,
          RoundN kISORound, typename IndexerT>
nda::matrix<HistCount> EHS_LUT(IndexerT& isocalc,
    const std::vector<ShowdownStrength>& showdown_lut, const bool verbose) {

  constexpr hand_index_t kSimulationSize = N_Choose_K(
      kDeckSize - (kPlayerCards - kSimulationCards),
      kSimulationCards);
  constexpr hand_index_t kOnePercentApprox = kLUTSize * kSimulationSize / 100;
  const double bucket_size = 1.0 / kBuckets;

  nda::matrix<HistCount> ehs_lut(nda::matrix_shape<>(kLUTSize, kBuckets), 0);

  Indexer<2, 5> showdown_calc;
  std::array<Card, kPlayerCards> rollout;
  std::array<hand_index_t, 2> indicies;
  CardCombinations simulations(kSimulationCards);
  constexpr CardN kNotSimulated = kPlayerCards - kSimulationCards;


  hand_index_t sd_count = 0;
  Timer t;
  for (hand_index_t idx = 0; idx < kLUTSize; ++idx) {
    std::array<ISO_Card, kNotSimulated> hand =
        isocalc.template Unindex<kISORound>(idx);
    std::copy_n(hand.begin(), kNotSimulated, rollout.begin());

    for (simulations.Reset(hand); !simulations.is_done(); ++simulations) {
      for (CardN j = 0; j < kSimulationCards; ++j) {
        rollout[kNotSimulated + j] = simulations(j);
      }

      // Get the appropriate showdown LUT index
      indicies = showdown_calc.Index(rollout);

      HistBucketId bucket_unbounded = showdown_lut[indicies[1]].ehs /
                                      bucket_size;
      HistBucketId bucket = std::min(bucket_unbounded, kBuckets - 1);

      ehs_lut(idx, bucket) += 1;

      sd_count += 1;
      if (verbose && sd_count % kOnePercentApprox == 0) {
        std::cout << 100.0 * sd_count / (kLUTSize * kSimulationSize)
                  << "%" << std::endl;
        t.Reset(std::cout << "iteration time: ") << std::endl;
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

  constexpr hand_index_t kLUTSize = ImperfectRecallHands(Round::kPreFlop);
  constexpr HistBucketN kBuckets = 50;
  constexpr CardN kSimulationSize = 5;
  constexpr RoundN kISORound = 0;
  using IndexerT = Indexer<2>;
  IndexerT isocalc;
  return EHS_LUT<kLUTSize, kBuckets, kSimulationSize, kISORound, IndexerT>(
      isocalc, showdown_lut, verbose);
}

nda::matrix<HistCount> FlopLUT(
    const std::vector<ShowdownStrength>& showdown_lut, const bool verbose) {
  if (verbose) {
    std::cout << "Generating Flop LUT..." << std::endl;
  }

  constexpr hand_index_t kLUTSize = ImperfectRecallHands(Round::kFlop);
  constexpr HistBucketN kBuckets = 50;
  constexpr CardN kSimulationSize = 2;
  constexpr RoundN kISORound = 1;
  using IndexerT = Indexer<2, 3>;
  IndexerT isocalc;
  return EHS_LUT<kLUTSize, kBuckets, kSimulationSize, kISORound, IndexerT>(
      isocalc, showdown_lut, verbose);
}

nda::matrix<HistCount> TurnLUT(
    const std::vector<ShowdownStrength>& showdown_lut, const bool verbose) {
  if (verbose) {
    std::cout << "Generating Turn LUT..." << std::endl;
  }

  constexpr hand_index_t kLUTSize = ImperfectRecallHands(Round::kTurn);
  constexpr HistBucketN kBuckets = 50;
  constexpr CardN kSimulationSize = 1;
  constexpr RoundN kISORound = 1;
  using IndexerT = Indexer<2, 4>;
  IndexerT isocalc;
  return EHS_LUT<kLUTSize, kBuckets, kSimulationSize, kISORound, IndexerT>(
      isocalc, showdown_lut, verbose);
}

nda::matrix<double> RiverLUT(
    const std::vector<ShowdownStrength>& showdown_lut, const bool verbose) {
  if (verbose) {
    std::cout << "Generating River LUT..." << std::endl;
  }

  constexpr hand_index_t kOnePercentApprox =
      ImperfectRecallHands(Round::kRiver) / 100;
  nda::matrix<double> river_lut(nda::matrix_shape<>{
      ImperfectRecallHands(Round::kRiver), kOCHS_N}, 0);

  Timer t;
  for (hand_index_t idx = 0; idx < ImperfectRecallHands(Round::kRiver); ++idx) {
    for (OCHS_Id j = 0; j < kOCHS_N; ++j) {
      river_lut(idx, j) = showdown_lut[idx].ochs_wins[j] /
                          showdown_lut[idx].ochs_totals[j];
    }

    if (verbose && idx % kOnePercentApprox == 0) {
      std::cout << 100.0 * idx / (ImperfectRecallHands(Round::kRiver) * 1.0)
                << "%" << std::endl;
      t.Reset(std::cout << "iteration time: ") << std::endl;
    }
  }  // for idx
  return river_lut;
}  // RiverLUT()

nda::matrix<double> OCHS_PreflopLUT(
    const std::vector<ShowdownStrength>& showdown_lut, const bool verbose) {
  if (verbose) {
    std::cout << "Generating OCHS Preflop LUT..." << std::endl;
  }

  constexpr hand_index_t kSimulationSize = N_Choose_K(50, 5);
  const hand_index_t kOnePercentApprox = kUniqueHands * kSimulationSize / 100;

  nda::matrix<double> ochs_preflop_lut(
      nda::matrix_shape<>(kUniqueHands, kOCHS_N), 0);

  Indexer<2, 5> showdown_calc;
  std::array<ISO_Card, 7> rollout;
  std::array<hand_index_t, 2> indicies;
  std::array<HistCount, kOCHS_N> sim_totals;
  CardCombinations simulations(5);

  hand_index_t sd_count = 0;
  Timer t;
  for (hand_index_t idx = 0; idx < kUniqueHands; ++idx) {
    std::array hand = showdown_calc.template Unindex<0>(idx);
    std::copy_n(hand.begin(), 2, rollout.begin());
    std::fill(sim_totals.begin(), sim_totals.end(), 0);

    for (simulations.Reset(hand); !simulations.is_done(); ++simulations) {
      for (CardN j = 0; j < 5; ++j) {
        rollout[2 + j] = simulations(j);
      }

      // Get the appropriate showdown LUT index
      indicies = showdown_calc.Index(rollout);

      for (OCHS_Id k = 0; k < kOCHS_N; ++k) {
        ochs_preflop_lut(idx, k) += showdown_lut[indicies[1]].ochs_wins[k];
        sim_totals[k] += showdown_lut[indicies[1]].ochs_totals[k];
      }

      sd_count += 1;
      if (verbose && sd_count % kOnePercentApprox == 0) {
        std::cout << 100.0 * sd_count / (kUniqueHands * kSimulationSize)
                  << "%" << std::endl;
        t.Reset(std::cout << "iteration time: ") << std::endl;
      }
    }  // for simulations

    for (OCHS_Id k = 0; k < kOCHS_N; ++k) {
      ochs_preflop_lut(idx, k) /= sim_totals[k];
    }
  }  // for idx

  return ochs_preflop_lut;
}  // OCHS_PreflopLUT

}  // namespace fishbait
