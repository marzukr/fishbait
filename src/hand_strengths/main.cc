// Copyright 2021 Marzuk Rashid

#include <vector>

#include "hand_strengths/lut_generators.h"

int main() {
  std::vector<hand_strengths::ShowdownStrength> showdown_lut =
      hand_strengths::ShowdownLUT();

  // uint32_t correct_bin = rollout_strength * 100 / (100 / kBins);
  // correct_bin = std::min(kBins - 1, correct_bin);

  return 0;
}
