// Copyright 2021 Marzuk Rashid

#include "hand_strengths/lut_generators.h"

// #include <omp.h>

#include <vector>
#include <algorithm>
#include <ostream>
#include <iostream>

#include "SKPokerEval/src/SevenEval.h"
#include "hand_strengths/ochs.h"
#include "hand_strengths/indexer.h"
#include "hand_strengths/card_combinations.h"
#include "hand_strengths/card_utils.h"
#include "utils/combination_matrix.h"
#include "utils/matrix.h"
#include "utils/timer.h"

namespace hand_strengths {

std::vector<ShowdownStrength> ShowdownLUT(const bool verbose) {
  utils::CombinationMatrix<uint8_t> op_clusters = SKClusterLUT();

  const uint32_t kNShowdowns = 123156254;
  const uint32_t kOneHandApprox = kNShowdowns / kUniqueHands;
  const uint32_t kNOpHands = 990;  // 45 choose 2

  std::vector<ShowdownStrength> showdown_lut(kNShowdowns);

  Indexer isocalc(2, {2, 5});
  std::array<uint8_t, 7> rollout;
  CardCombinations op_hands(2);

  uint32_t sd_count = 0;
  utils::Timer t;
  // #pragma omp parallel for firstprivate(rollout, op_hands)
  for (uint32_t idx = 0; idx < kNShowdowns; ++idx) {
    isocalc.unindex(1, idx, &rollout);
    std::transform(rollout.begin(), rollout.end(), rollout.begin(),
                   ConvertISOtoSK);

    for (op_hands.Reset(rollout); !op_hands.is_done(); ++op_hands) {
      uint16_t hero_rank = SevenEval::GetRank(rollout[0], rollout[1],
          rollout[2], rollout[3], rollout[4], rollout[5], rollout[6]);
      uint16_t villan_rank = SevenEval::GetRank(op_hands(0), op_hands(1),
          rollout[2], rollout[3], rollout[4], rollout[5], rollout[6]);

      double win_value = 0;
      if (hero_rank > villan_rank) {
        win_value = 1;
      } else if (hero_rank == villan_rank) {
        win_value = 0.5;
      }

      uint8_t cluster = op_clusters(op_hands(0), op_hands(1));
      showdown_lut[idx].ehs += win_value;
      showdown_lut[idx].ochs[cluster] += win_value;
    }

    showdown_lut[idx].ehs /= kNOpHands;
    std::for_each(showdown_lut[idx].ochs, showdown_lut[idx].ochs + kOCHS_N,
        [](double& a) { a /= kNOpHands; });

    sd_count += 1;
    if (verbose && sd_count % kOneHandApprox == 0) {
      std::cout << 100.0 * sd_count / kNShowdowns << "%" << std::endl;
      t.StopAndReset(true);
    }
  }

  return showdown_lut;
}

/*
  @brief Generic EHS LUT generating function.

  @param lut_size The number of rows in the LUT to generate.
  @param buckets The number of buckets per row in the LUT to generate.
  @param simulation_cards The number of cards to simulate per row in the LUT.
  @param iso_round The isocalc round that the LUT should generate rows from.
  @param isocalc The Indexer object to generate rows from.
  @param showdown_lut The showdown LUT to use.
  @param verbose Option to print progress.
*/
utils::Matrix<uint32_t> EHS_LUT(const uint32_t lut_size, const uint32_t buckets,
    const uint32_t simulation_cards, const uint32_t iso_round, Indexer* isocalc,
    const std::vector<ShowdownStrength>& showdown_lut, const bool verbose) {

  const uint32_t simulation_size = CardCombinations::N_Choose_K(
      52 - (7 - simulation_cards), simulation_cards);
  const uint32_t one_percent_approx = lut_size * simulation_size / 100;
  const double bucket_size = 1.0 / buckets;

  utils::Matrix<uint32_t> ehs_lut(lut_size, buckets, 0);

  Indexer showdown_calc(2, {2, 5});
  // Indexer isocalc(4, {2, 3, 1, 1});
  std::array<uint8_t, 7> rollout;
  std::array<uint64_t, 2> indicies;
  CardCombinations simulations(simulation_cards);

  uint32_t sd_count = 0;
  utils::Timer t;
  for (uint32_t idx = 0; idx < lut_size; ++idx) {
    isocalc->unindex(iso_round, idx, &rollout);
    uint32_t not_simulated = 7 - simulation_cards;
    utils::VectorView<uint8_t> hand(rollout.data(), not_simulated);

    for (simulations.Reset(hand); !simulations.is_done(); ++simulations) {
      for (uint32_t j = 0; j < simulation_cards; ++j) {
        rollout[not_simulated + j] = simulations(j);
      }

      // Get the appropriate showdown LUT index
      showdown_calc.index(rollout, &indicies);

      uint32_t bucket_unbounded = showdown_lut[indicies[1]].ehs / bucket_size;
      uint32_t bucket = std::min(bucket_unbounded, buckets - 1);

      ehs_lut(idx, bucket) += 1;

      sd_count += 1;
      if (verbose && sd_count % one_percent_approx == 0) {
        std::cout << 100.0 * sd_count / (lut_size * simulation_size)
                  << "%" << std::endl;
        t.StopAndReset(true);
      }
    }
  }

  return ehs_lut;
}

utils::Matrix<uint32_t> PreflopLUT(
    const std::vector<ShowdownStrength>& showdown_lut, const bool verbose) {
  const uint32_t lut_size = kUniqueHands;
  const uint32_t buckets = 50;
  const uint32_t simulation_size = 5;
  const uint32_t iso_round = 0;
  Indexer isocalc(1, {2});
  return EHS_LUT(lut_size, buckets, simulation_size, iso_round, &isocalc,
                 showdown_lut, verbose);
}

utils::Matrix<uint32_t> FlopLUT(
    const std::vector<ShowdownStrength>& showdown_lut, const bool verbose) {
  const uint32_t lut_size = 1286792;
  const uint32_t buckets = 50;
  const uint32_t simulation_size = 2;
  const uint32_t iso_round = 1;
  Indexer isocalc(2, {2, 3});
  return EHS_LUT(lut_size, buckets, simulation_size, iso_round, &isocalc,
                 showdown_lut, verbose);
}

utils::Matrix<uint32_t> TurnLUT(
    const std::vector<ShowdownStrength>& showdown_lut, const bool verbose) {
  const uint32_t lut_size = 13960050;
  const uint32_t buckets = 50;
  const uint32_t simulation_size = 1;
  const uint32_t iso_round = 1;
  Indexer isocalc(2, {2, 4});
  return EHS_LUT(lut_size, buckets, simulation_size, iso_round, &isocalc,
                 showdown_lut, verbose);
}

std::ostream& operator<<(std::ostream& os,
                         const std::vector<ShowdownStrength>& v) {
  os << "[";
  for (uint32_t i = 0; i < v.size(); ++i) {
    os << "{ehs: " << v[i].ehs << ", ";
    os << "ochs: [" << v[i].ochs[0];
    for (uint8_t j = 1; j < kOCHS_N; ++j) {
      os << ", " << v[i].ochs[j];
    }
    os << "]}";
    if (i < v.size() - 1) {
      os << ", ";
    }
  }
  os << "]";
  return os;
}

}  // namespace hand_strengths
