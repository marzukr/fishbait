// Copyright 2021 Marzuk Rashid

#include "catch2/catch.hpp"
#include "utils/fraction.h"

TEST_CASE("single number fraction constructor", "[utils][fraction]") {
  // 0
  utils::Fraction f1{0};
  REQUIRE(f1.numerator() == 0);
  REQUIRE(f1.denominator() == 1);
  // positive
  utils::Fraction f2{7};
  REQUIRE(f2.numerator() == 7);
  REQUIRE(f2.denominator() == 1);
  // negative
  utils::Fraction f3{-99};
  REQUIRE(f3.numerator() == -99);
  REQUIRE(f3.denominator() == 1);
}  // TEST_CASE "single number fraction constructor"

TEST_CASE("fraction n/d constructor", "[utils][fraction]") {
  /* n positive, d positive */ {
    // reducable
    utils::Fraction f0{10, 50};
    REQUIRE(f0.numerator() == 1);
    REQUIRE(f0.denominator() == 5);
    // non reducable
    utils::Fraction f1{7, 11};
    REQUIRE(f1.numerator() == 7);
    REQUIRE(f1.denominator() == 11);
  }
  /* n positive, d negative */ {
    // reducable
    utils::Fraction f0{7, -49};
    REQUIRE(f0.numerator() == -1);
    REQUIRE(f0.denominator() == 7);
    // non reducable
    utils::Fraction f1{19, -2};
    REQUIRE(f1.numerator() == -19);
    REQUIRE(f1.denominator() == 2);
  }
  /* n negative, d positive */ {
    // reducable
    utils::Fraction f0{-50, 4};
    REQUIRE(f0.numerator() == -25);
    REQUIRE(f0.denominator() == 2);
    // non reducable
    utils::Fraction f1{-23, 11};
    REQUIRE(f1.numerator() == -23);
    REQUIRE(f1.denominator() == 11);
  }
  /* n negative, d negative */ {
    // reducable
    utils::Fraction f0{-8, -64};
    REQUIRE(f0.numerator() == 1);
    REQUIRE(f0.denominator() == 8);
    // non reducable
    utils::Fraction f1{-10, -11};
    REQUIRE(f1.numerator() == 10);
    REQUIRE(f1.denominator() == 11);
  }
  // d = 0 throws
  REQUIRE_THROWS(utils::Fraction{4, 0});
  // n = 0 d = 0 throws
  REQUIRE_THROWS(utils::Fraction{0, 0});
  // n = 0, d = -1 -> 0/1
  utils::Fraction f0{0, -1};
  REQUIRE(f0.numerator() == 0);
  REQUIRE(f0.denominator() == 1);
  // n = 0, d = 5 -> 0/1
  utils::Fraction f1{0, 5};
  REQUIRE(f1.numerator() == 0);
  REQUIRE(f1.denominator() == 1);
}  // TEST_CASE "fraction n/d constructor"

TEST_CASE("fraction copy constructor", "[utils][fraction]") {
  utils::Fraction f0{3, 9};
  utils::Fraction f1{f0};
  REQUIRE(f0.numerator() == 1);
  REQUIRE(f1.denominator() == 3);
}  // TEST_CASE "fraction copy constructor"

TEST_CASE("fraction default constructor", "[utils][fraction]") {
  utils::Fraction f0;
  REQUIRE(f0.numerator() == 0);
  REQUIRE(f0.denominator() == 1);
}  // TEST_CASE "fraction default constructor"

TEST_CASE("fraction inversion", "[utils][fraction]") {
  // 0
  utils::Fraction f0{0};
  REQUIRE_THROWS(f0.Invert());
  // 1
  utils::Fraction f1{1};
  f1.Invert();
  REQUIRE(f1.numerator() == 1);
  REQUIRE(f1.denominator() == 1);
  // -1
  utils::Fraction f2{-1};
  f2.Invert();
  REQUIRE(f2.numerator() == -1);
  REQUIRE(f2.denominator() == 1);
  // positive
  utils::Fraction f3{8, 9};
  f3.Invert();
  REQUIRE(f3.numerator() == 9);
  REQUIRE(f3.denominator() == 8);
  // negative
  utils::Fraction f4{-34, 78};
  f4.Invert();
  REQUIRE(f4.numerator() == -39);
  REQUIRE(f4.denominator() == 17);
}  // TEST_CASE "fraction inversion"

TEST_CASE("fraction plus equals fraction", "[utils][fraction]") {
  // positive positive
  utils::Fraction f0{1, 3};
  utils::Fraction f1{2, 3};
  f0 += f1;
  REQUIRE(f0.numerator() == 1);
  REQUIRE(f0.denominator() == 1);
  // positive negative
  utils::Fraction f2{7, 9};
  f2 += utils::Fraction{-5, 2};
  REQUIRE(f2.numerator() == -31);
  REQUIRE(f2.denominator() == 18);
  // negative positive
  utils::Fraction f3{-4, 9};
  f3 += utils::Fraction{78, 67};
  REQUIRE(f3.numerator() == 434);
  REQUIRE(f3.denominator() == 603);
  // negative negative
  utils::Fraction f4{-98, 101};
  f4 += utils::Fraction{-4, 3};
  REQUIRE(f4.numerator() == -698);
  REQUIRE(f4.denominator() == 303);
  // positive 0
  utils::Fraction f5{500, 200};
  f5 += utils::Fraction{0};
  REQUIRE(f5.numerator() == 5);
  REQUIRE(f5.denominator() == 2);
  // negative 0
  utils::Fraction f6{-2, 5};
  f6 += utils::Fraction{0};
  REQUIRE(f6.numerator() == -2);
  REQUIRE(f6.denominator() == 5);
  // 0 positive
  utils::Fraction f7;
  f7 += utils::Fraction{456, 678};
  REQUIRE(f7.numerator() == 76);
  REQUIRE(f7.denominator() == 113);
  // 0 negative
  utils::Fraction f8;
  f8 += utils::Fraction{-98261, 3746664};
  REQUIRE(f8.numerator() == -98261);
  REQUIRE(f8.denominator() == 3746664);
  // 0, 0
  utils::Fraction f9;
  f9 += utils::Fraction{0};
  REQUIRE(f9.numerator() == 0);
  REQUIRE(f9.denominator() == 1);
}  // TEST_CASE "fraction plus equals fraction"

TEST_CASE("fraction plus equals int", "[utils][fraction]") {
  // positive positive
  utils::Fraction f0{4};
  f0 += 5;
  REQUIRE(f0.numerator() == 9);
  REQUIRE(f0.denominator() == 1);
  // positive negative
  utils::Fraction f1{3, 4};
  f1 += -7;
  REQUIRE(f1.numerator() == -25);
  REQUIRE(f1.denominator() == 4);
  // negative positive
  utils::Fraction f2{-3324878, 2847289};
  f2 += 8472987;
  REQUIRE(f2.numerator() == 24125039357365);
  REQUIRE(f2.denominator() == 2847289);
  // negative negative
  utils::Fraction f3{-38274, 3827593};
  f3 += -9381;
  REQUIRE(f3.numerator() == -35906688207);
  REQUIRE(f3.denominator() == 3827593);
  // positive 0
  utils::Fraction f4{7836, 3871};
  f4 += 0;
  REQUIRE(f4.numerator() == 7836);
  REQUIRE(f4.denominator() == 3871);
  // negative 0
  utils::Fraction f5{-3829, 4718};
  f5 += 0;
  REQUIRE(f5.numerator() == -547);
  REQUIRE(f5.denominator() == 674);
  // 0 positive
  utils::Fraction f6;
  f6 += 4873;
  REQUIRE(f6.numerator() == 4873);
  REQUIRE(f6.denominator() == 1);
  // 0 negative
  utils::Fraction f7;
  f7 += -47832;
  REQUIRE(f7.numerator() == -47832);
  REQUIRE(f7.denominator() == 1);
  // 0, 0
  utils::Fraction f8;
  f8 += 0;
  REQUIRE(f8.numerator() == 0);
  REQUIRE(f8.denominator() == 1);
}  // TEST_CASE "fraction plus equals int"

TEST_CASE("int plus fraction", "[utils][fraction]") {
  // positive positive
  utils::Fraction f0{3, 4};
  f0 = 9 + f0;
  REQUIRE(f0.numerator() == 39);
  REQUIRE(f0.denominator() == 4);
  // positive negative
  utils::Fraction f1{-38371, 48282};
  f1 = 56 + f1;
  REQUIRE(f1.numerator() == 2665421);
  REQUIRE(f1.denominator() == 48282);
  // negative positive
  utils::Fraction f2{9, 8};
  f2 = -5 + f2;
  REQUIRE(f2.numerator() == -31);
  REQUIRE(f2.denominator() == 8);
  // negative negative
  utils::Fraction f3{-382, 38174};
  f3 = -56 + f3;
  REQUIRE(f3.numerator() == -1069063);
  REQUIRE(f3.denominator() == 19087);
  // positive 0
  utils::Fraction f4;
  f4 = 31938 + f4;
  REQUIRE(f4.numerator() == 31938);
  REQUIRE(f4.denominator() == 1);
  // negative 0
  utils::Fraction f5;
  f5 = -47257 + f5;
  REQUIRE(f5.numerator() == -47257);
  REQUIRE(f5.denominator() == 1);
  // 0 positive
  utils::Fraction f6{4, 5};
  f6 = 0 + f6;
  REQUIRE(f6.numerator() == 4);
  REQUIRE(f6.denominator() == 5);
  // 0 negative
  utils::Fraction f7{-7, 2};
  f7 = 0 + f7;
  REQUIRE(f7.numerator() == -7);
  REQUIRE(f7.denominator() == 2);
  // 0, 0
  utils::Fraction f8;
  f8 = 0 + f8;
  REQUIRE(f8.numerator() == 0);
  REQUIRE(f8.denominator() == 1);
}  // TEST_CASE "int plus fraction"

TEST_CASE("fraction plus int", "[utils][fraction]") {
  // positive positive
  utils::Fraction f0{6, 7};
  f0 = f0 + 10;
  REQUIRE(f0.numerator() == 76);
  REQUIRE(f0.denominator() == 7);
  // positive negative
  utils::Fraction f1{475892, 43782};
  f1 = f1 + -78;
  REQUIRE(f1.numerator() == -1469552);
  REQUIRE(f1.denominator() == 21891);
  // negative positive
  utils::Fraction f2{-4726, 4061};
  f2 = f2 + 1;
  REQUIRE(f2.numerator() == -665);
  REQUIRE(f2.denominator() == 4061);
  // negative negative
  utils::Fraction f3{-691, 478};
  f3 = f3 + -1;
  REQUIRE(f3.numerator() == -1169);
  REQUIRE(f3.denominator() == 478);
  // positive 0
  utils::Fraction f4{5, 4};
  f4 = f4 + 0;
  REQUIRE(f4.numerator() == 5);
  REQUIRE(f4.denominator() == 4);
  // negative 0
  utils::Fraction f5{-9, 8};
  f5 = f5 + 0;
  REQUIRE(f5.numerator() == -9);
  REQUIRE(f5.denominator() == 8);
  // 0 positive
  utils::Fraction f6{};
  f6 = f6 + 9999;
  REQUIRE(f6.numerator() == 9999);
  REQUIRE(f6.denominator() == 1);
  // 0 negative
  utils::Fraction f7{};
  f7 = f7 + -9;
  REQUIRE(f7.numerator() == -9);
  REQUIRE(f7.denominator() == 1);
  // 0, 0
  utils::Fraction f8{};
  f8 = f8 + 0;
  REQUIRE(f8.numerator() == 0);
  REQUIRE(f8.denominator() == 1);
}  // TEST_CASE "fraction plus int"

TEST_CASE("fraction plus double", "[utils][fraction]") {
  // positive positive
  utils::Fraction f0{4, 5};
  REQUIRE(f0 + 0.5 == Approx(1.3));
  // positive negative
  utils::Fraction f1{9, 12};
  REQUIRE(f1 + -4.5 == Approx(-3.75));
  // negative positive
  utils::Fraction f2{-92348, 3478};
  REQUIRE(f2 + 10.0 == Approx(-16.552));
  // negative negative
  utils::Fraction f3{-3478, 4893};
  REQUIRE(f3 + -9.0 == Approx(-9.71081));
  // positive 0
  utils::Fraction f4{8472, 48383};
  REQUIRE(f4 + 0.0 == Approx(0.175103));
  // negative 0
  utils::Fraction f5{-493, 8888};
  REQUIRE(f5 + 0.0 == Approx(-0.055468));
  // 0 positive
  utils::Fraction f6;
  REQUIRE(f6 + 2.4 == Approx(2.4));
  // 0 negative
  utils::Fraction f7;
  REQUIRE(f7 + -6.7 == Approx(-6.7));
  // 0, 0
  utils::Fraction f8;
  REQUIRE(f8 + 0.0 == Approx(0.0));
}  // TEST_CASE "fraction plus double"

TEST_CASE("double plus fraction", "[utils][fraction]") {
  // positive positive
  utils::Fraction f0{2, 3};
  REQUIRE(5.4 + f0 == Approx(6.06667));
  // positive negative
  utils::Fraction f1{-3, 2};
  REQUIRE(8.9 + f1 == Approx(7.4));
  // negative positive
  utils::Fraction f2{4328, 43782};
  REQUIRE(-99.5 + f2 == Approx(-99.4011));
  // negative negative
  utils::Fraction f3{-3489, 3478};
  REQUIRE(-56.4 + f3 == Approx(-57.4032));
  // positive 0
  utils::Fraction f4;
  REQUIRE(55.3 + f4 == Approx(55.3));
  // negative 0
  utils::Fraction f5;
  REQUIRE(-4378.3 + f5 == Approx(-4378.3));
  // 0 positive
  utils::Fraction f6{5, 6};
  REQUIRE(0.0 + f6 == Approx(0.833333));
  // 0 negative
  utils::Fraction f7{-5, 6};
  REQUIRE(0.0 + f7 == Approx(-0.833333));
  // 0, 0
  utils::Fraction f8;
  REQUIRE(0.0 + f8 == Approx(0.0));
}  // TEST_CASE "double plus fraction"

TEST_CASE("fraction plus fraction", "[utils][fraction]") {
  // positive positive
  utils::Fraction f0 = utils::Fraction{9, 9} + utils::Fraction{7, 6};
  REQUIRE(f0.numerator() == 13);
  REQUIRE(f0.denominator() == 6);
  // positive negative
  utils::Fraction f1 = utils::Fraction{7, 8} + utils::Fraction{-16, 21};
  REQUIRE(f1.numerator() == 19);
  REQUIRE(f1.denominator() == 168);
  // negative positive
  utils::Fraction f2 =
      utils::Fraction{-462748, 4817512} + utils::Fraction{15, 17};
  REQUIRE(f2.numerator() == 16098991);
  REQUIRE(f2.denominator() == 20474426);
  // negative negative
  utils::Fraction f3 =
      utils::Fraction{-5757263, 38173} + utils::Fraction{-381178374, 381738};
  REQUIRE(f3.numerator() == -2791414688966);
  REQUIRE(f3.denominator() == 2428680779);
  // positive 0
  utils::Fraction f4 = utils::Fraction{4763813, 471837381} + utils::Fraction{};
  REQUIRE(f4.numerator() == 4763813);
  REQUIRE(f4.denominator() == 471837381);
  // negative 0
  utils::Fraction f5 = utils::Fraction{-481783, 48391} + utils::Fraction{};
  REQUIRE(f5.numerator() == -481783);
  REQUIRE(f5.denominator() == 48391);
  // 0 positive
  utils::Fraction f6 = utils::Fraction{} + utils::Fraction{481937, 492849};
  REQUIRE(f6.numerator() == 481937);
  REQUIRE(f6.denominator() == 492849);
  // 0 negative
  utils::Fraction f7 = utils::Fraction{} + utils::Fraction{-967123, 111111};
  REQUIRE(f7.numerator() == -967123);
  REQUIRE(f7.denominator() == 111111);
  // 0, 0
  utils::Fraction f8 = utils::Fraction{} + utils::Fraction{};
  REQUIRE(f8.numerator() == 0);
  REQUIRE(f8.denominator() == 1);
}  // TEST_CASE "fraction plus fraction"

TEST_CASE("fraction minus equals fraction", "[utils][fraction]") {
  // positive positive
  utils::Fraction f0{9654709, 3069241};
  f0 -= utils::Fraction{1498358, 1921927};
  REQUIRE(f0.numerator() == 1993832013995);
  REQUIRE(f0.denominator() == 842693878201);
  // positive negative
  utils::Fraction f1{3710755, 1459286};
  f1 -= utils::Fraction{-4389815, 9168468};
  REQUIRE(f1.numerator() == 20213967022715);
  REQUIRE(f1.denominator() == 6689708496924);
  // positive 0
  utils::Fraction f2{9078650, 9277563};
  f2 -= utils::Fraction{0, 1};
  REQUIRE(f2.numerator() == 9078650);
  REQUIRE(f2.denominator() == 9277563);
  // negative positive
  utils::Fraction f3{-272505, 793817};
  f3 -= utils::Fraction{5109191, 8065022};
  REQUIRE(f3.numerator() == -201726499747);
  REQUIRE(f3.denominator() == 206521018354);
  // negative negative
  utils::Fraction f4{-5283952, 526703};
  f4 -= utils::Fraction{-9778463, 6884359};
  REQUIRE(f4.numerator() == -31226276709279);
  REQUIRE(f4.denominator() == 3626012538377);
  // negative 0
  utils::Fraction f5{-767485, 4282576};
  f5 -= utils::Fraction{0, 1};
  REQUIRE(f5.numerator() == -767485);
  REQUIRE(f5.denominator() == 4282576);
  // 0 positive
  utils::Fraction f6{0, 1};
  f6 -= utils::Fraction{5955532, 8682873};
  REQUIRE(f6.numerator() == -5955532);
  REQUIRE(f6.denominator() == 8682873);
  // 0 negative
  utils::Fraction f7{0, 1};
  f7 -= utils::Fraction{-3155, 939017};
  REQUIRE(f7.numerator() == 3155);
  REQUIRE(f7.denominator() == 939017);
  // 0 0
  utils::Fraction f8{0, 1};
  f8 -= utils::Fraction{0, 1};
  REQUIRE(f8.numerator() == 0);
  REQUIRE(f8.denominator() == 1);
}  // TEST_CASE "fraction minus equals fraction"

TEST_CASE("fraction minus equals int", "[utils][fraction]") {
  // positive positive
  utils::Fraction f0{1396811, 2313317};
  f0 -= 5809802;
  REQUIRE(f0.numerator() == -13439912336423);
  REQUIRE(f0.denominator() == 2313317);
  // positive negative
  utils::Fraction f1{2512103, 4223509};
  f1 -= -1871748;
  REQUIRE(f1.numerator() == 7905347035835);
  REQUIRE(f1.denominator() == 4223509);
  // positive 0
  utils::Fraction f2{1148316, 660997};
  f2 -= 0;
  REQUIRE(f2.numerator() == 1148316);
  REQUIRE(f2.denominator() == 660997);
  // negative positive
  utils::Fraction f3{-3450689, 4749358};
  f3 -= 1403640;
  REQUIRE(f3.numerator() == -6666392313809);
  REQUIRE(f3.denominator() == 4749358);
  // negative negative
  utils::Fraction f4{-3773278, 281873};
  f4 -= -4981990;
  REQUIRE(f4.numerator() == 1404284693992);
  REQUIRE(f4.denominator() == 281873);
  // negative 0
  utils::Fraction f5{-3031174, 2483745};
  f5 -= 0;
  REQUIRE(f5.numerator() == -3031174);
  REQUIRE(f5.denominator() == 2483745);
  // 0 positive
  utils::Fraction f6{0, 1};
  f6 -= 7779197;
  REQUIRE(f6.numerator() == -7779197);
  REQUIRE(f6.denominator() == 1);
  // 0 negative
  utils::Fraction f7{0, 1};
  f7 -= -6149256;
  REQUIRE(f7.numerator() == 6149256);
  REQUIRE(f7.denominator() == 1);
  // 0 0
  utils::Fraction f8{0, 1};
  f8 -= 0;
  REQUIRE(f8.numerator() == 0);
  REQUIRE(f8.denominator() == 1);
}  // TEST_CASE "fraction minus equals int"

TEST_CASE("int minus fraction", "[utils][fraction]") {
  // positive positive
  utils::Fraction f0{2582701, 2387647};
  f0 = 9756394 - f0;
  REQUIRE(f0.numerator() == 23294822282217);
  REQUIRE(f0.denominator() == 2387647);
  // positive negative
  utils::Fraction f1{-960661, 3281253};
  f1 = 590110 - f1;
  REQUIRE(f1.numerator() == 1936301168491);
  REQUIRE(f1.denominator() == 3281253);
  // positive 0
  utils::Fraction f2{0, 1};
  f2 = 3018804 - f2;
  REQUIRE(f2.numerator() == 3018804);
  REQUIRE(f2.denominator() == 1);
  // negative positive
  utils::Fraction f3{1163501, 1300647};
  f3 = -1078585 - f3;
  REQUIRE(f3.numerator() == -1402859507996);
  REQUIRE(f3.denominator() == 1300647);
  // negative negative
  utils::Fraction f4{-9491755, 4849709};
  f4 = -9737607 - f4;
  REQUIRE(f4.numerator() == -47224550814608);
  REQUIRE(f4.denominator() == 4849709);
  // negative 0
  utils::Fraction f5{0, 1};
  f5 = -5367815 - f5;
  REQUIRE(f5.numerator() == -5367815);
  REQUIRE(f5.denominator() == 1);
  // 0 positive
  utils::Fraction f6{8459847, 906031};
  f6 = 0 - f6;
  REQUIRE(f6.numerator() == -8459847);
  REQUIRE(f6.denominator() == 906031);
  // 0 negative
  utils::Fraction f7{-1522041, 4868954};
  f7 = 0 - f7;
  REQUIRE(f7.numerator() == 1522041);
  REQUIRE(f7.denominator() == 4868954);
  // 0 0
  utils::Fraction f8{0, 1};
  f8 = 0 - f8;
  REQUIRE(f8.numerator() == 0);
  REQUIRE(f8.denominator() == 1);
}  // TEST_CASE "int minus fraction"

TEST_CASE("fraction minus int", "[utils][fraction]") {
  // positive positive
  utils::Fraction f0{8308622, 9517785};
  f0 = f0 - 1399436;
  REQUIRE(f0.numerator() == -13319522660638);
  REQUIRE(f0.denominator() == 9517785);
  // positive negative
  utils::Fraction f1{5584, 11075};
  f1 = f1 - -5845811;
  REQUIRE(f1.numerator() == 64742362409);
  REQUIRE(f1.denominator() == 11075);
  // positive 0
  utils::Fraction f2{1112319, 1413512};
  f2 = f2 - 0;
  REQUIRE(f2.numerator() == 1112319);
  REQUIRE(f2.denominator() == 1413512);
  // negative positive
  utils::Fraction f3{-4199661, 802814};
  f3 = f3 - 7661613;
  REQUIRE(f3.numerator() == -6150854378643);
  REQUIRE(f3.denominator() == 802814);
  // negative negative
  utils::Fraction f4{-3497806, 5771331};
  f4 = f4 - -5511331;
  REQUIRE(f4.numerator() == 31807711953755);
  REQUIRE(f4.denominator() == 5771331);
  // negative 0
  utils::Fraction f5{-6505470, 9106477};
  f5 = f5 - 0;
  REQUIRE(f5.numerator() == -6505470);
  REQUIRE(f5.denominator() == 9106477);
  // 0 positive
  utils::Fraction f6{0, 1};
  f6 = f6 - 1721440;
  REQUIRE(f6.numerator() == -1721440);
  REQUIRE(f6.denominator() == 1);
  // 0 negative
  utils::Fraction f7{0, 1};
  f7 = f7 - -1200599;
  REQUIRE(f7.numerator() == 1200599);
  REQUIRE(f7.denominator() == 1);
  // 0 0
  utils::Fraction f8{0, 1};
  f8 = f8 - 0;
  REQUIRE(f8.numerator() == 0);
  REQUIRE(f8.denominator() == 1);
}  // TEST_CASE "fraction minus int"

TEST_CASE("double minus fraction", "[utils][fraction]") {
  // positive positive
  utils::Fraction f0{4678619, 910134};
  REQUIRE(7706101.0821896065 - f0 == Approx(7706095.941607011));
  // positive negative
  utils::Fraction f1{-2335099, 2187271};
  REQUIRE(4933647.468814049 - f1 == Approx(4933648.536399638));
  // positive 0
  utils::Fraction f2{0, 1};
  REQUIRE(768892.3112290436 - f2 == Approx(768892.3112290436));
  // negative positive
  utils::Fraction f3{8534215, 353434};
  REQUIRE(-2554616.2976022894 - f3 == Approx(-2554640.444161477));
  // negative negative
  utils::Fraction f4{-724036, 277743};
  REQUIRE(-31251.105801293084 - f4 == Approx(-31248.498945314717));
  // negative 0
  utils::Fraction f5{0, 1};
  REQUIRE(-7378723.326711587 - f5 == Approx(-7378723.326711587));
  // 0 positive
  utils::Fraction f6{9339906, 938749};
  REQUIRE(0.0 - f6 == Approx(-9.949311264246353));
  // 0 negative
  utils::Fraction f7{-1762955, 3072516};
  REQUIRE(0.0 - f7 == Approx(0.5737822032497145));
  // 0 0
  utils::Fraction f8{0, 1};
  REQUIRE(0.0 - f8 == Approx(0.0));
}  // TEST_CASE "double minus fraction"

TEST_CASE("fraction minus double", "[utils][fraction]") {
  // positive positive
  utils::Fraction f0{537947, 23660};
  REQUIRE(f0 - 3817066.6148143657 == Approx(-3817043.8782547717));
  // positive negative
  utils::Fraction f1{6605167, 1560692};
  REQUIRE(f1 - -1699795.2465620963 == Approx(1699799.4787661443));
  // positive 0
  utils::Fraction f2{4841118, 4061039};
  REQUIRE(f2 - 0.0 == Approx(1.192088527098607));
  // negative positive
  utils::Fraction f3{-4766916, 4400965};
  REQUIRE(f3 - 5139514.7968474105 == Approx(-5139515.8799998555));
  // negative negative
  utils::Fraction f4{-349141, 244974};
  REQUIRE(f4 - -5602652.753172028 == Approx(5602651.327955474));
  // negative 0
  utils::Fraction f5{-7528739, 5868888};
  REQUIRE(f5 - 0.0 == Approx(-1.2828220610105356));
  // 0 positive
  utils::Fraction f6{0, 1};
  REQUIRE(f6 - 6059238.673038641 == Approx(-6059238.673038641));
  // 0 negative
  utils::Fraction f7{0, 1};
  REQUIRE(f7 - -9193359.058481917 == Approx(9193359.058481917));
  // 0 0
  utils::Fraction f8{0, 1};
  REQUIRE(f8 - 0.0 == Approx(0.0));
}  // TEST_CASE "fraction minus double"

TEST_CASE("fraction minus fraction", "[utils][fraction]") {
  // positive positive
  utils::Fraction f0 =
      utils::Fraction{33789, 340901} - utils::Fraction{8886272, 7617783};
  REQUIRE(f0.numerator() == -2771941741285);
  REQUIRE(f0.denominator() == 2596909842483);
  // positive negative
  utils::Fraction f1 =
      utils::Fraction{8309773, 6666303} - utils::Fraction{-9406111, 4092603};
  REQUIRE(f1.numerator() == 10745843098528);
  REQUIRE(f1.denominator() == 3031392406301);
  // positive 0
  utils::Fraction f2 =
      utils::Fraction{6351420, 3163151} - utils::Fraction{0, 1};
  REQUIRE(f2.numerator() == 6351420);
  REQUIRE(f2.denominator() == 3163151);
  // negative positive
  utils::Fraction f3 =
      utils::Fraction{-4831267, 4626569} - utils::Fraction{2551783, 7595058};
  REQUIRE(f3.numerator() == -48499753201013);
  REQUIRE(f3.denominator() == 35139059896002);
  // negative negative
  utils::Fraction f4 =
      utils::Fraction{-1894363, 3748966} - utils::Fraction{-6183410, 5572669};
  REQUIRE(f4.numerator() == 12624735889213);
  REQUIRE(f4.denominator() == 20891746610254);
  // negative 0
  utils::Fraction f5 =
      utils::Fraction{-159280, 518167} - utils::Fraction{0, 1};
  REQUIRE(f5.numerator() == -159280);
  REQUIRE(f5.denominator() == 518167);
  // 0 positive
  utils::Fraction f6 =
      utils::Fraction{0, 1} - utils::Fraction{2070395, 1043598};
  REQUIRE(f6.numerator() == -2070395);
  REQUIRE(f6.denominator() == 1043598);
  // 0 negative
  utils::Fraction f7 =
      utils::Fraction{0, 1} - utils::Fraction{-5046136, 6133859};
  REQUIRE(f7.numerator() == 5046136);
  REQUIRE(f7.denominator() == 6133859);
  // 0 0
  utils::Fraction f8 =
      utils::Fraction{0, 1} - utils::Fraction{0, 1};
  REQUIRE(f8.numerator() == 0);
  REQUIRE(f8.denominator() == 1);
}  // TEST_CASE "fraction minus fraction"

TEST_CASE("fraction multiply equals fraction", "[utils][fraction]") {
  // positive positive
  utils::Fraction f0{6248101, 6245628};
  f0 *= utils::Fraction{991829, 1953740};
  REQUIRE(f0.numerator() == 6197047766729);
  REQUIRE(f0.denominator() == 12202333248720);
  // positive negative
  utils::Fraction f1{5927882, 2260587};
  f1 *= utils::Fraction{-5281019, 3025365};
  REQUIRE(f1.numerator() == -31305257471758);
  REQUIRE(f1.denominator() == 6839100789255);
  // positive 0
  utils::Fraction f2{792533, 926035};
  f2 *= utils::Fraction{0, 1};
  REQUIRE(f2.numerator() == 0);
  REQUIRE(f2.denominator() == 1);
  // negative positive
  utils::Fraction f3{-7911202, 771705};
  f3 *= utils::Fraction{188341, 187933};
  REQUIRE(f3.numerator() == -1490003695882);
  REQUIRE(f3.denominator() == 145028835765);
  // negative negative
  utils::Fraction f4{-5665345, 1805543};
  f4 *= utils::Fraction{-1343317, 1499618};
  REQUIRE(f4.numerator() == 7610354249365);
  REQUIRE(f4.denominator() == 2707624782574);
  // negative 0
  utils::Fraction f5{-4564422, 4126703};
  f5 *= utils::Fraction{0, 1};
  REQUIRE(f5.numerator() == 0);
  REQUIRE(f5.denominator() == 1);
  // 0 positive
  utils::Fraction f6{0, 1};
  f6 *= utils::Fraction{2877651, 1314176};
  REQUIRE(f6.numerator() == 0);
  REQUIRE(f6.denominator() == 1);
  // 0 negative
  utils::Fraction f7{0, 1};
  f7 *= utils::Fraction{-2543989, 253569};
  REQUIRE(f7.numerator() == 0);
  REQUIRE(f7.denominator() == 1);
  // 0 0
  utils::Fraction f8{0, 1};
  f8 *= utils::Fraction{0, 1};
  REQUIRE(f8.numerator() == 0);
  REQUIRE(f8.denominator() == 1);
}  // TEST_CASE "fraction multiply equals fraction"

TEST_CASE("fraction multiply equals int", "[utils][fraction]") {
  // positive positive
  utils::Fraction f0{763943, 524519};
  f0 *= 8745776;
  REQUIRE(f0.numerator() == 6681274354768);
  REQUIRE(f0.denominator() == 524519);
  // positive negative
  utils::Fraction f1{5839596, 6033917};
  f1 *= -6236065;
  REQUIRE(f1.numerator() == -36416100229740);
  REQUIRE(f1.denominator() == 6033917);
  // positive 0
  utils::Fraction f2{7115171, 8827741};
  f2 *= 0;
  REQUIRE(f2.numerator() == 0);
  REQUIRE(f2.denominator() == 1);
  // negative positive
  utils::Fraction f3{-1786938, 2853335};
  f3 *= 7441446;
  REQUIRE(f3.numerator() == -13297402632348);
  REQUIRE(f3.denominator() == 2853335);
  // negative negative
  utils::Fraction f4{-3486818, 4280985};
  f4 *= -3322756;
  REQUIRE(f4.numerator() == 11585845430408);
  REQUIRE(f4.denominator() == 4280985);
  // negative 0
  utils::Fraction f5{-3435824, 198657};
  f5 *= 0;
  REQUIRE(f5.numerator() == 0);
  REQUIRE(f5.denominator() == 1);
  // 0 positive
  utils::Fraction f6{0, 1};
  f6 *= 1025091;
  REQUIRE(f6.numerator() == 0);
  REQUIRE(f6.denominator() == 1);
  // 0 negative
  utils::Fraction f7{0, 1};
  f7 *= -8212228;
  REQUIRE(f7.numerator() == 0);
  REQUIRE(f7.denominator() == 1);
  // 0 0
  utils::Fraction f8{0, 1};
  f8 *= 0;
  REQUIRE(f8.numerator() == 0);
  REQUIRE(f8.denominator() == 1);
}  // TEST_CASE "fraction multiply equals int"

TEST_CASE("int multiply fraction", "[utils][fraction]") {
  // positive positive
  utils::Fraction f0{1469929, 693023};
  f0 = 3852563 * f0;
  REQUIRE(f0.numerator() == 5662994078027);
  REQUIRE(f0.denominator() == 693023);
  // positive negative
  utils::Fraction f1{-3884784, 2385737};
  f1 = 8025907 * f1;
  REQUIRE(f1.numerator() == -31178915099088);
  REQUIRE(f1.denominator() == 2385737);
  // positive 0
  utils::Fraction f2{0, 1};
  f2 = 6304867 * f2;
  REQUIRE(f2.numerator() == 0);
  REQUIRE(f2.denominator() == 1);
  // negative positive
  utils::Fraction f3{4590, 25907};
  f3 = -8502753 * f3;
  REQUIRE(f3.numerator() == -5575376610);
  REQUIRE(f3.denominator() == 3701);
  // negative negative
  utils::Fraction f4{-527950, 200537};
  f4 = -1519387 * f4;
  REQUIRE(f4.numerator() == 802160366650);
  REQUIRE(f4.denominator() == 200537);
  // negative 0
  utils::Fraction f5{0, 1};
  f5 = -9044191 * f5;
  REQUIRE(f5.numerator() == 0);
  REQUIRE(f5.denominator() == 1);
  // 0 positive
  utils::Fraction f6{4341664, 3436059};
  f6 = 0 * f6;
  REQUIRE(f6.numerator() == 0);
  REQUIRE(f6.denominator() == 1);
  // 0 negative
  utils::Fraction f7{-3679819, 923827};
  f7 = 0 * f7;
  REQUIRE(f7.numerator() == 0);
  REQUIRE(f7.denominator() == 1);
  // 0 0
  utils::Fraction f8{0, 1};
  f8 = 0 * f8;
  REQUIRE(f8.numerator() == 0);
  REQUIRE(f8.denominator() == 1);
}  // TEST_CASE "int multiply fraction"

TEST_CASE("fraction multiply int", "[utils][fraction]") {
  // positive positive
  utils::Fraction f0{819313, 480918};
  f0 = f0 * 5478991;
  REQUIRE(f0.numerator() == 4489008553183);
  REQUIRE(f0.denominator() == 480918);
  // positive negative
  utils::Fraction f1{5411054, 4726949};
  f1 = f1 * -9811703;
  REQUIRE(f1.numerator() == -53091654764962);
  REQUIRE(f1.denominator() == 4726949);
  // positive 0
  utils::Fraction f2{2093971, 8029701};
  f2 = f2 * 0;
  REQUIRE(f2.numerator() == 0);
  REQUIRE(f2.denominator() == 1);
  // negative positive
  utils::Fraction f3{-5408583, 577589};
  f3 = f3 * 5536287;
  REQUIRE(f3.numerator() == -29943467751321);
  REQUIRE(f3.denominator() == 577589);
  // negative negative
  utils::Fraction f4{-9927706, 7901407};
  f4 = f4 * -5815269;
  REQUIRE(f4.numerator() == 57732280942914);
  REQUIRE(f4.denominator() == 7901407);
  // negative 0
  utils::Fraction f5{-226924, 1177367};
  f5 = f5 * 0;
  REQUIRE(f5.numerator() == 0);
  REQUIRE(f5.denominator() == 1);
  // 0 positive
  utils::Fraction f6{0, 1};
  f6 = f6 * 1114235;
  REQUIRE(f6.numerator() == 0);
  REQUIRE(f6.denominator() == 1);
  // 0 negative
  utils::Fraction f7{0, 1};
  f7 = f7 * -8695388;
  REQUIRE(f7.numerator() == 0);
  REQUIRE(f7.denominator() == 1);
  // 0 0
  utils::Fraction f8{0, 1};
  f8 = f8 * 0;
  REQUIRE(f8.numerator() == 0);
  REQUIRE(f8.denominator() == 1);
}  // TEST_CASE "fraction multiply int"

TEST_CASE("double multiply fraction", "[utils][fraction]") {
  // positive positive
  utils::Fraction f0{1439455, 1880667};
  REQUIRE(9421328.884032786 * f0 == Approx(7211047.446871462));
  // positive negative
  utils::Fraction f1{-5783575, 3879882};
  REQUIRE(6934160.021863206 * f1 == Approx(-10336457.281032642));
  // positive 0
  utils::Fraction f2{0, 1};
  REQUIRE(224659.05588524096 * f2 == Approx(0.0));
  // negative positive
  utils::Fraction f3{2124021, 6698950};
  REQUIRE(-8715692.579746926 * f3 == Approx(-2763465.030926734));
  // negative negative
  utils::Fraction f4{-8090671, 6410461};
  REQUIRE(-767153.7672797493 * f4 == Approx(968228.1410761279));
  // negative 0
  utils::Fraction f5{0, 1};
  REQUIRE(-7500883.526791926 * f5 == Approx(-0.0));
  // 0 positive
  utils::Fraction f6{2200855, 760679};
  REQUIRE(0.0 * f6 == Approx(0.0));
  // 0 negative
  utils::Fraction f7{-426249, 353158};
  REQUIRE(0.0 * f7 == Approx(-0.0));
  // 0 0
  utils::Fraction f8{0, 1};
  REQUIRE(0.0 * f8 == Approx(0.0));
}  // TEST_CASE "double multiply fraction"

TEST_CASE("fraction multiply double", "[utils][fraction]") {
  // positive positive
  utils::Fraction f0{2972046, 7634731};
  REQUIRE(f0 * 5519713.383013585 == Approx(2148712.519292689));
  // positive negative
  utils::Fraction f1{2938363, 4454449};
  REQUIRE(f1 * -5970668.948867515 == Approx(-3938532.627627165));
  // positive 0
  utils::Fraction f2{1090319, 1008600};
  REQUIRE(f2 * 0.0 == Approx(0.0));
  // negative positive
  utils::Fraction f3{-5216668, 4599503};
  REQUIRE(f3 * 4159367.8874662253 == Approx(-4717475.205206445));
  // negative negative
  utils::Fraction f4{-90023, 1869172};
  REQUIRE(f4 * -8994454.376557665 == Approx(433190.6139942449));
  // negative 0
  utils::Fraction f5{-5083198, 5447189};
  REQUIRE(f5 * 0.0 == Approx(-0.0));
  // 0 positive
  utils::Fraction f6{0, 1};
  REQUIRE(f6 * 3883134.1113958284 == Approx(0.0));
  // 0 negative
  utils::Fraction f7{0, 1};
  REQUIRE(f7 * -8308609.714983359 == Approx(-0.0));
  // 0 0
  utils::Fraction f8{0, 1};
  REQUIRE(f8 * 0.0 == Approx(0.0));
}  // TEST_CASE "fraction multiply double"

TEST_CASE("fraction multiply fraction", "[utils][fraction]") {
  // positive positive
  utils::Fraction f0 =
      utils::Fraction{2512839, 8277721} * utils::Fraction{6488127, 8935042};
  REQUIRE(f0.numerator() == 16303618562553);
  REQUIRE(f0.denominator() == 73961784799282);
  // positive negative
  utils::Fraction f1 =
      utils::Fraction{475093, 1150033} * utils::Fraction{-9653849, 9539188};
  REQUIRE(f1.numerator() == -4586476082957);
  REQUIRE(f1.denominator() == 10970380993204);
  // positive 0
  utils::Fraction f2 = utils::Fraction{882420, 8915129} * utils::Fraction{0, 1};
  REQUIRE(f2.numerator() == 0);
  REQUIRE(f2.denominator() == 1);
  // negative positive
  utils::Fraction f3 =
      utils::Fraction{-4185337, 7620936} * utils::Fraction{8968906, 1840675};
  REQUIRE(f3.numerator() == -18768947065661);
  REQUIRE(f3.denominator() == 7013833185900);
  // negative negative
  utils::Fraction f4 =
      utils::Fraction{-9542209, 3394742} * utils::Fraction{-2794651, 3186470};
  REQUIRE(f4.numerator() == 26667143924059);
  REQUIRE(f4.denominator() == 10817243540740);
  // negative 0
  utils::Fraction f5 = utils::Fraction{-368101, 518689} * utils::Fraction{0, 1};
  REQUIRE(f5.numerator() == 0);
  REQUIRE(f5.denominator() == 1);
  // 0 positive
  utils::Fraction f6 =
      utils::Fraction{0, 1} * utils::Fraction{4287562, 7313701};
  REQUIRE(f6.numerator() == 0);
  REQUIRE(f6.denominator() == 1);
  // 0 negative
  utils::Fraction f7 =
      utils::Fraction{0, 1} * utils::Fraction{-7035569, 9412274};
  REQUIRE(f7.numerator() == 0);
  REQUIRE(f7.denominator() == 1);
  // 0 0
  utils::Fraction f8 = utils::Fraction{0, 1} * utils::Fraction{0, 1};
  REQUIRE(f8.numerator() == 0);
  REQUIRE(f8.denominator() == 1);
}  // TEST_CASE "fraction multiply fraction"

TEST_CASE("fraction divide equals fraction", "[utils][fraction]") {
  // positive positive
  utils::Fraction f0{1164599, 791810};
  f0 /= utils::Fraction{3541544, 9048489};
  REQUIRE(f0.numerator() == 10537861240911);
  REQUIRE(f0.denominator() == 2804229954640);
  // positive negative
  utils::Fraction f1{880313, 447162};
  f1 /= utils::Fraction{-482694, 1173551};
  REQUIRE(f1.numerator() == -1033092201463);
  REQUIRE(f1.denominator() == 215842414428);
  // negative positive
  utils::Fraction f2{-7037857, 931078};
  f2 /= utils::Fraction{36515, 256137};
  REQUIRE(f2.numerator() == -1802655578409);
  REQUIRE(f2.denominator() == 33998313170);
  // negative negative
  utils::Fraction f3{-8234575, 8625808};
  f3 /= utils::Fraction{-517079, 276784};
  REQUIRE(f3.numerator() == 142449912925);
  REQUIRE(f3.denominator() == 278764010927);
  // 0 positive
  utils::Fraction f4{0, 1};
  f4 /= utils::Fraction{6886735, 5253869};
  REQUIRE(f4.numerator() == 0);
  REQUIRE(f4.denominator() == 1);
  // 0 negative
  utils::Fraction f5{0, 1};
  f5 /= utils::Fraction{-9225163, 5697141};
  REQUIRE(f5.numerator() == 0);
  REQUIRE(f5.denominator() == 1);
}  // TEST_CASE "fraction divide equals fraction"

TEST_CASE("fraction divide equals int", "[utils][fraction]") {
  // positive positive
  utils::Fraction f0{9263666, 8875817};
  f0 /= 1271142;
  REQUIRE(f0.numerator() == 4631833);
  REQUIRE(f0.denominator() == 5641211886507);
  // positive negative
  utils::Fraction f1{2133257, 1026810};
  f1 /= -3819222;
  REQUIRE(f1.numerator() == -2133257);
  REQUIRE(f1.denominator() == 3921615341820);
  // negative positive
  utils::Fraction f2{-1203287, 294349};
  f2 /= 1914558;
  REQUIRE(f2.numerator() == -1203287);
  REQUIRE(f2.denominator() == 563548232742);
  // negative negative
  utils::Fraction f3{-1860373, 1758266};
  f3 /= -424034;
  REQUIRE(f3.numerator() == 1860373);
  REQUIRE(f3.denominator() == 745564565044);
  // 0 positive
  utils::Fraction f4{0, 1};
  f4 /= 5642592;
  REQUIRE(f4.numerator() == 0);
  REQUIRE(f4.denominator() == 1);
  // 0 negative
  utils::Fraction f5{0, 1};
  f5 /= -1094497;
  REQUIRE(f5.numerator() == 0);
  REQUIRE(f5.denominator() == 1);
}  // TEST_CASE "fraction divide equals int"

TEST_CASE("int divide fraction", "[utils][fraction]") {
  // positive positive
  utils::Fraction f0{807350, 1983637};
  f0 = 4922489 / f0;
  REQUIRE(f0.numerator() == 9764431312493);
  REQUIRE(f0.denominator() == 807350);
  // positive negative
  utils::Fraction f1{-1799587, 6548567};
  f1 = 5733552 / f1;
  REQUIRE(f1.numerator() == -37546549419984);
  REQUIRE(f1.denominator() == 1799587);
  // negative positive
  utils::Fraction f2{9800021, 5702084};
  f2 = -3828436 / f2;
  REQUIRE(f2.numerator() == -21830063660624);
  REQUIRE(f2.denominator() == 9800021);
  // negative negative
  utils::Fraction f3{-1620131, 9037889};
  f3 = -1415275 / f3;
  REQUIRE(f3.numerator() == 12791098354475);
  REQUIRE(f3.denominator() == 1620131);
  // 0 positive
  utils::Fraction f4{954678, 2395987};
  f4 = 0 / f4;
  REQUIRE(f4.numerator() == 0);
  REQUIRE(f4.denominator() == 1);
  // 0 negative
  utils::Fraction f5{-4348211, 3698025};
  f5 = 0 / f5;
  REQUIRE(f5.numerator() == 0);
  REQUIRE(f5.denominator() == 1);
}  // TEST_CASE "int divide fraction"

TEST_CASE("fraction divide int", "[utils][fraction]") {
  // positive positive
  utils::Fraction f0{1123903, 2492872};
  f0 = f0 / 1205784;
  REQUIRE(f0.numerator() == 1123903);
  REQUIRE(f0.denominator() == 3005865171648);
  // positive negative
  utils::Fraction f1{4848057, 8968396};
  f1 = f1 / -1256919;
  REQUIRE(f1.numerator() == -1616019);
  REQUIRE(f1.denominator() == 3757515777308);
  // negative positive
  utils::Fraction f2{-4230391, 5527334};
  f2 = f2 / 2728743;
  REQUIRE(f2.numerator() == -4230391);
  REQUIRE(f2.denominator() == 15082673961162);
  // negative negative
  utils::Fraction f3{-9899875, 2033328};
  f3 = f3 / -6043049;
  REQUIRE(f3.numerator() == 341375);
  REQUIRE(f3.denominator() == 423706921968);
  // 0 positive
  utils::Fraction f4{0, 1};
  f4 = f4 / 2760951;
  REQUIRE(f4.numerator() == 0);
  REQUIRE(f4.denominator() == 1);
  // 0 negative
  utils::Fraction f5{0, 1};
  f5 = f5 / -5920297;
  REQUIRE(f5.numerator() == 0);
  REQUIRE(f5.denominator() == 1);
}  // TEST_CASE "fraction divide int"

TEST_CASE("double divide fraction", "[utils][fraction]") {
  // positive positive
  utils::Fraction f0{4128247, 2773051};
  REQUIRE(654925.9586394846 / f0 == Approx(439930.8191906108));
  // positive negative
  utils::Fraction f1{-1240325, 628766};
  REQUIRE(8162365.204244544 / f1 == Approx(-4137800.7538443757));
  // negative positive
  utils::Fraction f2{4353193, 3132195};
  REQUIRE(-6798018.035934732 / f2 == Approx(-4891287.407212267));
  // negative negative
  utils::Fraction f3{-858005, 488776};
  REQUIRE(-4872856.108668634 / f3 == Approx(2775898.878643621));
  // 0 positive
  utils::Fraction f4{9410336, 4323273};
  REQUIRE(0.0 / f4 == Approx(0.0));
  // 0 negative
  utils::Fraction f5{-17349, 4485337};
  REQUIRE(0.0 / f5 == Approx(-0.0));
}  // TEST_CASE "double divide fraction"

TEST_CASE("fraction divide double", "[utils][fraction]") {
  // positive positive
  utils::Fraction f0{3377623, 6419111};
  REQUIRE(f0 / 3175544.632823142 == Approx(1.6569830524285917e-07));
  // positive negative
  utils::Fraction f1{9821441, 3688221};
  REQUIRE(f1 / -7264063.090286443 == Approx(-3.6658835768246273e-07));
  // negative positive
  utils::Fraction f2{-491732, 3745807};
  REQUIRE(f2 / 6068127.527170794 == Approx(-2.1633579065850494e-08));
  // negative negative
  utils::Fraction f3{-9854333, 2548008};
  REQUIRE(f3 / -106503.33934252698 == Approx(3.6313091313750726e-05));
  // 0 positive
  utils::Fraction f4{0, 1};
  REQUIRE(f4 / 5188730.192278302 == Approx(0.0));
  // 0 negative
  utils::Fraction f5{0, 1};
  REQUIRE(f5 / -3523428.2175066974 == Approx(-0.0));
}  // TEST_CASE "fraction divide double"

TEST_CASE("fraction divide fraction", "[utils][fraction]") {
  // positive positive
  utils::Fraction f0 =
      utils::Fraction{8571853, 336180} / utils::Fraction{3565873, 6935150};
  REQUIRE(f0.numerator() == 5944708633295);
  REQUIRE(f0.denominator() == 119877518514);
  // positive negative
  utils::Fraction f1 =
      utils::Fraction{2881517, 4585780} / utils::Fraction{-4338658, 1880417};
  REQUIRE(f1.numerator() == -5418453552589);
  REQUIRE(f1.denominator() == 19896131083240);
  // negative positive
  utils::Fraction f2 =
      utils::Fraction{-181208, 904815} / utils::Fraction{2128496, 2907057};
  REQUIRE(f2.numerator() == -21949249369);
  REQUIRE(f2.denominator() == 80245629510);
  // negative negative
  utils::Fraction f3 =
      utils::Fraction{-818768, 3634601} / utils::Fraction{-2242249, 7040776};
  REQUIRE(f3.numerator() == 5764762083968);
  REQUIRE(f3.denominator() == 8149680457649);
  // 0 positive
  utils::Fraction f4 =
      utils::Fraction{0, 1} / utils::Fraction{1494179, 1954764};
  REQUIRE(f4.numerator() == 0);
  REQUIRE(f4.denominator() == 1);
  // 0 negative
  utils::Fraction f5 =
      utils::Fraction{0, 1} / utils::Fraction{-5519224, 5657953};
  REQUIRE(f5.numerator() == 0);
  REQUIRE(f5.denominator() == 1);
}  // TEST_CASE "fraction divide fraction"

TEST_CASE("fraction fraction equals", "[utils][fraction]") {
  // positive positive equal
  utils::Fraction f0{5208097, 7562868};
  utils::Fraction f1{5208097, 7562868};
  REQUIRE((f0 == f1) == true);
  // positive positive not equal
  utils::Fraction f2{393213, 406655};
  utils::Fraction f3{1991698, 9496701};
  REQUIRE((f2 == f3) == false);
  // positive negative
  utils::Fraction f4{5816931, 3269278};
  utils::Fraction f5{-66449, 482236};
  REQUIRE((f4 == f5) == false);
  // positive 0
  utils::Fraction f6{3502927, 8028574};
  utils::Fraction f7{0, 1};
  REQUIRE((f6 == f7) == false);
  // negative positive
  utils::Fraction f8{-1860091, 8462090};
  utils::Fraction f9{1224339, 2926894};
  REQUIRE((f8 == f9) == false);
  // negative negative equal
  utils::Fraction f10{-112914, 44825};
  utils::Fraction f11{-112914, 44825};
  REQUIRE((f10 == f11) == true);
  // negative negative not equal
  utils::Fraction f12{-194570, 1241163};
  utils::Fraction f13{-9455627, 6620544};
  REQUIRE((f12 == f13) == false);
  // negative 0
  utils::Fraction f14{-6483708, 3631027};
  utils::Fraction f15{0, 1};
  REQUIRE((f14 == f15) == false);
  // 0 positive
  utils::Fraction f16{0, 1};
  utils::Fraction f17{827039, 1946067};
  REQUIRE((f16 == f17) == false);
  // 0 negative
  utils::Fraction f18{0, 1};
  utils::Fraction f19{-8175608, 7661503};
  REQUIRE((f18 == f19) == false);
  // 0 0
  utils::Fraction f20{0, 1};
  utils::Fraction f21{0, 1};
  REQUIRE((f20 == f21) == true);
}  // TEST_CASE "fraction fraction equals"

TEST_CASE("int fraction equals", "[utils][fraction]") {
  // positive positive equal
  utils::Fraction f0{4165821, 1};
  REQUIRE((4165821 == f0) == true);
  // positive positive not equal
  utils::Fraction f1{2652901, 999829};
  REQUIRE((4239549 == f1) == false);
  // positive negative
  utils::Fraction f2{-981623, 804657};
  REQUIRE((437436 == f2) == false);
  // positive 0
  utils::Fraction f3{0, 1};
  REQUIRE((7676974 == f3) == false);
  // negative positive
  utils::Fraction f4{6926477, 4262280};
  REQUIRE((-3033616 == f4) == false);
  // negative negative equal
  utils::Fraction f5{-2439356, 1};
  REQUIRE((-2439356 == f5) == true);
  // negative negative not equal
  utils::Fraction f6{-9975638, 7369573};
  REQUIRE((-4880447 == f6) == false);
  // negative 0
  utils::Fraction f7{0, 1};
  REQUIRE((-4544407 == f7) == false);
  // 0 positive
  utils::Fraction f8{1503619, 861247};
  REQUIRE((0 == f8) == false);
  // 0 negative
  utils::Fraction f9{-9806619, 1958161};
  REQUIRE((0 == f9) == false);
  // 0 0
  utils::Fraction f10{0, 1};
  REQUIRE((0 == f10) == true);
}  // TEST_CASE "int fraction equals"

TEST_CASE("fraction int equals", "[utils][fraction]") {
  // positive positive equal
  utils::Fraction f0{3245799, 1};
  REQUIRE((f0 == 3245799) == true);
  // positive positive not equal
  utils::Fraction f1{2254033, 543879};
  REQUIRE((f1 == 592018) == false);
  // positive negative
  utils::Fraction f2{252308, 625193};
  REQUIRE((f2 == -9888125) == false);
  // positive 0
  utils::Fraction f3{4706231, 5221549};
  REQUIRE((f3 == 0) == false);
  // negative positive
  utils::Fraction f4{-7997407, 4065395};
  REQUIRE((f4 == 3015480) == false);
  // negative negative equal
  utils::Fraction f5{-9398541, 1};
  REQUIRE((f5 == -9398541) == true);
  // negative negative not equal
  utils::Fraction f6{-6862677, 3545626};
  REQUIRE((f6 == -5807032) == false);
  // negative 0
  utils::Fraction f7{-4683920, 4438657};
  REQUIRE((f7 == 0) == false);
  // 0 positive
  utils::Fraction f8{0, 1};
  REQUIRE((f8 == 823280) == false);
  // 0 negative
  utils::Fraction f9{0, 1};
  REQUIRE((f9 == -4979155) == false);
  // 0 0
  utils::Fraction f10{0, 1};
  REQUIRE((f10 == 0) == true);
}  // TEST_CASE "fraction int equals"

TEST_CASE("double fraction equals", "[utils][fraction]") {
  // positive positive equal
  utils::Fraction f0{1884900570277195, 268435456};
  REQUIRE((7021801.808018964 == f0) == true);
  // positive positive not equal
  utils::Fraction f1{1345305, 2417173};
  REQUIRE((9765826.384704461 == f1) == false);
  // positive negative
  utils::Fraction f2{-2566579, 6031182};
  REQUIRE((6563229.830809355 == f2) == false);
  // positive 0
  utils::Fraction f3{0, 1};
  REQUIRE((1173838.4503581077 == f3) == false);
  // negative positive
  utils::Fraction f4{2339493, 564046};
  REQUIRE((-7895459.579848616 == f4) == false);
  // negative negative equal
  utils::Fraction f5{-6926143153589177, 1073741824};
  REQUIRE((-6450473.473956042 == f5) == true);
  // negative negative not equal
  utils::Fraction f6{-5798682, 7313579};
  REQUIRE((-3807394.027297098 == f6) == false);
  // negative 0
  utils::Fraction f7{0, 1};
  REQUIRE((-5671640.817678012 == f7) == false);
  // 0 positive
  utils::Fraction f8{5884963, 5552177};
  REQUIRE((0.0 == f8) == false);
  // 0 negative
  utils::Fraction f9{-549868, 1592359};
  REQUIRE((0.0 == f9) == false);
  // 0 0
  utils::Fraction f10{0, 1};
  REQUIRE((0.0 == f10) == true);
}  // TEST_CASE "double fraction equals"

TEST_CASE("fraction double equals", "[utils][fraction]") {
  // positive positive equal
  utils::Fraction f0{2750383154365213, 536870912};
  REQUIRE((f0 == 5122987.8410049025) == true);
  // positive positive not equal
  utils::Fraction f1{577848, 447457};
  REQUIRE((f1 == 8291535.039493079) == false);
  // positive negative
  utils::Fraction f2{4176900, 3849487};
  REQUIRE((f2 == -728515.460582697) == false);
  // positive 0
  utils::Fraction f3{1931089, 3239787};
  REQUIRE((f3 == 0.0) == false);
  // negative positive
  utils::Fraction f4{-3000934, 4342961};
  REQUIRE((f4 == 3368232.439332046) == false);
  // negative negative equal
  utils::Fraction f5{-2459497117128637, 268435456};
  REQUIRE((f5 == -9162340.749534357) == true);
  // negative negative not equal
  utils::Fraction f6{-7371681, 9705022};
  REQUIRE((f6 == -7565648.308972023) == false);
  // negative 0
  utils::Fraction f7{-1697739, 329354};
  REQUIRE((f7 == 0.0) == false);
  // 0 positive
  utils::Fraction f8{0, 1};
  REQUIRE((f8 == 7592846.97967686) == false);
  // 0 negative
  utils::Fraction f9{0, 1};
  REQUIRE((f9 == -5655968.638809932) == false);
  // 0 0
  utils::Fraction f10{0, 1};
  REQUIRE((f10 == 0.0) == true);
}  // TEST_CASE "fraction double equals"

TEST_CASE("fraction fraction not equals", "[utils][fraction]") {
  // positive positive equal
  utils::Fraction f0{9406301, 5720132};
  utils::Fraction f1{9406301, 5720132};
  REQUIRE((f0 != f1) == false);
  // positive positive not equal
  utils::Fraction f2{9185117, 7924158};
  utils::Fraction f3{160051, 5716389};
  REQUIRE((f2 != f3) == true);
  // positive negative
  utils::Fraction f4{1070, 41653};
  utils::Fraction f5{-7352481, 1694053};
  REQUIRE((f4 != f5) == true);
  // positive 0
  utils::Fraction f6{181949, 149558};
  utils::Fraction f7{0, 1};
  REQUIRE((f6 != f7) == true);
  // negative positive
  utils::Fraction f8{-1377773, 9822712};
  utils::Fraction f9{372503, 680187};
  REQUIRE((f8 != f9) == true);
  // negative negative equal
  utils::Fraction f10{-7423752, 7737613};
  utils::Fraction f11{-7423752, 7737613};
  REQUIRE((f10 != f11) == false);
  // negative negative not equal
  utils::Fraction f12{-3940922, 5768901};
  utils::Fraction f13{-7567737, 4068961};
  REQUIRE((f12 != f13) == true);
  // negative 0
  utils::Fraction f14{-725104, 106529};
  utils::Fraction f15{0, 1};
  REQUIRE((f14 != f15) == true);
  // 0 positive
  utils::Fraction f16{0, 1};
  utils::Fraction f17{5416645, 5802733};
  REQUIRE((f16 != f17) == true);
  // 0 negative
  utils::Fraction f18{0, 1};
  utils::Fraction f19{-613661, 345289};
  REQUIRE((f18 != f19) == true);
  // 0 0
  utils::Fraction f20{0, 1};
  utils::Fraction f21{0, 1};
  REQUIRE((f20 != f21) == false);
}  // TEST_CASE "fraction fraction not equals"

TEST_CASE("int fraction not equals", "[utils][fraction]") {
  // positive positive equal
  utils::Fraction f0{9105656, 1};
  REQUIRE((9105656 != f0) == false);
  // positive positive not equal
  utils::Fraction f1{1207169, 3210601};
  REQUIRE((6083252 != f1) == true);
  // positive negative
  utils::Fraction f2{-1997311, 8913142};
  REQUIRE((3518560 != f2) == true);
  // positive 0
  utils::Fraction f3{0, 1};
  REQUIRE((8093226 != f3) == true);
  // negative positive
  utils::Fraction f4{7804878, 8651933};
  REQUIRE((-9468279 != f4) == true);
  // negative negative equal
  utils::Fraction f5{-3341578, 1};
  REQUIRE((-3341578 != f5) == false);
  // negative negative not equal
  utils::Fraction f6{-4812347, 2864892};
  REQUIRE((-9323239 != f6) == true);
  // negative 0
  utils::Fraction f7{0, 1};
  REQUIRE((-5875539 != f7) == true);
  // 0 positive
  utils::Fraction f8{7905742, 2589175};
  REQUIRE((0 != f8) == true);
  // 0 negative
  utils::Fraction f9{-908685, 2402434};
  REQUIRE((0 != f9) == true);
  // 0 0
  utils::Fraction f10{0, 1};
  REQUIRE((0 != f10) == false);
}  // TEST_CASE "int fraction not equals"

TEST_CASE("fraction int not equals", "[utils][fraction]") {
  // positive positive equal
  utils::Fraction f0{7919717, 1};
  REQUIRE((f0 != 7919717) == false);
  // positive positive not equal
  utils::Fraction f1{6546244, 4752457};
  REQUIRE((f1 != 987934) == true);
  // positive negative
  utils::Fraction f2{2040934, 3930411};
  REQUIRE((f2 != -7074369) == true);
  // positive 0
  utils::Fraction f3{9306422, 3223225};
  REQUIRE((f3 != 0) == true);
  // negative positive
  utils::Fraction f4{-1017685, 1011413};
  REQUIRE((f4 != 7457425) == true);
  // negative negative equal
  utils::Fraction f5{-8968512, 1};
  REQUIRE((f5 != -8968512) == false);
  // negative negative not equal
  utils::Fraction f6{-8630694, 216703};
  REQUIRE((f6 != -3136014) == true);
  // negative 0
  utils::Fraction f7{-2068914, 371063};
  REQUIRE((f7 != 0) == true);
  // 0 positive
  utils::Fraction f8{0, 1};
  REQUIRE((f8 != 2163059) == true);
  // 0 negative
  utils::Fraction f9{0, 1};
  REQUIRE((f9 != -4273566) == true);
  // 0 0
  utils::Fraction f10{0, 1};
  REQUIRE((f10 != 0) == false);
}  // TEST_CASE "fraction int not equals"

TEST_CASE("double fraction not equals", "[utils][fraction]") {
  // positive positive equal
  utils::Fraction f0{5367067407190993, 536870912};
  REQUIRE((9996942.071599873 != f0) == false);
  // positive positive not equal
  utils::Fraction f1{4056633, 9586993};
  REQUIRE((7209381.17529696 != f1) == true);
  // positive negative
  utils::Fraction f2{-4936060, 5395683};
  REQUIRE((2190487.1861884003 != f2) == true);
  // positive 0
  utils::Fraction f3{0, 1};
  REQUIRE((2549900.988990433 != f3) == true);
  // negative positive
  utils::Fraction f4{3531693, 3956954};
  REQUIRE((-9921204.378890874 != f4) == true);
  // negative negative equal
  utils::Fraction f5{-3394868716049487, 1073741824};
  REQUIRE((-3161717.873112752 != f5) == false);
  // negative negative not equal
  utils::Fraction f6{-743291, 1097133};
  REQUIRE((-3953476.8587351753 != f6) == true);
  // negative 0
  utils::Fraction f7{0, 1};
  REQUIRE((-871036.9841661468 != f7) == true);
  // 0 positive
  utils::Fraction f8{2265666, 330227};
  REQUIRE((0.0 != f8) == true);
  // 0 negative
  utils::Fraction f9{-8450560, 1368539};
  REQUIRE((0.0 != f9) == true);
  // 0 0
  utils::Fraction f10{0, 1};
  REQUIRE((0.0 != f10) == false);
}  // TEST_CASE "double fraction not equals"

TEST_CASE("fraction double not equals", "[utils][fraction]") {
  // positive positive equal
  utils::Fraction f0{8210449792089805, 1073741824};
  REQUIRE((f0 != 7646577.2391201975) == false);
  // positive positive not equal
  utils::Fraction f1{8208287, 8938985};
  REQUIRE((f1 != 2275630.5170100243) == true);
  // positive negative
  utils::Fraction f2{799225, 3926803};
  REQUIRE((f2 != -4932977.553335783) == true);
  // positive 0
  utils::Fraction f3{60827, 898854};
  REQUIRE((f3 != 0.0) == true);
  // negative positive
  utils::Fraction f4{-5861079, 1981222};
  REQUIRE((f4 != 4310062.075976251) == true);
  // negative negative equal
  utils::Fraction f5{-857341553107647, 134217728};
  REQUIRE((f5 != -6387692.3405203745) == false);
  // negative negative not equal
  utils::Fraction f6{-1348929, 1320196};
  REQUIRE((f6 != -6239710.131785368) == true);
  // negative 0
  utils::Fraction f7{-17413, 1661681};
  REQUIRE((f7 != 0.0) == true);
  // 0 positive
  utils::Fraction f8{0, 1};
  REQUIRE((f8 != 1000022.5830168251) == true);
  // 0 negative
  utils::Fraction f9{0, 1};
  REQUIRE((f9 != -5218222.191668119) == true);
  // 0 0
  utils::Fraction f10{0, 1};
  REQUIRE((f10 != 0.0) == false);
}  // TEST_CASE "fraction double not equals"

TEST_CASE("fraction fraction less than", "[utils][fraction]") {
  // positive positive equal
  utils::Fraction f0{2812529, 791844};
  utils::Fraction f1{2812529, 791844};
  REQUIRE((f0 < f1) == false);
  // positive positive not equal
  utils::Fraction f2{4664467, 7860997};
  utils::Fraction f3{4338049, 2782731};
  REQUIRE((f2 < f3) == true);
  // positive negative
  utils::Fraction f4{302190, 471229};
  utils::Fraction f5{-5239943, 7191466};
  REQUIRE((f4 < f5) == false);
  // positive 0
  utils::Fraction f6{1604701, 422293};
  utils::Fraction f7{0, 1};
  REQUIRE((f6 < f7) == false);
  // negative positive
  utils::Fraction f8{-5228457, 2751454};
  utils::Fraction f9{7416287, 7238077};
  REQUIRE((f8 < f9) == true);
  // negative negative equal
  utils::Fraction f10{-2626348, 3253149};
  utils::Fraction f11{-2626348, 3253149};
  REQUIRE((f10 < f11) == false);
  // negative negative not equal
  utils::Fraction f12{-325873, 3591562};
  utils::Fraction f13{-5151496, 7842559};
  REQUIRE((f12 < f13) == false);
  // negative 0
  utils::Fraction f14{-5901193, 3428004};
  utils::Fraction f15{0, 1};
  REQUIRE((f14 < f15) == true);
  // 0 positive
  utils::Fraction f16{0, 1};
  utils::Fraction f17{2067, 13207};
  REQUIRE((f16 < f17) == true);
  // 0 negative
  utils::Fraction f18{0, 1};
  utils::Fraction f19{-5436899, 3494084};
  REQUIRE((f18 < f19) == false);
  // 0 0
  utils::Fraction f20{0, 1};
  utils::Fraction f21{0, 1};
  REQUIRE((f20 < f21) == false);
}  // TEST_CASE "fraction fraction less than"

TEST_CASE("int fraction less than", "[utils][fraction]") {
  // positive positive equal
  utils::Fraction f0{8462799, 1};
  REQUIRE((8462799 < f0) == false);
  // positive positive not equal
  utils::Fraction f1{2024929, 1847329};
  REQUIRE((9008390 < f1) == false);
  // positive negative
  utils::Fraction f2{-7687602, 7280293};
  REQUIRE((8991854 < f2) == false);
  // positive 0
  utils::Fraction f3{0, 1};
  REQUIRE((9116440 < f3) == false);
  // negative positive
  utils::Fraction f4{1494565, 2486517};
  REQUIRE((-4293807 < f4) == true);
  // negative negative equal
  utils::Fraction f5{-6928707, 1};
  REQUIRE((-6928707 < f5) == false);
  // negative negative not equal
  utils::Fraction f6{-533162, 2176829};
  REQUIRE((-1948084 < f6) == true);
  // negative 0
  utils::Fraction f7{0, 1};
  REQUIRE((-5847817 < f7) == true);
  // 0 positive
  utils::Fraction f8{6050669, 7043572};
  REQUIRE((0 < f8) == true);
  // 0 negative
  utils::Fraction f9{-5448833, 5900330};
  REQUIRE((0 < f9) == false);
  // 0 0
  utils::Fraction f10{0, 1};
  REQUIRE((0 < f10) == false);
}  // TEST_CASE "int fraction less than"

TEST_CASE("fraction int less than", "[utils][fraction]") {
  // positive positive equal
  utils::Fraction f0{8337854, 1};
  REQUIRE((f0 < 8337854) == false);
  // positive positive not equal
  utils::Fraction f1{313118, 2952531};
  REQUIRE((f1 < 1241030) == true);
  // positive negative
  utils::Fraction f2{2061737, 2297336};
  REQUIRE((f2 < -6823598) == false);
  // positive 0
  utils::Fraction f3{555760, 2640273};
  REQUIRE((f3 < 0) == false);
  // negative positive
  utils::Fraction f4{-6557966, 8637899};
  REQUIRE((f4 < 6463602) == true);
  // negative negative equal
  utils::Fraction f5{-3988891, 1};
  REQUIRE((f5 < -3988891) == false);
  // negative negative not equal
  utils::Fraction f6{-7975433, 713483};
  REQUIRE((f6 < -4001623) == false);
  // negative 0
  utils::Fraction f7{-1912919, 1167788};
  REQUIRE((f7 < 0) == true);
  // 0 positive
  utils::Fraction f8{0, 1};
  REQUIRE((f8 < 1138885) == true);
  // 0 negative
  utils::Fraction f9{0, 1};
  REQUIRE((f9 < -2635926) == false);
  // 0 0
  utils::Fraction f10{0, 1};
  REQUIRE((f10 < 0) == false);
}  // TEST_CASE "fraction int less than"

TEST_CASE("double fraction less than", "[utils][fraction]") {
  // positive positive equal
  utils::Fraction f0{2350347996389553, 536870912};
  REQUIRE((4377864.2944797 < f0) == false);
  // positive positive not equal
  utils::Fraction f1{1828379, 5151276};
  REQUIRE((100335.54450798257 < f1) == false);
  // positive negative
  utils::Fraction f2{-791677, 907979};
  REQUIRE((8118597.672451035 < f2) == false);
  // positive 0
  utils::Fraction f3{0, 1};
  REQUIRE((2354554.8064102787 < f3) == false);
  // negative positive
  utils::Fraction f4{8570443, 983677};
  REQUIRE((-2245799.069858269 < f4) == true);
  // negative negative equal
  utils::Fraction f5{-1462107411732023, 536870912};
  REQUIRE((-2723387.2781172823 < f5) == false);
  // negative negative not equal
  utils::Fraction f6{-1706365, 399844};
  REQUIRE((-1756931.6343766635 < f6) == true);
  // negative 0
  utils::Fraction f7{0, 1};
  REQUIRE((-3746113.650020383 < f7) == true);
  // 0 positive
  utils::Fraction f8{3689023, 159311};
  REQUIRE((0.0 < f8) == true);
  // 0 negative
  utils::Fraction f9{-622862, 830679};
  REQUIRE((0.0 < f9) == false);
  // 0 0
  utils::Fraction f10{0, 1};
  REQUIRE((0.0 < f10) == false);
}  // TEST_CASE "double fraction less than"

TEST_CASE("fraction double less than", "[utils][fraction]") {
  // positive positive equal
  utils::Fraction f0{576828878268735, 268435456};
  REQUIRE((f0 < 2148855.024087187) == false);
  // positive positive not equal
  utils::Fraction f1{322861, 225989};
  REQUIRE((f1 < 5368988.763014849) == true);
  // positive negative
  utils::Fraction f2{7288392, 9071209};
  REQUIRE((f2 < -4517043.353096134) == false);
  // positive 0
  utils::Fraction f3{8280880, 2591549};
  REQUIRE((f3 < 0.0) == false);
  // negative positive
  utils::Fraction f4{-1364717, 1926023};
  REQUIRE((f4 < 8394363.071812708) == true);
  // negative negative equal
  utils::Fraction f5{-5325507281727409, 1073741824};
  REQUIRE((f5 < -4959765.152751849) == false);
  // negative negative not equal
  utils::Fraction f6{-5749782, 3327173};
  REQUIRE((f6 < -8814174.039995233) == false);
  // negative 0
  utils::Fraction f7{-203143, 90798};
  REQUIRE((f7 < 0.0) == true);
  // 0 positive
  utils::Fraction f8{0, 1};
  REQUIRE((f8 < 9978094.198962806) == true);
  // 0 negative
  utils::Fraction f9{0, 1};
  REQUIRE((f9 < -350486.7243978657) == false);
  // 0 0
  utils::Fraction f10{0, 1};
  REQUIRE((f10 < 0.0) == false);
}  // TEST_CASE "fraction double less than"

TEST_CASE("fraction fraction greater than", "[utils][fraction]") {
  // positive positive equal
  utils::Fraction f0{1103242, 6440103};
  utils::Fraction f1{1103242, 6440103};
  REQUIRE((f0 > f1) == false);
  // positive positive not equal
  utils::Fraction f2{7002033, 6372755};
  utils::Fraction f3{9481352, 165179};
  REQUIRE((f2 > f3) == false);
  // positive negative
  utils::Fraction f4{860606, 787933};
  utils::Fraction f5{-8048875, 8736268};
  REQUIRE((f4 > f5) == true);
  // positive 0
  utils::Fraction f6{9732400, 1795563};
  utils::Fraction f7{0, 1};
  REQUIRE((f6 > f7) == true);
  // negative positive
  utils::Fraction f8{-5511109, 119622};
  utils::Fraction f9{7975367, 9160087};
  REQUIRE((f8 > f9) == false);
  // negative negative equal
  utils::Fraction f10{-2091535, 4355213};
  utils::Fraction f11{-2091535, 4355213};
  REQUIRE((f10 > f11) == false);
  // negative negative not equal
  utils::Fraction f12{-5112893, 5682353};
  utils::Fraction f13{-4560744, 582979};
  REQUIRE((f12 > f13) == true);
  // negative 0
  utils::Fraction f14{-7979723, 9882853};
  utils::Fraction f15{0, 1};
  REQUIRE((f14 > f15) == false);
  // 0 positive
  utils::Fraction f16{0, 1};
  utils::Fraction f17{99146, 56177};
  REQUIRE((f16 > f17) == false);
  // 0 negative
  utils::Fraction f18{0, 1};
  utils::Fraction f19{-3363828, 6418499};
  REQUIRE((f18 > f19) == true);
  // 0 0
  utils::Fraction f20{0, 1};
  utils::Fraction f21{0, 1};
  REQUIRE((f20 > f21) == false);
}  // TEST_CASE "fraction fraction greater than"

TEST_CASE("int fraction greater than", "[utils][fraction]") {
  // positive positive equal
  utils::Fraction f0{8752173, 1};
  REQUIRE((8752173 > f0) == false);
  // positive positive not equal
  utils::Fraction f1{1652931, 8351113};
  REQUIRE((2369101 > f1) == true);
  // positive negative
  utils::Fraction f2{-14616, 128407};
  REQUIRE((3807972 > f2) == true);
  // positive 0
  utils::Fraction f3{0, 1};
  REQUIRE((6817744 > f3) == true);
  // negative positive
  utils::Fraction f4{1030723, 3270752};
  REQUIRE((-6787246 > f4) == false);
  // negative negative equal
  utils::Fraction f5{-3372259, 1};
  REQUIRE((-3372259 > f5) == false);
  // negative negative not equal
  utils::Fraction f6{-411981, 8958638};
  REQUIRE((-9856967 > f6) == false);
  // negative 0
  utils::Fraction f7{0, 1};
  REQUIRE((-7686692 > f7) == false);
  // 0 positive
  utils::Fraction f8{6308551, 7695062};
  REQUIRE((0 > f8) == false);
  // 0 negative
  utils::Fraction f9{-9770253, 8667577};
  REQUIRE((0 > f9) == true);
  // 0 0
  utils::Fraction f10{0, 1};
  REQUIRE((0 > f10) == false);
}  // TEST_CASE "int fraction greater than"

TEST_CASE("fraction int greater than", "[utils][fraction]") {
  // positive positive equal
  utils::Fraction f0{4358932, 1};
  REQUIRE((f0 > 4358932) == false);
  // positive positive not equal
  utils::Fraction f1{264527, 524270};
  REQUIRE((f1 > 8036241) == false);
  // positive negative
  utils::Fraction f2{6062635, 4642442};
  REQUIRE((f2 > -1823534) == true);
  // positive 0
  utils::Fraction f3{2852877, 1395187};
  REQUIRE((f3 > 0) == true);
  // negative positive
  utils::Fraction f4{-781977, 658852};
  REQUIRE((f4 > 7642627) == false);
  // negative negative equal
  utils::Fraction f5{-9905302, 1};
  REQUIRE((f5 > -9905302) == false);
  // negative negative not equal
  utils::Fraction f6{-9665259, 4880129};
  REQUIRE((f6 > -9084390) == true);
  // negative 0
  utils::Fraction f7{-3505729, 6307263};
  REQUIRE((f7 > 0) == false);
  // 0 positive
  utils::Fraction f8{0, 1};
  REQUIRE((f8 > 9868361) == false);
  // 0 negative
  utils::Fraction f9{0, 1};
  REQUIRE((f9 > -6403809) == true);
  // 0 0
  utils::Fraction f10{0, 1};
  REQUIRE((f10 > 0) == false);
}  // TEST_CASE "fraction int greater than"

TEST_CASE("double fraction greater than", "[utils][fraction]") {
  // positive positive equal
  utils::Fraction f0{2310028683896873, 268435456};
  REQUIRE((8605527.445289765 > f0) == false);
  // positive positive not equal
  utils::Fraction f1{152054, 254013};
  REQUIRE((6054055.393483396 > f1) == true);
  // positive negative
  utils::Fraction f2{-4113647, 3657121};
  REQUIRE((2857884.803521136 > f2) == true);
  // positive 0
  utils::Fraction f3{0, 1};
  REQUIRE((1170805.9230371388 > f3) == true);
  // negative positive
  utils::Fraction f4{10204, 207799};
  REQUIRE((-5993982.6722301 > f4) == false);
  // negative negative equal
  utils::Fraction f5{-438529147597119, 67108864};
  REQUIRE((-6534593.516545281 > f5) == false);
  // negative negative not equal
  utils::Fraction f6{-6456043, 2708760};
  REQUIRE((-6716814.242242505 > f6) == false);
  // negative 0
  utils::Fraction f7{0, 1};
  REQUIRE((-1027492.4510356319 > f7) == false);
  // 0 positive
  utils::Fraction f8{3465827, 596423};
  REQUIRE((0.0 > f8) == false);
  // 0 negative
  utils::Fraction f9{-9405947, 8757003};
  REQUIRE((0.0 > f9) == true);
  // 0 0
  utils::Fraction f10{0, 1};
  REQUIRE((0.0 > f10) == false);
}  // TEST_CASE "double fraction greater than"

TEST_CASE("fraction double greater than", "[utils][fraction]") {
  // positive positive equal
  utils::Fraction f0{7043331353692683, 1073741824};
  REQUIRE((f0 > 6559613.490190993) == false);
  // positive positive not equal
  utils::Fraction f1{874262, 1401307};
  REQUIRE((f1 > 9565923.867598716) == false);
  // positive negative
  utils::Fraction f2{8786408, 3971855};
  REQUIRE((f2 > -9224010.42996452) == true);
  // positive 0
  utils::Fraction f3{2240786, 8493535};
  REQUIRE((f3 > 0.0) == true);
  // negative positive
  utils::Fraction f4{-4945301, 3895181};
  REQUIRE((f4 > 7952957.48239915) == false);
  // negative negative equal
  utils::Fraction f5{-6366362499636571, 2147483648};
  REQUIRE((f5 > -2964568.5570484824) == false);
  // negative negative not equal
  utils::Fraction f6{-6468530, 6629097};
  REQUIRE((f6 > -1507922.469463963) == true);
  // negative 0
  utils::Fraction f7{-3574399, 243520};
  REQUIRE((f7 > 0.0) == false);
  // 0 positive
  utils::Fraction f8{0, 1};
  REQUIRE((f8 > 8895227.11671512) == false);
  // 0 negative
  utils::Fraction f9{0, 1};
  REQUIRE((f9 > -4343142.061603221) == true);
  // 0 0
  utils::Fraction f10{0, 1};
  REQUIRE((f10 > 0.0) == false);
}  // TEST_CASE "fraction double greater than"

TEST_CASE("fraction fraction less than equal to", "[utils][fraction]") {
  // positive positive equal
  utils::Fraction f0{5877874, 5834091};
  utils::Fraction f1{5877874, 5834091};
  REQUIRE((f0 <= f1) == true);
  // positive positive not equal
  utils::Fraction f2{6067457, 1757546};
  utils::Fraction f3{335527, 55695};
  REQUIRE((f2 <= f3) == true);
  // positive negative
  utils::Fraction f4{2888659, 9309670};
  utils::Fraction f5{-6589527, 7468904};
  REQUIRE((f4 <= f5) == false);
  // positive 0
  utils::Fraction f6{399883, 1827637};
  utils::Fraction f7{0, 1};
  REQUIRE((f6 <= f7) == false);
  // negative positive
  utils::Fraction f8{-371030, 7705829};
  utils::Fraction f9{7239419, 3698423};
  REQUIRE((f8 <= f9) == true);
  // negative negative equal
  utils::Fraction f10{-423706, 5407617};
  utils::Fraction f11{-423706, 5407617};
  REQUIRE((f10 <= f11) == true);
  // negative negative not equal
  utils::Fraction f12{-3687345, 6661541};
  utils::Fraction f13{-1045231, 436053};
  REQUIRE((f12 <= f13) == false);
  // negative 0
  utils::Fraction f14{-7006531, 396616};
  utils::Fraction f15{0, 1};
  REQUIRE((f14 <= f15) == true);
  // 0 positive
  utils::Fraction f16{0, 1};
  utils::Fraction f17{7001975, 668494};
  REQUIRE((f16 <= f17) == true);
  // 0 negative
  utils::Fraction f18{0, 1};
  utils::Fraction f19{-3215706, 5537083};
  REQUIRE((f18 <= f19) == false);
  // 0 0
  utils::Fraction f20{0, 1};
  utils::Fraction f21{0, 1};
  REQUIRE((f20 <= f21) == true);
}  // TEST_CASE "fraction fraction less than equal to"

TEST_CASE("int fraction less than equal to", "[utils][fraction]") {
  // positive positive equal
  utils::Fraction f0{8684215, 1};
  REQUIRE((8684215 <= f0) == true);
  // positive positive not equal
  utils::Fraction f1{7114864, 1080727};
  REQUIRE((5927480 <= f1) == false);
  // positive negative
  utils::Fraction f2{-9364129, 945295};
  REQUIRE((8713616 <= f2) == false);
  // positive 0
  utils::Fraction f3{0, 1};
  REQUIRE((8971273 <= f3) == false);
  // negative positive
  utils::Fraction f4{207748, 5167667};
  REQUIRE((-7756358 <= f4) == true);
  // negative negative equal
  utils::Fraction f5{-6400018, 1};
  REQUIRE((-6400018 <= f5) == true);
  // negative negative not equal
  utils::Fraction f6{-8311056, 9920239};
  REQUIRE((-3288597 <= f6) == true);
  // negative 0
  utils::Fraction f7{0, 1};
  REQUIRE((-4114714 <= f7) == true);
  // 0 positive
  utils::Fraction f8{6247525, 1638921};
  REQUIRE((0 <= f8) == true);
  // 0 negative
  utils::Fraction f9{-780391, 4601377};
  REQUIRE((0 <= f9) == false);
  // 0 0
  utils::Fraction f10{0, 1};
  REQUIRE((0 <= f10) == true);
}  // TEST_CASE "int fraction less than equal to"

TEST_CASE("fraction int less than equal to", "[utils][fraction]") {
  // positive positive equal
  utils::Fraction f0{6208681, 1};
  REQUIRE((f0 <= 6208681) == true);
  // positive positive not equal
  utils::Fraction f1{4097535, 3555886};
  REQUIRE((f1 <= 748295) == true);
  // positive negative
  utils::Fraction f2{2976718, 655833};
  REQUIRE((f2 <= -4352978) == false);
  // positive 0
  utils::Fraction f3{206436, 383963};
  REQUIRE((f3 <= 0) == false);
  // negative positive
  utils::Fraction f4{-4048091, 5995325};
  REQUIRE((f4 <= 4076453) == true);
  // negative negative equal
  utils::Fraction f5{-7400251, 1};
  REQUIRE((f5 <= -7400251) == true);
  // negative negative not equal
  utils::Fraction f6{-3091154, 6436833};
  REQUIRE((f6 <= -5254900) == false);
  // negative 0
  utils::Fraction f7{-7092223, 1601536};
  REQUIRE((f7 <= 0) == true);
  // 0 positive
  utils::Fraction f8{0, 1};
  REQUIRE((f8 <= 5456014) == true);
  // 0 negative
  utils::Fraction f9{0, 1};
  REQUIRE((f9 <= -2418702) == false);
  // 0 0
  utils::Fraction f10{0, 1};
  REQUIRE((f10 <= 0) == true);
}  // TEST_CASE "fraction int less than equal to"

TEST_CASE("double fraction less than equal to", "[utils][fraction]") {
  // positive positive equal
  utils::Fraction f0{5805948053868697, 34359738368};
  REQUIRE((168975.32780039756 <= f0) == true);
  // positive positive not equal
  utils::Fraction f1{6538030, 4997021};
  REQUIRE((5293567.793982778 <= f1) == false);
  // positive negative
  utils::Fraction f2{-1474993, 29864};
  REQUIRE((4496221.145666871 <= f2) == false);
  // positive 0
  utils::Fraction f3{0, 1};
  REQUIRE((2738034.8614294208 <= f3) == false);
  // negative positive
  utils::Fraction f4{1275706, 144591};
  REQUIRE((-8318770.07922276 <= f4) == true);
  // negative negative equal
  utils::Fraction f5{-8950356471814619, 2147483648};
  REQUIRE((-4167834.5165283508 <= f5) == true);
  // negative negative not equal
  utils::Fraction f6{-2033292, 8052775};
  REQUIRE((-158948.5115785971 <= f6) == true);
  // negative 0
  utils::Fraction f7{0, 1};
  REQUIRE((-9801095.223970259 <= f7) == true);
  // 0 positive
  utils::Fraction f8{1670287, 4611981};
  REQUIRE((0.0 <= f8) == true);
  // 0 negative
  utils::Fraction f9{-7693199, 8718148};
  REQUIRE((0.0 <= f9) == false);
  // 0 0
  utils::Fraction f10{0, 1};
  REQUIRE((0.0 <= f10) == true);
}  // TEST_CASE "double fraction less than equal to"

TEST_CASE("fraction double less than equal to", "[utils][fraction]") {
  // positive positive equal
  utils::Fraction f0{3152012939947759, 2147483648};
  REQUIRE((f0 <= 1467770.4032267253) == true);
  // positive positive not equal
  utils::Fraction f1{865003, 5332496};
  REQUIRE((f1 <= 7877137.313227173) == true);
  // positive negative
  utils::Fraction f2{187703, 65384};
  REQUIRE((f2 <= -3600720.411704268) == false);
  // positive 0
  utils::Fraction f3{999261, 2611955};
  REQUIRE((f3 <= 0.0) == false);
  // negative positive
  utils::Fraction f4{-9016641, 906058};
  REQUIRE((f4 <= 8174483.7145019155) == true);
  // negative negative equal
  utils::Fraction f5{-4318803708051205, 536870912};
  REQUIRE((f5 <= -8044398.777282247) == true);
  // negative negative not equal
  utils::Fraction f6{-2266877, 186457};
  REQUIRE((f6 <= -9691020.278637402) == false);
  // negative 0
  utils::Fraction f7{-5773965, 5491384};
  REQUIRE((f7 <= 0.0) == true);
  // 0 positive
  utils::Fraction f8{0, 1};
  REQUIRE((f8 <= 6803439.417099992) == true);
  // 0 negative
  utils::Fraction f9{0, 1};
  REQUIRE((f9 <= -9141351.025220029) == false);
  // 0 0
  utils::Fraction f10{0, 1};
  REQUIRE((f10 <= 0.0) == true);
}  // TEST_CASE "fraction double less than equal to"

TEST_CASE("fraction fraction greater than equal to", "[utils][fraction]") {
  // positive positive equal
  utils::Fraction f0{220902, 408955};
  utils::Fraction f1{220902, 408955};
  REQUIRE((f0 >= f1) == true);
  // positive positive not equal
  utils::Fraction f2{6309210, 4521827};
  utils::Fraction f3{1383185, 6639581};
  REQUIRE((f2 >= f3) == true);
  // positive negative
  utils::Fraction f4{8754187, 9178155};
  utils::Fraction f5{-9068302, 4361449};
  REQUIRE((f4 >= f5) == true);
  // positive 0
  utils::Fraction f6{4116739, 6358986};
  utils::Fraction f7{0, 1};
  REQUIRE((f6 >= f7) == true);
  // negative positive
  utils::Fraction f8{-6780053, 8243652};
  utils::Fraction f9{2417672, 31169};
  REQUIRE((f8 >= f9) == false);
  // negative negative equal
  utils::Fraction f10{-8499872, 2285635};
  utils::Fraction f11{-8499872, 2285635};
  REQUIRE((f10 >= f11) == true);
  // negative negative not equal
  utils::Fraction f12{-5618791, 1015089};
  utils::Fraction f13{-2750597, 408527};
  REQUIRE((f12 >= f13) == true);
  // negative 0
  utils::Fraction f14{-9167125, 3567078};
  utils::Fraction f15{0, 1};
  REQUIRE((f14 >= f15) == false);
  // 0 positive
  utils::Fraction f16{0, 1};
  utils::Fraction f17{4381967, 5955869};
  REQUIRE((f16 >= f17) == false);
  // 0 negative
  utils::Fraction f18{0, 1};
  utils::Fraction f19{-43424, 157195};
  REQUIRE((f18 >= f19) == true);
  // 0 0
  utils::Fraction f20{0, 1};
  utils::Fraction f21{0, 1};
  REQUIRE((f20 >= f21) == true);
}  // TEST_CASE "fraction fraction greater than equal to"

TEST_CASE("int fraction greater than equal to", "[utils][fraction]") {
  // positive positive equal
  utils::Fraction f0{3218017, 1};
  REQUIRE((3218017 >= f0) == true);
  // positive positive not equal
  utils::Fraction f1{3159613, 679810};
  REQUIRE((6853547 >= f1) == true);
  // positive negative
  utils::Fraction f2{-8320847, 8092652};
  REQUIRE((9194731 >= f2) == true);
  // positive 0
  utils::Fraction f3{0, 1};
  REQUIRE((4094662 >= f3) == true);
  // negative positive
  utils::Fraction f4{9448205, 7303382};
  REQUIRE((-7125236 >= f4) == false);
  // negative negative equal
  utils::Fraction f5{-8686074, 1};
  REQUIRE((-8686074 >= f5) == true);
  // negative negative not equal
  utils::Fraction f6{-6846906, 6593975};
  REQUIRE((-5115043 >= f6) == false);
  // negative 0
  utils::Fraction f7{0, 1};
  REQUIRE((-14438 >= f7) == false);
  // 0 positive
  utils::Fraction f8{5761007, 6206580};
  REQUIRE((0 >= f8) == false);
  // 0 negative
  utils::Fraction f9{-7363627, 8807903};
  REQUIRE((0 >= f9) == true);
  // 0 0
  utils::Fraction f10{0, 1};
  REQUIRE((0 >= f10) == true);
}  // TEST_CASE "int fraction greater than equal to"

TEST_CASE("fraction int greater than equal to", "[utils][fraction]") {
  // positive positive equal
  utils::Fraction f0{8724414, 1};
  REQUIRE((f0 >= 8724414) == true);
  // positive positive not equal
  utils::Fraction f1{1068398, 3248361};
  REQUIRE((f1 >= 7842393) == false);
  // positive negative
  utils::Fraction f2{714827, 905692};
  REQUIRE((f2 >= -9815831) == true);
  // positive 0
  utils::Fraction f3{2655189, 8335438};
  REQUIRE((f3 >= 0) == true);
  // negative positive
  utils::Fraction f4{-7185045, 6413557};
  REQUIRE((f4 >= 2255092) == false);
  // negative negative equal
  utils::Fraction f5{-6561571, 1};
  REQUIRE((f5 >= -6561571) == true);
  // negative negative not equal
  utils::Fraction f6{-479705, 235012};
  REQUIRE((f6 >= -2552158) == true);
  // negative 0
  utils::Fraction f7{-2179992, 2976959};
  REQUIRE((f7 >= 0) == false);
  // 0 positive
  utils::Fraction f8{0, 1};
  REQUIRE((f8 >= 6273453) == false);
  // 0 negative
  utils::Fraction f9{0, 1};
  REQUIRE((f9 >= -9570885) == true);
  // 0 0
  utils::Fraction f10{0, 1};
  REQUIRE((f10 >= 0) == true);
}  // TEST_CASE "fraction int greater than equal to"

TEST_CASE("double fraction greater than equal to", "[utils][fraction]") {
  // positive positive equal
  utils::Fraction f0{8369113001434497, 1073741824};
  REQUIRE((7794343.867743851 >= f0) == true);
  // positive positive not equal
  utils::Fraction f1{3829025, 1494972};
  REQUIRE((2670558.03667 >= f1) == true);
  // positive negative
  utils::Fraction f2{-2601900, 610403};
  REQUIRE((9546527.366434377 >= f2) == true);
  // positive 0
  utils::Fraction f3{0, 1};
  REQUIRE((9995761.358090015 >= f3) == true);
  // negative positive
  utils::Fraction f4{8181472, 9262211};
  REQUIRE((-2937024.274056353 >= f4) == false);
  // negative negative equal
  utils::Fraction f5{-784001368622607, 2147483648};
  REQUIRE((-365079.0865638326 >= f5) == true);
  // negative negative not equal
  utils::Fraction f6{-8607629, 7767521};
  REQUIRE((-1344360.5360690139 >= f6) == false);
  // negative 0
  utils::Fraction f7{0, 1};
  REQUIRE((-9098278.248743644 >= f7) == false);
  // 0 positive
  utils::Fraction f8{660237, 439981};
  REQUIRE((0.0 >= f8) == false);
  // 0 negative
  utils::Fraction f9{-6928429, 9746858};
  REQUIRE((0.0 >= f9) == true);
  // 0 0
  utils::Fraction f10{0, 1};
  REQUIRE((0.0 >= f10) == true);
}  // TEST_CASE "double fraction greater than equal to"

TEST_CASE("fraction double greater than equal to", "[utils][fraction]") {
  // positive positive equal
  utils::Fraction f0{7776399397320689, 4294967296};
  REQUIRE((f0 >= 1810584.07698774) == true);
  // positive positive not equal
  utils::Fraction f1{148689, 963995};
  REQUIRE((f1 >= 6434547.0535869105) == false);
  // positive negative
  utils::Fraction f2{5617580, 1435807};
  REQUIRE((f2 >= -3015997.578718147) == true);
  // positive 0
  utils::Fraction f3{7544136, 4607387};
  REQUIRE((f3 >= 0.0) == true);
  // negative positive
  utils::Fraction f4{-641892, 579377};
  REQUIRE((f4 >= 272767.2137122113) == false);
  // negative negative equal
  utils::Fraction f5{-7586513350466977, 4294967296};
  REQUIRE((f5 >= -1766372.7864779013) == true);
  // negative negative not equal
  utils::Fraction f6{-756579, 6551629};
  REQUIRE((f6 >= -828776.37434927) == true);
  // negative 0
  utils::Fraction f7{-7323886, 638353};
  REQUIRE((f7 >= 0.0) == false);
  // 0 positive
  utils::Fraction f8{0, 1};
  REQUIRE((f8 >= 8516633.587847915) == false);
  // 0 negative
  utils::Fraction f9{0, 1};
  REQUIRE((f9 >= -3511899.1544164494) == true);
  // 0 0
  utils::Fraction f10{0, 1};
  REQUIRE((f10 >= 0.0) == true);
}  // TEST_CASE "fraction double greater than equal to"

// TEST_CASE("fraction conversion operators", "[utils][fraction]") {
//   // int
//   // -- positive
//   utils::Fraction f16{};
//   f16 += ;
//   REQUIRE(f16.numerator() == );
//   REQUIRE(f16.denominator() == );
//   // -- negative
//   utils::Fraction f16{};
//   f16 += ;
//   REQUIRE(f16.numerator() == );
//   REQUIRE(f16.denominator() == );
//   // -- 0
//   utils::Fraction f16{};
//   f16 += ;
//   REQUIRE(f16.numerator() == );
//   REQUIRE(f16.denominator() == );
//   // -- rational
//   utils::Fraction f16{};
//   f16 += ;
//   REQUIRE(f16.numerator() == );
//   REQUIRE(f16.denominator() == );
//   // -- irrational
//   utils::Fraction f16{};
//   f16 += ;
//   REQUIRE(f16.numerator() == );
//   REQUIRE(f16.denominator() == );
//   // double
//   // -- positive
//   utils::Fraction f16{};
//   f16 += ;
//   REQUIRE(f16.numerator() == );
//   REQUIRE(f16.denominator() == );
//   // -- negative
//   utils::Fraction f16{};
//   f16 += ;
//   REQUIRE(f16.numerator() == );
//   REQUIRE(f16.denominator() == );
//   // -- rational
//   utils::Fraction f16{};
//   f16 += ;
//   REQUIRE(f16.numerator() == );
//   REQUIRE(f16.denominator() == );
//   // -- irrational
//   utils::Fraction f16{};
//   f16 += ;
//   REQUIRE(f16.numerator() == );
//   REQUIRE(f16.denominator() == );
//   // -- 0
//   utils::Fraction f16{};
//   f16 += ;
//   REQUIRE(f16.numerator() == );
//   REQUIRE(f16.denominator() == );
// }  // TEST_CASE "fraction conversion operators"

// TEST_CASE("fraction print operator", "[utils][fraction]") {
//   // positive
//   utils::Fraction f16{};
//   f16 += ;
//   REQUIRE(f16.numerator() == );
//   REQUIRE(f16.denominator() == );
//   // negative
//   utils::Fraction f16{};
//   f16 += ;
//   REQUIRE(f16.numerator() == );
//   REQUIRE(f16.denominator() == );
//   // 0
//   utils::Fraction f16{};
//   f16 += ;
//   REQUIRE(f16.numerator() == );
//   REQUIRE(f16.denominator() == );
//   // rational
//   utils::Fraction f16{};
//   f16 += ;
//   REQUIRE(f16.numerator() == );
//   REQUIRE(f16.denominator() == );
//   // irrational
//   utils::Fraction f16{};
//   f16 += ;
//   REQUIRE(f16.numerator() == );
//   REQUIRE(f16.denominator() == );
// }  // TEST_CASE "fraction print operator"

// TEST_CASE("negative fraction", "[utils][fraction]") {
//   // positive
//   utils::Fraction f16{};
//   f16 += ;
//   REQUIRE(f16.numerator() == );
//   REQUIRE(f16.denominator() == );
//   // negative
//   utils::Fraction f16{};
//   f16 += ;
//   REQUIRE(f16.numerator() == );
//   REQUIRE(f16.denominator() == );
//   // 0
//   utils::Fraction f16{};
//   f16 += ;
//   REQUIRE(f16.numerator() == );
//   REQUIRE(f16.denominator() == );
//   // rational
//   utils::Fraction f16{};
//   f16 += ;
//   REQUIRE(f16.numerator() == );
//   REQUIRE(f16.denominator() == );
//   // irrational
//   utils::Fraction f16{};
//   f16 += ;
//   REQUIRE(f16.numerator() == );
//   REQUIRE(f16.denominator() == );
// }  // TEST_CASE "negative fraction"

// TEST_CASE("fraction GCD", "[utils][fraction]") {
//   // 8, 32
//   // 8, 12
//   // 54827, 3716
//   // 7, 19
//   // 0, 0
//   // 0, 10
//   // 10, 0
// }  // TEST_CASE "fraction GCD"

// TEST_CASE("fraction LCM", "[utils][fraction]") {
//   // 8, 32
//   // 8, 12
//   // 54827, 3716
//   // 7, 19
//   // 0, 0
//   // 0, 10
//   // 10, 0
// }  // TEST_CASE "fraction LCM"
