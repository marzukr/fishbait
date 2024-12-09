#ifndef AI_SRC_HAND_STRENGTHS_DEFINITIONS_H_
#define AI_SRC_HAND_STRENGTHS_DEFINITIONS_H_

#include <cstdint>
#include <string>

namespace fishbait {

using OCHS_Id = uint8_t;
using OCHS_N = OCHS_Id;

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

}  // namespace fishbait

#endif  // AI_SRC_HAND_STRENGTHS_DEFINITIONS_H_
