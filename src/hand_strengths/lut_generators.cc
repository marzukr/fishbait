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

std::vector<ShowdownStrength> ShowdownLUT(bool verbose) {
  utils::CombinationMatrix<uint8_t> op_clusters = SKClusterLUT();

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
    std::for_each(rollout.begin(), rollout.end(), ConvertISOtoSK);

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

utils::Matrix<uint32_t> PreflopLUT(
    const std::vector<ShowdownStrength>& showdown_lut, bool verbose) {
  const uint32_t kOneHandApprox = kNShowdowns / kUniqueHands;
  const uint32_t kNBuckets = 50;
  const double kBucketSize = 1.0 / kNBuckets;

  utils::Matrix<uint32_t> preflop_lut(kUniqueHands, kNBuckets, 0);

  Indexer isocalc(2, {2, 5});
  std::array<uint8_t, 7> rollout;
  std::array<uint64_t, 2> indicies;

  uint32_t sd_count = 0;
  utils::Timer t;
  for (uint32_t idx = 0; idx < kNShowdowns; ++idx) {
    isocalc.unindex(1, idx, &rollout);
    isocalc.index(rollout, &indicies);

    uint32_t bucket_unbounded = showdown_lut[idx].ehs / kBucketSize;
    uint32_t bucket = std::min(bucket_unbounded, kNBuckets - 1);

    preflop_lut(indicies[0], bucket) += 1;

    sd_count += 1;
    if (verbose && sd_count % kOneHandApprox == 0) {
      std::cout << 100.0 * sd_count / kNShowdowns << "%" << std::endl;
      t.StopAndReset(true);
    }
  }

  return preflop_lut;
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
