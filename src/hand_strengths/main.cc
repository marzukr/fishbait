// Copyright 2021 Marzuk Rashid

#include <vector>
#include <fstream>

#include "cereal/types/vector.hpp"
#include "cereal/archives/portable_binary.hpp"
#include "hand_strengths/lut_generators.h"

int main() {
  // std::vector<hand_strengths::ShowdownStrength> showdown_lut =
  //     hand_strengths::ShowdownLUT(true);

  // std::ofstream os("luts/showdown_lut.cereal", std::ios::binary);
  // cereal::PortableBinaryOutputArchive archive(os);
  // archive(showdown_lut);

  std::ifstream ins("luts/showdown_lut.cereal", std::ios::binary);
  cereal::PortableBinaryInputArchive iarchive(ins);
  std::vector<hand_strengths::ShowdownStrength> showdown_lut;
  iarchive(showdown_lut);

  utils::Matrix<uint32_t> preflop_lut = PreflopLUT(showdown_lut, true);
  std::ofstream os("luts/preflop_lut.cereal", std::ios::binary);
  cereal::PortableBinaryOutputArchive archive(os);
  archive(preflop_lut);

  return 0;
}
