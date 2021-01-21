// Copyright 2021 Marzuk Rashid

#include "hand_strengths/ochs.h"

#include <cstdint>

#include "hand_strengths/indexer.h"
#include "hand_strengths/card_combinations.h"
#include "hand_strengths/card_utils.h"
#include "utils/combination_matrix.h"

namespace hand_strengths {

utils::CombinationMatrix<uint8_t> SKClusterLUT() {
  utils::CombinationMatrix<uint8_t> op_clusters(52, -1);
  Indexer handcalc(1, {2});
  for (uint8_t i = 0; i < kDeckSize; ++i) {
    for (uint8_t j = i + 1; j < kDeckSize; ++j) {
      uint32_t hand_idx = handcalc({ConvertSKtoISO(i), ConvertSKtoISO(j)});
      uint8_t cluster = kOpClusters[hand_idx] - 1;
      op_clusters(i, j) = cluster;
    }
  }
  return op_clusters;
}

}  // namespace hand_strengths
