// Copyright 2021 Marzuk Rashid

#ifndef SRC_HAND_STRENGTHS_OCHS_H_
#define SRC_HAND_STRENGTHS_OCHS_H_

#include <cstdint>

#include "utils/combination_matrix.h"

namespace hand_strengths {

  const uint8_t kUniqueHands = 169;

  const uint32_t kOCHS_N = 8;

  // indexed by iso card numbers
  const uint8_t kOpClusters[kUniqueHands] = { 4, 1, 6, 1, 1, 6, 1, 1, 1, 6, 1,
      1, 1, 1, 7, 1, 1, 1, 2, 3, 7, 1, 1, 2, 2, 3, 3, 8, 2, 2, 2, 2, 3, 3, 3, 8,
      2, 2, 2, 2, 3, 3, 3, 5, 8, 2, 2, 4, 4, 4, 4, 5, 5, 5, 8, 4, 4, 4, 4, 4, 4,
      5, 5, 5, 5, 8, 4, 4, 4, 6, 6, 6, 6, 6, 7, 7, 7, 8, 6, 6, 6, 6, 6, 6, 6, 7,
      7, 7, 7, 7, 8, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3, 1, 1, 2, 3, 3, 2, 2, 2, 3, 3,
      3, 2, 2, 2, 3, 3, 3, 3, 2, 3, 3, 3, 3, 5, 5, 5, 4, 4, 4, 4, 4, 5, 5, 5, 5,
      4, 4, 4, 4, 5, 5, 5, 5, 7, 7, 4, 6, 6, 6, 6, 6, 6, 7, 7, 7, 7, 6, 6, 6, 6,
      6, 7, 7, 7, 7, 7, 7, 7 };

  // Given two SK card numbers, access the OCHS cluster number (zero indexed)
  utils::CombinationMatrix<uint8_t> SKClusterLUT();

}  // namespace hand_strengths

#endif  // SRC_HAND_STRENGTHS_OCHS_H_
