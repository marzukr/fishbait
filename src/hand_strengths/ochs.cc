// Copyright 2021 Marzuk Rashid

#include "hand_strengths/ochs.h"

#include <cstdint>

#include "deck/card_utils.h"
#include "deck/card_combinations.h"
#include "deck/constants.h"
#include "deck/indexer.h"
#include "utils/combination_matrix.h"

namespace hand_strengths {

utils::CombinationMatrix<uint8_t> SKClusterLUT() {
  utils::CombinationMatrix<uint8_t> op_clusters(52, -1);
  deck::Indexer handcalc(1, {2});
  for (uint8_t i = 0; i < deck::kDeckSize; ++i) {
    for (uint8_t j = i + 1; j < deck::kDeckSize; ++j) {
      uint32_t hand_idx = handcalc.index({deck::ConvertSKtoISO(i),
          deck::ConvertSKtoISO(j)});
      uint8_t cluster = kOpClusters[hand_idx] - 1;
      op_clusters(i, j) = cluster;
    }
  }
  return op_clusters;
}

}  // namespace hand_strengths
