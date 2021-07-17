// Copyright 2021 Marzuk Rashid

#ifndef SRC_HAND_STRENGTHS_DEFINITIONS_H_
#define SRC_HAND_STRENGTHS_DEFINITIONS_H_

#include <cstdint>

namespace hand_strengths {

using OCHS_Id = uint8_t;
using OCHS_N = OCHS_Id;
using OCHS_VectT = double;

constexpr OCHS_N kOCHS_N = 8;

struct ShowdownStrength {
  double ehs = 0;
  double ochs_wins[kOCHS_N] = {0, 0, 0, 0, 0, 0, 0, 0};
  uint32_t ochs_totals[kOCHS_N] = {0, 0, 0, 0, 0, 0, 0, 0};

  template <class Archive>
  void serialize(Archive& ar) {  // NOLINT(runtime/references)
    ar(ehs, ochs_wins, ochs_totals);
  }
};

using HistCount = uint32_t;
using HistBucketId = uint32_t;
using HistBucketN = HistBucketId;

}  // namespace hand_strengths

#endif  // SRC_HAND_STRENGTHS_DEFINITIONS_H_
