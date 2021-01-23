// Copyright 2021 Marzuk Rashid

#ifndef SRC_HAND_STRENGTHS_LUT_GENERATORS_H_
#define SRC_HAND_STRENGTHS_LUT_GENERATORS_H_

#include <vector>
#include <iostream>

#include "hand_strengths/ochs.h"

namespace hand_strengths {

struct ShowdownStrength {
  double ehs = 0;
  double ochs[kOCHS_N] = {0, 0, 0, 0, 0, 0, 0, 0};

  template <class Archive>
  void serialize(Archive & ar) {
    ar(ehs, ochs);
  }
};

std::vector<ShowdownStrength> ShowdownLUT(bool verbose = false);

std::ostream& operator<<(std::ostream& os,
                         const std::vector<ShowdownStrength>& v);

}  // namespace hand_strengths

#endif  // SRC_HAND_STRENGTHS_LUT_GENERATORS_H_
