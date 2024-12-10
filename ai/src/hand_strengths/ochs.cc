#include "hand_strengths/ochs.h"

#include <cstdint>

#include "hand_strengths/card_combinations.h"
#include "hand_strengths/definitions.h"
#include "poker/card_utils.h"
#include "poker/definitions.h"
#include "poker/indexer.h"
#include "utils/combination_matrix.h"

namespace fishbait {

CombinationMatrix<OCHS_Id> SKClusterLUT() {
  CombinationMatrix<OCHS_Id> op_clusters(52, -1);
  Indexer<2> handcalc;
  for (ISO_Card i = 0; i < kDeckSize; ++i) {
    for (ISO_Card j = i + 1; j < kDeckSize; ++j) {
      hand_index_t hand_idx = handcalc.IndexLast({ConvertSKtoISO(i),
                                                  ConvertSKtoISO(j)});
      OCHS_Id cluster = kOpClusters[hand_idx] - 1;
      op_clusters(i, j) = cluster;
    }
  }
  return op_clusters;
}

}  // namespace fishbait
