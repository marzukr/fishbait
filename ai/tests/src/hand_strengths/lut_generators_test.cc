#include <array>

#include "catch2/catch.hpp"

#include "array/array.h"
#include "array/matrix.h"
#include "hand_strengths/definitions.h"
#include "hand_strengths/lut_files.h"
#include "poker/card_utils.h"
#include "poker/definitions.h"
#include "poker/indexer.h"
#include "utils/cereal.h"

TEST_CASE("Preflop LUT", "[.][hand_strengths][lut_generators]") {
  // Load Preflop LUT
  nda::matrix<fishbait::HistCount> preflop_lut({1, 1}, 0);
  fishbait::PreflopLUT_File(fishbait::FileAction::Load, &preflop_lut, true);

  // Test Preflop LUT
  fishbait::ISO_Card c1;
  fishbait::ISO_Card c2;
  hand_index_t index;
  fishbait::Indexer<2> handcalc;

  // Test 4s4h
  std::array<fishbait::HistCount, 50> python{328, 19856, 1944, 7464, 702, 7410,
      108, 8172, 1398, 7782, 28428, 360, 5712, 39062, 42672, 19236, 18474,
      48012, 106676, 40744, 29168, 38374, 117208, 196468, 178518, 103474,
      104212, 31482, 78090, 87866, 78948, 116550, 4122, 58378, 14544, 17318,
      24216, 16228, 26888, 9504, 19524, 7740, 0, 1120, 10614, 39838, 50670,
      65002, 108000, 80156};
  c1 = fishbait::ISOCardFromStr("4s");
  c2 = fishbait::ISOCardFromStr("4h");
  index = handcalc.IndexLast({c1, c2});
  REQUIRE(preflop_lut(index, nda::all) ==
          nda::vector_ref<fishbait::HistCount>{python});

  // Test 6s6h
  python = {0, 8, 0, 1088, 396, 20916, 218, 4374, 1458, 1074, 8028, 5736, 1998,
      15214, 19980, 16986, 8274, 17040, 30638, 38186, 13504, 27280, 44236,
      92556, 97794, 85150, 121220, 119406, 115818, 111074, 124568, 209692,
      50130, 64526, 22200, 45678, 39840, 67686, 17428, 51950, 23358, 18196, 0,
      480, 6892, 50132, 37722, 69878, 80232, 118522};
  c1 = fishbait::ISOCardFromStr("6s");
  c2 = fishbait::ISOCardFromStr("6h");
  index = handcalc.IndexLast({c1, c2});
  REQUIRE(preflop_lut(index, nda::all) ==
          nda::vector_ref<fishbait::HistCount>{python});

  // Test KsQs
  python = {0, 0, 0, 1080, 1845, 7875, 19170, 3708, 11070, 61014, 6885, 51120,
      58488, 50721, 19953, 16077, 37530, 85947, 187392, 59547, 5112, 9528,
      13635, 63438, 67965, 66337, 17544, 9612, 11961, 9618, 16312, 8688, 9488,
      9837, 12663, 30690, 32598, 48042, 48999, 74067, 81702, 84816, 138372,
      107064, 87969, 33836, 45250, 50607, 114084, 129504};
  c1 = fishbait::ISOCardFromStr("Ks");
  c2 = fishbait::ISOCardFromStr("Qs");
  index = handcalc.IndexLast({c1, c2});
  REQUIRE(preflop_lut(index, nda::all) ==
          nda::vector_ref<fishbait::HistCount>{python});

  // Test TsJs
  python = {360, 8418, 11325, 24621, 14940, 36849, 31383, 59721, 64326, 74367,
      3798, 36702, 97905, 131505, 95253, 5646, 30808, 50362, 24819, 43082, 9764,
      7206, 10869, 15194, 2759, 7868, 6384, 1575, 7341, 10707, 23916, 23415,
      39225, 67725, 59985, 56700, 112224, 58794, 84012, 43724, 54996, 52503,
      40155, 34604, 41670, 33057, 66873, 81042, 79584, 138699};
  c1 = fishbait::ISOCardFromStr("Ts");
  c2 = fishbait::ISOCardFromStr("Js");
  index = handcalc.IndexLast({c1, c2});
  REQUIRE(preflop_lut(index, nda::all) ==
          nda::vector_ref<fishbait::HistCount>{python});
}  // TEST_CASE "Preflop LUT"

TEST_CASE("Flop LUT", "[.][hand_strengths][lut_generators]") {
  // Load Flop LUT
  nda::matrix<fishbait::HistCount> flop_lut({1, 1}, 0);
  fishbait::FlopLUT_File(fishbait::FileAction::Load, &flop_lut, true);

  // Test Flop LUT
  std::array<fishbait::HistCount, 50> python{0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1081};
  REQUIRE(flop_lut(302847, nda::all) ==
          nda::vector_ref<fishbait::HistCount>{python});

  python = {0, 0, 0, 0, 0, 0, 0, 0, 27, 0, 0, 36, 18, 0, 0, 99, 36, 36, 0, 0,
      60, 60, 96, 24, 0, 0, 0, 0, 6, 12, 0, 0, 0, 18, 27, 42, 30, 42, 0, 3, 0,
      54, 39, 162, 35, 103, 1, 15, 0, 0};
  REQUIRE(flop_lut(932810, nda::all) ==
          nda::vector_ref<fishbait::HistCount>{python});

  python = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 7, 4,
      5, 0, 6, 24, 5, 8, 0, 4, 5, 0, 42, 47, 16, 40, 29, 47, 88, 104, 152, 133,
      95, 125, 8, 30, 21, 6, 29};
  REQUIRE(flop_lut(46123, nda::all) ==
          nda::vector_ref<fishbait::HistCount>{python});

  python = {0, 1, 1, 20, 7, 36, 42, 73, 37, 29, 6, 52, 150, 135, 74, 0, 20, 33,
      22, 27, 2, 5, 11, 7, 0, 0, 0, 0, 1, 0, 0, 7, 17, 18, 23, 43, 44, 40, 21,
      0, 0, 7, 0, 15, 0, 4, 4, 21, 2, 24};
  REQUIRE(flop_lut(397600, nda::all) ==
          nda::vector_ref<fishbait::HistCount>{python});
}  // TEST_CASE "Flop LUT"

TEST_CASE("Turn LUT", "[.][hand_strengths][lut_generators]") {
  // Load Turn LUT
  nda::matrix<fishbait::HistCount> turn_lut({1, 1}, 0);
  fishbait::TurnLUT_File(fishbait::FileAction::Load, &turn_lut, true);

  // Test Turn LUT
  std::array<fishbait::HistCount, 50> python{0, 0, 0, 0, 0, 0, 4, 4, 0, 0,
      8, 12, 0, 0, 0, 3, 6, 0, 0, 0, 1, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 2, 2, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  REQUIRE(turn_lut(970, nda::all) ==
          nda::vector_ref<fishbait::HistCount>{python});

  python = {29, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0,
      0, 0, 2, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 0, 4, 0,
      0, 0, 0};
  REQUIRE(turn_lut(7563, nda::all) ==
          nda::vector_ref<fishbait::HistCount>{python});

  python = {0, 0, 0, 0, 0, 0, 6, 2, 0, 2, 8, 6, 0, 0, 0, 6, 10, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 1, 0, 0, 2, 0, 0, 1, 0, 0, 0, 0,
      0, 0, 0};
  REQUIRE(turn_lut(48392, nda::all) ==
          nda::vector_ref<fishbait::HistCount>{python});

  python = {0, 30, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 0, 0, 0, 0, 1, 1, 1, 1, 1, 4,
      0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0};
  REQUIRE(turn_lut(647295, nda::all) ==
          nda::vector_ref<fishbait::HistCount>{python});

  python = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 8, 8, 0, 4, 18, 0, 0, 0, 0, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      2, 0, 0};
  REQUIRE(turn_lut(3847299, nda::all) ==
          nda::vector_ref<fishbait::HistCount>{python});

  python = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 2, 0, 0, 0,
      0, 0, 0, 0, 0, 1, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 11, 23, 0, 0,
      0, 0, 0};
  REQUIRE(turn_lut(12867386, nda::all) ==
          nda::vector_ref<fishbait::HistCount>{python});
}  // TEST_CASE "Turn LUT"

TEST_CASE("River LUT", "[.][hand_strengths][lut_generators]") {
  // Load River LUT
  nda::matrix<double> river_lut({1, 1}, 0);
  fishbait::RiverLUT_File(fishbait::FileAction::Load, &river_lut, true);

  // Test River LUT
  std::array<double, 8> python{0.0871559633027523, 0, 0.4496124031007752,
      0.16551724137931034, 0.9, 0.36875, 0.8589743589743589, 0};
  REQUIRE(river_lut(567, nda::all) == nda::vector_ref<double>{python});

  python = {0.7720588235294118, 0.7105263157894737, 0.75, 0.9276315789473685,
      0.9327731092436975, 0.8427672955974843, 0.94, 0.8333333333333334};
  REQUIRE(river_lut(84729, nda::all) == nda::vector_ref<double>{python});

  python = {1, 1, 0.8666666666666667, 1, 0.9680851063829787, 0.5714285714285714,
      0.6078431372549019, 0};
  REQUIRE(river_lut(8372956, nda::all) == nda::vector_ref<double>{python});

  python = {0.7470588235294118, 0.7894736842105263, 0.9508196721311475,
      0.7006369426751592, 0.5841584158415841, 0.9808917197452229,
      0.7478991596638656, 0.1935483870967742};
  REQUIRE(river_lut(74629159, nda::all) == nda::vector_ref<double>{python});

  python = {0.6502463054187192, 0, 0.01694915254237288, 0.08630952380952381, 0,
      0.08041958041958042, 0, 0};
  REQUIRE(river_lut(112294656, nda::all) == nda::vector_ref<double>{python});
}  // TEST_CASE "River LUT"

TEST_CASE("OCHS preflop LUT", "[.][hand_strengths][lut_generators]") {
  // Load OCHS Preflop LUT
  nda::matrix<double> ochs_pflop_lut({1, 1}, 0);
  fishbait::OCHS_PreflopLUT_File(fishbait::FileAction::Load, &ochs_pflop_lut,
                                 true);

  // Test OCHS Preflop LUT
  fishbait::ISO_Card c1;
  fishbait::ISO_Card c2;
  hand_index_t index;
  fishbait::Indexer<2> handcalc;

  // Test 4s4h
  std::array<double, 8> python{0.7067924301854952, 0.63773542235149039,
      0.51629005043410425, 0.62033047285995946, 0.50607466814971436,
      0.57061730769146102, 0.49623908415211315, 0.18910648209329986};
  c1 = fishbait::ISOCardFromStr("4s");
  c2 = fishbait::ISOCardFromStr("4h");
  index = handcalc.IndexLast({c1, c2});
  REQUIRE(ochs_pflop_lut(index, nda::all) == nda::vector_ref<double>{python});

  // Test 6s6h
  python = {0.80545143648256701, 0.69675317583793528, 0.58721879993272219,
      0.67776445492975701, 0.52137624112798064, 0.65133747155571486,
      0.51928199966903676, 0.19143041856385981};
  c1 = fishbait::ISOCardFromStr("6s");
  c2 = fishbait::ISOCardFromStr("6h");
  index = handcalc.IndexLast({c1, c2});
  REQUIRE(ochs_pflop_lut(index, nda::all) == nda::vector_ref<double>{python});

  // Test KsQs
  python = {0.66592875635067472, 0.68572378956421143, 0.65743699894924057,
      0.72201515172127217, 0.69275740417212617, 0.55303989813124876,
      0.49228674983853477, 0.38410719805984606};
  c1 = fishbait::ISOCardFromStr("Ks");
  c2 = fishbait::ISOCardFromStr("Qs");
  index = handcalc.IndexLast({c1, c2});
  REQUIRE(ochs_pflop_lut(index, nda::all) == nda::vector_ref<double>{python});

  // Test TsJs
  python = {0.67918141757288086, 0.71507626721815865, 0.68650369512587017,
      0.55169020567956739, 0.54276122624837642, 0.48086173700940682,
      0.38734159963565767, 0.30993595958038606};
  c1 = fishbait::ISOCardFromStr("Ts");
  c2 = fishbait::ISOCardFromStr("Js");
  index = handcalc.IndexLast({c1, c2});
  REQUIRE(ochs_pflop_lut(index, nda::all) == nda::vector_ref<double>{python});
}  // TEST_CASE "OCHS preflop LUT"
