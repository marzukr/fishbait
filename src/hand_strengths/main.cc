// Copyright 2021 Marzuk Rashid

#include <vector>
#include <fstream>

#include "cereal/types/vector.hpp"
#include "cereal/archives/portable_binary.hpp"
#include "hand_strengths/lut_generators.h"

int main() {
  std::vector<hand_strengths::ShowdownStrength> showdown_lut =
      hand_strengths::ShowdownLUT(true);

  std::ofstream os("luts/showdown_lut.cereal", std::ios::binary);
  cereal::PortableBinaryOutputArchive archive(os);
  archive(showdown_lut);

  // std::ifstream ins("luts/showdown_lut.cereal", std::ios::binary);
  // cereal::PortableBinaryInputArchive iarchive(ins);
  // std::vector<hand_strengths::ShowdownStrength> showdown_lut;
  // std::cout << showdown_lut << std::endl;
  // iarchive(showdown_lut);
  // std::cout << showdown_lut << std::endl;

  // uint32_t correct_bin = rollout_strength * 100 / (100 / kBins);
  // correct_bin = std::min(kBins - 1, correct_bin);

  return 0;
}
