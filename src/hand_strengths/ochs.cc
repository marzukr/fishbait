// Copyright 2021 Marzuk Rashid

#include "hand_strengths/ochs.h"

#include <cstdint>

#include "deck/card_combinations.h"
#include "deck/card_utils.h"
#include "deck/definitions.h"
#include "deck/indexer.h"
#include "hand_strengths/definitions.h"
#include "utils/combination_matrix.h"

namespace hand_strengths {

utils::CombinationMatrix<OCHS_Id> SKClusterLUT() {
  utils::CombinationMatrix<OCHS_Id> op_clusters(52, -1);
  deck::Indexer handcalc(1, {2});
  for (deck::ISO_Card i = 0; i < deck::kDeckSize; ++i) {
    for (deck::ISO_Card j = i + 1; j < deck::kDeckSize; ++j) {
      hand_index_t hand_idx = handcalc.index({deck::ConvertSKtoISO(i),
                                              deck::ConvertSKtoISO(j)});
      OCHS_Id cluster = kOpClusters[hand_idx] - 1;
      op_clusters(i, j) = cluster;
    }
  }
  return op_clusters;
}

}  // namespace hand_strengths
