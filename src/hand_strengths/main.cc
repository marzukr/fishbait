// Copyright 2021 Marzuk Rashid

#include <vector>
#include <fstream>
#include <iostream>

#include "cereal/types/vector.hpp"
#include "cereal/archives/portable_binary.hpp"
#include "hand_strengths/lut_generators.h"
#include "hand_strengths/card_utils.h"
#include "hand_strengths/indexer.h"

int main() {
  // Generate Showdown LUT
  // std::cout << "Generating Showdown LUT..." << std::endl;
  // std::vector<hand_strengths::ShowdownStrength> showdown_lut =
  //     hand_strengths::ShowdownLUT(true);
  // std::ofstream os("luts/showdown_lut.cereal", std::ios::binary);
  // cereal::PortableBinaryOutputArchive archive(os);
  // archive(showdown_lut);

  // Load Showdown LUT
  // std::cout << "Loading Showdown LUT..." << std::endl;
  // std::ifstream ins("luts/showdown_lut.cereal", std::ios::binary);
  // cereal::PortableBinaryInputArchive iarchive(ins);
  // std::vector<hand_strengths::ShowdownStrength> showdown_lut;
  // iarchive(showdown_lut);

  // Generate Preflop LUT
  // std::cout << "Generating Preflop LUT..." << std::endl;
  // utils::Matrix<uint32_t> preflop_lut = PreflopLUT(showdown_lut, true);
  // std::ofstream os("luts/preflop_lut.cereal", std::ios::binary);
  // cereal::PortableBinaryOutputArchive archive(os);
  // archive(preflop_lut);

  // Load Preflop LUT
  // std::cout << "Loading Preflop LUT..." << std::endl;
  // std::ifstream ins("luts/preflop_lut.cereal", std::ios::binary);
  // cereal::PortableBinaryInputArchive iarchive(ins);
  // utils::Matrix<uint32_t> preflop_lut(1, 1, 0);
  // iarchive(preflop_lut);

  // Test Preflop LUT
  // uint8_t c1 = hand_strengths::ISOCardFromStr("Qs");
  // uint8_t c2 = hand_strengths::ISOCardFromStr("Ks");
  // hand_strengths::Indexer handcalc(1, {2});
  // uint32_t index = handcalc.index({c1, c2});
  // std::cout << preflop_lut(index) << std::endl;

  // Generate Flop LUT
  // std::cout << "Generating Flop LUT..." << std::endl;
  // utils::Matrix<uint32_t> flop_lut = FlopLUT(showdown_lut, true);
  // std::ofstream os("luts/flop_lut.cereal", std::ios::binary);
  // cereal::PortableBinaryOutputArchive archive(os);
  // archive(flop_lut);

  // Load Flop LUT
  // std::cout << "Loading Flop LUT..." << std::endl;
  // std::ifstream ins("luts/flop_lut.cereal", std::ios::binary);
  // cereal::PortableBinaryInputArchive iarchive(ins);
  // utils::Matrix<uint32_t> flop_lut(1, 1, 0);
  // iarchive(flop_lut);

  // Test Flop LUT
  // std::cout << flop_lut(302847) << std::endl;

  // Generate Turn LUT
  // std::cout << "Generating Turn LUT..." << std::endl;
  // utils::Matrix<uint32_t> turn_lut = TurnLUT(showdown_lut, true);
  // std::ofstream os("luts/turn_lut.cereal", std::ios::binary);
  // cereal::PortableBinaryOutputArchive archive(os);
  // archive(turn_lut);

  // Load Turn LUT
  // std::cout << "Loading Turn LUT..." << std::endl;
  // std::ifstream ins("luts/turn_lut.cereal", std::ios::binary);
  // cereal::PortableBinaryInputArchive iarchive(ins);
  // utils::Matrix<uint32_t> turn_lut(1, 1, 0);
  // iarchive(turn_lut);

  // Test Turn LUT
  // std::cout << turn_lut(12867386) << std::endl;

  // Generate River LUT
  // std::cout << "Generating River LUT..." << std::endl;
  // utils::Matrix<double> river_lut = RiverLUT(showdown_lut, true);
  // std::ofstream os("luts/river_lut.cereal", std::ios::binary);
  // cereal::PortableBinaryOutputArchive archive(os);
  // archive(river_lut);

  // Load River LUT
  // std::cout << "Loading River LUT..." << std::endl;
  // std::ifstream ins("luts/river_lut.cereal", std::ios::binary);
  // cereal::PortableBinaryInputArchive iarchive(ins);
  // utils::Matrix<double> river_lut(1, 1, 0);
  // iarchive(river_lut);

  // Test River LUT
  // std::cout.precision(16);
  // std::cout << river_lut(567) << std::endl;
  // std::cout << river_lut(84729) << std::endl;
  // std::cout << river_lut(8372956) << std::endl;
  // std::cout << river_lut(74629159) << std::endl;
  // std::cout << river_lut(112294656) << std::endl;

  // Generate OCHS Preflop LUT
  // std::cout << "Generating OCHS Preflop LUT..." << std::endl;
  // utils::Matrix<double> ochs_pflop_lut = OCHS_PreflopLUT(showdown_lut, true);
  // std::ofstream os("luts/ochs_preflop_lut.cereal", std::ios::binary);
  // cereal::PortableBinaryOutputArchive archive(os);
  // archive(ochs_pflop_lut);

  // Load OCHS Preflop LUT
  std::cout << "Loading OCHS Preflop LUT..." << std::endl;
  std::ifstream ins("luts/ochs_preflop_lut.cereal", std::ios::binary);
  cereal::PortableBinaryInputArchive iarchive(ins);
  utils::Matrix<double> ochs_pflop_lut(1, 1, 0);
  iarchive(ochs_pflop_lut);

  // Test OCHS Preflop LUT
  uint8_t c1 = hand_strengths::ISOCardFromStr("Ts");
  uint8_t c2 = hand_strengths::ISOCardFromStr("Js");
  hand_strengths::Indexer handcalc(1, {2});
  uint32_t index = handcalc.index({c1, c2});
  std::cout.precision(16);
  std::cout << ochs_pflop_lut(index) << std::endl;

  return 0;
}
