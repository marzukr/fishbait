// Copyright 2021 Marzuk Rashid

#include "hand_strengths/lut_generators.h"

#include <vector>
#include <algorithm>

#include "SKPokerEval/src/SevenEval.h"
#include "hand_strengths/ochs.h"
#include "hand_strengths/indexer.h"
#include "hand_strengths/card_combinations.h"
#include "hand_strengths/card_utils.h"
#include "utils/combination_matrix.h"
#include "utils/timer.h"

namespace hand_strengths {

std::vector<ShowdownStrength> ShowdownLUT() {
  utils::CombinationMatrix<uint8_t> op_clusters = SKClusterLUT();

  const uint32_t kNShowdowns = 123156254;
  const uint32_t kOneHandApprox = kNShowdowns / kUniqueHands;
  const uint32_t kNOpHands = 990;  // 45 choose 2

  std::vector<ShowdownStrength> showdown_lut(kNShowdowns);

  Indexer isocalc(2, {2, 5});
  std::array<uint8_t, 7> rollout;
  CardCombinations op_hands(2);

  utils::Timer t;
  for (uint32_t idx = 0; idx < kNShowdowns; ++idx) {
    isocalc(1, idx, &rollout);
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

    if (idx % kOneHandApprox == 0) {
      std::cout << 100.0 * idx / kNShowdowns << "%" << std::endl;
      t.StopAndReset(true);
    }
  }

  return showdown_lut;
}

}  // namespace hand_strengths
