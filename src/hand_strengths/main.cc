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

  // Debugging Code
  // hand_strengths::Indexer isocalc(2, {2,5});
  // std::array<uint8_t, 7> rollout = {48, 49, 42, 46, 47, 50, 51};
  // std::array<uint8_t, 7> rollout;
  // isocalc.unindex(1, 0, &rollout);
  // std::cout << +rollout[0] << "," << +rollout[1] << "," << +rollout[2] << ","
  //           << +rollout[3] << "," << +rollout[4] << "," << +rollout[5] << ","
  //           << +rollout[6] << std::endl;
  // std::cout << +(hand_strengths::ConvertISOtoSK(rollout[0])) << ","
  //           << +(hand_strengths::ConvertISOtoSK(rollout[1])) << ","
  //           << +(hand_strengths::ConvertISOtoSK(rollout[2])) << ","
  //           << +(hand_strengths::ConvertISOtoSK(rollout[3])) << ","
  //           << +(hand_strengths::ConvertISOtoSK(rollout[4])) << ","
  //           << +(hand_strengths::ConvertISOtoSK(rollout[5])) << ","
  //           << +(hand_strengths::ConvertISOtoSK(rollout[6])) << std::endl;
  // std::transform(rollout.begin(), rollout.end(), rollout.begin(),
  //                hand_strengths::ConvertISOtoSK);
  // std::cout << +rollout[0] << "," << +rollout[1] << "," << +rollout[2] << ","
  //           << +rollout[3] << "," << +rollout[4] << "," << +rollout[5] << ","
  //           << +rollout[6] << std::endl;
  // uint32_t sk_index = isocalc.index(rollout);
  // std::cout << sk_index << std::endl;
  // (48, 49)
  // (42, 46, 47, 50, 51)
  // std::cout.precision(17);
  // hand_strengths::ShowdownLUT(true);
  // std::cout << showdown_lut[0].ehs << std::endl;

  // Generate Preflop LUT
  // std::cout << "Generating Preflop LUT..." << std::endl;
  // utils::Matrix<uint32_t> preflop_lut = PreflopLUT(showdown_lut, true);
  // std::ofstream os("luts/preflop_lut.cereal", std::ios::binary);
  // cereal::PortableBinaryOutputArchive archive(os);
  // archive(preflop_lut);

  // Load Preflop LUT
  std::cout << "Loading Preflop LUT..." << std::endl;
  std::ifstream ins("luts/preflop_lut.cereal", std::ios::binary);
  cereal::PortableBinaryInputArchive iarchive(ins);
  utils::Matrix<uint32_t> preflop_lut(1, 1, 0);
  iarchive(preflop_lut);

  // Test Preflop LUT
  uint8_t c1 = hand_strengths::ISOCardFromStr("4s");
  uint8_t c2 = hand_strengths::ISOCardFromStr("4h");
  hand_strengths::Indexer handcalc(1, {2});
  uint32_t index = handcalc.index({c1, c2});
  std::cout << preflop_lut(index) << std::endl;

  return 0;
}
