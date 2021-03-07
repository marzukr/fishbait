// Copyright 2021 Marzuk Rashid

#ifndef SRC_HAND_STRENGTHS_LUT_GENERATORS_H_
#define SRC_HAND_STRENGTHS_LUT_GENERATORS_H_

#include <vector>
#include <iostream>

#include "hand_strengths/ochs.h"
#include "utils/matrix.h"

namespace hand_strengths {

struct ShowdownStrength {
  double ehs = 0;
  double ochs[kOCHS_N] = {0, 0, 0, 0, 0, 0, 0, 0};

  template <class Archive>
  void serialize(Archive& ar) {  // NOLINT(runtime/references)
    ar(ehs, ochs);
  }
};

std::vector<ShowdownStrength> ShowdownLUT(const bool verbose = false);

utils::Matrix<uint32_t> EHS_LUT(const uint32_t lut_size, const uint32_t buckets,
    const uint32_t simulation_size, const uint32_t lut_round,
    const std::vector<ShowdownStrength>& showdown_lut,
    const bool verbose = false);

utils::Matrix<uint32_t> PreflopLUT(
    const std::vector<ShowdownStrength>& showdown_lut,
    const bool verbose = false);

utils::Matrix<uint32_t> FlopLUT(
    const std::vector<ShowdownStrength>& showdown_lut,
    const bool verbose = false);

utils::Matrix<uint32_t> TurnLUT(
    const std::vector<ShowdownStrength>& showdown_lut,
    const bool verbose = false);

std::ostream& operator<<(std::ostream& os,
                         const std::vector<ShowdownStrength>& v);

}  // namespace hand_strengths

#endif  // SRC_HAND_STRENGTHS_LUT_GENERATORS_H_
