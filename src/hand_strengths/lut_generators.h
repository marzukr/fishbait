// Copyright 2021 Marzuk Rashid

#ifndef SRC_HAND_STRENGTHS_LUT_GENERATORS_H_
#define SRC_HAND_STRENGTHS_LUT_GENERATORS_H_

#include <vector>
#include <iostream>

#include "hand_strengths/ochs.h"
#include "utils/matrix.h"

namespace hand_strengths {

const uint32_t kNShowdowns = 123156254;

struct ShowdownStrength {
  double ehs = 0;
  double ochs_wins[kOCHS_N] = {0, 0, 0, 0, 0, 0, 0, 0};
  uint32_t ochs_totals[kOCHS_N] = {0, 0, 0, 0, 0, 0, 0, 0};

  template <class Archive>
  void serialize(Archive& ar) {  // NOLINT(runtime/references)
    ar(ehs, ochs_wins, ochs_totals);
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

utils::Matrix<double> RiverLUT(
    const std::vector<ShowdownStrength>& showdown_lut,
    const bool verbose = false);

std::ostream& operator<<(std::ostream& os,
                         const std::vector<ShowdownStrength>& v);

}  // namespace hand_strengths

#endif  // SRC_HAND_STRENGTHS_LUT_GENERATORS_H_
