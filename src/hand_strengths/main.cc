// Copyright 2021 Marzuk Rashid

#include <vector>
#include <fstream>
#include <iostream>
#include <cstring>

#include "hand_strengths/lut_generators.h"
#include "hand_strengths/card_utils.h"
#include "hand_strengths/indexer.h"
#include "utils/cereal.h"
#include "utils/matrix.h"

int main(int argc, char *argv[]) {
  bool error = true;
  if (argc == 1 + 1) {
    if (!strcmp(argv[1], "showdown")) {
      // Generate Showdown LUT
      std::vector<hand_strengths::ShowdownStrength> showdown_lut =
          hand_strengths::ShowdownLUT(true);
      utils::CerealSave("luts/showdown_lut_64.cereal", &showdown_lut, true);
      return 0;
    } else if (!strcmp(argv[1], "preflop") | !strcmp(argv[1], "flop")
        | !strcmp(argv[1], "turn") | !strcmp(argv[1], "river")
        | !strcmp(argv[1], "ochs_preflop")) {
      error = false;
    }
  }

  // Print usage on invalid input
  if (error) {
    std::cout << "Usage: hand_strengths.out (showdown | preflop | flop | turn"
      " | river | ochs_preflop)" << std::endl;
    return -1;
  }

  // Load Showdown LUT
  std::vector<hand_strengths::ShowdownStrength> showdown_lut;
  utils::CerealLoad("luts/showdown_lut_64.cereal", &showdown_lut, true);

  if (!strcmp(argv[1], "preflop")) {
    // Generate preflop LUT
    utils::Matrix<uint32_t> preflop_lut = PreflopLUT(showdown_lut, true);
    utils::CerealSave("luts/preflop_lut_64.cereal", &preflop_lut, true);
  } else if (!strcmp(argv[1], "flop")) {
    // Generate Flop LUT
    utils::Matrix<uint32_t> flop_lut = FlopLUT(showdown_lut, true);
    utils::CerealSave("luts/flop_lut_64.cereal", &flop_lut, true);
  } else if (!strcmp(argv[1], "turn")) {
    // Generate Turn LUT
    utils::Matrix<uint32_t> turn_lut = TurnLUT(showdown_lut, true);
    utils::CerealSave("luts/turn_lut_64.cereal", &turn_lut, true);
  } else if (!strcmp(argv[1], "river")) {
    // Generate River LUT
    utils::Matrix<double> river_lut = RiverLUT(showdown_lut, true);
    utils::CerealSave("luts/river_lut_64.cereal", &river_lut, true);
  } else if (!strcmp(argv[1], "ochs_preflop")) {
    // Generate OCHS Preflop LUT
    utils::Matrix<double> ochs_pflop_lut = OCHS_PreflopLUT(showdown_lut, true);
    utils::CerealSave(
        "luts/ochs_preflop_lut_64.cereal", &ochs_pflop_lut, true);
  }
  return 0;
}
