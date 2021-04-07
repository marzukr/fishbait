// Copyright 2021 Marzuk Rashid

#include <vector>
#include <fstream>
#include <iostream>

#include "hand_strengths/lut_generators.h"
#include "hand_strengths/card_utils.h"
#include "hand_strengths/indexer.h"

int main() {
  // Generate Showdown LUT
  // std::vector<hand_strengths::ShowdownStrength> showdown_lut =
  //     hand_strengths::ShowdownLUT(true);
  // hand_strengths::SaveLUT("luts/showdown_lut.cereal", &showdown_lut, true);

  // Load Showdown LUT
  // std::vector<hand_strengths::ShowdownStrength> showdown_lut;
  // hand_strengths::LoadLUT("luts/showdown_lut.cereal", &showdown_lut, true);

  // Generate Preflop LUT
  // utils::Matrix<uint32_t> preflop_lut = PreflopLUT(showdown_lut, true);
  // hand_strengths::SaveLUT("luts/preflop_lut.cereal", &preflop_lut, true);

  // Load Preflop LUT
  // utils::Matrix<uint32_t> preflop_lut(1, 1, 0);
  // hand_strengths::LoadLUT("luts/preflop_lut.cereal", &preflop_lut, true);

  // Test Preflop LUT
  // uint8_t c1 = hand_strengths::ISOCardFromStr("4s");
  // uint8_t c2 = hand_strengths::ISOCardFromStr("4h");
  // hand_strengths::Indexer handcalc(1, {2});
  // uint32_t index = handcalc.index({c1, c2});
  // std::cout << preflop_lut(index) << std::endl;

  // Generate Flop LUT
  // utils::Matrix<uint32_t> flop_lut = FlopLUT(showdown_lut, true);
  // hand_strengths::SaveLUT("luts/flop_lut.cereal", &flop_lut, true);

  // Load Flop LUT
  // utils::Matrix<uint32_t> flop_lut(1, 1, 0);
  // hand_strengths::LoadLUT("luts/flop_lut.cereal", &flop_lut, true);

  // Test Flop LUT
  // std::cout << flop_lut(397600) << std::endl;

  // Generate Turn LUT
  // utils::Matrix<uint32_t> turn_lut = TurnLUT(showdown_lut, true);
  // hand_strengths::SaveLUT("luts/turn_lut.cereal", &turn_lut, true);

  // Load Turn LUT
  // utils::Matrix<uint32_t> turn_lut(1, 1, 0);
  // hand_strengths::LoadLUT("luts/turn_lut.cereal", &turn_lut, true);

  // Test Turn LUT
  // std::cout << turn_lut(970) << std::endl;

  // Generate River LUT
  // utils::Matrix<double> river_lut = RiverLUT(showdown_lut, true);
  // hand_strengths::SaveLUT("luts/river_lut.cereal", &river_lut, true);

  // Load River LUT
  // utils::Matrix<double> river_lut(1, 1, 0);
  // hand_strengths::LoadLUT("luts/river_lut.cereal", &river_lut, true);

  // Test River LUT
  // std::cout.precision(16);
  // std::cout << river_lut(567) << std::endl;
  // std::cout << river_lut(84729) << std::endl;
  // std::cout << river_lut(8372956) << std::endl;
  // std::cout << river_lut(74629159) << std::endl;
  // std::cout << river_lut(112294656) << std::endl;

  // Generate OCHS Preflop LUT
  // utils::Matrix<double> ochs_pflop_lut = OCHS_PreflopLUT(showdown_lut, true);
  // hand_strengths::SaveLUT(
  //     "luts/ochs_preflop_lut.cereal", &ochs_pflop_lut, true
  // );

  // Load OCHS Preflop LUT
  // utils::Matrix<double> ochs_pflop_lut(1, 1, 0);
  // hand_strengths::LoadLUT(
  //     "luts/ochs_preflop_lut.cereal", &ochs_pflop_lut, true
  // );

  // Test OCHS Preflop LUT
  // uint8_t c1 = hand_strengths::ISOCardFromStr("Ts");
  // uint8_t c2 = hand_strengths::ISOCardFromStr("Js");
  // hand_strengths::Indexer handcalc(1, {2});
  // uint32_t index = handcalc.index({c1, c2});
  // std::cout.precision(16);
  // std::cout << ochs_pflop_lut(index) << std::endl;

  return 0;
}
