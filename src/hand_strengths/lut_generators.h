// Copyright 2021 Marzuk Rashid

#ifndef SRC_HAND_STRENGTHS_LUT_GENERATORS_H_
#define SRC_HAND_STRENGTHS_LUT_GENERATORS_H_

#include <vector>

namespace hand_strengths {

struct ShowdownStrength {
  double ehs = 0;
  double ochs[8] = {0, 0, 0, 0, 0, 0, 0, 0};
};

std::vector<ShowdownStrength> ShowdownLUT();

}  // namespace hand_strengths

#endif  // SRC_HAND_STRENGTHS_LUT_GENERATORS_H_
