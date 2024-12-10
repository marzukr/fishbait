#include <cstring>
#include <fstream>
#include <iostream>
#include <vector>

#include "array/array.h"
#include "array/matrix.h"
#include "hand_strengths/definitions.h"
#include "hand_strengths/lut_files.h"
#include "hand_strengths/lut_generators.h"
#include "hand_strengths/ochs.h"
#include "poker/card_utils.h"
#include "poker/indexer.h"
#include "utils/cereal.h"

int main(int argc, char *argv[]) {
  bool error = true;
  if (argc == 1 + 1) {
    if (!strcmp(argv[1], "showdown")) {
      // Generate Showdown LUT
      std::vector<fishbait::ShowdownStrength> showdown_lut =
          fishbait::ShowdownLUT(true);
      fishbait::CerealSave("out/ai/hand_strengths/showdown_lut_vector.cereal",
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
  std::vector<fishbait::ShowdownStrength> showdown_lut;
  fishbait::ShowdownLUT_File(fishbait::FileAction::Load, &showdown_lut, true);

  if (!strcmp(argv[1], "preflop")) {
    // Generate preflop LUT
    nda::matrix<fishbait::HistCount> preflop_lut =
        fishbait::PreflopLUT(showdown_lut, true);
    fishbait::PreflopLUT_File(fishbait::FileAction::Save, &preflop_lut, true);
  } else if (!strcmp(argv[1], "flop")) {
    // Generate Flop LUT
    nda::matrix<fishbait::HistCount> flop_lut =
        fishbait::FlopLUT(showdown_lut, true);
    fishbait::FlopLUT_File(fishbait::FileAction::Save, &flop_lut, true);
  } else if (!strcmp(argv[1], "turn")) {
    // Generate Turn LUT
    nda::matrix<fishbait::HistCount> turn_lut =
        fishbait::TurnLUT(showdown_lut, true);
    fishbait::TurnLUT_File(fishbait::FileAction::Save, &turn_lut, true);
  } else if (!strcmp(argv[1], "river")) {
    // Generate River LUT
    nda::matrix<double> river_lut = fishbait::RiverLUT(showdown_lut, true);
    fishbait::RiverLUT_File(fishbait::FileAction::Save, &river_lut, true);
  } else if (!strcmp(argv[1], "ochs_preflop")) {
    // Generate OCHS Preflop LUT
    nda::matrix<double> ochs_pflop_lut =
        fishbait::OCHS_PreflopLUT(showdown_lut, true);
    fishbait::OCHS_PreflopLUT_File(fishbait::FileAction::Save, &ochs_pflop_lut,
                                   true);
  }
  return 0;
}
