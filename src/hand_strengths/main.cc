// Copyright 2021 Marzuk Rashid

#include <cstring>
#include <fstream>
#include <iostream>
#include <vector>

#include "array/array.h"
#include "array/matrix.h"
#include "deck/card_utils.h"
#include "deck/indexer.h"
#include "hand_strengths/definitions.h"
#include "hand_strengths/lut_files.h"
#include "hand_strengths/lut_generators.h"
#include "hand_strengths/ochs.h"
#include "utils/cereal.h"

int main(int argc, char *argv[]) {
  bool error = true;
  if (argc == 1 + 1) {
    if (!strcmp(argv[1], "showdown")) {
      // Generate Showdown LUT
      std::vector<hand_strengths::ShowdownStrength> showdown_lut =
          hand_strengths::ShowdownLUT(true);
      utils::CerealSave("out/hand_strengths/showdown_lut_vector.cereal",
                        &showdown_lut, true);
      return 0;
    } else if (!strcmp(argv[1], "preflop") || !strcmp(argv[1], "flop")
        || !strcmp(argv[1], "turn") || !strcmp(argv[1], "river")
        || !strcmp(argv[1], "ochs_preflop")) {
      error = false;
    }
  }

  // Print usage on invalid input
  if (error) {
    std::cout << "Usage: hand_strengths.out (showdown | preflop | flop | turn"
      " | river | ochs_preflop)" << std::endl;
    return 1;
  }

  // Load Showdown LUT
  std::vector<hand_strengths::ShowdownStrength> showdown_lut;
  hand_strengths::ShowdownLUT_File(utils::FileAction::Load, &showdown_lut,
                                   true);

  if (!strcmp(argv[1], "preflop")) {
    // Generate preflop LUT
    nda::matrix<hand_strengths::HistCount> preflop_lut =
        PreflopLUT(showdown_lut, true);
    hand_strengths::PreflopLUT_File(utils::FileAction::Save, &preflop_lut,
                                    true);
  } else if (!strcmp(argv[1], "flop")) {
    // Generate Flop LUT
    nda::matrix<hand_strengths::HistCount> flop_lut =
        FlopLUT(showdown_lut, true);
    hand_strengths::FlopLUT_File(utils::FileAction::Save, &flop_lut, true);
  } else if (!strcmp(argv[1], "turn")) {
    // Generate Turn LUT
    nda::matrix<hand_strengths::HistCount> turn_lut =
        TurnLUT(showdown_lut, true);
    hand_strengths::TurnLUT_File(utils::FileAction::Save, &turn_lut, true);
  } else if (!strcmp(argv[1], "river")) {
    // Generate River LUT
    nda::matrix<double> river_lut = RiverLUT(showdown_lut, true);
    hand_strengths::RiverLUT_File(utils::FileAction::Save, &river_lut, true);
  } else if (!strcmp(argv[1], "ochs_preflop")) {
    // Generate OCHS Preflop LUT
    nda::matrix<double> ochs_pflop_lut = OCHS_PreflopLUT(showdown_lut, true);
    hand_strengths::OCHS_PreflopLUT_File(utils::FileAction::Save,
                                         &ochs_pflop_lut, true);
  }
  return 0;
}
