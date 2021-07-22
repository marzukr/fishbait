// Copyright 2021 Marzuk Rashid

#include "hand_strengths/ochs.h"

#include <cstdint>

#include "engine/card_combinations.h"
#include "engine/card_utils.h"
#include "engine/definitions.h"
#include "engine/indexer.h"
#include "hand_strengths/definitions.h"
#include "utils/combination_matrix.h"

namespace hand_strengths {

utils::CombinationMatrix<OCHS_Id> SKClusterLUT() {
  utils::CombinationMatrix<OCHS_Id> op_clusters(52, -1);
  engine::Indexer<2> handcalc;
  for (engine::ISO_Card i = 0; i < engine::kDeckSize; ++i) {
    for (engine::ISO_Card j = i + 1; j < engine::kDeckSize; ++j) {
      hand_index_t hand_idx = handcalc.IndexLast({engine::ConvertSKtoISO(i),
                                                  engine::ConvertSKtoISO(j)});
      OCHS_Id cluster = kOpClusters[hand_idx] - 1;
      op_clusters(i, j) = cluster;
    }
  }
  return op_clusters;
}

}  // namespace hand_strengths
