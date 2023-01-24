#include <cstdint>
#include <sstream>
#include <string>

#include "catch2/catch.hpp"
#include "utils/fraction.h"

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
/* ----------------------------- Constructors ------------------------------- */
/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */

TEST_CASE("single number fraction constructor", "[utils][fraction]") {
  // 0
  fishbait::Fraction f1{0};
  REQUIRE(f1.numerator() == 0);
  REQUIRE(f1.denominator() == 1);
  // positive
  fishbait::Fraction f2{7};
  REQUIRE(f2.numerator() == 7);
  REQUIRE(f2.denominator() == 1);
  // negative
  fishbait::Fraction f3{-99};
  REQUIRE(f3.numerator() == -99);
  REQUIRE(f3.denominator() == 1);
}  // TEST_CASE "single number fraction constructor"

TEST_CASE("fraction n/d constructor", "[utils][fraction]") {
  /* n positive, d positive */ {
    // reducable
    fishbait::Fraction f0{10, 50};
    REQUIRE(f0.numerator() == 1);
    REQUIRE(f0.denominator() == 5);
    // non reducable
    fishbait::Fraction f1{7, 11};
    REQUIRE(f1.numerator() == 7);
    REQUIRE(f1.denominator() == 11);
  }
  /* n positive, d negative */ {
    // reducable
    fishbait::Fraction f0{7, -49};
    REQUIRE(f0.numerator() == -1);
    REQUIRE(f0.denominator() == 7);
    // non reducable
    fishbait::Fraction f1{19, -2};
    REQUIRE(f1.numerator() == -19);
    REQUIRE(f1.denominator() == 2);
  }
  /* n negative, d positive */ {
    // reducable
    fishbait::Fraction f0{-50, 4};
    REQUIRE(f0.numerator() == -25);
    REQUIRE(f0.denominator() == 2);
    // non reducable
    fishbait::Fraction f1{-23, 11};
    REQUIRE(f1.numerator() == -23);
    REQUIRE(f1.denominator() == 11);
  }
  /* n negative, d negative */ {
    // reducable
    fishbait::Fraction f0{-8, -64};
    REQUIRE(f0.numerator() == 1);
    REQUIRE(f0.denominator() == 8);
    // non reducable
    fishbait::Fraction f1{-10, -11};
    REQUIRE(f1.numerator() == 10);
    REQUIRE(f1.denominator() == 11);
  }
  // d = 0 throws
  REQUIRE_THROWS(fishbait::Fraction{4, 0});
  // n = 0 d = 0 throws
  REQUIRE_THROWS(fishbait::Fraction{0, 0});
  // n = 0, d = -1 -> 0/1
  fishbait::Fraction f0{0, -1};
  REQUIRE(f0.numerator() == 0);
  REQUIRE(f0.denominator() == 1);
  // n = 0, d = 5 -> 0/1
  fishbait::Fraction f1{0, 5};
  REQUIRE(f1.numerator() == 0);
  REQUIRE(f1.denominator() == 1);
}  // TEST_CASE "fraction n/d constructor"

TEST_CASE("fraction copy constructor", "[utils][fraction]") {
  fishbait::Fraction f0{3, 9};
  fishbait::Fraction f1{f0};
  REQUIRE(f0.numerator() == 1);
  REQUIRE(f1.denominator() == 3);
}  // TEST_CASE "fraction copy constructor"

TEST_CASE("fraction default constructor", "[utils][fraction]") {
  fishbait::Fraction f0;
  REQUIRE(f0.numerator() == 0);
  REQUIRE(f0.denominator() == 1);
}  // TEST_CASE "fraction default constructor"

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
/* ------------------------------- Addition --------------------------------- */
/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */

TEST_CASE("fraction plus equals fraction", "[utils][fraction]") {
  // positive positive
  fishbait::Fraction f0{7693261, 1260869};
  f0 += fishbait::Fraction{2604888, 936541};
  REQUIRE(f0.numerator() == 10489476877873);
  REQUIRE(f0.denominator() == 1180855514129);
  // positive negative
  fishbait::Fraction f1{8691755, 1484767};
  f1 += fishbait::Fraction{-6792008, 6140401};
  REQUIRE(f1.numerator() == 43286311751619);
  REQUIRE(f1.denominator() == 9117064771567);
  // positive 0
  fishbait::Fraction f2{271363, 9538095};
  f2 += fishbait::Fraction{0, 1};
  REQUIRE(f2.numerator() == 271363);
  REQUIRE(f2.denominator() == 9538095);
  // negative positive
  fishbait::Fraction f3{-252465, 9683294};
  f3 += fishbait::Fraction{8234269, 9591948};
  REQUIRE(f3.numerator() == 38656608225133);
  REQUIRE(f3.denominator() == 46440826258356);
  // negative negative
  fishbait::Fraction f4{-2166838, 7389327};
  f4 += fishbait::Fraction{-2800307, 4056521};
  REQUIRE(f4.numerator() == -29482207973987);
  REQUIRE(f4.denominator() == 29974960151367);
  // negative 0
  fishbait::Fraction f5{-4421642, 4975081};
  f5 += fishbait::Fraction{0, 1};
  REQUIRE(f5.numerator() == -4421642);
  REQUIRE(f5.denominator() == 4975081);
  // 0 positive
  fishbait::Fraction f6{0, 1};
  f6 += fishbait::Fraction{4873799, 864785};
  REQUIRE(f6.numerator() == 4873799);
  REQUIRE(f6.denominator() == 864785);
  // 0 negative
  fishbait::Fraction f7{0, 1};
  f7 += fishbait::Fraction{-4005026, 2642953};
  REQUIRE(f7.numerator() == -4005026);
  REQUIRE(f7.denominator() == 2642953);
  // 0 0
  fishbait::Fraction f8{0, 1};
  f8 += fishbait::Fraction{0, 1};
  REQUIRE(f8.numerator() == 0);
  REQUIRE(f8.denominator() == 1);
}  // TEST_CASE "fraction plus equals fraction"

TEST_CASE("fraction plus equals int8_t", "[utils][fraction]") {
  // positive positive
  fishbait::Fraction f0{825929, 600010};
  f0 += int8_t{65};
  REQUIRE(f0.numerator() == 39826579);
  REQUIRE(f0.denominator() == 600010);
  // positive negative
  fishbait::Fraction f1{3793559, 4373016};
  f1 += int8_t{-46};
  REQUIRE(f1.numerator() == -197365177);
  REQUIRE(f1.denominator() == 4373016);
  // positive 0
  fishbait::Fraction f2{46577, 332579};
  f2 += int8_t{0};
  REQUIRE(f2.numerator() == 46577);
  REQUIRE(f2.denominator() == 332579);
  // negative positive
  fishbait::Fraction f3{-7237656, 6747263};
  f3 += int8_t{65};
  REQUIRE(f3.numerator() == 431334439);
  REQUIRE(f3.denominator() == 6747263);
  // negative negative
  fishbait::Fraction f4{-543581, 799376};
  f4 += int8_t{-49};
  REQUIRE(f4.numerator() == -39713005);
  REQUIRE(f4.denominator() == 799376);
  // negative 0
  fishbait::Fraction f5{-556979, 949778};
  f5 += int8_t{0};
  REQUIRE(f5.numerator() == -556979);
  REQUIRE(f5.denominator() == 949778);
  // 0 positive
  fishbait::Fraction f6{0, 1};
  f6 += int8_t{111};
  REQUIRE(f6.numerator() == 111);
  REQUIRE(f6.denominator() == 1);
  // 0 negative
  fishbait::Fraction f7{0, 1};
  f7 += int8_t{-84};
  REQUIRE(f7.numerator() == -84);
  REQUIRE(f7.denominator() == 1);
  // 0 0
  fishbait::Fraction f8{0, 1};
  f8 += int8_t{0};
  REQUIRE(f8.numerator() == 0);
  REQUIRE(f8.denominator() == 1);
}  // TEST_CASE "fraction plus equals int8_t"

TEST_CASE("int8_t plus fraction", "[utils][fraction]") {
  // positive positive
  fishbait::Fraction f0{2586872, 8422255};
  f0 = int8_t{52} + f0;
  REQUIRE(f0.numerator() == 440544132);
  REQUIRE(f0.denominator() == 8422255);
  // positive negative
  fishbait::Fraction f1{-6955329, 7406468};
  f1 = int8_t{86} + f1;
  REQUIRE(f1.numerator() == 630000919);
  REQUIRE(f1.denominator() == 7406468);
  // positive 0
  fishbait::Fraction f2{0, 1};
  f2 = int8_t{24} + f2;
  REQUIRE(f2.numerator() == 24);
  REQUIRE(f2.denominator() == 1);
  // negative positive
  fishbait::Fraction f3{1124980, 444171};
  f3 = int8_t{-23} + f3;
  REQUIRE(f3.numerator() == -9090953);
  REQUIRE(f3.denominator() == 444171);
  // negative negative
  fishbait::Fraction f4{-2090908, 2471217};
  f4 = int8_t{-98} + f4;
  REQUIRE(f4.numerator() == -244270174);
  REQUIRE(f4.denominator() == 2471217);
  // negative 0
  fishbait::Fraction f5{0, 1};
  f5 = int8_t{-43} + f5;
  REQUIRE(f5.numerator() == -43);
  REQUIRE(f5.denominator() == 1);
  // 0 positive
  fishbait::Fraction f6{7147770, 1973837};
  f6 = int8_t{0} + f6;
  REQUIRE(f6.numerator() == 7147770);
  REQUIRE(f6.denominator() == 1973837);
  // 0 negative
  fishbait::Fraction f7{-172222, 150147};
  f7 = int8_t{0} + f7;
  REQUIRE(f7.numerator() == -172222);
  REQUIRE(f7.denominator() == 150147);
  // 0 0
  fishbait::Fraction f8{0, 1};
  f8 = int8_t{0} + f8;
  REQUIRE(f8.numerator() == 0);
  REQUIRE(f8.denominator() == 1);
}  // TEST_CASE "int8_t plus fraction"

TEST_CASE("fraction plus int8_t", "[utils][fraction]") {
  // positive positive
  fishbait::Fraction f0{9961281, 1999696};
  f0 = f0 + int8_t{102};
  REQUIRE(f0.numerator() == 213930273);
  REQUIRE(f0.denominator() == 1999696);
  // positive negative
  fishbait::Fraction f1{6740299, 2420983};
  f1 = f1 + int8_t{-54};
  REQUIRE(f1.numerator() == -123992783);
  REQUIRE(f1.denominator() == 2420983);
  // positive 0
  fishbait::Fraction f2{1752274, 7052855};
  f2 = f2 + int8_t{0};
  REQUIRE(f2.numerator() == 1752274);
  REQUIRE(f2.denominator() == 7052855);
  // negative positive
  fishbait::Fraction f3{-4044802, 1250321};
  f3 = f3 + int8_t{30};
  REQUIRE(f3.numerator() == 33464828);
  REQUIRE(f3.denominator() == 1250321);
  // negative negative
  fishbait::Fraction f4{-9823951, 8998208};
  f4 = f4 + int8_t{-98};
  REQUIRE(f4.numerator() == -891648335);
  REQUIRE(f4.denominator() == 8998208);
  // negative 0
  fishbait::Fraction f5{-635627, 2523055};
  f5 = f5 + int8_t{0};
  REQUIRE(f5.numerator() == -635627);
  REQUIRE(f5.denominator() == 2523055);
  // 0 positive
  fishbait::Fraction f6{0, 1};
  f6 = f6 + int8_t{7};
  REQUIRE(f6.numerator() == 7);
  REQUIRE(f6.denominator() == 1);
  // 0 negative
  fishbait::Fraction f7{0, 1};
  f7 = f7 + int8_t{-40};
  REQUIRE(f7.numerator() == -40);
  REQUIRE(f7.denominator() == 1);
  // 0 0
  fishbait::Fraction f8{0, 1};
  f8 = f8 + int8_t{0};
  REQUIRE(f8.numerator() == 0);
  REQUIRE(f8.denominator() == 1);
}  // TEST_CASE "fraction plus int8_t"

TEST_CASE("fraction plus equals uint8_t", "[utils][fraction]") {
  // positive positive
  fishbait::Fraction f0{4942607, 4060491};
  f0 += uint8_t{121};
  REQUIRE(f0.numerator() == 496262018);
  REQUIRE(f0.denominator() == 4060491);
  // positive 0
  fishbait::Fraction f1{2048386, 2256537};
  f1 += uint8_t{0};
  REQUIRE(f1.numerator() == 2048386);
  REQUIRE(f1.denominator() == 2256537);
  // negative positive
  fishbait::Fraction f2{-1889703, 1599967};
  f2 += uint8_t{131};
  REQUIRE(f2.numerator() == 207705974);
  REQUIRE(f2.denominator() == 1599967);
  // negative 0
  fishbait::Fraction f3{-332570, 362289};
  f3 += uint8_t{0};
  REQUIRE(f3.numerator() == -332570);
  REQUIRE(f3.denominator() == 362289);
  // 0 positive
  fishbait::Fraction f4{0, 1};
  f4 += uint8_t{205};
  REQUIRE(f4.numerator() == 205);
  REQUIRE(f4.denominator() == 1);
  // 0 0
  fishbait::Fraction f5{0, 1};
  f5 += uint8_t{0};
  REQUIRE(f5.numerator() == 0);
  REQUIRE(f5.denominator() == 1);
}  // TEST_CASE "fraction plus equals uint8_t"

TEST_CASE("uint8_t plus fraction", "[utils][fraction]") {
  // positive positive
  fishbait::Fraction f0{196391, 5137656};
  f0 = uint8_t{220} + f0;
  REQUIRE(f0.numerator() == 1130480711);
  REQUIRE(f0.denominator() == 5137656);
  // positive negative
  fishbait::Fraction f1{-204083, 128125};
  f1 = uint8_t{10} + f1;
  REQUIRE(f1.numerator() == 1077167);
  REQUIRE(f1.denominator() == 128125);
  // positive 0
  fishbait::Fraction f2{0, 1};
  f2 = uint8_t{236} + f2;
  REQUIRE(f2.numerator() == 236);
  REQUIRE(f2.denominator() == 1);
  // 0 positive
  fishbait::Fraction f3{7744077, 1737107};
  f3 = uint8_t{0} + f3;
  REQUIRE(f3.numerator() == 7744077);
  REQUIRE(f3.denominator() == 1737107);
  // 0 negative
  fishbait::Fraction f4{-5828007, 5909818};
  f4 = uint8_t{0} + f4;
  REQUIRE(f4.numerator() == -5828007);
  REQUIRE(f4.denominator() == 5909818);
  // 0 0
  fishbait::Fraction f5{0, 1};
  f5 = uint8_t{0} + f5;
  REQUIRE(f5.numerator() == 0);
  REQUIRE(f5.denominator() == 1);
}  // TEST_CASE "uint8_t plus fraction"

TEST_CASE("fraction plus uint8_t", "[utils][fraction]") {
  // positive positive
  fishbait::Fraction f0{9373214, 9438681};
  f0 = f0 + uint8_t{220};
  REQUIRE(f0.numerator() == 2085883034);
  REQUIRE(f0.denominator() == 9438681);
  // positive 0
  fishbait::Fraction f1{2295953, 2818388};
  f1 = f1 + uint8_t{0};
  REQUIRE(f1.numerator() == 2295953);
  REQUIRE(f1.denominator() == 2818388);
  // negative positive
  fishbait::Fraction f2{-2448592, 45105};
  f2 = f2 + uint8_t{61};
  REQUIRE(f2.numerator() == 302813);
  REQUIRE(f2.denominator() == 45105);
  // negative 0
  fishbait::Fraction f3{-2352355, 3454048};
  f3 = f3 + uint8_t{0};
  REQUIRE(f3.numerator() == -2352355);
  REQUIRE(f3.denominator() == 3454048);
  // 0 positive
  fishbait::Fraction f4{0, 1};
  f4 = f4 + uint8_t{241};
  REQUIRE(f4.numerator() == 241);
  REQUIRE(f4.denominator() == 1);
  // 0 0
  fishbait::Fraction f5{0, 1};
  f5 = f5 + uint8_t{0};
  REQUIRE(f5.numerator() == 0);
  REQUIRE(f5.denominator() == 1);
}  // TEST_CASE "fraction plus uint8_t"

TEST_CASE("fraction plus equals int16_t", "[utils][fraction]") {
  // positive positive
  fishbait::Fraction f0{5870268, 311617};
  f0 += int16_t{24592};
  REQUIRE(f0.numerator() == 7669155532);
  REQUIRE(f0.denominator() == 311617);
  // positive negative
  fishbait::Fraction f1{6356079, 1793492};
  f1 += int16_t{-1951};
  REQUIRE(f1.numerator() == -3492746813);
  REQUIRE(f1.denominator() == 1793492);
  // positive 0
  fishbait::Fraction f2{9321295, 1885347};
  f2 += int16_t{0};
  REQUIRE(f2.numerator() == 9321295);
  REQUIRE(f2.denominator() == 1885347);
  // negative positive
  fishbait::Fraction f3{-6756262, 4615571};
  f3 += int16_t{12555};
  REQUIRE(f3.numerator() == 57941737643);
  REQUIRE(f3.denominator() == 4615571);
  // negative negative
  fishbait::Fraction f4{-4741461, 4649740};
  f4 += int16_t{-22835};
  REQUIRE(f4.numerator() == -106181554361);
  REQUIRE(f4.denominator() == 4649740);
  // negative 0
  fishbait::Fraction f5{-724775, 1040383};
  f5 += int16_t{0};
  REQUIRE(f5.numerator() == -724775);
  REQUIRE(f5.denominator() == 1040383);
  // 0 positive
  fishbait::Fraction f6{0, 1};
  f6 += int16_t{31449};
  REQUIRE(f6.numerator() == 31449);
  REQUIRE(f6.denominator() == 1);
  // 0 negative
  fishbait::Fraction f7{0, 1};
  f7 += int16_t{-25493};
  REQUIRE(f7.numerator() == -25493);
  REQUIRE(f7.denominator() == 1);
  // 0 0
  fishbait::Fraction f8{0, 1};
  f8 += int16_t{0};
  REQUIRE(f8.numerator() == 0);
  REQUIRE(f8.denominator() == 1);
}  // TEST_CASE "fraction plus equals int16_t"

TEST_CASE("int16_t plus fraction", "[utils][fraction]") {
  // positive positive
  fishbait::Fraction f0{3021589, 491358};
  f0 = int16_t{14477} + f0;
  REQUIRE(f0.numerator() == 7116411355);
  REQUIRE(f0.denominator() == 491358);
  // positive negative
  fishbait::Fraction f1{-163157, 754237};
  f1 = int16_t{6144} + f1;
  REQUIRE(f1.numerator() == 4633868971);
  REQUIRE(f1.denominator() == 754237);
  // positive 0
  fishbait::Fraction f2{0, 1};
  f2 = int16_t{12506} + f2;
  REQUIRE(f2.numerator() == 12506);
  REQUIRE(f2.denominator() == 1);
  // negative positive
  fishbait::Fraction f3{2361319, 9938191};
  f3 = int16_t{-27007} + f3;
  REQUIRE(f3.numerator() == -268398363018);
  REQUIRE(f3.denominator() == 9938191);
  // negative negative
  fishbait::Fraction f4{-669053, 810925};
  f4 = int16_t{-27953} + f4;
  REQUIRE(f4.numerator() == -22668455578);
  REQUIRE(f4.denominator() == 810925);
  // negative 0
  fishbait::Fraction f5{0, 1};
  f5 = int16_t{-14580} + f5;
  REQUIRE(f5.numerator() == -14580);
  REQUIRE(f5.denominator() == 1);
  // 0 positive
  fishbait::Fraction f6{6247222, 7180587};
  f6 = int16_t{0} + f6;
  REQUIRE(f6.numerator() == 6247222);
  REQUIRE(f6.denominator() == 7180587);
  // 0 negative
  fishbait::Fraction f7{-1433549, 1569546};
  f7 = int16_t{0} + f7;
  REQUIRE(f7.numerator() == -1433549);
  REQUIRE(f7.denominator() == 1569546);
  // 0 0
  fishbait::Fraction f8{0, 1};
  f8 = int16_t{0} + f8;
  REQUIRE(f8.numerator() == 0);
  REQUIRE(f8.denominator() == 1);
}  // TEST_CASE "int16_t plus fraction"

TEST_CASE("fraction plus int16_t", "[utils][fraction]") {
  // positive positive
  fishbait::Fraction f0{8178361, 4735664};
  f0 = f0 + int16_t{18778};
  REQUIRE(f0.numerator() == 88934476953);
  REQUIRE(f0.denominator() == 4735664);
  // positive negative
  fishbait::Fraction f1{2677653, 2888225};
  f1 = f1 + int16_t{-10958};
  REQUIRE(f1.numerator() == -31646491897);
  REQUIRE(f1.denominator() == 2888225);
  // positive 0
  fishbait::Fraction f2{2609974, 441397};
  f2 = f2 + int16_t{0};
  REQUIRE(f2.numerator() == 2609974);
  REQUIRE(f2.denominator() == 441397);
  // negative positive
  fishbait::Fraction f3{-379219, 1162425};
  f3 = f3 + int16_t{10036};
  REQUIRE(f3.numerator() == 11665718081);
  REQUIRE(f3.denominator() == 1162425);
  // negative negative
  fishbait::Fraction f4{-3028779, 2426861};
  f4 = f4 + int16_t{-26926};
  REQUIRE(f4.numerator() == -65348688065);
  REQUIRE(f4.denominator() == 2426861);
  // negative 0
  fishbait::Fraction f5{-9457070, 7918643};
  f5 = f5 + int16_t{0};
  REQUIRE(f5.numerator() == -9457070);
  REQUIRE(f5.denominator() == 7918643);
  // 0 positive
  fishbait::Fraction f6{0, 1};
  f6 = f6 + int16_t{27540};
  REQUIRE(f6.numerator() == 27540);
  REQUIRE(f6.denominator() == 1);
  // 0 negative
  fishbait::Fraction f7{0, 1};
  f7 = f7 + int16_t{-27956};
  REQUIRE(f7.numerator() == -27956);
  REQUIRE(f7.denominator() == 1);
  // 0 0
  fishbait::Fraction f8{0, 1};
  f8 = f8 + int16_t{0};
  REQUIRE(f8.numerator() == 0);
  REQUIRE(f8.denominator() == 1);
}  // TEST_CASE "fraction plus int16_t"

TEST_CASE("fraction plus equals uint16_t", "[utils][fraction]") {
  // positive positive
  fishbait::Fraction f0{1363978, 2437033};
  f0 += uint16_t{43209};
  REQUIRE(f0.numerator() == 105303122875);
  REQUIRE(f0.denominator() == 2437033);
  // positive 0
  fishbait::Fraction f1{8487263, 2656172};
  f1 += uint16_t{0};
  REQUIRE(f1.numerator() == 8487263);
  REQUIRE(f1.denominator() == 2656172);
  // negative positive
  fishbait::Fraction f2{-29089, 1823503};
  f2 += uint16_t{46219};
  REQUIRE(f2.numerator() == 84280456068);
  REQUIRE(f2.denominator() == 1823503);
  // negative 0
  fishbait::Fraction f3{-9259433, 8067792};
  f3 += uint16_t{0};
  REQUIRE(f3.numerator() == -9259433);
  REQUIRE(f3.denominator() == 8067792);
  // 0 positive
  fishbait::Fraction f4{0, 1};
  f4 += uint16_t{2840};
  REQUIRE(f4.numerator() == 2840);
  REQUIRE(f4.denominator() == 1);
  // 0 0
  fishbait::Fraction f5{0, 1};
  f5 += uint16_t{0};
  REQUIRE(f5.numerator() == 0);
  REQUIRE(f5.denominator() == 1);
}  // TEST_CASE "fraction plus equals uint16_t"

TEST_CASE("uint16_t plus fraction", "[utils][fraction]") {
  // positive positive
  fishbait::Fraction f0{7159778, 5490007};
  f0 = uint16_t{38964} + f0;
  REQUIRE(f0.numerator() == 213919792526);
  REQUIRE(f0.denominator() == 5490007);
  // positive negative
  fishbait::Fraction f1{-489029, 1851855};
  f1 = uint16_t{54525} + f1;
  REQUIRE(f1.numerator() == 100971904846);
  REQUIRE(f1.denominator() == 1851855);
  // positive 0
  fishbait::Fraction f2{0, 1};
  f2 = uint16_t{39721} + f2;
  REQUIRE(f2.numerator() == 39721);
  REQUIRE(f2.denominator() == 1);
  // 0 positive
  fishbait::Fraction f3{9402754, 8402683};
  f3 = uint16_t{0} + f3;
  REQUIRE(f3.numerator() == 9402754);
  REQUIRE(f3.denominator() == 8402683);
  // 0 negative
  fishbait::Fraction f4{-2873463, 3474514};
  f4 = uint16_t{0} + f4;
  REQUIRE(f4.numerator() == -2873463);
  REQUIRE(f4.denominator() == 3474514);
  // 0 0
  fishbait::Fraction f5{0, 1};
  f5 = uint16_t{0} + f5;
  REQUIRE(f5.numerator() == 0);
  REQUIRE(f5.denominator() == 1);
}  // TEST_CASE "uint16_t plus fraction"

TEST_CASE("fraction plus uint16_t", "[utils][fraction]") {
  // positive positive
  fishbait::Fraction f0{9602630, 7969597};
  f0 = f0 + uint16_t{11816};
  REQUIRE(f0.numerator() == 94178360782);
  REQUIRE(f0.denominator() == 7969597);
  // positive 0
  fishbait::Fraction f1{9911397, 7358248};
  f1 = f1 + uint16_t{0};
  REQUIRE(f1.numerator() == 9911397);
  REQUIRE(f1.denominator() == 7358248);
  // negative positive
  fishbait::Fraction f2{-2993643, 6147197};
  f2 = f2 + uint16_t{36203};
  REQUIRE(f2.numerator() == 222543979348);
  REQUIRE(f2.denominator() == 6147197);
  // negative 0
  fishbait::Fraction f3{-2200261, 3685223};
  f3 = f3 + uint16_t{0};
  REQUIRE(f3.numerator() == -2200261);
  REQUIRE(f3.denominator() == 3685223);
  // 0 positive
  fishbait::Fraction f4{0, 1};
  f4 = f4 + uint16_t{53347};
  REQUIRE(f4.numerator() == 53347);
  REQUIRE(f4.denominator() == 1);
  // 0 0
  fishbait::Fraction f5{0, 1};
  f5 = f5 + uint16_t{0};
  REQUIRE(f5.numerator() == 0);
  REQUIRE(f5.denominator() == 1);
}  // TEST_CASE "fraction plus uint16_t"

TEST_CASE("fraction plus equals int32_t", "[utils][fraction]") {
  // positive positive
  fishbait::Fraction f0{338266, 874069};
  f0 += int32_t{3776379};
  REQUIRE(f0.numerator() == 3300816154417);
  REQUIRE(f0.denominator() == 874069);
  // positive negative
  fishbait::Fraction f1{433475, 6220896};
  f1 += int32_t{-9881349};
  REQUIRE(f1.numerator() == -61470844035229);
  REQUIRE(f1.denominator() == 6220896);
  // positive 0
  fishbait::Fraction f2{1586303, 9251023};
  f2 += int32_t{0};
  REQUIRE(f2.numerator() == 1586303);
  REQUIRE(f2.denominator() == 9251023);
  // negative positive
  fishbait::Fraction f3{-8524723, 6960385};
  f3 += int32_t{4287225};
  REQUIRE(f3.numerator() == 29840728056902);
  REQUIRE(f3.denominator() == 6960385);
  // negative negative
  fishbait::Fraction f4{-2090468, 1357759};
  f4 += int32_t{-7376705};
  REQUIRE(f4.numerator() == -10015789694563);
  REQUIRE(f4.denominator() == 1357759);
  // negative 0
  fishbait::Fraction f5{-208908, 3994645};
  f5 += int32_t{0};
  REQUIRE(f5.numerator() == -208908);
  REQUIRE(f5.denominator() == 3994645);
  // 0 positive
  fishbait::Fraction f6{0, 1};
  f6 += int32_t{5819465};
  REQUIRE(f6.numerator() == 5819465);
  REQUIRE(f6.denominator() == 1);
  // 0 negative
  fishbait::Fraction f7{0, 1};
  f7 += int32_t{-7618690};
  REQUIRE(f7.numerator() == -7618690);
  REQUIRE(f7.denominator() == 1);
  // 0 0
  fishbait::Fraction f8{0, 1};
  f8 += int32_t{0};
  REQUIRE(f8.numerator() == 0);
  REQUIRE(f8.denominator() == 1);
}  // TEST_CASE "fraction plus equals int32_t"

TEST_CASE("int32_t plus fraction", "[utils][fraction]") {
  // positive positive
  fishbait::Fraction f0{5644160, 5830251};
  f0 = int32_t{459041} + f0;
  REQUIRE(f0.numerator() == 2676329893451);
  REQUIRE(f0.denominator() == 5830251);
  // positive negative
  fishbait::Fraction f1{-2255661, 955702};
  f1 = int32_t{7178761} + f1;
  REQUIRE(f1.numerator() == 6860753989561);
  REQUIRE(f1.denominator() == 955702);
  // positive 0
  fishbait::Fraction f2{0, 1};
  f2 = int32_t{3629011} + f2;
  REQUIRE(f2.numerator() == 3629011);
  REQUIRE(f2.denominator() == 1);
  // negative positive
  fishbait::Fraction f3{1372143, 528250};
  f3 = int32_t{-3721847} + f3;
  REQUIRE(f3.numerator() == -1966064305607);
  REQUIRE(f3.denominator() == 528250);
  // negative negative
  fishbait::Fraction f4{-9988011, 7205806};
  f4 = int32_t{-7756762} + f4;
  REQUIRE(f4.numerator() == -55893732148183);
  REQUIRE(f4.denominator() == 7205806);
  // negative 0
  fishbait::Fraction f5{0, 1};
  f5 = int32_t{-3129345} + f5;
  REQUIRE(f5.numerator() == -3129345);
  REQUIRE(f5.denominator() == 1);
  // 0 positive
  fishbait::Fraction f6{1647215, 8553912};
  f6 = int32_t{0} + f6;
  REQUIRE(f6.numerator() == 1647215);
  REQUIRE(f6.denominator() == 8553912);
  // 0 negative
  fishbait::Fraction f7{-946638, 3035629};
  f7 = int32_t{0} + f7;
  REQUIRE(f7.numerator() == -946638);
  REQUIRE(f7.denominator() == 3035629);
  // 0 0
  fishbait::Fraction f8{0, 1};
  f8 = int32_t{0} + f8;
  REQUIRE(f8.numerator() == 0);
  REQUIRE(f8.denominator() == 1);
}  // TEST_CASE "int32_t plus fraction"

TEST_CASE("fraction plus int32_t", "[utils][fraction]") {
  // positive positive
  fishbait::Fraction f0{1037937, 1415305};
  f0 = f0 + int32_t{2037816};
  REQUIRE(f0.numerator() == 2884132211817);
  REQUIRE(f0.denominator() == 1415305);
  // positive negative
  fishbait::Fraction f1{8157065, 8871334};
  f1 = f1 + int32_t{-7495195};
  REQUIRE(f1.numerator() == -66492370083065);
  REQUIRE(f1.denominator() == 8871334);
  // positive 0
  fishbait::Fraction f2{1032383, 830325};
  f2 = f2 + int32_t{0};
  REQUIRE(f2.numerator() == 1032383);
  REQUIRE(f2.denominator() == 830325);
  // negative positive
  fishbait::Fraction f3{-296045, 1117189};
  f3 = f3 + int32_t{6346375};
  REQUIRE(f3.numerator() == 7090100043830);
  REQUIRE(f3.denominator() == 1117189);
  // negative negative
  fishbait::Fraction f4{-1144679, 2276763};
  f4 = f4 + int32_t{-6768429};
  REQUIRE(f4.numerator() == -15410109860006);
  REQUIRE(f4.denominator() == 2276763);
  // negative 0
  fishbait::Fraction f5{-47441, 5118722};
  f5 = f5 + int32_t{0};
  REQUIRE(f5.numerator() == -47441);
  REQUIRE(f5.denominator() == 5118722);
  // 0 positive
  fishbait::Fraction f6{0, 1};
  f6 = f6 + int32_t{9123416};
  REQUIRE(f6.numerator() == 9123416);
  REQUIRE(f6.denominator() == 1);
  // 0 negative
  fishbait::Fraction f7{0, 1};
  f7 = f7 + int32_t{-7984279};
  REQUIRE(f7.numerator() == -7984279);
  REQUIRE(f7.denominator() == 1);
  // 0 0
  fishbait::Fraction f8{0, 1};
  f8 = f8 + int32_t{0};
  REQUIRE(f8.numerator() == 0);
  REQUIRE(f8.denominator() == 1);
}  // TEST_CASE "fraction plus int32_t"

TEST_CASE("fraction plus equals uint32_t", "[utils][fraction]") {
  // positive positive
  fishbait::Fraction f0{633219, 6676499};
  f0 += uint32_t{8085355};
  REQUIRE(f0.numerator() == 53981865205364);
  REQUIRE(f0.denominator() == 6676499);
  // positive 0
  fishbait::Fraction f1{1201459, 3520938};
  f1 += uint32_t{0};
  REQUIRE(f1.numerator() == 1201459);
  REQUIRE(f1.denominator() == 3520938);
  // negative positive
  fishbait::Fraction f2{-1658868, 9557213};
  f2 += uint32_t{4379356};
  REQUIRE(f2.numerator() == 41854436435960);
  REQUIRE(f2.denominator() == 9557213);
  // negative 0
  fishbait::Fraction f3{-3694120, 3800517};
  f3 += uint32_t{0};
  REQUIRE(f3.numerator() == -3694120);
  REQUIRE(f3.denominator() == 3800517);
  // 0 positive
  fishbait::Fraction f4{0, 1};
  f4 += uint32_t{5493979};
  REQUIRE(f4.numerator() == 5493979);
  REQUIRE(f4.denominator() == 1);
  // 0 0
  fishbait::Fraction f5{0, 1};
  f5 += uint32_t{0};
  REQUIRE(f5.numerator() == 0);
  REQUIRE(f5.denominator() == 1);
}  // TEST_CASE "fraction plus equals uint32_t"

TEST_CASE("uint32_t plus fraction", "[utils][fraction]") {
  // positive positive
  fishbait::Fraction f0{5328929, 5363358};
  f0 = uint32_t{7001726} + f0;
  REQUIRE(f0.numerator() == 37552768484837);
  REQUIRE(f0.denominator() == 5363358);
  // positive negative
  fishbait::Fraction f1{-2268624, 824735};
  f1 = uint32_t{1626508} + f1;
  REQUIRE(f1.numerator() == 1341435806756);
  REQUIRE(f1.denominator() == 824735);
  // positive 0
  fishbait::Fraction f2{0, 1};
  f2 = uint32_t{5874524} + f2;
  REQUIRE(f2.numerator() == 5874524);
  REQUIRE(f2.denominator() == 1);
  // 0 positive
  fishbait::Fraction f3{9357610, 3530111};
  f3 = uint32_t{0} + f3;
  REQUIRE(f3.numerator() == 9357610);
  REQUIRE(f3.denominator() == 3530111);
  // 0 negative
  fishbait::Fraction f4{-8967053, 2107827};
  f4 = uint32_t{0} + f4;
  REQUIRE(f4.numerator() == -8967053);
  REQUIRE(f4.denominator() == 2107827);
  // 0 0
  fishbait::Fraction f5{0, 1};
  f5 = uint32_t{0} + f5;
  REQUIRE(f5.numerator() == 0);
  REQUIRE(f5.denominator() == 1);
}  // TEST_CASE "uint32_t plus fraction"

TEST_CASE("fraction plus uint32_t", "[utils][fraction]") {
  // positive positive
  fishbait::Fraction f0{341353, 377915};
  f0 = f0 + uint32_t{6384028};
  REQUIRE(f0.numerator() == 2412620282973);
  REQUIRE(f0.denominator() == 377915);
  // positive 0
  fishbait::Fraction f1{206805, 196201};
  f1 = f1 + uint32_t{0};
  REQUIRE(f1.numerator() == 206805);
  REQUIRE(f1.denominator() == 196201);
  // negative positive
  fishbait::Fraction f2{-3422337, 964202};
  f2 = f2 + uint32_t{7554809};
  REQUIRE(f2.numerator() == 7284358525081);
  REQUIRE(f2.denominator() == 964202);
  // negative 0
  fishbait::Fraction f3{-6790963, 745741};
  f3 = f3 + uint32_t{0};
  REQUIRE(f3.numerator() == -6790963);
  REQUIRE(f3.denominator() == 745741);
  // 0 positive
  fishbait::Fraction f4{0, 1};
  f4 = f4 + uint32_t{2790188};
  REQUIRE(f4.numerator() == 2790188);
  REQUIRE(f4.denominator() == 1);
  // 0 0
  fishbait::Fraction f5{0, 1};
  f5 = f5 + uint32_t{0};
  REQUIRE(f5.numerator() == 0);
  REQUIRE(f5.denominator() == 1);
}  // TEST_CASE "fraction plus uint32_t"

TEST_CASE("fraction plus equals int64_t", "[utils][fraction]") {
  // positive positive
  fishbait::Fraction f0{3005713, 1150024};
  f0 += int64_t{5857288};
  REQUIRE(f0.numerator() == 6736024780625);
  REQUIRE(f0.denominator() == 1150024);
  // positive negative
  fishbait::Fraction f1{4420816, 807181};
  f1 += int64_t{-4120418};
  REQUIRE(f1.numerator() == -3325918700842);
  REQUIRE(f1.denominator() == 807181);
  // positive 0
  fishbait::Fraction f2{9560615, 5768168};
  f2 += int64_t{0};
  REQUIRE(f2.numerator() == 9560615);
  REQUIRE(f2.denominator() == 5768168);
  // negative positive
  fishbait::Fraction f3{-862593, 1327382};
  f3 += int64_t{7611322};
  REQUIRE(f3.numerator() == 10103130956411);
  REQUIRE(f3.denominator() == 1327382);
  // negative negative
  fishbait::Fraction f4{-3173014, 3746627};
  f4 += int64_t{-1895564};
  REQUIRE(f4.numerator() == -7101974435642);
  REQUIRE(f4.denominator() == 3746627);
  // negative 0
  fishbait::Fraction f5{-1357239, 624814};
  f5 += int64_t{0};
  REQUIRE(f5.numerator() == -1357239);
  REQUIRE(f5.denominator() == 624814);
  // 0 positive
  fishbait::Fraction f6{0, 1};
  f6 += int64_t{2158972};
  REQUIRE(f6.numerator() == 2158972);
  REQUIRE(f6.denominator() == 1);
  // 0 negative
  fishbait::Fraction f7{0, 1};
  f7 += int64_t{-8859416};
  REQUIRE(f7.numerator() == -8859416);
  REQUIRE(f7.denominator() == 1);
  // 0 0
  fishbait::Fraction f8{0, 1};
  f8 += int64_t{0};
  REQUIRE(f8.numerator() == 0);
  REQUIRE(f8.denominator() == 1);
}  // TEST_CASE "fraction plus equals int64_t"

TEST_CASE("int64_t plus fraction", "[utils][fraction]") {
  // positive positive
  fishbait::Fraction f0{2565000, 1654027};
  f0 = int64_t{6418203} + f0;
  REQUIRE(f0.numerator() == 10615883618481);
  REQUIRE(f0.denominator() == 1654027);
  // positive negative
  fishbait::Fraction f1{-1926592, 2379753};
  f1 = int64_t{5992964} + f1;
  REQUIRE(f1.numerator() == 14261772131300);
  REQUIRE(f1.denominator() == 2379753);
  // positive 0
  fishbait::Fraction f2{0, 1};
  f2 = int64_t{5332897} + f2;
  REQUIRE(f2.numerator() == 5332897);
  REQUIRE(f2.denominator() == 1);
  // negative positive
  fishbait::Fraction f3{7757856, 8980039};
  f3 = int64_t{-2226648} + f3;
  REQUIRE(f3.numerator() == -19995378121416);
  REQUIRE(f3.denominator() == 8980039);
  // negative negative
  fishbait::Fraction f4{-90109, 598696};
  f4 = int64_t{-7714445} + f4;
  REQUIRE(f4.numerator() == -4618607453829);
  REQUIRE(f4.denominator() == 598696);
  // negative 0
  fishbait::Fraction f5{0, 1};
  f5 = int64_t{-8465350} + f5;
  REQUIRE(f5.numerator() == -8465350);
  REQUIRE(f5.denominator() == 1);
  // 0 positive
  fishbait::Fraction f6{444568, 1203411};
  f6 = int64_t{0} + f6;
  REQUIRE(f6.numerator() == 444568);
  REQUIRE(f6.denominator() == 1203411);
  // 0 negative
  fishbait::Fraction f7{-1800520, 4254429};
  f7 = int64_t{0} + f7;
  REQUIRE(f7.numerator() == -1800520);
  REQUIRE(f7.denominator() == 4254429);
  // 0 0
  fishbait::Fraction f8{0, 1};
  f8 = int64_t{0} + f8;
  REQUIRE(f8.numerator() == 0);
  REQUIRE(f8.denominator() == 1);
}  // TEST_CASE "int64_t plus fraction"

TEST_CASE("fraction plus int64_t", "[utils][fraction]") {
  // positive positive
  fishbait::Fraction f0{1848392, 6462805};
  f0 = f0 + int64_t{225847};
  REQUIRE(f0.numerator() == 1459606969227);
  REQUIRE(f0.denominator() == 6462805);
  // positive negative
  fishbait::Fraction f1{4707091, 730573};
  f1 = f1 + int64_t{-2213111};
  REQUIRE(f1.numerator() == -1616834435512);
  REQUIRE(f1.denominator() == 730573);
  // positive 0
  fishbait::Fraction f2{1998071, 4195830};
  f2 = f2 + int64_t{0};
  REQUIRE(f2.numerator() == 1998071);
  REQUIRE(f2.denominator() == 4195830);
  // negative positive
  fishbait::Fraction f3{-1176658, 3186509};
  f3 = f3 + int64_t{8914364};
  REQUIRE(f3.numerator() == 28405699938618);
  REQUIRE(f3.denominator() == 3186509);
  // negative negative
  fishbait::Fraction f4{-6013587, 5197486};
  f4 = f4 + int64_t{-6419335};
  REQUIRE(f4.numerator() == -33364409805397);
  REQUIRE(f4.denominator() == 5197486);
  // negative 0
  fishbait::Fraction f5{-1661469, 6896698};
  f5 = f5 + int64_t{0};
  REQUIRE(f5.numerator() == -1661469);
  REQUIRE(f5.denominator() == 6896698);
  // 0 positive
  fishbait::Fraction f6{0, 1};
  f6 = f6 + int64_t{8771494};
  REQUIRE(f6.numerator() == 8771494);
  REQUIRE(f6.denominator() == 1);
  // 0 negative
  fishbait::Fraction f7{0, 1};
  f7 = f7 + int64_t{-4706180};
  REQUIRE(f7.numerator() == -4706180);
  REQUIRE(f7.denominator() == 1);
  // 0 0
  fishbait::Fraction f8{0, 1};
  f8 = f8 + int64_t{0};
  REQUIRE(f8.numerator() == 0);
  REQUIRE(f8.denominator() == 1);
}  // TEST_CASE "fraction plus int64_t"

TEST_CASE("double plus fraction", "[utils][fraction]") {
  // positive positive
  fishbait::Fraction f0{4168484, 622985};
  REQUIRE(3705789.5471597645 + f0 == Approx(3705796.2383064213));
  // positive negative
  fishbait::Fraction f1{-29470, 415599};
  REQUIRE(8456809.44655986 + f1 == Approx(8456809.37565016));
  // positive 0
  fishbait::Fraction f2{0, 1};
  REQUIRE(388597.992075856 + f2 == Approx(388597.992075856));
  // negative positive
  fishbait::Fraction f3{241872, 2203235};
  REQUIRE(-5033204.885754146 + f3 == Approx(-5033204.775973755));
  // negative negative
  fishbait::Fraction f4{-2563373, 1886754};
  REQUIRE(-9455632.75884343 + f4 == Approx(-9455634.117458807));
  // negative 0
  fishbait::Fraction f5{0, 1};
  REQUIRE(-2522740.981662336 + f5 == Approx(-2522740.981662336));
  // 0 positive
  fishbait::Fraction f6{1669590, 750209};
  REQUIRE(0.0 + f6 == Approx(2.2254998273814364));
  // 0 negative
  fishbait::Fraction f7{-6423234, 4476719};
  REQUIRE(0.0 + f7 == Approx(-1.4348083942726806));
  // 0 0
  fishbait::Fraction f8{0, 1};
  REQUIRE(0.0 + f8 == Approx(0.0));
}  // TEST_CASE "double plus fraction"

TEST_CASE("fraction plus double", "[utils][fraction]") {
  // positive positive
  fishbait::Fraction f0{617795, 1529978};
  REQUIRE(f0 + 4970136.830896603 == Approx(4970137.234689991));
  // positive negative
  fishbait::Fraction f1{5681759, 3603393};
  REQUIRE(f1 + -7758630.532921955 == Approx(-7758628.956141681));
  // positive 0
  fishbait::Fraction f2{5845808, 2128193};
  REQUIRE(f2 + 0.0 == Approx(2.7468410994679524));
  // negative positive
  fishbait::Fraction f3{-9870827, 9914450};
  REQUIRE(f3 + 6119081.475872337 == Approx(6119080.480272278));
  // negative negative
  fishbait::Fraction f4{-418263, 1625531};
  REQUIRE(f4 + -4898754.673571569 == Approx(-4898754.930880104));
  // negative 0
  fishbait::Fraction f5{-481451, 416149};
  REQUIRE(f5 + 0.0 == Approx(-1.1569197571062289));
  // 0 positive
  fishbait::Fraction f6{0, 1};
  REQUIRE(f6 + 5847690.093500701 == Approx(5847690.093500701));
  // 0 negative
  fishbait::Fraction f7{0, 1};
  REQUIRE(f7 + -4704940.85575075 == Approx(-4704940.85575075));
  // 0 0
  fishbait::Fraction f8{0, 1};
  REQUIRE(f8 + 0.0 == Approx(0.0));
}  // TEST_CASE "fraction plus double"

TEST_CASE("fraction plus fraction", "[utils][fraction]") {
  // positive positive
  fishbait::Fraction f0 = fishbait::Fraction(4460803, 3974972) +
                          fishbait::Fraction(9462287, 9721515);
  REQUIRE(f0.numerator() == 80978089157509);
  REQUIRE(f0.denominator() == 38642749922580);
  // positive negative
  fishbait::Fraction f1 = fishbait::Fraction(1339988, 2206125) +
                          fishbait::Fraction(-141165, 538378);
  REQUIRE(f1.numerator() == 409992423839);
  REQUIRE(f1.denominator() == 1187729165250);
  // positive 0
  fishbait::Fraction f2 = fishbait::Fraction(646554, 429037) +
                          fishbait::Fraction(0, 1);
  REQUIRE(f2.numerator() == 646554);
  REQUIRE(f2.denominator() == 429037);
  // negative positive
  fishbait::Fraction f3 = fishbait::Fraction(-1853114, 1414099) +
                          fishbait::Fraction(6913815, 2000107);
  REQUIRE(f3.numerator() == 6070392594487);
  REQUIRE(f3.denominator() == 2828349308593);
  // negative negative
  fishbait::Fraction f4 = fishbait::Fraction(-2375461, 2719709) +
                          fishbait::Fraction(-4440429, 3589049);
  REQUIRE(f4.numerator() == -20602320641750);
  REQUIRE(f4.denominator() == 9761168866741);
  // negative 0
  fishbait::Fraction f5 = fishbait::Fraction(-3956936, 5982621) +
                          fishbait::Fraction(0, 1);
  REQUIRE(f5.numerator() == -3956936);
  REQUIRE(f5.denominator() == 5982621);
  // 0 positive
  fishbait::Fraction f6 = fishbait::Fraction(0, 1) +
                          fishbait::Fraction(4376117, 7998591);
  REQUIRE(f6.numerator() == 4376117);
  REQUIRE(f6.denominator() == 7998591);
  // 0 negative
  fishbait::Fraction f7 = fishbait::Fraction(0, 1) +
                          fishbait::Fraction(-1268635, 1266482);
  REQUIRE(f7.numerator() == -1268635);
  REQUIRE(f7.denominator() == 1266482);
  // 0 0
  fishbait::Fraction f8 = fishbait::Fraction{0, 1} + fishbait::Fraction{0, 1};
  REQUIRE(f8.numerator() == 0);
  REQUIRE(f8.denominator() == 1);
}  // TEST_CASE "fraction plus fraction"

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
/* ----------------------------- Subtraction -------------------------------- */
/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */

TEST_CASE("fraction minus equals fraction", "[utils][fraction]") {
  // positive positive
  fishbait::Fraction f0{1165817, 840781};
  f0 -= fishbait::Fraction{7681271, 1487224};
  REQUIRE(f0.numerator() == -4724435690643);
  REQUIRE(f0.denominator() == 1250429681944);
  // positive negative
  fishbait::Fraction f1{4292059, 4669502};
  f1 -= fishbait::Fraction{-3275736, 2293577};
  REQUIRE(f1.numerator() == 25140223608515);
  REQUIRE(f1.denominator() == 10709862388654);
  // positive 0
  fishbait::Fraction f2{85371, 2713135};
  f2 -= fishbait::Fraction{0, 1};
  REQUIRE(f2.numerator() == 85371);
  REQUIRE(f2.denominator() == 2713135);
  // negative positive
  fishbait::Fraction f3{-1809565, 1064173};
  f3 -= fishbait::Fraction{2874775, 4193341};
  REQUIRE(f3.numerator() == -10647381042740);
  REQUIRE(f3.denominator() == 4462440271993);
  // negative negative
  fishbait::Fraction f4{-2525110, 3224337};
  f4 -= fishbait::Fraction{-6725749, 318201};
  REQUIRE(f4.numerator() == 2320287647367);
  REQUIRE(f4.denominator() == 113998584193);
  // negative 0
  fishbait::Fraction f5{-2465565, 1818208};
  f5 -= fishbait::Fraction{0, 1};
  REQUIRE(f5.numerator() == -2465565);
  REQUIRE(f5.denominator() == 1818208);
  // 0 positive
  fishbait::Fraction f6{0, 1};
  f6 -= fishbait::Fraction{3754355, 645727};
  REQUIRE(f6.numerator() == -3754355);
  REQUIRE(f6.denominator() == 645727);
  // 0 negative
  fishbait::Fraction f7{0, 1};
  f7 -= fishbait::Fraction{-8724881, 2664282};
  REQUIRE(f7.numerator() == 8724881);
  REQUIRE(f7.denominator() == 2664282);
  // 0 0
  fishbait::Fraction f8{0, 1};
  f8 -= fishbait::Fraction{0, 1};
  REQUIRE(f8.numerator() == 0);
  REQUIRE(f8.denominator() == 1);
}  // TEST_CASE "fraction minus equals fraction"

TEST_CASE("fraction minus equals int8_t", "[utils][fraction]") {
  // positive positive
  fishbait::Fraction f0{816000, 3787547};
  f0 -= int8_t{124};
  REQUIRE(f0.numerator() == -468839828);
  REQUIRE(f0.denominator() == 3787547);
  // positive negative
  fishbait::Fraction f1{1066441, 870577};
  f1 -= int8_t{-23};
  REQUIRE(f1.numerator() == 21089712);
  REQUIRE(f1.denominator() == 870577);
  // positive 0
  fishbait::Fraction f2{4832491, 1753742};
  f2 -= int8_t{0};
  REQUIRE(f2.numerator() == 4832491);
  REQUIRE(f2.denominator() == 1753742);
  // negative positive
  fishbait::Fraction f3{-54001, 736784};
  f3 -= int8_t{9};
  REQUIRE(f3.numerator() == -6685057);
  REQUIRE(f3.denominator() == 736784);
  // negative negative
  fishbait::Fraction f4{-7057761, 71624};
  f4 -= int8_t{-114};
  REQUIRE(f4.numerator() == 1107375);
  REQUIRE(f4.denominator() == 71624);
  // negative 0
  fishbait::Fraction f5{-6809161, 3656633};
  f5 -= int8_t{0};
  REQUIRE(f5.numerator() == -6809161);
  REQUIRE(f5.denominator() == 3656633);
  // 0 positive
  fishbait::Fraction f6{0, 1};
  f6 -= int8_t{27};
  REQUIRE(f6.numerator() == -27);
  REQUIRE(f6.denominator() == 1);
  // 0 negative
  fishbait::Fraction f7{0, 1};
  f7 -= int8_t{-79};
  REQUIRE(f7.numerator() == 79);
  REQUIRE(f7.denominator() == 1);
  // 0 0
  fishbait::Fraction f8{0, 1};
  f8 -= int8_t{0};
  REQUIRE(f8.numerator() == 0);
  REQUIRE(f8.denominator() == 1);
}  // TEST_CASE "fraction minus equals int8_t"

TEST_CASE("int8_t minus fraction", "[utils][fraction]") {
  // positive positive
  fishbait::Fraction f0{1832879, 7252952};
  f0 = int8_t{101} - f0;
  REQUIRE(f0.numerator() == 730715273);
  REQUIRE(f0.denominator() == 7252952);
  // positive negative
  fishbait::Fraction f1{-1556441, 3981341};
  f1 = int8_t{70} - f1;
  REQUIRE(f1.numerator() == 280250311);
  REQUIRE(f1.denominator() == 3981341);
  // positive 0
  fishbait::Fraction f2{0, 1};
  f2 = int8_t{99} - f2;
  REQUIRE(f2.numerator() == 99);
  REQUIRE(f2.denominator() == 1);
  // negative positive
  fishbait::Fraction f3{1076042, 933707};
  f3 = int8_t{-80} - f3;
  REQUIRE(f3.numerator() == -75772602);
  REQUIRE(f3.denominator() == 933707);
  // negative negative
  fishbait::Fraction f4{-5538257, 5349881};
  f4 = int8_t{-118} - f4;
  REQUIRE(f4.numerator() == -625747701);
  REQUIRE(f4.denominator() == 5349881);
  // negative 0
  fishbait::Fraction f5{0, 1};
  f5 = int8_t{-17} - f5;
  REQUIRE(f5.numerator() == -17);
  REQUIRE(f5.denominator() == 1);
  // 0 positive
  fishbait::Fraction f6{430693, 8939732};
  f6 = int8_t{0} - f6;
  REQUIRE(f6.numerator() == -430693);
  REQUIRE(f6.denominator() == 8939732);
  // 0 negative
  fishbait::Fraction f7{-7204183, 7240336};
  f7 = int8_t{0} - f7;
  REQUIRE(f7.numerator() == 7204183);
  REQUIRE(f7.denominator() == 7240336);
  // 0 0
  fishbait::Fraction f8{0, 1};
  f8 = int8_t{0} - f8;
  REQUIRE(f8.numerator() == 0);
  REQUIRE(f8.denominator() == 1);
}  // TEST_CASE "int8_t minus fraction"

TEST_CASE("fraction minus int8_t", "[utils][fraction]") {
  // positive positive
  fishbait::Fraction f0{573193, 141213};
  f0 = f0 - int8_t{90};
  REQUIRE(f0.numerator() == -12135977);
  REQUIRE(f0.denominator() == 141213);
  // positive negative
  fishbait::Fraction f1{166361, 5546699};
  f1 = f1 - int8_t{-14};
  REQUIRE(f1.numerator() == 77820147);
  REQUIRE(f1.denominator() == 5546699);
  // positive 0
  fishbait::Fraction f2{2173077, 863408};
  f2 = f2 - int8_t{0};
  REQUIRE(f2.numerator() == 2173077);
  REQUIRE(f2.denominator() == 863408);
  // negative positive
  fishbait::Fraction f3{-7936451, 2051336};
  f3 = f3 - int8_t{31};
  REQUIRE(f3.numerator() == -71527867);
  REQUIRE(f3.denominator() == 2051336);
  // negative negative
  fishbait::Fraction f4{-6621422, 9098799};
  f4 = f4 - int8_t{-41};
  REQUIRE(f4.numerator() == 366429337);
  REQUIRE(f4.denominator() == 9098799);
  // negative 0
  fishbait::Fraction f5{-619193, 9922632};
  f5 = f5 - int8_t{0};
  REQUIRE(f5.numerator() == -619193);
  REQUIRE(f5.denominator() == 9922632);
  // 0 positive
  fishbait::Fraction f6{0, 1};
  f6 = f6 - int8_t{108};
  REQUIRE(f6.numerator() == -108);
  REQUIRE(f6.denominator() == 1);
  // 0 negative
  fishbait::Fraction f7{0, 1};
  f7 = f7 - int8_t{-63};
  REQUIRE(f7.numerator() == 63);
  REQUIRE(f7.denominator() == 1);
  // 0 0
  fishbait::Fraction f8{0, 1};
  f8 = f8 - int8_t{0};
  REQUIRE(f8.numerator() == 0);
  REQUIRE(f8.denominator() == 1);
}  // TEST_CASE "fraction minus int8_t"

TEST_CASE("fraction minus equals uint8_t", "[utils][fraction]") {
  // positive positive
  fishbait::Fraction f0{1636963, 3697979};
  f0 -= uint8_t{167};
  REQUIRE(f0.numerator() == -615925530);
  REQUIRE(f0.denominator() == 3697979);
  // positive 0
  fishbait::Fraction f1{2308566, 9932287};
  f1 -= uint8_t{0};
  REQUIRE(f1.numerator() == 2308566);
  REQUIRE(f1.denominator() == 9932287);
  // negative positive
  fishbait::Fraction f2{-359560, 30911};
  f2 -= uint8_t{188};
  REQUIRE(f2.numerator() == -6170828);
  REQUIRE(f2.denominator() == 30911);
  // negative 0
  fishbait::Fraction f3{-3021988, 3517849};
  f3 -= uint8_t{0};
  REQUIRE(f3.numerator() == -3021988);
  REQUIRE(f3.denominator() == 3517849);
  // 0 positive
  fishbait::Fraction f4{0, 1};
  f4 -= uint8_t{136};
  REQUIRE(f4.numerator() == -136);
  REQUIRE(f4.denominator() == 1);
  // 0 0
  fishbait::Fraction f5{0, 1};
  f5 -= uint8_t{0};
  REQUIRE(f5.numerator() == 0);
  REQUIRE(f5.denominator() == 1);
}  // TEST_CASE "fraction minus equals uint8_t"

TEST_CASE("uint8_t minus fraction", "[utils][fraction]") {
  // positive positive
  fishbait::Fraction f0{385858, 658463};
  f0 = uint8_t{135} - f0;
  REQUIRE(f0.numerator() == 88506647);
  REQUIRE(f0.denominator() == 658463);
  // positive negative
  fishbait::Fraction f1{-843757, 792310};
  f1 = uint8_t{172} - f1;
  REQUIRE(f1.numerator() == 137121077);
  REQUIRE(f1.denominator() == 792310);
  // positive 0
  fishbait::Fraction f2{0, 1};
  f2 = uint8_t{6} - f2;
  REQUIRE(f2.numerator() == 6);
  REQUIRE(f2.denominator() == 1);
  // 0 positive
  fishbait::Fraction f3{70279, 1749179};
  f3 = uint8_t{0} - f3;
  REQUIRE(f3.numerator() == -70279);
  REQUIRE(f3.denominator() == 1749179);
  // 0 negative
  fishbait::Fraction f4{-9678171, 4186867};
  f4 = uint8_t{0} - f4;
  REQUIRE(f4.numerator() == 9678171);
  REQUIRE(f4.denominator() == 4186867);
  // 0 0
  fishbait::Fraction f5{0, 1};
  f5 = uint8_t{0} - f5;
  REQUIRE(f5.numerator() == 0);
  REQUIRE(f5.denominator() == 1);
}  // TEST_CASE "uint8_t minus fraction"

TEST_CASE("fraction minus uint8_t", "[utils][fraction]") {
  // positive positive
  fishbait::Fraction f0{3025217, 539560};
  f0 = f0 - uint8_t{132};
  REQUIRE(f0.numerator() == -68196703);
  REQUIRE(f0.denominator() == 539560);
  // positive 0
  fishbait::Fraction f1{970408, 240815};
  f1 = f1 - uint8_t{0};
  REQUIRE(f1.numerator() == 970408);
  REQUIRE(f1.denominator() == 240815);
  // negative positive
  fishbait::Fraction f2{-9316093, 3476097};
  f2 = f2 - uint8_t{37};
  REQUIRE(f2.numerator() == -137931682);
  REQUIRE(f2.denominator() == 3476097);
  // negative 0
  fishbait::Fraction f3{-8415989, 814257};
  f3 = f3 - uint8_t{0};
  REQUIRE(f3.numerator() == -8415989);
  REQUIRE(f3.denominator() == 814257);
  // 0 positive
  fishbait::Fraction f4{0, 1};
  f4 = f4 - uint8_t{174};
  REQUIRE(f4.numerator() == -174);
  REQUIRE(f4.denominator() == 1);
  // 0 0
  fishbait::Fraction f5{0, 1};
  f5 = f5 - uint8_t{0};
  REQUIRE(f5.numerator() == 0);
  REQUIRE(f5.denominator() == 1);
}  // TEST_CASE "fraction minus uint8_t"

TEST_CASE("fraction minus equals int16_t", "[utils][fraction]") {
  // positive positive
  fishbait::Fraction f0{198170, 245569};
  f0 -= int16_t{18388};
  REQUIRE(f0.numerator() == -4515324602);
  REQUIRE(f0.denominator() == 245569);
  // positive negative
  fishbait::Fraction f1{4237136, 1567289};
  f1 -= int16_t{-10239};
  REQUIRE(f1.numerator() == 16051709207);
  REQUIRE(f1.denominator() == 1567289);
  // positive 0
  fishbait::Fraction f2{264007, 4845277};
  f2 -= int16_t{0};
  REQUIRE(f2.numerator() == 264007);
  REQUIRE(f2.denominator() == 4845277);
  // negative positive
  fishbait::Fraction f3{-3091409, 263042};
  f3 -= int16_t{583};
  REQUIRE(f3.numerator() == -156444895);
  REQUIRE(f3.denominator() == 263042);
  // negative negative
  fishbait::Fraction f4{-8171893, 3019526};
  f4 -= int16_t{-17647};
  REQUIRE(f4.numerator() == 53277403429);
  REQUIRE(f4.denominator() == 3019526);
  // negative 0
  fishbait::Fraction f5{-141267, 8238200};
  f5 -= int16_t{0};
  REQUIRE(f5.numerator() == -141267);
  REQUIRE(f5.denominator() == 8238200);
  // 0 positive
  fishbait::Fraction f6{0, 1};
  f6 -= int16_t{13264};
  REQUIRE(f6.numerator() == -13264);
  REQUIRE(f6.denominator() == 1);
  // 0 negative
  fishbait::Fraction f7{0, 1};
  f7 -= int16_t{-29366};
  REQUIRE(f7.numerator() == 29366);
  REQUIRE(f7.denominator() == 1);
  // 0 0
  fishbait::Fraction f8{0, 1};
  f8 -= int16_t{0};
  REQUIRE(f8.numerator() == 0);
  REQUIRE(f8.denominator() == 1);
}  // TEST_CASE "fraction minus equals int16_t"

TEST_CASE("int16_t minus fraction", "[utils][fraction]") {
  // positive positive
  fishbait::Fraction f0{5151173, 1901601};
  f0 = int16_t{19212} - f0;
  REQUIRE(f0.numerator() == 36528407239);
  REQUIRE(f0.denominator() == 1901601);
  // positive negative
  fishbait::Fraction f1{-536182, 4697303};
  f1 = int16_t{3137} - f1;
  REQUIRE(f1.numerator() == 14735975693);
  REQUIRE(f1.denominator() == 4697303);
  // positive 0
  fishbait::Fraction f2{0, 1};
  f2 = int16_t{2037} - f2;
  REQUIRE(f2.numerator() == 2037);
  REQUIRE(f2.denominator() == 1);
  // negative positive
  fishbait::Fraction f3{2853197, 6509520};
  f3 = int16_t{-22971} - f3;
  REQUIRE(f3.numerator() == -149533037117);
  REQUIRE(f3.denominator() == 6509520);
  // negative negative
  fishbait::Fraction f4{-3633106, 8172363};
  f4 = int16_t{-13688} - f4;
  REQUIRE(f4.numerator() == -111859671638);
  REQUIRE(f4.denominator() == 8172363);
  // negative 0
  fishbait::Fraction f5{0, 1};
  f5 = int16_t{-8169} - f5;
  REQUIRE(f5.numerator() == -8169);
  REQUIRE(f5.denominator() == 1);
  // 0 positive
  fishbait::Fraction f6{2569032, 105485};
  f6 = int16_t{0} - f6;
  REQUIRE(f6.numerator() == -2569032);
  REQUIRE(f6.denominator() == 105485);
  // 0 negative
  fishbait::Fraction f7{-2486137, 4698668};
  f7 = int16_t{0} - f7;
  REQUIRE(f7.numerator() == 2486137);
  REQUIRE(f7.denominator() == 4698668);
  // 0 0
  fishbait::Fraction f8{0, 1};
  f8 = int16_t{0} - f8;
  REQUIRE(f8.numerator() == 0);
  REQUIRE(f8.denominator() == 1);
}  // TEST_CASE "int16_t minus fraction"

TEST_CASE("fraction minus int16_t", "[utils][fraction]") {
  // positive positive
  fishbait::Fraction f0{4992647, 354984};
  f0 = f0 - int16_t{24313};
  REQUIRE(f0.numerator() == -8625733345);
  REQUIRE(f0.denominator() == 354984);
  // positive negative
  fishbait::Fraction f1{2814167, 1446646};
  f1 = f1 - int16_t{-6252};
  REQUIRE(f1.numerator() == 9047244959);
  REQUIRE(f1.denominator() == 1446646);
  // positive 0
  fishbait::Fraction f2{726203, 2481706};
  f2 = f2 - int16_t{0};
  REQUIRE(f2.numerator() == 726203);
  REQUIRE(f2.denominator() == 2481706);
  // negative positive
  fishbait::Fraction f3{-5544819, 3195754};
  f3 = f3 - int16_t{13096};
  REQUIRE(f3.numerator() == -41857139203);
  REQUIRE(f3.denominator() == 3195754);
  // negative negative
  fishbait::Fraction f4{-8880109, 639479};
  f4 = f4 - int16_t{-5492};
  REQUIRE(f4.numerator() == 3503138559);
  REQUIRE(f4.denominator() == 639479);
  // negative 0
  fishbait::Fraction f5{-4335657, 417500};
  f5 = f5 - int16_t{0};
  REQUIRE(f5.numerator() == -4335657);
  REQUIRE(f5.denominator() == 417500);
  // 0 positive
  fishbait::Fraction f6{0, 1};
  f6 = f6 - int16_t{23535};
  REQUIRE(f6.numerator() == -23535);
  REQUIRE(f6.denominator() == 1);
  // 0 negative
  fishbait::Fraction f7{0, 1};
  f7 = f7 - int16_t{-9882};
  REQUIRE(f7.numerator() == 9882);
  REQUIRE(f7.denominator() == 1);
  // 0 0
  fishbait::Fraction f8{0, 1};
  f8 = f8 - int16_t{0};
  REQUIRE(f8.numerator() == 0);
  REQUIRE(f8.denominator() == 1);
}  // TEST_CASE "fraction minus int16_t"

TEST_CASE("fraction minus equals uint16_t", "[utils][fraction]") {
  // positive positive
  fishbait::Fraction f0{7917147, 2226569};
  f0 -= uint16_t{2558};
  REQUIRE(f0.numerator() == -5687646355);
  REQUIRE(f0.denominator() == 2226569);
  // positive 0
  fishbait::Fraction f1{254137, 2165556};
  f1 -= uint16_t{0};
  REQUIRE(f1.numerator() == 254137);
  REQUIRE(f1.denominator() == 2165556);
  // negative positive
  fishbait::Fraction f2{-631179, 774479};
  f2 -= uint16_t{37201};
  REQUIRE(f2.numerator() == -28812024458);
  REQUIRE(f2.denominator() == 774479);
  // negative 0
  fishbait::Fraction f3{-1034780, 233003};
  f3 -= uint16_t{0};
  REQUIRE(f3.numerator() == -1034780);
  REQUIRE(f3.denominator() == 233003);
  // 0 positive
  fishbait::Fraction f4{0, 1};
  f4 -= uint16_t{50415};
  REQUIRE(f4.numerator() == -50415);
  REQUIRE(f4.denominator() == 1);
  // 0 0
  fishbait::Fraction f5{0, 1};
  f5 -= uint16_t{0};
  REQUIRE(f5.numerator() == 0);
  REQUIRE(f5.denominator() == 1);
}  // TEST_CASE "fraction minus equals uint16_t"

TEST_CASE("uint16_t minus fraction", "[utils][fraction]") {
  // positive positive
  fishbait::Fraction f0{712079, 551223};
  f0 = uint16_t{63163} - f0;
  REQUIRE(f0.numerator() == 34816186270);
  REQUIRE(f0.denominator() == 551223);
  // positive negative
  fishbait::Fraction f1{-4854472, 1003673};
  f1 = uint16_t{50956} - f1;
  REQUIRE(f1.numerator() == 51148015860);
  REQUIRE(f1.denominator() == 1003673);
  // positive 0
  fishbait::Fraction f2{0, 1};
  f2 = uint16_t{9399} - f2;
  REQUIRE(f2.numerator() == 9399);
  REQUIRE(f2.denominator() == 1);
  // 0 positive
  fishbait::Fraction f3{641399, 9873458};
  f3 = uint16_t{0} - f3;
  REQUIRE(f3.numerator() == -641399);
  REQUIRE(f3.denominator() == 9873458);
  // 0 negative
  fishbait::Fraction f4{-1372233, 2828771};
  f4 = uint16_t{0} - f4;
  REQUIRE(f4.numerator() == 1372233);
  REQUIRE(f4.denominator() == 2828771);
  // 0 0
  fishbait::Fraction f5{0, 1};
  f5 = uint16_t{0} - f5;
  REQUIRE(f5.numerator() == 0);
  REQUIRE(f5.denominator() == 1);
}  // TEST_CASE "uint16_t minus fraction"

TEST_CASE("fraction minus uint16_t", "[utils][fraction]") {
  // positive positive
  fishbait::Fraction f0{82121, 71358};
  f0 = f0 - uint16_t{64688};
  REQUIRE(f0.numerator() == -4615924183);
  REQUIRE(f0.denominator() == 71358);
  // positive 0
  fishbait::Fraction f1{6266413, 9405245};
  f1 = f1 - uint16_t{0};
  REQUIRE(f1.numerator() == 6266413);
  REQUIRE(f1.denominator() == 9405245);
  // negative positive
  fishbait::Fraction f2{-1788103, 4188499};
  f2 = f2 - uint16_t{40091};
  REQUIRE(f2.numerator() == -167922901512);
  REQUIRE(f2.denominator() == 4188499);
  // negative 0
  fishbait::Fraction f3{-6580391, 3156659};
  f3 = f3 - uint16_t{0};
  REQUIRE(f3.numerator() == -6580391);
  REQUIRE(f3.denominator() == 3156659);
  // 0 positive
  fishbait::Fraction f4{0, 1};
  f4 = f4 - uint16_t{43067};
  REQUIRE(f4.numerator() == -43067);
  REQUIRE(f4.denominator() == 1);
  // 0 0
  fishbait::Fraction f5{0, 1};
  f5 = f5 - uint16_t{0};
  REQUIRE(f5.numerator() == 0);
  REQUIRE(f5.denominator() == 1);
}  // TEST_CASE "fraction minus uint16_t"

TEST_CASE("fraction minus equals int32_t", "[utils][fraction]") {
  // positive positive
  fishbait::Fraction f0{5251293, 919115};
  f0 -= int32_t{1925871};
  REQUIRE(f0.numerator() == -1770091672872);
  REQUIRE(f0.denominator() == 919115);
  // positive negative
  fishbait::Fraction f1{9913704, 6431243};
  f1 -= int32_t{-3909651};
  REQUIRE(f1.numerator() == 25143925539897);
  REQUIRE(f1.denominator() == 6431243);
  // positive 0
  fishbait::Fraction f2{845095, 1843688};
  f2 -= int32_t{0};
  REQUIRE(f2.numerator() == 845095);
  REQUIRE(f2.denominator() == 1843688);
  // negative positive
  fishbait::Fraction f3{-6722836, 2650931};
  f3 -= int32_t{7877695};
  REQUIRE(f3.numerator() == -20883232606881);
  REQUIRE(f3.denominator() == 2650931);
  // negative negative
  fishbait::Fraction f4{-9485800, 6558223};
  f4 -= int32_t{-2362593};
  REQUIRE(f4.numerator() == 15494402266439);
  REQUIRE(f4.denominator() == 6558223);
  // negative 0
  fishbait::Fraction f5{-2468365, 2429298};
  f5 -= int32_t{0};
  REQUIRE(f5.numerator() == -2468365);
  REQUIRE(f5.denominator() == 2429298);
  // 0 positive
  fishbait::Fraction f6{0, 1};
  f6 -= int32_t{2128308};
  REQUIRE(f6.numerator() == -2128308);
  REQUIRE(f6.denominator() == 1);
  // 0 negative
  fishbait::Fraction f7{0, 1};
  f7 -= int32_t{-1429662};
  REQUIRE(f7.numerator() == 1429662);
  REQUIRE(f7.denominator() == 1);
  // 0 0
  fishbait::Fraction f8{0, 1};
  f8 -= int32_t{0};
  REQUIRE(f8.numerator() == 0);
  REQUIRE(f8.denominator() == 1);
}  // TEST_CASE "fraction minus equals int32_t"

TEST_CASE("int32_t minus fraction", "[utils][fraction]") {
  // positive positive
  fishbait::Fraction f0{2133736, 1678527};
  f0 = int32_t{5882131} - f0;
  REQUIRE(f0.numerator() == 9873313567301);
  REQUIRE(f0.denominator() == 1678527);
  // positive negative
  fishbait::Fraction f1{-9798034, 3089507};
  f1 = int32_t{9685856} - f1;
  REQUIRE(f1.numerator() == 29924529711026);
  REQUIRE(f1.denominator() == 3089507);
  // positive 0
  fishbait::Fraction f2{0, 1};
  f2 = int32_t{5635995} - f2;
  REQUIRE(f2.numerator() == 5635995);
  REQUIRE(f2.denominator() == 1);
  // negative positive
  fishbait::Fraction f3{3493759, 4853979};
  f3 = int32_t{-8590202} - f3;
  REQUIRE(f3.numerator() == -41696663607517);
  REQUIRE(f3.denominator() == 4853979);
  // negative negative
  fishbait::Fraction f4{-9964776, 2157665};
  f4 = int32_t{-7487494} - f4;
  REQUIRE(f4.numerator() == -16155493776734);
  REQUIRE(f4.denominator() == 2157665);
  // negative 0
  fishbait::Fraction f5{0, 1};
  f5 = int32_t{-9122780} - f5;
  REQUIRE(f5.numerator() == -9122780);
  REQUIRE(f5.denominator() == 1);
  // 0 positive
  fishbait::Fraction f6{4017683, 9318527};
  f6 = int32_t{0} - f6;
  REQUIRE(f6.numerator() == -4017683);
  REQUIRE(f6.denominator() == 9318527);
  // 0 negative
  fishbait::Fraction f7{-1460674, 520439};
  f7 = int32_t{0} - f7;
  REQUIRE(f7.numerator() == 1460674);
  REQUIRE(f7.denominator() == 520439);
  // 0 0
  fishbait::Fraction f8{0, 1};
  f8 = int32_t{0} - f8;
  REQUIRE(f8.numerator() == 0);
  REQUIRE(f8.denominator() == 1);
}  // TEST_CASE "int32_t minus fraction"

TEST_CASE("fraction minus int32_t", "[utils][fraction]") {
  // positive positive
  fishbait::Fraction f0{8974779, 301711};
  f0 = f0 - int32_t{950654};
  REQUIRE(f0.numerator() == -286813794215);
  REQUIRE(f0.denominator() == 301711);
  // positive negative
  fishbait::Fraction f1{7357744, 8441987};
  f1 = f1 - int32_t{-2056113};
  REQUIRE(f1.numerator() == 17357686574275);
  REQUIRE(f1.denominator() == 8441987);
  // positive 0
  fishbait::Fraction f2{1695104, 403501};
  f2 = f2 - int32_t{0};
  REQUIRE(f2.numerator() == 1695104);
  REQUIRE(f2.denominator() == 403501);
  // negative positive
  fishbait::Fraction f3{-4508235, 6322927};
  f3 = f3 - int32_t{3607466};
  REQUIRE(f3.numerator() == -22809748681217);
  REQUIRE(f3.denominator() == 6322927);
  // negative negative
  fishbait::Fraction f4{-6098464, 3820483};
  f4 = f4 - int32_t{-1827492};
  REQUIRE(f4.numerator() == 6981896020172);
  REQUIRE(f4.denominator() == 3820483);
  // negative 0
  fishbait::Fraction f5{-8121259, 6705260};
  f5 = f5 - int32_t{0};
  REQUIRE(f5.numerator() == -8121259);
  REQUIRE(f5.denominator() == 6705260);
  // 0 positive
  fishbait::Fraction f6{0, 1};
  f6 = f6 - int32_t{3295430};
  REQUIRE(f6.numerator() == -3295430);
  REQUIRE(f6.denominator() == 1);
  // 0 negative
  fishbait::Fraction f7{0, 1};
  f7 = f7 - int32_t{-965595};
  REQUIRE(f7.numerator() == 965595);
  REQUIRE(f7.denominator() == 1);
  // 0 0
  fishbait::Fraction f8{0, 1};
  f8 = f8 - int32_t{0};
  REQUIRE(f8.numerator() == 0);
  REQUIRE(f8.denominator() == 1);
}  // TEST_CASE "fraction minus int32_t"

TEST_CASE("fraction minus equals uint32_t", "[utils][fraction]") {
  // positive positive
  fishbait::Fraction f0{2960361, 3609362};
  f0 -= uint32_t{6106639};
  REQUIRE(f0.numerator() == -22041067793957);
  REQUIRE(f0.denominator() == 3609362);
  // positive 0
  fishbait::Fraction f1{4934913, 3467300};
  f1 -= uint32_t{0};
  REQUIRE(f1.numerator() == 4934913);
  REQUIRE(f1.denominator() == 3467300);
  // negative positive
  fishbait::Fraction f2{-5792777, 5221908};
  f2 -= uint32_t{3772609};
  REQUIRE(f2.numerator() == -19700222910749);
  REQUIRE(f2.denominator() == 5221908);
  // negative 0
  fishbait::Fraction f3{-6310991, 5943088};
  f3 -= uint32_t{0};
  REQUIRE(f3.numerator() == -6310991);
  REQUIRE(f3.denominator() == 5943088);
  // 0 positive
  fishbait::Fraction f4{0, 1};
  f4 -= uint32_t{4120989};
  REQUIRE(f4.numerator() == -4120989);
  REQUIRE(f4.denominator() == 1);
  // 0 0
  fishbait::Fraction f5{0, 1};
  f5 -= uint32_t{0};
  REQUIRE(f5.numerator() == 0);
  REQUIRE(f5.denominator() == 1);
}  // TEST_CASE "fraction minus equals uint32_t"

TEST_CASE("uint32_t minus fraction", "[utils][fraction]") {
  // positive positive
  fishbait::Fraction f0{8095439, 7977531};
  f0 = uint32_t{7838334} - f0;
  REQUIRE(f0.numerator() == 62530544377915);
  REQUIRE(f0.denominator() == 7977531);
  // positive negative
  fishbait::Fraction f1{-1284259, 4117600};
  f1 = uint32_t{6913556} - f1;
  REQUIRE(f1.numerator() == 28467259469859);
  REQUIRE(f1.denominator() == 4117600);
  // positive 0
  fishbait::Fraction f2{0, 1};
  f2 = uint32_t{5707725} - f2;
  REQUIRE(f2.numerator() == 5707725);
  REQUIRE(f2.denominator() == 1);
  // 0 positive
  fishbait::Fraction f3{2923561, 3276818};
  f3 = uint32_t{0} - f3;
  REQUIRE(f3.numerator() == -2923561);
  REQUIRE(f3.denominator() == 3276818);
  // 0 negative
  fishbait::Fraction f4{-8837053, 7009602};
  f4 = uint32_t{0} - f4;
  REQUIRE(f4.numerator() == 8837053);
  REQUIRE(f4.denominator() == 7009602);
  // 0 0
  fishbait::Fraction f5{0, 1};
  f5 = uint32_t{0} - f5;
  REQUIRE(f5.numerator() == 0);
  REQUIRE(f5.denominator() == 1);
}  // TEST_CASE "uint32_t minus fraction"

TEST_CASE("fraction minus uint32_t", "[utils][fraction]") {
  // positive positive
  fishbait::Fraction f0{6125173, 6594779};
  f0 = f0 - uint32_t{2016812};
  REQUIRE(f0.numerator() == -13300423299375);
  REQUIRE(f0.denominator() == 6594779);
  // positive 0
  fishbait::Fraction f1{5886688, 3931223};
  f1 = f1 - uint32_t{0};
  REQUIRE(f1.numerator() == 5886688);
  REQUIRE(f1.denominator() == 3931223);
  // negative positive
  fishbait::Fraction f2{-5664559, 3009912};
  f2 = f2 - uint32_t{1031496};
  REQUIRE(f2.numerator() == -3104717852911);
  REQUIRE(f2.denominator() == 3009912);
  // negative 0
  fishbait::Fraction f3{-4620968, 8004461};
  f3 = f3 - uint32_t{0};
  REQUIRE(f3.numerator() == -4620968);
  REQUIRE(f3.denominator() == 8004461);
  // 0 positive
  fishbait::Fraction f4{0, 1};
  f4 = f4 - uint32_t{6143721};
  REQUIRE(f4.numerator() == -6143721);
  REQUIRE(f4.denominator() == 1);
  // 0 0
  fishbait::Fraction f5{0, 1};
  f5 = f5 - uint32_t{0};
  REQUIRE(f5.numerator() == 0);
  REQUIRE(f5.denominator() == 1);
}  // TEST_CASE "fraction minus uint32_t"

TEST_CASE("fraction minus equals int64_t", "[utils][fraction]") {
  // positive positive
  fishbait::Fraction f0{318247, 339774};
  f0 -= int64_t{8958593};
  REQUIRE(f0.numerator() == -3043896659735);
  REQUIRE(f0.denominator() == 339774);
  // positive negative
  fishbait::Fraction f1{175291, 2315378};
  f1 -= int64_t{-8381343};
  REQUIRE(f1.numerator() == 19405977367945);
  REQUIRE(f1.denominator() == 2315378);
  // positive 0
  fishbait::Fraction f2{148070, 131593};
  f2 -= int64_t{0};
  REQUIRE(f2.numerator() == 148070);
  REQUIRE(f2.denominator() == 131593);
  // negative positive
  fishbait::Fraction f3{-5417234, 2768087};
  f3 -= int64_t{3321229};
  REQUIRE(f3.numerator() == -9193456236157);
  REQUIRE(f3.denominator() == 2768087);
  // negative negative
  fishbait::Fraction f4{-7749818, 3594127};
  f4 -= int64_t{-6489706};
  REQUIRE(f4.numerator() == 23324819806844);
  REQUIRE(f4.denominator() == 3594127);
  // negative 0
  fishbait::Fraction f5{-131213, 3239260};
  f5 -= int64_t{0};
  REQUIRE(f5.numerator() == -131213);
  REQUIRE(f5.denominator() == 3239260);
  // 0 positive
  fishbait::Fraction f6{0, 1};
  f6 -= int64_t{6491530};
  REQUIRE(f6.numerator() == -6491530);
  REQUIRE(f6.denominator() == 1);
  // 0 negative
  fishbait::Fraction f7{0, 1};
  f7 -= int64_t{-3043228};
  REQUIRE(f7.numerator() == 3043228);
  REQUIRE(f7.denominator() == 1);
  // 0 0
  fishbait::Fraction f8{0, 1};
  f8 -= int64_t{0};
  REQUIRE(f8.numerator() == 0);
  REQUIRE(f8.denominator() == 1);
}  // TEST_CASE "fraction minus equals int64_t"

TEST_CASE("int64_t minus fraction", "[utils][fraction]") {
  // positive positive
  fishbait::Fraction f0{2172423, 2020603};
  f0 = int64_t{6326398} - f0;
  REQUIRE(f0.numerator() == 12783136605571);
  REQUIRE(f0.denominator() == 2020603);
  // positive negative
  fishbait::Fraction f1{-1942040, 2197503};
  f1 = int64_t{8331371} - f1;
  REQUIRE(f1.numerator() == 18308214708653);
  REQUIRE(f1.denominator() == 2197503);
  // positive 0
  fishbait::Fraction f2{0, 1};
  f2 = int64_t{3145003} - f2;
  REQUIRE(f2.numerator() == 3145003);
  REQUIRE(f2.denominator() == 1);
  // negative positive
  fishbait::Fraction f3{6437117, 5984371};
  f3 = int64_t{-4687663} - f3;
  REQUIRE(f3.numerator() == -28052720952090);
  REQUIRE(f3.denominator() == 5984371);
  // negative negative
  fishbait::Fraction f4{-7295002, 7422733};
  f4 = int64_t{-1470409} - f4;
  REQUIRE(f4.numerator() == -10914446112795);
  REQUIRE(f4.denominator() == 7422733);
  // negative 0
  fishbait::Fraction f5{0, 1};
  f5 = int64_t{-3955821} - f5;
  REQUIRE(f5.numerator() == -3955821);
  REQUIRE(f5.denominator() == 1);
  // 0 positive
  fishbait::Fraction f6{4352452, 5535121};
  f6 = int64_t{0} - f6;
  REQUIRE(f6.numerator() == -4352452);
  REQUIRE(f6.denominator() == 5535121);
  // 0 negative
  fishbait::Fraction f7{-28945, 52919};
  f7 = int64_t{0} - f7;
  REQUIRE(f7.numerator() == 28945);
  REQUIRE(f7.denominator() == 52919);
  // 0 0
  fishbait::Fraction f8{0, 1};
  f8 = int64_t{0} - f8;
  REQUIRE(f8.numerator() == 0);
  REQUIRE(f8.denominator() == 1);
}  // TEST_CASE "int64_t minus fraction"

TEST_CASE("fraction minus int64_t", "[utils][fraction]") {
  // positive positive
  fishbait::Fraction f0{1400431, 436563};
  f0 = f0 - int64_t{8975680};
  REQUIRE(f0.numerator() == -3918448387409);
  REQUIRE(f0.denominator() == 436563);
  // positive negative
  fishbait::Fraction f1{4571184, 2161727};
  f1 = f1 - int64_t{-35048};
  REQUIRE(f1.numerator() == 75768779080);
  REQUIRE(f1.denominator() == 2161727);
  // positive 0
  fishbait::Fraction f2{2803200, 9413143};
  f2 = f2 - int64_t{0};
  REQUIRE(f2.numerator() == 2803200);
  REQUIRE(f2.denominator() == 9413143);
  // negative positive
  fishbait::Fraction f3{-42451, 230772};
  f3 = f3 - int64_t{9581844};
  REQUIRE(f3.numerator() == -2211221346019);
  REQUIRE(f3.denominator() == 230772);
  // negative negative
  fishbait::Fraction f4{-4757751, 1010813};
  f4 = f4 - int64_t{-4717366};
  REQUIRE(f4.numerator() == 4768370120807);
  REQUIRE(f4.denominator() == 1010813);
  // negative 0
  fishbait::Fraction f5{-9745703, 3625302};
  f5 = f5 - int64_t{0};
  REQUIRE(f5.numerator() == -9745703);
  REQUIRE(f5.denominator() == 3625302);
  // 0 positive
  fishbait::Fraction f6{0, 1};
  f6 = f6 - int64_t{4861205};
  REQUIRE(f6.numerator() == -4861205);
  REQUIRE(f6.denominator() == 1);
  // 0 negative
  fishbait::Fraction f7{0, 1};
  f7 = f7 - int64_t{-8530142};
  REQUIRE(f7.numerator() == 8530142);
  REQUIRE(f7.denominator() == 1);
  // 0 0
  fishbait::Fraction f8{0, 1};
  f8 = f8 - int64_t{0};
  REQUIRE(f8.numerator() == 0);
  REQUIRE(f8.denominator() == 1);
}  // TEST_CASE "fraction minus int64_t"

TEST_CASE("double minus fraction", "[utils][fraction]") {
  // positive positive
  fishbait::Fraction f0{3568779, 4308574};
  REQUIRE(8708732.961218623 - f0 == Approx(8708732.132921604));
  // positive negative
  fishbait::Fraction f1{-699793, 1258746};
  REQUIRE(557806.6389891679 - f1 == Approx(557807.194933735));
  // positive 0
  fishbait::Fraction f2{0, 1};
  REQUIRE(6977518.277723202 - f2 == Approx(6977518.277723202));
  // negative positive
  fishbait::Fraction f3{8682931, 3033697};
  REQUIRE(-7270722.1105922805 - f3 == Approx(-7270724.972753861));
  // negative negative
  fishbait::Fraction f4{-8099435, 2124168};
  REQUIRE(-3380390.155571599 - f4 == Approx(-3380386.342579877));
  // negative 0
  fishbait::Fraction f5{0, 1};
  REQUIRE(-3508906.2744100466 - f5 == Approx(-3508906.2744100466));
  // 0 positive
  fishbait::Fraction f6{6514416, 9502811};
  REQUIRE(0.0 - f6 == Approx(-0.6855251567141554));
  // 0 negative
  fishbait::Fraction f7{-5401098, 1306093};
  REQUIRE(0.0 - f7 == Approx(4.135308894542732));
  // 0 0
  fishbait::Fraction f8{0, 1};
  REQUIRE(0.0 - f8 == Approx(0.0));
}  // TEST_CASE "double minus fraction"

TEST_CASE("fraction minus double", "[utils][fraction]") {
  // positive positive
  fishbait::Fraction f0{523408, 302181};
  REQUIRE(f0 - 832121.355225241 == Approx(-832119.6231242816));
  // positive negative
  fishbait::Fraction f1{2726866, 2405177};
  REQUIRE(f1 - -4131588.454180516 == Approx(4131589.5879290923));
  // positive 0
  fishbait::Fraction f2{9785519, 3848301};
  REQUIRE(f2 - 0.0 == Approx(2.542815387881561));
  // negative positive
  fishbait::Fraction f3{-9643760, 8629511};
  REQUIRE(f3 - 7546134.679961854 == Approx(-7546135.797494469));
  // negative negative
  fishbait::Fraction f4{-1120966, 3177115};
  REQUIRE(f4 - -8994005.193033742 == Approx(8994004.840208616));
  // negative 0
  fishbait::Fraction f5{-1190751, 4762748};
  REQUIRE(f5 - 0.0 == Approx(-0.2500134376204662));
  // 0 positive
  fishbait::Fraction f6{0, 1};
  REQUIRE(f6 - 4134142.2309708055 == Approx(-4134142.2309708055));
  // 0 negative
  fishbait::Fraction f7{0, 1};
  REQUIRE(f7 - -5865238.95115592 == Approx(5865238.95115592));
  // 0 0
  fishbait::Fraction f8{0, 1};
  REQUIRE(f8 - 0.0 == Approx(0.0));
}  // TEST_CASE "fraction minus double"

TEST_CASE("fraction minus fraction", "[utils][fraction]") {
  // positive positive
  fishbait::Fraction f0 = fishbait::Fraction(5797076, 6912101) -
                          fishbait::Fraction(6435366, 7952381);
  REQUIRE(f0.numerator() == 1618657273990);
  REQUIRE(f0.denominator() == 54967660662481);
  // positive negative
  fishbait::Fraction f1 = fishbait::Fraction(5090813, 6929121) -
                          fishbait::Fraction(-64609, 1882624);
  REQUIRE(f1.numerator() == 10031770312001);
  REQUIRE(f1.denominator() == 13044929493504);
  // positive 0
  fishbait::Fraction f2 = fishbait::Fraction(8531732, 3552485) -
                          fishbait::Fraction(0, 1);
  REQUIRE(f2.numerator() == 8531732);
  REQUIRE(f2.denominator() == 3552485);
  // negative positive
  fishbait::Fraction f3 = fishbait::Fraction(-1258505, 1018883) -
                          fishbait::Fraction(566192, 772351);
  REQUIRE(f3.numerator() == -1548890998791);
  REQUIRE(f3.denominator() == 786935303933);
  // negative negative
  fishbait::Fraction f4 = fishbait::Fraction(-2035043, 7459503) -
                          fishbait::Fraction(-9732243, 5877002);
  REQUIRE(f4.numerator() == 60637744074143);
  REQUIRE(f4.denominator() == 43839514050006);
  // negative 0
  fishbait::Fraction f5 = fishbait::Fraction(-2144643, 1370075) -
                          fishbait::Fraction(0, 1);
  REQUIRE(f5.numerator() == -2144643);
  REQUIRE(f5.denominator() == 1370075);
  // 0 positive
  fishbait::Fraction f6 = fishbait::Fraction(0, 1) -
                          fishbait::Fraction(1329326, 7223453);
  REQUIRE(f6.numerator() == -1329326);
  REQUIRE(f6.denominator() == 7223453);
  // 0 negative
  fishbait::Fraction f7 = fishbait::Fraction(0, 1) -
                          fishbait::Fraction(-1091041, 570462);
  REQUIRE(f7.numerator() == 1091041);
  REQUIRE(f7.denominator() == 570462);
  // 0 0
  fishbait::Fraction f8 = fishbait::Fraction{0, 1} - fishbait::Fraction{0, 1};
  REQUIRE(f8.numerator() == 0);
  REQUIRE(f8.denominator() == 1);
}  // TEST_CASE "fraction minus fraction"

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
/* ---------------------------- Multiplication ------------------------------ */
/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */

TEST_CASE("fraction multiply equals fraction", "[utils][fraction]") {
  // positive positive
  fishbait::Fraction f0{3486785, 7485456};
  f0 *= fishbait::Fraction{1647097, 361347};
  REQUIRE(f0.numerator() == 5743073113145);
  REQUIRE(f0.denominator() == 2704847069232);
  // positive negative
  fishbait::Fraction f1{4872031, 1545821};
  f1 *= fishbait::Fraction{-2087705, 4346904};
  REQUIRE(f1.numerator() == -10171363478855);
  REQUIRE(f1.denominator() == 6719535488184);
  // positive 0
  fishbait::Fraction f2{8287069, 4656112};
  f2 *= fishbait::Fraction{0, 1};
  REQUIRE(f2.numerator() == 0);
  REQUIRE(f2.denominator() == 1);
  // negative positive
  fishbait::Fraction f3{-5202585, 4817539};
  f3 *= fishbait::Fraction{9476753, 1504934};
  REQUIRE(f3.numerator() == -49303613006505);
  REQUIRE(f3.denominator() == 7250078237426);
  // negative negative
  fishbait::Fraction f4{-19691, 2984};
  f4 *= fishbait::Fraction{-3910069, 2662511};
  REQUIRE(f4.numerator() == 76993168679);
  REQUIRE(f4.denominator() == 7944932824);
  // negative 0
  fishbait::Fraction f5{-498661, 76359};
  f5 *= fishbait::Fraction{0, 1};
  REQUIRE(f5.numerator() == 0);
  REQUIRE(f5.denominator() == 1);
  // 0 positive
  fishbait::Fraction f6{0, 1};
  f6 *= fishbait::Fraction{5856247, 7750330};
  REQUIRE(f6.numerator() == 0);
  REQUIRE(f6.denominator() == 1);
  // 0 negative
  fishbait::Fraction f7{0, 1};
  f7 *= fishbait::Fraction{-545398, 483673};
  REQUIRE(f7.numerator() == 0);
  REQUIRE(f7.denominator() == 1);
  // 0 0
  fishbait::Fraction f8{0, 1};
  f8 *= fishbait::Fraction{0, 1};
  REQUIRE(f8.numerator() == 0);
  REQUIRE(f8.denominator() == 1);
}  // TEST_CASE "fraction multiply equals fraction"

TEST_CASE("fraction multiply equals int8_t", "[utils][fraction]") {
  // positive positive
  fishbait::Fraction f0{4358648, 4912769};
  f0 *= int8_t{73};
  REQUIRE(f0.numerator() == 318181304);
  REQUIRE(f0.denominator() == 4912769);
  // positive negative
  fishbait::Fraction f1{5235182, 8828631};
  f1 *= int8_t{-93};
  REQUIRE(f1.numerator() == -162290642);
  REQUIRE(f1.denominator() == 2942877);
  // positive 0
  fishbait::Fraction f2{5642939, 9544202};
  f2 *= int8_t{0};
  REQUIRE(f2.numerator() == 0);
  REQUIRE(f2.denominator() == 1);
  // negative positive
  fishbait::Fraction f3{-2093131, 1869001};
  f3 *= int8_t{94};
  REQUIRE(f3.numerator() == -196754314);
  REQUIRE(f3.denominator() == 1869001);
  // negative negative
  fishbait::Fraction f4{-9300277, 1052043};
  f4 *= int8_t{-77};
  REQUIRE(f4.numerator() == 716121329);
  REQUIRE(f4.denominator() == 1052043);
  // negative 0
  fishbait::Fraction f5{-6577007, 5759580};
  f5 *= int8_t{0};
  REQUIRE(f5.numerator() == 0);
  REQUIRE(f5.denominator() == 1);
  // 0 positive
  fishbait::Fraction f6{0, 1};
  f6 *= int8_t{79};
  REQUIRE(f6.numerator() == 0);
  REQUIRE(f6.denominator() == 1);
  // 0 negative
  fishbait::Fraction f7{0, 1};
  f7 *= int8_t{-13};
  REQUIRE(f7.numerator() == 0);
  REQUIRE(f7.denominator() == 1);
  // 0 0
  fishbait::Fraction f8{0, 1};
  f8 *= int8_t{0};
  REQUIRE(f8.numerator() == 0);
  REQUIRE(f8.denominator() == 1);
}  // TEST_CASE "fraction multiply equals int8_t"

TEST_CASE("int8_t multiply fraction", "[utils][fraction]") {
  // positive positive
  fishbait::Fraction f0{7887713, 3393490};
  f0 = int8_t{124} * f0;
  REQUIRE(f0.numerator() == 489038206);
  REQUIRE(f0.denominator() == 1696745);
  // positive negative
  fishbait::Fraction f1{-4085519, 1566460};
  f1 = int8_t{47} * f1;
  REQUIRE(f1.numerator() == -192019393);
  REQUIRE(f1.denominator() == 1566460);
  // positive 0
  fishbait::Fraction f2{0, 1};
  f2 = int8_t{38} * f2;
  REQUIRE(f2.numerator() == 0);
  REQUIRE(f2.denominator() == 1);
  // negative positive
  fishbait::Fraction f3{4622874, 855371};
  f3 = int8_t{-66} * f3;
  REQUIRE(f3.numerator() == -27737244);
  REQUIRE(f3.denominator() == 77761);
  // negative negative
  fishbait::Fraction f4{-3759591, 5705288};
  f4 = int8_t{-77} * f4;
  REQUIRE(f4.numerator() == 289488507);
  REQUIRE(f4.denominator() == 5705288);
  // negative 0
  fishbait::Fraction f5{0, 1};
  f5 = int8_t{-91} * f5;
  REQUIRE(f5.numerator() == 0);
  REQUIRE(f5.denominator() == 1);
  // 0 positive
  fishbait::Fraction f6{3440501, 4963202};
  f6 = int8_t{0} * f6;
  REQUIRE(f6.numerator() == 0);
  REQUIRE(f6.denominator() == 1);
  // 0 negative
  fishbait::Fraction f7{-1400405, 1335181};
  f7 = int8_t{0} * f7;
  REQUIRE(f7.numerator() == 0);
  REQUIRE(f7.denominator() == 1);
  // 0 0
  fishbait::Fraction f8{0, 1};
  f8 = int8_t{0} * f8;
  REQUIRE(f8.numerator() == 0);
  REQUIRE(f8.denominator() == 1);
}  // TEST_CASE "int8_t multiply fraction"

TEST_CASE("fraction multiply int8_t", "[utils][fraction]") {
  // positive positive
  fishbait::Fraction f0{4759598, 330637};
  f0 = f0 * int8_t{123};
  REQUIRE(f0.numerator() == 585430554);
  REQUIRE(f0.denominator() == 330637);
  // positive negative
  fishbait::Fraction f1{2785377, 481837};
  f1 = f1 * int8_t{-90};
  REQUIRE(f1.numerator() == -250683930);
  REQUIRE(f1.denominator() == 481837);
  // positive 0
  fishbait::Fraction f2{1371124, 2225257};
  f2 = f2 * int8_t{0};
  REQUIRE(f2.numerator() == 0);
  REQUIRE(f2.denominator() == 1);
  // negative positive
  fishbait::Fraction f3{-55716, 344807};
  f3 = f3 * int8_t{89};
  REQUIRE(f3.numerator() == -4958724);
  REQUIRE(f3.denominator() == 344807);
  // negative negative
  fishbait::Fraction f4{-5322809, 1667251};
  f4 = f4 * int8_t{-25};
  REQUIRE(f4.numerator() == 133070225);
  REQUIRE(f4.denominator() == 1667251);
  // negative 0
  fishbait::Fraction f5{-6883624, 1511407};
  f5 = f5 * int8_t{0};
  REQUIRE(f5.numerator() == 0);
  REQUIRE(f5.denominator() == 1);
  // 0 positive
  fishbait::Fraction f6{0, 1};
  f6 = f6 * int8_t{82};
  REQUIRE(f6.numerator() == 0);
  REQUIRE(f6.denominator() == 1);
  // 0 negative
  fishbait::Fraction f7{0, 1};
  f7 = f7 * int8_t{-84};
  REQUIRE(f7.numerator() == 0);
  REQUIRE(f7.denominator() == 1);
  // 0 0
  fishbait::Fraction f8{0, 1};
  f8 = f8 * int8_t{0};
  REQUIRE(f8.numerator() == 0);
  REQUIRE(f8.denominator() == 1);
}  // TEST_CASE "fraction multiply int8_t"

TEST_CASE("fraction multiply equals uint8_t", "[utils][fraction]") {
  // positive positive
  fishbait::Fraction f0{3752871, 5311798};
  f0 *= uint8_t{237};
  REQUIRE(f0.numerator() == 889430427);
  REQUIRE(f0.denominator() == 5311798);
  // positive 0
  fishbait::Fraction f1{5059323, 9283876};
  f1 *= uint8_t{0};
  REQUIRE(f1.numerator() == 0);
  REQUIRE(f1.denominator() == 1);
  // negative positive
  fishbait::Fraction f2{-706131, 423907};
  f2 *= uint8_t{93};
  REQUIRE(f2.numerator() == -65670183);
  REQUIRE(f2.denominator() == 423907);
  // negative 0
  fishbait::Fraction f3{-2464834, 6674901};
  f3 *= uint8_t{0};
  REQUIRE(f3.numerator() == 0);
  REQUIRE(f3.denominator() == 1);
  // 0 positive
  fishbait::Fraction f4{0, 1};
  f4 *= uint8_t{34};
  REQUIRE(f4.numerator() == 0);
  REQUIRE(f4.denominator() == 1);
  // 0 0
  fishbait::Fraction f5{0, 1};
  f5 *= uint8_t{0};
  REQUIRE(f5.numerator() == 0);
  REQUIRE(f5.denominator() == 1);
}  // TEST_CASE "fraction multiply equals uint8_t"

TEST_CASE("uint8_t multiply fraction", "[utils][fraction]") {
  // positive positive
  fishbait::Fraction f0{2127776, 2085997};
  f0 = uint8_t{236} * f0;
  REQUIRE(f0.numerator() == 502155136);
  REQUIRE(f0.denominator() == 2085997);
  // positive negative
  fishbait::Fraction f1{-1712765, 603636};
  f1 = uint8_t{160} * f1;
  REQUIRE(f1.numerator() == -68510600);
  REQUIRE(f1.denominator() == 150909);
  // positive 0
  fishbait::Fraction f2{0, 1};
  f2 = uint8_t{102} * f2;
  REQUIRE(f2.numerator() == 0);
  REQUIRE(f2.denominator() == 1);
  // 0 positive
  fishbait::Fraction f3{618227, 8241};
  f3 = uint8_t{0} * f3;
  REQUIRE(f3.numerator() == 0);
  REQUIRE(f3.denominator() == 1);
  // 0 negative
  fishbait::Fraction f4{-510011, 3643799};
  f4 = uint8_t{0} * f4;
  REQUIRE(f4.numerator() == 0);
  REQUIRE(f4.denominator() == 1);
  // 0 0
  fishbait::Fraction f5{0, 1};
  f5 = uint8_t{0} * f5;
  REQUIRE(f5.numerator() == 0);
  REQUIRE(f5.denominator() == 1);
}  // TEST_CASE "uint8_t multiply fraction"

TEST_CASE("fraction multiply uint8_t", "[utils][fraction]") {
  // positive positive
  fishbait::Fraction f0{3131620, 4494323};
  f0 = f0 * uint8_t{172};
  REQUIRE(f0.numerator() == 538638640);
  REQUIRE(f0.denominator() == 4494323);
  // positive 0
  fishbait::Fraction f1{944137, 846221};
  f1 = f1 * uint8_t{0};
  REQUIRE(f1.numerator() == 0);
  REQUIRE(f1.denominator() == 1);
  // negative positive
  fishbait::Fraction f2{-2034282, 2372687};
  f2 = f2 * uint8_t{148};
  REQUIRE(f2.numerator() == -301073736);
  REQUIRE(f2.denominator() == 2372687);
  // negative 0
  fishbait::Fraction f3{-745465, 7374636};
  f3 = f3 * uint8_t{0};
  REQUIRE(f3.numerator() == 0);
  REQUIRE(f3.denominator() == 1);
  // 0 positive
  fishbait::Fraction f4{0, 1};
  f4 = f4 * uint8_t{167};
  REQUIRE(f4.numerator() == 0);
  REQUIRE(f4.denominator() == 1);
  // 0 0
  fishbait::Fraction f5{0, 1};
  f5 = f5 * uint8_t{0};
  REQUIRE(f5.numerator() == 0);
  REQUIRE(f5.denominator() == 1);
}  // TEST_CASE "fraction multiply uint8_t"

TEST_CASE("fraction multiply equals int16_t", "[utils][fraction]") {
  // positive positive
  fishbait::Fraction f0{6904622, 1441657};
  f0 *= int16_t{10263};
  REQUIRE(f0.numerator() == 70862135586);
  REQUIRE(f0.denominator() == 1441657);
  // positive negative
  fishbait::Fraction f1{319209, 357385};
  f1 *= int16_t{-32445};
  REQUIRE(f1.numerator() == -295906743);
  REQUIRE(f1.denominator() == 10211);
  // positive 0
  fishbait::Fraction f2{9527302, 9673599};
  f2 *= int16_t{0};
  REQUIRE(f2.numerator() == 0);
  REQUIRE(f2.denominator() == 1);
  // negative positive
  fishbait::Fraction f3{-9893315, 3436243};
  f3 *= int16_t{1422};
  REQUIRE(f3.numerator() == -14068293930);
  REQUIRE(f3.denominator() == 3436243);
  // negative negative
  fishbait::Fraction f4{-2508233, 7523718};
  f4 *= int16_t{-5911};
  REQUIRE(f4.numerator() == 14826165263);
  REQUIRE(f4.denominator() == 7523718);
  // negative 0
  fishbait::Fraction f5{-1417785, 6367748};
  f5 *= int16_t{0};
  REQUIRE(f5.numerator() == 0);
  REQUIRE(f5.denominator() == 1);
  // 0 positive
  fishbait::Fraction f6{0, 1};
  f6 *= int16_t{4112};
  REQUIRE(f6.numerator() == 0);
  REQUIRE(f6.denominator() == 1);
  // 0 negative
  fishbait::Fraction f7{0, 1};
  f7 *= int16_t{-3586};
  REQUIRE(f7.numerator() == 0);
  REQUIRE(f7.denominator() == 1);
  // 0 0
  fishbait::Fraction f8{0, 1};
  f8 *= int16_t{0};
  REQUIRE(f8.numerator() == 0);
  REQUIRE(f8.denominator() == 1);
}  // TEST_CASE "fraction multiply equals int16_t"

TEST_CASE("int16_t multiply fraction", "[utils][fraction]") {
  // positive positive
  fishbait::Fraction f0{2884941, 279304};
  f0 = int16_t{22685} * f0;
  REQUIRE(f0.numerator() == 65444886585);
  REQUIRE(f0.denominator() == 279304);
  // positive negative
  fishbait::Fraction f1{-3518833, 135189};
  f1 = int16_t{6477} * f1;
  REQUIRE(f1.numerator() == -7597160447);
  REQUIRE(f1.denominator() == 45063);
  // positive 0
  fishbait::Fraction f2{0, 1};
  f2 = int16_t{22819} * f2;
  REQUIRE(f2.numerator() == 0);
  REQUIRE(f2.denominator() == 1);
  // negative positive
  fishbait::Fraction f3{3938291, 9211050};
  f3 = int16_t{-32308} * f3;
  REQUIRE(f3.numerator() == -63619152814);
  REQUIRE(f3.denominator() == 4605525);
  // negative negative
  fishbait::Fraction f4{-95275, 69191};
  f4 = int16_t{-4247} * f4;
  REQUIRE(f4.numerator() == 404632925);
  REQUIRE(f4.denominator() == 69191);
  // negative 0
  fishbait::Fraction f5{0, 1};
  f5 = int16_t{-2186} * f5;
  REQUIRE(f5.numerator() == 0);
  REQUIRE(f5.denominator() == 1);
  // 0 positive
  fishbait::Fraction f6{704775, 134404};
  f6 = int16_t{0} * f6;
  REQUIRE(f6.numerator() == 0);
  REQUIRE(f6.denominator() == 1);
  // 0 negative
  fishbait::Fraction f7{-4075742, 3680533};
  f7 = int16_t{0} * f7;
  REQUIRE(f7.numerator() == 0);
  REQUIRE(f7.denominator() == 1);
  // 0 0
  fishbait::Fraction f8{0, 1};
  f8 = int16_t{0} * f8;
  REQUIRE(f8.numerator() == 0);
  REQUIRE(f8.denominator() == 1);
}  // TEST_CASE "int16_t multiply fraction"

TEST_CASE("fraction multiply int16_t", "[utils][fraction]") {
  // positive positive
  fishbait::Fraction f0{1680585, 536003};
  f0 = f0 * int16_t{21191};
  REQUIRE(f0.numerator() == 35613276735);
  REQUIRE(f0.denominator() == 536003);
  // positive negative
  fishbait::Fraction f1{5559341, 9864297};
  f1 = f1 * int16_t{-17901};
  REQUIRE(f1.numerator() == -11057529249);
  REQUIRE(f1.denominator() == 1096033);
  // positive 0
  fishbait::Fraction f2{320477, 652480};
  f2 = f2 * int16_t{0};
  REQUIRE(f2.numerator() == 0);
  REQUIRE(f2.denominator() == 1);
  // negative positive
  fishbait::Fraction f3{-1950875, 9562699};
  f3 = f3 * int16_t{15518};
  REQUIRE(f3.numerator() == -30273678250);
  REQUIRE(f3.denominator() == 9562699);
  // negative negative
  fishbait::Fraction f4{-1114937, 7574578};
  f4 = f4 * int16_t{-9456};
  REQUIRE(f4.numerator() == 5271422136);
  REQUIRE(f4.denominator() == 3787289);
  // negative 0
  fishbait::Fraction f5{-672045, 3976498};
  f5 = f5 * int16_t{0};
  REQUIRE(f5.numerator() == 0);
  REQUIRE(f5.denominator() == 1);
  // 0 positive
  fishbait::Fraction f6{0, 1};
  f6 = f6 * int16_t{22916};
  REQUIRE(f6.numerator() == 0);
  REQUIRE(f6.denominator() == 1);
  // 0 negative
  fishbait::Fraction f7{0, 1};
  f7 = f7 * int16_t{-14548};
  REQUIRE(f7.numerator() == 0);
  REQUIRE(f7.denominator() == 1);
  // 0 0
  fishbait::Fraction f8{0, 1};
  f8 = f8 * int16_t{0};
  REQUIRE(f8.numerator() == 0);
  REQUIRE(f8.denominator() == 1);
}  // TEST_CASE "fraction multiply int16_t"

TEST_CASE("fraction multiply equals uint16_t", "[utils][fraction]") {
  // positive positive
  fishbait::Fraction f0{555463, 1428684};
  f0 *= uint16_t{36806};
  REQUIRE(f0.numerator() == 10222185589);
  REQUIRE(f0.denominator() == 714342);
  // positive 0
  fishbait::Fraction f1{9038593, 9066408};
  f1 *= uint16_t{0};
  REQUIRE(f1.numerator() == 0);
  REQUIRE(f1.denominator() == 1);
  // negative positive
  fishbait::Fraction f2{-9102307, 5563281};
  f2 *= uint16_t{40271};
  REQUIRE(f2.numerator() == -366559005197);
  REQUIRE(f2.denominator() == 5563281);
  // negative 0
  fishbait::Fraction f3{-3841837, 2385467};
  f3 *= uint16_t{0};
  REQUIRE(f3.numerator() == 0);
  REQUIRE(f3.denominator() == 1);
  // 0 positive
  fishbait::Fraction f4{0, 1};
  f4 *= uint16_t{10744};
  REQUIRE(f4.numerator() == 0);
  REQUIRE(f4.denominator() == 1);
  // 0 0
  fishbait::Fraction f5{0, 1};
  f5 *= uint16_t{0};
  REQUIRE(f5.numerator() == 0);
  REQUIRE(f5.denominator() == 1);
}  // TEST_CASE "fraction multiply equals uint16_t"

TEST_CASE("uint16_t multiply fraction", "[utils][fraction]") {
  // positive positive
  fishbait::Fraction f0{4238660, 6909697};
  f0 = uint16_t{31545} * f0;
  REQUIRE(f0.numerator() == 133708529700);
  REQUIRE(f0.denominator() == 6909697);
  // positive negative
  fishbait::Fraction f1{-400684, 102143};
  f1 = uint16_t{64101} * f1;
  REQUIRE(f1.numerator() == -1116706308);
  REQUIRE(f1.denominator() == 4441);
  // positive 0
  fishbait::Fraction f2{0, 1};
  f2 = uint16_t{28647} * f2;
  REQUIRE(f2.numerator() == 0);
  REQUIRE(f2.denominator() == 1);
  // 0 positive
  fishbait::Fraction f3{2039209, 3015148};
  f3 = uint16_t{0} * f3;
  REQUIRE(f3.numerator() == 0);
  REQUIRE(f3.denominator() == 1);
  // 0 negative
  fishbait::Fraction f4{-196123, 217299};
  f4 = uint16_t{0} * f4;
  REQUIRE(f4.numerator() == 0);
  REQUIRE(f4.denominator() == 1);
  // 0 0
  fishbait::Fraction f5{0, 1};
  f5 = uint16_t{0} * f5;
  REQUIRE(f5.numerator() == 0);
  REQUIRE(f5.denominator() == 1);
}  // TEST_CASE "uint16_t multiply fraction"

TEST_CASE("fraction multiply uint16_t", "[utils][fraction]") {
  // positive positive
  fishbait::Fraction f0{2233680, 8715641};
  f0 = f0 * uint16_t{41238};
  REQUIRE(f0.numerator() == 92112495840);
  REQUIRE(f0.denominator() == 8715641);
  // positive 0
  fishbait::Fraction f1{6594303, 496004};
  f1 = f1 * uint16_t{0};
  REQUIRE(f1.numerator() == 0);
  REQUIRE(f1.denominator() == 1);
  // negative positive
  fishbait::Fraction f2{-236129, 1561484};
  f2 = f2 * uint16_t{38260};
  REQUIRE(f2.numerator() == -2258573885);
  REQUIRE(f2.denominator() == 390371);
  // negative 0
  fishbait::Fraction f3{-7039585, 3856729};
  f3 = f3 * uint16_t{0};
  REQUIRE(f3.numerator() == 0);
  REQUIRE(f3.denominator() == 1);
  // 0 positive
  fishbait::Fraction f4{0, 1};
  f4 = f4 * uint16_t{54573};
  REQUIRE(f4.numerator() == 0);
  REQUIRE(f4.denominator() == 1);
  // 0 0
  fishbait::Fraction f5{0, 1};
  f5 = f5 * uint16_t{0};
  REQUIRE(f5.numerator() == 0);
  REQUIRE(f5.denominator() == 1);
}  // TEST_CASE "fraction multiply uint16_t"

TEST_CASE("fraction multiply equals int32_t", "[utils][fraction]") {
  // positive positive
  fishbait::Fraction f0{5056965, 8681291};
  f0 *= int32_t{618934};
  REQUIRE(f0.numerator() == 3129927575310);
  REQUIRE(f0.denominator() == 8681291);
  // positive negative
  fishbait::Fraction f1{8959643, 702440};
  f1 *= int32_t{-7207320};
  REQUIRE(f1.numerator() == -94963256157);
  REQUIRE(f1.denominator() == 1033);
  // positive 0
  fishbait::Fraction f2{8496045, 3576611};
  f2 *= int32_t{0};
  REQUIRE(f2.numerator() == 0);
  REQUIRE(f2.denominator() == 1);
  // negative positive
  fishbait::Fraction f3{-1217863, 1383503};
  f3 *= int32_t{9061244};
  REQUIRE(f3.numerator() == -11035353801572);
  REQUIRE(f3.denominator() == 1383503);
  // negative negative
  fishbait::Fraction f4{-2095341, 3843940};
  f4 *= int32_t{-3120838};
  REQUIRE(f4.numerator() == 3269609907879);
  REQUIRE(f4.denominator() == 1921970);
  // negative 0
  fishbait::Fraction f5{-809229, 989170};
  f5 *= int32_t{0};
  REQUIRE(f5.numerator() == 0);
  REQUIRE(f5.denominator() == 1);
  // 0 positive
  fishbait::Fraction f6{0, 1};
  f6 *= int32_t{376645};
  REQUIRE(f6.numerator() == 0);
  REQUIRE(f6.denominator() == 1);
  // 0 negative
  fishbait::Fraction f7{0, 1};
  f7 *= int32_t{-2283657};
  REQUIRE(f7.numerator() == 0);
  REQUIRE(f7.denominator() == 1);
  // 0 0
  fishbait::Fraction f8{0, 1};
  f8 *= int32_t{0};
  REQUIRE(f8.numerator() == 0);
  REQUIRE(f8.denominator() == 1);
}  // TEST_CASE "fraction multiply equals int32_t"

TEST_CASE("int32_t multiply fraction", "[utils][fraction]") {
  // positive positive
  fishbait::Fraction f0{4551837, 4944592};
  f0 = int32_t{5235408} * f0;
  REQUIRE(f0.numerator() == 1489420240281);
  REQUIRE(f0.denominator() == 309037);
  // positive negative
  fishbait::Fraction f1{-2558396, 3132087};
  f1 = int32_t{3633076} * f1;
  REQUIRE(f1.numerator() == -9294847106096);
  REQUIRE(f1.denominator() == 3132087);
  // positive 0
  fishbait::Fraction f2{0, 1};
  f2 = int32_t{88438} * f2;
  REQUIRE(f2.numerator() == 0);
  REQUIRE(f2.denominator() == 1);
  // negative positive
  fishbait::Fraction f3{2386037, 7627188};
  f3 = int32_t{-1365911} * f3;
  REQUIRE(f3.numerator() == -3259114184707);
  REQUIRE(f3.denominator() == 7627188);
  // negative negative
  fishbait::Fraction f4{-971769, 686417};
  f4 = int32_t{-487524} * f4;
  REQUIRE(f4.numerator() == 473760709956);
  REQUIRE(f4.denominator() == 686417);
  // negative 0
  fishbait::Fraction f5{0, 1};
  f5 = int32_t{-7075558} * f5;
  REQUIRE(f5.numerator() == 0);
  REQUIRE(f5.denominator() == 1);
  // 0 positive
  fishbait::Fraction f6{3806464, 3314335};
  f6 = int32_t{0} * f6;
  REQUIRE(f6.numerator() == 0);
  REQUIRE(f6.denominator() == 1);
  // 0 negative
  fishbait::Fraction f7{-3049801, 9394146};
  f7 = int32_t{0} * f7;
  REQUIRE(f7.numerator() == 0);
  REQUIRE(f7.denominator() == 1);
  // 0 0
  fishbait::Fraction f8{0, 1};
  f8 = int32_t{0} * f8;
  REQUIRE(f8.numerator() == 0);
  REQUIRE(f8.denominator() == 1);
}  // TEST_CASE "int32_t multiply fraction"

TEST_CASE("fraction multiply int32_t", "[utils][fraction]") {
  // positive positive
  fishbait::Fraction f0{8479092, 7732703};
  f0 = f0 * int32_t{1894387};
  REQUIRE(f0.numerator() == 1460243786964);
  REQUIRE(f0.denominator() == 702973);
  // positive negative
  fishbait::Fraction f1{147286, 237331};
  f1 = f1 * int32_t{-8413649};
  REQUIRE(f1.numerator() == -1239212706614);
  REQUIRE(f1.denominator() == 237331);
  // positive 0
  fishbait::Fraction f2{1937803, 2309977};
  f2 = f2 * int32_t{0};
  REQUIRE(f2.numerator() == 0);
  REQUIRE(f2.denominator() == 1);
  // negative positive
  fishbait::Fraction f3{-5505565, 7307637};
  f3 = f3 * int32_t{800701};
  REQUIRE(f3.numerator() == -4408311401065);
  REQUIRE(f3.denominator() == 7307637);
  // negative negative
  fishbait::Fraction f4{-1896517, 4637487};
  f4 = f4 * int32_t{-9710850};
  REQUIRE(f4.numerator() == 6138930703150);
  REQUIRE(f4.denominator() == 1545829);
  // negative 0
  fishbait::Fraction f5{-8536877, 4727654};
  f5 = f5 * int32_t{0};
  REQUIRE(f5.numerator() == 0);
  REQUIRE(f5.denominator() == 1);
  // 0 positive
  fishbait::Fraction f6{0, 1};
  f6 = f6 * int32_t{5452885};
  REQUIRE(f6.numerator() == 0);
  REQUIRE(f6.denominator() == 1);
  // 0 negative
  fishbait::Fraction f7{0, 1};
  f7 = f7 * int32_t{-3840441};
  REQUIRE(f7.numerator() == 0);
  REQUIRE(f7.denominator() == 1);
  // 0 0
  fishbait::Fraction f8{0, 1};
  f8 = f8 * int32_t{0};
  REQUIRE(f8.numerator() == 0);
  REQUIRE(f8.denominator() == 1);
}  // TEST_CASE "fraction multiply int32_t"

TEST_CASE("fraction multiply equals uint32_t", "[utils][fraction]") {
  // positive positive
  fishbait::Fraction f0{3246991, 1094067};
  f0 *= uint32_t{3961344};
  REQUIRE(f0.numerator() == 4287482771968);
  REQUIRE(f0.denominator() == 364689);
  // positive 0
  fishbait::Fraction f1{9606886, 5422173};
  f1 *= uint32_t{0};
  REQUIRE(f1.numerator() == 0);
  REQUIRE(f1.denominator() == 1);
  // negative positive
  fishbait::Fraction f2{-1415641, 1876965};
  f2 *= uint32_t{5200343};
  REQUIRE(f2.numerator() == -7361818764863);
  REQUIRE(f2.denominator() == 1876965);
  // negative 0
  fishbait::Fraction f3{-6976018, 346567};
  f3 *= uint32_t{0};
  REQUIRE(f3.numerator() == 0);
  REQUIRE(f3.denominator() == 1);
  // 0 positive
  fishbait::Fraction f4{0, 1};
  f4 *= uint32_t{6354084};
  REQUIRE(f4.numerator() == 0);
  REQUIRE(f4.denominator() == 1);
  // 0 0
  fishbait::Fraction f5{0, 1};
  f5 *= uint32_t{0};
  REQUIRE(f5.numerator() == 0);
  REQUIRE(f5.denominator() == 1);
}  // TEST_CASE "fraction multiply equals uint32_t"

TEST_CASE("uint32_t multiply fraction", "[utils][fraction]") {
  // positive positive
  fishbait::Fraction f0{3718403, 7535733};
  f0 = uint32_t{8824477} * f0;
  REQUIRE(f0.numerator() == 32812961750231);
  REQUIRE(f0.denominator() == 7535733);
  // positive negative
  fishbait::Fraction f1{-38669, 4577433};
  f1 = uint32_t{9886613} * f1;
  REQUIRE(f1.numerator() == -382305438097);
  REQUIRE(f1.denominator() == 4577433);
  // positive 0
  fishbait::Fraction f2{0, 1};
  f2 = uint32_t{184219} * f2;
  REQUIRE(f2.numerator() == 0);
  REQUIRE(f2.denominator() == 1);
  // 0 positive
  fishbait::Fraction f3{2633008, 3468565};
  f3 = uint32_t{0} * f3;
  REQUIRE(f3.numerator() == 0);
  REQUIRE(f3.denominator() == 1);
  // 0 negative
  fishbait::Fraction f4{-6141397, 6481466};
  f4 = uint32_t{0} * f4;
  REQUIRE(f4.numerator() == 0);
  REQUIRE(f4.denominator() == 1);
  // 0 0
  fishbait::Fraction f5{0, 1};
  f5 = uint32_t{0} * f5;
  REQUIRE(f5.numerator() == 0);
  REQUIRE(f5.denominator() == 1);
}  // TEST_CASE "uint32_t multiply fraction"

TEST_CASE("fraction multiply uint32_t", "[utils][fraction]") {
  // positive positive
  fishbait::Fraction f0{193821, 1063757};
  f0 = f0 * uint32_t{718996};
  REQUIRE(f0.numerator() == 139356523716);
  REQUIRE(f0.denominator() == 1063757);
  // positive 0
  fishbait::Fraction f1{5899664, 3685145};
  f1 = f1 * uint32_t{0};
  REQUIRE(f1.numerator() == 0);
  REQUIRE(f1.denominator() == 1);
  // negative positive
  fishbait::Fraction f2{-5621678, 3472943};
  f2 = f2 * uint32_t{4643974};
  REQUIRE(f2.numerator() == -26106926468372);
  REQUIRE(f2.denominator() == 3472943);
  // negative 0
  fishbait::Fraction f3{-5328333, 924313};
  f3 = f3 * uint32_t{0};
  REQUIRE(f3.numerator() == 0);
  REQUIRE(f3.denominator() == 1);
  // 0 positive
  fishbait::Fraction f4{0, 1};
  f4 = f4 * uint32_t{9080354};
  REQUIRE(f4.numerator() == 0);
  REQUIRE(f4.denominator() == 1);
  // 0 0
  fishbait::Fraction f5{0, 1};
  f5 = f5 * uint32_t{0};
  REQUIRE(f5.numerator() == 0);
  REQUIRE(f5.denominator() == 1);
}  // TEST_CASE "fraction multiply uint32_t"

TEST_CASE("fraction multiply equals int64_t", "[utils][fraction]") {
  // positive positive
  fishbait::Fraction f0{8594314, 8449265};
  f0 *= int64_t{5013866};
  REQUIRE(f0.numerator() == 3917339887084);
  REQUIRE(f0.denominator() == 768115);
  // positive negative
  fishbait::Fraction f1{248189, 7042202};
  f1 *= int64_t{-3867417};
  REQUIRE(f1.numerator() == -959850357813);
  REQUIRE(f1.denominator() == 7042202);
  // positive 0
  fishbait::Fraction f2{4348034, 6074447};
  f2 *= int64_t{0};
  REQUIRE(f2.numerator() == 0);
  REQUIRE(f2.denominator() == 1);
  // negative positive
  fishbait::Fraction f3{-6832403, 6108732};
  f3 *= int64_t{231165};
  REQUIRE(f3.numerator() == -175490271055);
  REQUIRE(f3.denominator() == 678748);
  // negative negative
  fishbait::Fraction f4{-85599, 477967};
  f4 *= int64_t{-788969};
  REQUIRE(f4.numerator() == 67534957431);
  REQUIRE(f4.denominator() == 477967);
  // negative 0
  fishbait::Fraction f5{-7706026, 263071};
  f5 *= int64_t{0};
  REQUIRE(f5.numerator() == 0);
  REQUIRE(f5.denominator() == 1);
  // 0 positive
  fishbait::Fraction f6{0, 1};
  f6 *= int64_t{9388426};
  REQUIRE(f6.numerator() == 0);
  REQUIRE(f6.denominator() == 1);
  // 0 negative
  fishbait::Fraction f7{0, 1};
  f7 *= int64_t{-468304};
  REQUIRE(f7.numerator() == 0);
  REQUIRE(f7.denominator() == 1);
  // 0 0
  fishbait::Fraction f8{0, 1};
  f8 *= int64_t{0};
  REQUIRE(f8.numerator() == 0);
  REQUIRE(f8.denominator() == 1);
}  // TEST_CASE "fraction multiply equals int64_t"

TEST_CASE("int64_t multiply fraction", "[utils][fraction]") {
  // positive positive
  fishbait::Fraction f0{5812839, 5686723};
  f0 = int64_t{5489235} * f0;
  REQUIRE(f0.numerator() == 31908039288165);
  REQUIRE(f0.denominator() == 5686723);
  // positive negative
  fishbait::Fraction f1{-853987, 3622125};
  f1 = int64_t{218621} * f1;
  REQUIRE(f1.numerator() == -14361499379);
  REQUIRE(f1.denominator() == 278625);
  // positive 0
  fishbait::Fraction f2{0, 1};
  f2 = int64_t{9767079} * f2;
  REQUIRE(f2.numerator() == 0);
  REQUIRE(f2.denominator() == 1);
  // negative positive
  fishbait::Fraction f3{2164003, 2106144};
  f3 = int64_t{-6292188} * f3;
  REQUIRE(f3.numerator() == -378230936349);
  REQUIRE(f3.denominator() == 58504);
  // negative negative
  fishbait::Fraction f4{-6271900, 6230681};
  f4 = int64_t{-2047407} * f4;
  REQUIRE(f4.numerator() == 12841131963300);
  REQUIRE(f4.denominator() == 6230681);
  // negative 0
  fishbait::Fraction f5{0, 1};
  f5 = int64_t{-8078154} * f5;
  REQUIRE(f5.numerator() == 0);
  REQUIRE(f5.denominator() == 1);
  // 0 positive
  fishbait::Fraction f6{49982, 99669};
  f6 = int64_t{0} * f6;
  REQUIRE(f6.numerator() == 0);
  REQUIRE(f6.denominator() == 1);
  // 0 negative
  fishbait::Fraction f7{-4370053, 3939005};
  f7 = int64_t{0} * f7;
  REQUIRE(f7.numerator() == 0);
  REQUIRE(f7.denominator() == 1);
  // 0 0
  fishbait::Fraction f8{0, 1};
  f8 = int64_t{0} * f8;
  REQUIRE(f8.numerator() == 0);
  REQUIRE(f8.denominator() == 1);
}  // TEST_CASE "int64_t multiply fraction"

TEST_CASE("fraction multiply int64_t", "[utils][fraction]") {
  // positive positive
  fishbait::Fraction f0{3842573, 7690079};
  f0 = f0 * int64_t{327171};
  REQUIRE(f0.numerator() == 1257178450983);
  REQUIRE(f0.denominator() == 7690079);
  // positive negative
  fishbait::Fraction f1{8796131, 9964606};
  f1 = f1 * int64_t{-8707619};
  REQUIRE(f1.numerator() == -76593357422089);
  REQUIRE(f1.denominator() == 9964606);
  // positive 0
  fishbait::Fraction f2{4291375, 5039373};
  f2 = f2 * int64_t{0};
  REQUIRE(f2.numerator() == 0);
  REQUIRE(f2.denominator() == 1);
  // negative positive
  fishbait::Fraction f3{-2166457, 4509736};
  f3 = f3 * int64_t{9037128};
  REQUIRE(f3.numerator() == -2447318651937);
  REQUIRE(f3.denominator() == 563717);
  // negative negative
  fishbait::Fraction f4{-1122193, 768618};
  f4 = f4 * int64_t{-3078491};
  REQUIRE(f4.numerator() == 3454661050763);
  REQUIRE(f4.denominator() == 768618);
  // negative 0
  fishbait::Fraction f5{-1466772, 9329843};
  f5 = f5 * int64_t{0};
  REQUIRE(f5.numerator() == 0);
  REQUIRE(f5.denominator() == 1);
  // 0 positive
  fishbait::Fraction f6{0, 1};
  f6 = f6 * int64_t{9964005};
  REQUIRE(f6.numerator() == 0);
  REQUIRE(f6.denominator() == 1);
  // 0 negative
  fishbait::Fraction f7{0, 1};
  f7 = f7 * int64_t{-7916162};
  REQUIRE(f7.numerator() == 0);
  REQUIRE(f7.denominator() == 1);
  // 0 0
  fishbait::Fraction f8{0, 1};
  f8 = f8 * int64_t{0};
  REQUIRE(f8.numerator() == 0);
  REQUIRE(f8.denominator() == 1);
}  // TEST_CASE "fraction multiply int64_t"

TEST_CASE("double multiply fraction", "[utils][fraction]") {
  // positive positive
  fishbait::Fraction f0{4255313, 4544094};
  REQUIRE(5450933.391495973 * f0 == Approx(5104521.984573141));
  // positive negative
  fishbait::Fraction f1{-8903533, 6293054};
  REQUIRE(6417672.980418905 * f1 == Approx(-9079846.31378788));
  // positive 0
  fishbait::Fraction f2{0, 1};
  REQUIRE(8629914.331530217 * f2 == Approx(0.0));
  // negative positive
  fishbait::Fraction f3{4086295, 6017229};
  REQUIRE(-5738883.154781103 * f3 == Approx(-3897270.577697184));
  // negative negative
  fishbait::Fraction f4{-313585, 9776694};
  REQUIRE(-3757600.1636341135 * f4 == Approx(120524.07974650772));
  // negative 0
  fishbait::Fraction f5{0, 1};
  REQUIRE(-2611782.9686606787 * f5 == Approx(-0.0));
  // 0 positive
  fishbait::Fraction f6{9009, 332380};
  REQUIRE(0.0 * f6 == Approx(0.0));
  // 0 negative
  fishbait::Fraction f7{-483077, 1274687};
  REQUIRE(0.0 * f7 == Approx(-0.0));
  // 0 0
  fishbait::Fraction f8{0, 1};
  REQUIRE(0.0 * f8 == Approx(0.0));
}  // TEST_CASE "double multiply fraction"

TEST_CASE("fraction multiply double", "[utils][fraction]") {
  // positive positive
  fishbait::Fraction f0{45069, 234997};
  REQUIRE(f0 * 6238029.178230092 == Approx(1196363.089884773));
  // positive negative
  fishbait::Fraction f1{8690215, 7989713};
  REQUIRE(f1 * -1533439.0296567357 == Approx(-1667884.0475381794));
  // positive 0
  fishbait::Fraction f2{9143070, 8433991};
  REQUIRE(f2 * 0.0 == Approx(0.0));
  // negative positive
  fishbait::Fraction f3{-1270421, 750110};
  REQUIRE(f3 * 4986978.258916593 == Approx(-8446177.102919675));
  // negative negative
  fishbait::Fraction f4{-6942121, 1987055};
  REQUIRE(f4 * -6745133.862187792 == Approx(23565294.082199525));
  // negative 0
  fishbait::Fraction f5{-1047792, 1037597};
  REQUIRE(f5 * 0.0 == Approx(-0.0));
  // 0 positive
  fishbait::Fraction f6{0, 1};
  REQUIRE(f6 * 2192568.9677406005 == Approx(0.0));
  // 0 negative
  fishbait::Fraction f7{0, 1};
  REQUIRE(f7 * -3478527.860238506 == Approx(-0.0));
  // 0 0
  fishbait::Fraction f8{0, 1};
  REQUIRE(f8 * 0.0 == Approx(0.0));
}  // TEST_CASE "fraction multiply double"

TEST_CASE("fraction multiply fraction", "[utils][fraction]") {
  // positive positive
  fishbait::Fraction f0 = fishbait::Fraction(9240540, 3001133) *
                          fishbait::Fraction(3532022, 7650105);
  REQUIRE(f0.numerator() == 2175852704792);
  REQUIRE(f0.denominator() == 1530598837931);
  // positive negative
  fishbait::Fraction f1 = fishbait::Fraction(1957031, 6194303) *
                          fishbait::Fraction(-1572570, 1593221);
  REQUIRE(f1.numerator() == -3077568239670);
  REQUIRE(f1.denominator() == 9868893619963);
  // positive 0
  fishbait::Fraction f2 = fishbait::Fraction(3756151, 7379219) *
                          fishbait::Fraction(0, 1);
  REQUIRE(f2.numerator() == 0);
  REQUIRE(f2.denominator() == 1);
  // negative positive
  fishbait::Fraction f3 = fishbait::Fraction(-3347493, 2051467) *
                          fishbait::Fraction(1542449, 6516388);
  REQUIRE(f3.numerator() == -5163337230357);
  REQUIRE(f3.denominator() == 13368154941196);
  // negative negative
  fishbait::Fraction f4 = fishbait::Fraction(-393047, 196573) *
                          fishbait::Fraction(-377308, 1065005);
  REQUIRE(f4.numerator() == 148299777476);
  REQUIRE(f4.denominator() == 209351227865);
  // negative 0
  fishbait::Fraction f5 = fishbait::Fraction(-1730885, 5574321) *
                          fishbait::Fraction(0, 1);
  REQUIRE(f5.numerator() == 0);
  REQUIRE(f5.denominator() == 1);
  // 0 positive
  fishbait::Fraction f6 = fishbait::Fraction(0, 1) *
                          fishbait::Fraction(8340595, 3040052);
  REQUIRE(f6.numerator() == 0);
  REQUIRE(f6.denominator() == 1);
  // 0 negative
  fishbait::Fraction f7 = fishbait::Fraction(0, 1) *
                          fishbait::Fraction(-9123783, 2884922);
  REQUIRE(f7.numerator() == 0);
  REQUIRE(f7.denominator() == 1);
  // 0 0
  fishbait::Fraction f8 = fishbait::Fraction{0, 1} * fishbait::Fraction{0, 1};
  REQUIRE(f8.numerator() == 0);
  REQUIRE(f8.denominator() == 1);
}  // TEST_CASE "fraction multiply fraction"

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
/* ------------------------------- Division --------------------------------- */
/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */

TEST_CASE("fraction divide equals fraction", "[utils][fraction]") {
  // positive positive
  fishbait::Fraction f0{1273717, 988789};
  f0 /= fishbait::Fraction{9561909, 8690237};
  REQUIRE(f0.numerator() == 11068902600929);
  REQUIRE(f0.denominator() == 9454710438201);
  // positive negative
  fishbait::Fraction f1{45988, 6860369};
  f1 /= fishbait::Fraction{-4986491, 4328380};
  REQUIRE(f1.numerator() == -199053539440);
  REQUIRE(f1.denominator() == 34209168275179);
  // negative positive
  fishbait::Fraction f2{-284248, 4507873};
  f2 /= fishbait::Fraction{2613537, 3720287};
  REQUIRE(f2.numerator() == -1057484139176);
  REQUIRE(f2.denominator() == 11781492876801);
  // negative negative
  fishbait::Fraction f3{-5500578, 9610199};
  f3 /= fishbait::Fraction{-8717313, 8970551};
  REQUIRE(f3.numerator() == 16447738492826);
  REQUIRE(f3.denominator() == 27925037558429);
  // 0 positive
  fishbait::Fraction f4{0, 1};
  f4 /= fishbait::Fraction{8137596, 7724083};
  REQUIRE(f4.numerator() == 0);
  REQUIRE(f4.denominator() == 1);
  // 0 negative
  fishbait::Fraction f5{0, 1};
  f5 /= fishbait::Fraction{-4925405, 3648622};
  REQUIRE(f5.numerator() == 0);
  REQUIRE(f5.denominator() == 1);
}  // TEST_CASE "fraction divide equals fraction"

TEST_CASE("fraction divide equals int8_t", "[utils][fraction]") {
  // positive positive
  fishbait::Fraction f0{3537806, 8553111};
  f0 /= int8_t{47};
  REQUIRE(f0.numerator() == 3537806);
  REQUIRE(f0.denominator() == 401996217);
  // positive negative
  fishbait::Fraction f1{6114688, 9668177};
  f1 /= int8_t{-42};
  REQUIRE(f1.numerator() == -3057344);
  REQUIRE(f1.denominator() == 203031717);
  // negative positive
  fishbait::Fraction f2{-143117, 288417};
  f2 /= int8_t{115};
  REQUIRE(f2.numerator() == -143117);
  REQUIRE(f2.denominator() == 33167955);
  // negative negative
  fishbait::Fraction f3{-4093319, 2597169};
  f3 /= int8_t{-85};
  REQUIRE(f3.numerator() == 4093319);
  REQUIRE(f3.denominator() == 220759365);
  // 0 positive
  fishbait::Fraction f4{0, 1};
  f4 /= int8_t{117};
  REQUIRE(f4.numerator() == 0);
  REQUIRE(f4.denominator() == 1);
  // 0 negative
  fishbait::Fraction f5{0, 1};
  f5 /= int8_t{-75};
  REQUIRE(f5.numerator() == 0);
  REQUIRE(f5.denominator() == 1);
}  // TEST_CASE "fraction divide equals int8_t"

TEST_CASE("int8_t divide fraction", "[utils][fraction]") {
  // positive positive
  fishbait::Fraction f0{2608463, 9641683};
  f0 = int8_t{120} / f0;
  REQUIRE(f0.numerator() == 1157001960);
  REQUIRE(f0.denominator() == 2608463);
  // positive negative
  fishbait::Fraction f1{-5927116, 8097893};
  f1 = int8_t{124} / f1;
  REQUIRE(f1.numerator() == -251034683);
  REQUIRE(f1.denominator() == 1481779);
  // negative positive
  fishbait::Fraction f2{1946747, 704519};
  f2 = int8_t{-17} / f2;
  REQUIRE(f2.numerator() == -11976823);
  REQUIRE(f2.denominator() == 1946747);
  // negative negative
  fishbait::Fraction f3{-5434205, 6826066};
  f3 = int8_t{-93} / f3;
  REQUIRE(f3.numerator() == 634824138);
  REQUIRE(f3.denominator() == 5434205);
  // 0 positive
  fishbait::Fraction f4{1162105, 3010206};
  f4 = int8_t{0} / f4;
  REQUIRE(f4.numerator() == 0);
  REQUIRE(f4.denominator() == 1);
  // 0 negative
  fishbait::Fraction f5{-3045527, 1536804};
  f5 = int8_t{0} / f5;
  REQUIRE(f5.numerator() == 0);
  REQUIRE(f5.denominator() == 1);
}  // TEST_CASE "int8_t divide fraction"

TEST_CASE("fraction divide int8_t", "[utils][fraction]") {
  // positive positive
  fishbait::Fraction f0{3483943, 9999363};
  f0 = f0 / int8_t{75};
  REQUIRE(f0.numerator() == 3483943);
  REQUIRE(f0.denominator() == 749952225);
  // positive negative
  fishbait::Fraction f1{3348847, 9688600};
  f1 = f1 / int8_t{-98};
  REQUIRE(f1.numerator() == -3348847);
  REQUIRE(f1.denominator() == 949482800);
  // negative positive
  fishbait::Fraction f2{-1328925, 4602592};
  f2 = f2 / int8_t{98};
  REQUIRE(f2.numerator() == -1328925);
  REQUIRE(f2.denominator() == 451054016);
  // negative negative
  fishbait::Fraction f3{-437098, 5508305};
  f3 = f3 / int8_t{-36};
  REQUIRE(f3.numerator() == 218549);
  REQUIRE(f3.denominator() == 99149490);
  // 0 positive
  fishbait::Fraction f4{0, 1};
  f4 = f4 / int8_t{42};
  REQUIRE(f4.numerator() == 0);
  REQUIRE(f4.denominator() == 1);
  // 0 negative
  fishbait::Fraction f5{0, 1};
  f5 = f5 / int8_t{-2};
  REQUIRE(f5.numerator() == 0);
  REQUIRE(f5.denominator() == 1);
}  // TEST_CASE "fraction divide int8_t"

TEST_CASE("fraction divide equals uint8_t", "[utils][fraction]") {
  // positive positive
  fishbait::Fraction f0{1914359, 696782};
  f0 /= uint8_t{234};
  REQUIRE(f0.numerator() == 1914359);
  REQUIRE(f0.denominator() == 163046988);
  // negative positive
  fishbait::Fraction f1{-669153, 464188};
  f1 /= uint8_t{151};
  REQUIRE(f1.numerator() == -669153);
  REQUIRE(f1.denominator() == 70092388);
  // 0 positive
  fishbait::Fraction f2{0, 1};
  f2 /= uint8_t{109};
  REQUIRE(f2.numerator() == 0);
  REQUIRE(f2.denominator() == 1);
}  // TEST_CASE "fraction divide equals uint8_t"

TEST_CASE("uint8_t divide fraction", "[utils][fraction]") {
  // positive positive
  fishbait::Fraction f0{1740672, 1703245};
  f0 = uint8_t{79} / f0;
  REQUIRE(f0.numerator() == 134556355);
  REQUIRE(f0.denominator() == 1740672);
  // positive negative
  fishbait::Fraction f1{-4389568, 3681023};
  f1 = uint8_t{91} / f1;
  REQUIRE(f1.numerator() == -334973093);
  REQUIRE(f1.denominator() == 4389568);
  // 0 positive
  fishbait::Fraction f2{4938827, 23361};
  f2 = uint8_t{0} / f2;
  REQUIRE(f2.numerator() == 0);
  REQUIRE(f2.denominator() == 1);
  // 0 negative
  fishbait::Fraction f3{-2951757, 1303307};
  f3 = uint8_t{0} / f3;
  REQUIRE(f3.numerator() == 0);
  REQUIRE(f3.denominator() == 1);
}  // TEST_CASE "uint8_t divide fraction"

TEST_CASE("fraction divide uint8_t", "[utils][fraction]") {
  // positive positive
  fishbait::Fraction f0{1310149, 123177};
  f0 = f0 / uint8_t{242};
  REQUIRE(f0.numerator() == 1310149);
  REQUIRE(f0.denominator() == 29808834);
  // negative positive
  fishbait::Fraction f1{-6140100, 3123769};
  f1 = f1 / uint8_t{176};
  REQUIRE(f1.numerator() == -1535025);
  REQUIRE(f1.denominator() == 137445836);
  // 0 positive
  fishbait::Fraction f2{0, 1};
  f2 = f2 / uint8_t{33};
  REQUIRE(f2.numerator() == 0);
  REQUIRE(f2.denominator() == 1);
}  // TEST_CASE "fraction divide uint8_t"

TEST_CASE("fraction divide equals int16_t", "[utils][fraction]") {
  // positive positive
  fishbait::Fraction f0{2120072, 5022819};
  f0 /= int16_t{21572};
  REQUIRE(f0.numerator() == 530018);
  REQUIRE(f0.denominator() == 27088062867);
  // positive negative
  fishbait::Fraction f1{1426061, 1091574};
  f1 /= int16_t{-12897};
  REQUIRE(f1.numerator() == -1426061);
  REQUIRE(f1.denominator() == 14078029878);
  // negative positive
  fishbait::Fraction f2{-8412334, 2039879};
  f2 /= int16_t{14737};
  REQUIRE(f2.numerator() == -8412334);
  REQUIRE(f2.denominator() == 30061696823);
  // negative negative
  fishbait::Fraction f3{-3552585, 99332};
  f3 /= int16_t{-24011};
  REQUIRE(f3.numerator() == 3552585);
  REQUIRE(f3.denominator() == 2385060652);
  // 0 positive
  fishbait::Fraction f4{0, 1};
  f4 /= int16_t{3237};
  REQUIRE(f4.numerator() == 0);
  REQUIRE(f4.denominator() == 1);
  // 0 negative
  fishbait::Fraction f5{0, 1};
  f5 /= int16_t{-5755};
  REQUIRE(f5.numerator() == 0);
  REQUIRE(f5.denominator() == 1);
}  // TEST_CASE "fraction divide equals int16_t"

TEST_CASE("int16_t divide fraction", "[utils][fraction]") {
  // positive positive
  fishbait::Fraction f0{446797, 7194379};
  f0 = int16_t{25778} / f0;
  REQUIRE(f0.numerator() == 185456701862);
  REQUIRE(f0.denominator() == 446797);
  // positive negative
  fishbait::Fraction f1{-1518083, 498056};
  f1 = int16_t{30091} / f1;
  REQUIRE(f1.numerator() == -14987003096);
  REQUIRE(f1.denominator() == 1518083);
  // negative positive
  fishbait::Fraction f2{524459, 870725};
  f2 = int16_t{-12606} / f2;
  REQUIRE(f2.numerator() == -10976359350);
  REQUIRE(f2.denominator() == 524459);
  // negative negative
  fishbait::Fraction f3{-3543353, 7455891};
  f3 = int16_t{-29956} / f3;
  REQUIRE(f3.numerator() == 223348670796);
  REQUIRE(f3.denominator() == 3543353);
  // 0 positive
  fishbait::Fraction f4{119475, 243049};
  f4 = int16_t{0} / f4;
  REQUIRE(f4.numerator() == 0);
  REQUIRE(f4.denominator() == 1);
  // 0 negative
  fishbait::Fraction f5{-3650529, 8192695};
  f5 = int16_t{0} / f5;
  REQUIRE(f5.numerator() == 0);
  REQUIRE(f5.denominator() == 1);
}  // TEST_CASE "int16_t divide fraction"

TEST_CASE("fraction divide int16_t", "[utils][fraction]") {
  // positive positive
  fishbait::Fraction f0{1387937, 1365340};
  f0 = f0 / int16_t{19529};
  REQUIRE(f0.numerator() == 1387937);
  REQUIRE(f0.denominator() == 26663724860);
  // positive negative
  fishbait::Fraction f1{3527795, 9388082};
  f1 = f1 / int16_t{-31750};
  REQUIRE(f1.numerator() == -705559);
  REQUIRE(f1.denominator() == 59614320700);
  // negative positive
  fishbait::Fraction f2{-5445803, 8426408};
  f2 = f2 / int16_t{9371};
  REQUIRE(f2.numerator() == -5445803);
  REQUIRE(f2.denominator() == 78963869368);
  // negative negative
  fishbait::Fraction f3{-9594496, 609815};
  f3 = f3 / int16_t{-14970};
  REQUIRE(f3.numerator() == 4797248);
  REQUIRE(f3.denominator() == 4564465275);
  // 0 positive
  fishbait::Fraction f4{0, 1};
  f4 = f4 / int16_t{20851};
  REQUIRE(f4.numerator() == 0);
  REQUIRE(f4.denominator() == 1);
  // 0 negative
  fishbait::Fraction f5{0, 1};
  f5 = f5 / int16_t{-13928};
  REQUIRE(f5.numerator() == 0);
  REQUIRE(f5.denominator() == 1);
}  // TEST_CASE "fraction divide int16_t"

TEST_CASE("fraction divide equals uint16_t", "[utils][fraction]") {
  // positive positive
  fishbait::Fraction f0{7310880, 7876873};
  f0 /= uint16_t{45923};
  REQUIRE(f0.numerator() == 7310880);
  REQUIRE(f0.denominator() == 361729638779);
  // negative positive
  fishbait::Fraction f1{-5231105, 5256978};
  f1 /= uint16_t{39829};
  REQUIRE(f1.numerator() == -5231105);
  REQUIRE(f1.denominator() == 209380176762);
  // 0 positive
  fishbait::Fraction f2{0, 1};
  f2 /= uint16_t{60672};
  REQUIRE(f2.numerator() == 0);
  REQUIRE(f2.denominator() == 1);
}  // TEST_CASE "fraction divide equals uint16_t"

TEST_CASE("uint16_t divide fraction", "[utils][fraction]") {
  // positive positive
  fishbait::Fraction f0{8903950, 832349};
  f0 = uint16_t{39922} / f0;
  REQUIRE(f0.numerator() == 16614518389);
  REQUIRE(f0.denominator() == 4451975);
  // positive negative
  fishbait::Fraction f1{-6719063, 1803944};
  f1 = uint16_t{31551} / f1;
  REQUIRE(f1.numerator() == -4378172088);
  REQUIRE(f1.denominator() == 516851);
  // 0 positive
  fishbait::Fraction f2{4819836, 7619867};
  f2 = uint16_t{0} / f2;
  REQUIRE(f2.numerator() == 0);
  REQUIRE(f2.denominator() == 1);
  // 0 negative
  fishbait::Fraction f3{-4073301, 6536228};
  f3 = uint16_t{0} / f3;
  REQUIRE(f3.numerator() == 0);
  REQUIRE(f3.denominator() == 1);
}  // TEST_CASE "uint16_t divide fraction"

TEST_CASE("fraction divide uint16_t", "[utils][fraction]") {
  // positive positive
  fishbait::Fraction f0{4375862, 8459419};
  f0 = f0 / uint16_t{51993};
  REQUIRE(f0.numerator() == 4375862);
  REQUIRE(f0.denominator() == 439830572067);
  // negative positive
  fishbait::Fraction f1{-1515089, 1980494};
  f1 = f1 / uint16_t{27753};
  REQUIRE(f1.numerator() == -1515089);
  REQUIRE(f1.denominator() == 54964649982);
  // 0 positive
  fishbait::Fraction f2{0, 1};
  f2 = f2 / uint16_t{59146};
  REQUIRE(f2.numerator() == 0);
  REQUIRE(f2.denominator() == 1);
}  // TEST_CASE "fraction divide uint16_t"

TEST_CASE("fraction divide equals int32_t", "[utils][fraction]") {
  // positive positive
  fishbait::Fraction f0{1345075, 4596254};
  f0 /= int32_t{509837};
  REQUIRE(f0.numerator() == 1345075);
  REQUIRE(f0.denominator() == 2343340350598);
  // positive negative
  fishbait::Fraction f1{7823133, 323417};
  f1 /= int32_t{-2371005};
  REQUIRE(f1.numerator() == -869237);
  REQUIRE(f1.denominator() == 85202591565);
  // negative positive
  fishbait::Fraction f2{-687851, 224190};
  f2 /= int32_t{1168794};
  REQUIRE(f2.numerator() == -687851);
  REQUIRE(f2.denominator() == 262031926860);
  // negative negative
  fishbait::Fraction f3{-1731695, 8711574};
  f3 /= int32_t{-9963059};
  REQUIRE(f3.numerator() == 1731695);
  REQUIRE(f3.denominator() == 86793925744866);
  // 0 positive
  fishbait::Fraction f4{0, 1};
  f4 /= int32_t{1308227};
  REQUIRE(f4.numerator() == 0);
  REQUIRE(f4.denominator() == 1);
  // 0 negative
  fishbait::Fraction f5{0, 1};
  f5 /= int32_t{-6385781};
  REQUIRE(f5.numerator() == 0);
  REQUIRE(f5.denominator() == 1);
}  // TEST_CASE "fraction divide equals int32_t"

TEST_CASE("int32_t divide fraction", "[utils][fraction]") {
  // positive positive
  fishbait::Fraction f0{6385774, 4311095};
  f0 = int32_t{472894} / f0;
  REQUIRE(f0.numerator() == 1019345479465);
  REQUIRE(f0.denominator() == 3192887);
  // positive negative
  fishbait::Fraction f1{-4939442, 8358183};
  f1 = int32_t{2504439} / f1;
  REQUIRE(f1.numerator() == -20932559474337);
  REQUIRE(f1.denominator() == 4939442);
  // negative positive
  fishbait::Fraction f2{3456977, 3195741};
  f2 = int32_t{-8603303} / f2;
  REQUIRE(f2.numerator() == -27493928132523);
  REQUIRE(f2.denominator() == 3456977);
  // negative negative
  fishbait::Fraction f3{-2323290, 5572093};
  f3 = int32_t{-3310561} / f3;
  REQUIRE(f3.numerator() == 18446753774173);
  REQUIRE(f3.denominator() == 2323290);
  // 0 positive
  fishbait::Fraction f4{341009, 4673302};
  f4 = int32_t{0} / f4;
  REQUIRE(f4.numerator() == 0);
  REQUIRE(f4.denominator() == 1);
  // 0 negative
  fishbait::Fraction f5{-875073, 1127173};
  f5 = int32_t{0} / f5;
  REQUIRE(f5.numerator() == 0);
  REQUIRE(f5.denominator() == 1);
}  // TEST_CASE "int32_t divide fraction"

TEST_CASE("fraction divide int32_t", "[utils][fraction]") {
  // positive positive
  fishbait::Fraction f0{701136, 7946371};
  f0 = f0 / int32_t{3669375};
  REQUIRE(f0.numerator() == 233712);
  REQUIRE(f0.denominator() == 9719405029375);
  // positive negative
  fishbait::Fraction f1{62867, 147831};
  f1 = f1 / int32_t{-5795067};
  REQUIRE(f1.numerator() == -62867);
  REQUIRE(f1.denominator() == 856690549677);
  // negative positive
  fishbait::Fraction f2{-5928379, 1984741};
  f2 = f2 / int32_t{4379830};
  REQUIRE(f2.numerator() == -5928379);
  REQUIRE(f2.denominator() == 8692828174030);
  // negative negative
  fishbait::Fraction f3{-535401, 2620303};
  f3 = f3 / int32_t{-7268758};
  REQUIRE(f3.numerator() == 535401);
  REQUIRE(f3.denominator() == 19046348393674);
  // 0 positive
  fishbait::Fraction f4{0, 1};
  f4 = f4 / int32_t{6341529};
  REQUIRE(f4.numerator() == 0);
  REQUIRE(f4.denominator() == 1);
  // 0 negative
  fishbait::Fraction f5{0, 1};
  f5 = f5 / int32_t{-7234594};
  REQUIRE(f5.numerator() == 0);
  REQUIRE(f5.denominator() == 1);
}  // TEST_CASE "fraction divide int32_t"

TEST_CASE("fraction divide equals uint32_t", "[utils][fraction]") {
  // positive positive
  fishbait::Fraction f0{847499, 8707120};
  f0 /= uint32_t{7206775};
  REQUIRE(f0.numerator() == 847499);
  REQUIRE(f0.denominator() == 62750254738000);
  // negative positive
  fishbait::Fraction f1{-7995121, 9213523};
  f1 /= uint32_t{6514383};
  REQUIRE(f1.numerator() == -7995121);
  REQUIRE(f1.denominator() == 60020417601309);
  // 0 positive
  fishbait::Fraction f2{0, 1};
  f2 /= uint32_t{6430429};
  REQUIRE(f2.numerator() == 0);
  REQUIRE(f2.denominator() == 1);
}  // TEST_CASE "fraction divide equals uint32_t"

TEST_CASE("uint32_t divide fraction", "[utils][fraction]") {
  // positive positive
  fishbait::Fraction f0{2841416, 5175611};
  f0 = uint32_t{4849451} / f0;
  REQUIRE(f0.numerator() == 25098871939561);
  REQUIRE(f0.denominator() == 2841416);
  // positive negative
  fishbait::Fraction f1{-1187504, 2734439};
  f1 = uint32_t{7975362} / f1;
  REQUIRE(f1.numerator() == -10904070445959);
  REQUIRE(f1.denominator() == 593752);
  // 0 positive
  fishbait::Fraction f2{7680531, 2150110};
  f2 = uint32_t{0} / f2;
  REQUIRE(f2.numerator() == 0);
  REQUIRE(f2.denominator() == 1);
  // 0 negative
  fishbait::Fraction f3{-6253024, 2209401};
  f3 = uint32_t{0} / f3;
  REQUIRE(f3.numerator() == 0);
  REQUIRE(f3.denominator() == 1);
}  // TEST_CASE "uint32_t divide fraction"

TEST_CASE("fraction divide uint32_t", "[utils][fraction]") {
  // positive positive
  fishbait::Fraction f0{7010211, 1988798};
  f0 = f0 / uint32_t{9589357};
  REQUIRE(f0.numerator() == 7010211);
  REQUIRE(f0.denominator() == 19071294022886);
  // negative positive
  fishbait::Fraction f1{-5245451, 4692342};
  f1 = f1 / uint32_t{6263833};
  REQUIRE(f1.numerator() == -5245451);
  REQUIRE(f1.denominator() == 29392046666886);
  // 0 positive
  fishbait::Fraction f2{0, 1};
  f2 = f2 / uint32_t{7822345};
  REQUIRE(f2.numerator() == 0);
  REQUIRE(f2.denominator() == 1);
}  // TEST_CASE "fraction divide uint32_t"

TEST_CASE("fraction divide equals int64_t", "[utils][fraction]") {
  // positive positive
  fishbait::Fraction f0{114183, 712802};
  f0 /= int64_t{1385474};
  REQUIRE(f0.numerator() == 114183);
  REQUIRE(f0.denominator() == 987568638148);
  // positive negative
  fishbait::Fraction f1{2978304, 37243};
  f1 /= int64_t{-4592996};
  REQUIRE(f1.numerator() == -744576);
  REQUIRE(f1.denominator() == 42764237507);
  // negative positive
  fishbait::Fraction f2{-306188, 404835};
  f2 /= int64_t{3996083};
  REQUIRE(f2.numerator() == -306188);
  REQUIRE(f2.denominator() == 1617754261305);
  // negative negative
  fishbait::Fraction f3{-249722, 106979};
  f3 /= int64_t{-6284316};
  REQUIRE(f3.numerator() == 124861);
  REQUIRE(f3.denominator() == 336144920682);
  // 0 positive
  fishbait::Fraction f4{0, 1};
  f4 /= int64_t{4667662};
  REQUIRE(f4.numerator() == 0);
  REQUIRE(f4.denominator() == 1);
  // 0 negative
  fishbait::Fraction f5{0, 1};
  f5 /= int64_t{-1698363};
  REQUIRE(f5.numerator() == 0);
  REQUIRE(f5.denominator() == 1);
}  // TEST_CASE "fraction divide equals int64_t"

TEST_CASE("int64_t divide fraction", "[utils][fraction]") {
  // positive positive
  fishbait::Fraction f0{749659, 9274129};
  f0 = int64_t{8911184} / f0;
  REQUIRE(f0.numerator() == 82643469958736);
  REQUIRE(f0.denominator() == 749659);
  // positive negative
  fishbait::Fraction f1{-3424227, 5781935};
  f1 = int64_t{3284888} / f1;
  REQUIRE(f1.numerator() == -18993008898280);
  REQUIRE(f1.denominator() == 3424227);
  // negative positive
  fishbait::Fraction f2{4371105, 3859294};
  f2 = int64_t{-9881104} / f2;
  REQUIRE(f2.numerator() == -38134085380576);
  REQUIRE(f2.denominator() == 4371105);
  // negative negative
  fishbait::Fraction f3{-656774, 1957961};
  f3 = int64_t{-5482355} / f3;
  REQUIRE(f3.numerator() == 10734237278155);
  REQUIRE(f3.denominator() == 656774);
  // 0 positive
  fishbait::Fraction f4{390974, 1101171};
  f4 = int64_t{0} / f4;
  REQUIRE(f4.numerator() == 0);
  REQUIRE(f4.denominator() == 1);
  // 0 negative
  fishbait::Fraction f5{-5210993, 2251343};
  f5 = int64_t{0} / f5;
  REQUIRE(f5.numerator() == 0);
  REQUIRE(f5.denominator() == 1);
}  // TEST_CASE "int64_t divide fraction"

TEST_CASE("fraction divide int64_t", "[utils][fraction]") {
  // positive positive
  fishbait::Fraction f0{1073422, 3249385};
  f0 = f0 / int64_t{8606909};
  REQUIRE(f0.numerator() == 1073422);
  REQUIRE(f0.denominator() == 27967161000965);
  // positive negative
  fishbait::Fraction f1{9385944, 2030039};
  f1 = f1 / int64_t{-4903123};
  REQUIRE(f1.numerator() == -9385944);
  REQUIRE(f1.denominator() == 9953530911797);
  // negative positive
  fishbait::Fraction f2{-1304752, 4409335};
  f2 = f2 / int64_t{3232366};
  REQUIRE(f2.numerator() == -652376);
  REQUIRE(f2.denominator() == 7126292268305);
  // negative negative
  fishbait::Fraction f3{-8864162, 350447};
  f3 = f3 / int64_t{-3916502};
  REQUIRE(f3.numerator() == 4432081);
  REQUIRE(f3.denominator() == 686263188197);
  // 0 positive
  fishbait::Fraction f4{0, 1};
  f4 = f4 / int64_t{8548876};
  REQUIRE(f4.numerator() == 0);
  REQUIRE(f4.denominator() == 1);
  // 0 negative
  fishbait::Fraction f5{0, 1};
  f5 = f5 / int64_t{-7969066};
  REQUIRE(f5.numerator() == 0);
  REQUIRE(f5.denominator() == 1);
}  // TEST_CASE "fraction divide int64_t"

TEST_CASE("double divide fraction", "[utils][fraction]") {
  // positive positive
  fishbait::Fraction f0{8093025, 9377948};
  REQUIRE(8420695.656372929 / f0 == Approx(9757642.660104373));
  // positive negative
  fishbait::Fraction f1{-4544916, 9659521};
  REQUIRE(2932173.375873551 / f1 == Approx(-6231884.219618461));
  // negative positive
  fishbait::Fraction f2{8439648, 6998963};
  REQUIRE(-1721066.8333151126 / f2 == Approx(-1427273.1619730634));
  // negative negative
  fishbait::Fraction f3{-3424250, 4072979};
  REQUIRE(-437836.23347182403 / f3 == Approx(520784.92644223885));
  // 0 positive
  fishbait::Fraction f4{6214821, 5580722};
  REQUIRE(0.0 / f4 == Approx(0.0));
  // 0 negative
  fishbait::Fraction f5{-7248671, 3027306};
  REQUIRE(0.0 / f5 == Approx(-0.0));
}  // TEST_CASE "double divide fraction"

TEST_CASE("fraction divide double", "[utils][fraction]") {
  // positive positive
  fishbait::Fraction f0{1333028, 2111073};
  REQUIRE(f0 / 9649035.534962928 == Approx(6.544132962909185e-08));
  // positive negative
  fishbait::Fraction f1{6756898, 6446193};
  REQUIRE(f1 / -46379.372884804536 == Approx(-2.260055929399173e-05));
  // negative positive
  fishbait::Fraction f2{-591676, 828055};
  REQUIRE(f2 / 2961530.157667278 == Approx(-2.4127293655990715e-07));
  // negative negative
  fishbait::Fraction f3{-509676, 4947197};
  REQUIRE(f3 / -2997850.844100309 == Approx(3.4365681290596e-08));
  // 0 positive
  fishbait::Fraction f4{0, 1};
  REQUIRE(f4 / 9936459.201396106 == Approx(0.0));
  // 0 negative
  fishbait::Fraction f5{0, 1};
  REQUIRE(f5 / -1913004.2060920424 == Approx(-0.0));
}  // TEST_CASE "fraction divide double"

TEST_CASE("fraction divide fraction", "[utils][fraction]") {
  // positive positive
  fishbait::Fraction f0 = fishbait::Fraction(251619, 7538764) /
                          fishbait::Fraction(663527, 1585043);
  REQUIRE(f0.numerator() == 398826934617);
  REQUIRE(f0.denominator() == 5002173460628);
  // positive negative
  fishbait::Fraction f1 = fishbait::Fraction(6278912, 9846649) /
                          fishbait::Fraction(-3518442, 5086997);
  REQUIRE(f1.numerator() == -15970403253632);
  REQUIRE(f1.denominator() == 17322431700429);
  // negative positive
  fishbait::Fraction f2 = fishbait::Fraction(-818086, 210847) /
                          fishbait::Fraction(1012134, 86359);
  REQUIRE(f2.numerator() == -388181807);
  REQUIRE(f2.denominator() == 1172557239);
  // negative negative
  fishbait::Fraction f3 = fishbait::Fraction(-2916527, 2629343) /
                          fishbait::Fraction(-4498061, 5187826);
  REQUIRE(f3.numerator() == 15130434600302);
  REQUIRE(f3.denominator() == 11826945203923);
  // 0 positive
  fishbait::Fraction f4 = fishbait::Fraction(0, 1) /
                          fishbait::Fraction(4101953, 9793471);
  REQUIRE(f4.numerator() == 0);
  REQUIRE(f4.denominator() == 1);
  // 0 negative
  fishbait::Fraction f5 = fishbait::Fraction(0, 1) /
                          fishbait::Fraction(-9804379, 7304334);
  REQUIRE(f5.numerator() == 0);
  REQUIRE(f5.denominator() == 1);
}  // TEST_CASE "fraction divide fraction"

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
/* -------------------------------- Equals ---------------------------------- */
/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */

TEST_CASE("fraction fraction equals", "[utils][fraction]") {
  // positive positive equal
  fishbait::Fraction f0{395027, 7918289};
  fishbait::Fraction f1{395027, 7918289};
  REQUIRE((f0 == f1) == true);
  // positive positive not equal
  fishbait::Fraction f2{5522729, 1838588};
  fishbait::Fraction f3{500786, 2494539};
  REQUIRE((f2 == f3) == false);
  // positive negative
  fishbait::Fraction f4{1653151, 6619608};
  fishbait::Fraction f5{-9082831, 5024934};
  REQUIRE((f4 == f5) == false);
  // positive 0
  fishbait::Fraction f6{8444623, 1434861};
  fishbait::Fraction f7{0, 1};
  REQUIRE((f6 == f7) == false);
  // negative positive
  fishbait::Fraction f8{-7486411, 6257001};
  fishbait::Fraction f9{2593515, 9243037};
  REQUIRE((f8 == f9) == false);
  // negative negative equal
  fishbait::Fraction f10{-3502523, 9373887};
  fishbait::Fraction f11{-3502523, 9373887};
  REQUIRE((f10 == f11) == true);
  // negative negative not equal
  fishbait::Fraction f12{-1069983, 6202538};
  fishbait::Fraction f13{-2413556, 1558355};
  REQUIRE((f12 == f13) == false);
  // negative 0
  fishbait::Fraction f14{-1605071, 1463825};
  fishbait::Fraction f15{0, 1};
  REQUIRE((f14 == f15) == false);
  // 0 positive
  fishbait::Fraction f16{0, 1};
  fishbait::Fraction f17{403071, 601651};
  REQUIRE((f16 == f17) == false);
  // 0 negative
  fishbait::Fraction f18{0, 1};
  fishbait::Fraction f19{-3078073, 1348377};
  REQUIRE((f18 == f19) == false);
  // 0 0
  fishbait::Fraction f20{0, 1};
  fishbait::Fraction f21{0, 1};
  REQUIRE((f20 == f21) == true);
}  // TEST_CASE "fraction fraction equals"

TEST_CASE("int8_t fraction equals", "[utils][fraction]") {
  // positive positive equal
  fishbait::Fraction f0{69, 1};
  REQUIRE((int8_t{69} == f0) == true);
  // positive positive not equal
  fishbait::Fraction f1{212560, 2640647};
  REQUIRE((int8_t{1} == f1) == false);
  // positive negative
  fishbait::Fraction f2{-1979000, 1240341};
  REQUIRE((int8_t{56} == f2) == false);
  // positive 0
  fishbait::Fraction f3{0, 1};
  REQUIRE((int8_t{91} == f3) == false);
  // negative positive
  fishbait::Fraction f4{157351, 506307};
  REQUIRE((int8_t{-18} == f4) == false);
  // negative negative equal
  fishbait::Fraction f5{-115, 1};
  REQUIRE((int8_t{-115} == f5) == true);
  // negative negative not equal
  fishbait::Fraction f6{-2630613, 60922};
  REQUIRE((int8_t{-108} == f6) == false);
  // negative 0
  fishbait::Fraction f7{0, 1};
  REQUIRE((int8_t{-108} == f7) == false);
  // 0 positive
  fishbait::Fraction f8{4421437, 4429574};
  REQUIRE((int8_t{0} == f8) == false);
  // 0 negative
  fishbait::Fraction f9{-10332, 9071};
  REQUIRE((int8_t{0} == f9) == false);
  // 0 0
  fishbait::Fraction f10{0, 1};
  REQUIRE((int8_t{0} == f10) == true);
}  // TEST_CASE "int8_t fraction equals"

TEST_CASE("fraction equals int8_t", "[utils][fraction]") {
  // positive positive equal
  fishbait::Fraction f0{96, 1};
  REQUIRE((f0 == int8_t{96}) == true);
  // positive positive not equal
  fishbait::Fraction f1{653810, 1493003};
  REQUIRE((f1 == int8_t{15}) == false);
  // positive negative
  fishbait::Fraction f2{183452, 1209487};
  REQUIRE((f2 == int8_t{-102}) == false);
  // positive 0
  fishbait::Fraction f3{9988565, 4933592};
  REQUIRE((f3 == int8_t{0}) == false);
  // negative positive
  fishbait::Fraction f4{-858011, 1614804};
  REQUIRE((f4 == int8_t{98}) == false);
  // negative negative equal
  fishbait::Fraction f5{-68, 1};
  REQUIRE((f5 == int8_t{-68}) == true);
  // negative negative not equal
  fishbait::Fraction f6{-5876944, 4440791};
  REQUIRE((f6 == int8_t{-63}) == false);
  // negative 0
  fishbait::Fraction f7{-4554365, 2528263};
  REQUIRE((f7 == int8_t{0}) == false);
  // 0 positive
  fishbait::Fraction f8{0, 1};
  REQUIRE((f8 == int8_t{111}) == false);
  // 0 negative
  fishbait::Fraction f9{0, 1};
  REQUIRE((f9 == int8_t{-114}) == false);
  // 0 0
  fishbait::Fraction f10{0, 1};
  REQUIRE((f10 == int8_t{0}) == true);
}  // TEST_CASE "fraction equals int8_t"

TEST_CASE("uint8_t fraction equals", "[utils][fraction]") {
  // positive positive equal
  fishbait::Fraction f0{57, 1};
  REQUIRE((uint8_t{57} == f0) == true);
  // positive positive not equal
  fishbait::Fraction f1{6680192, 308089};
  REQUIRE((uint8_t{10} == f1) == false);
  // positive negative
  fishbait::Fraction f2{-451878, 141655};
  REQUIRE((uint8_t{124} == f2) == false);
  // positive 0
  fishbait::Fraction f3{0, 1};
  REQUIRE((uint8_t{114} == f3) == false);
  // 0 positive
  fishbait::Fraction f4{4082171, 3325404};
  REQUIRE((uint8_t{0} == f4) == false);
  // 0 negative
  fishbait::Fraction f5{-196326, 6014597};
  REQUIRE((uint8_t{0} == f5) == false);
  // 0 0
  fishbait::Fraction f6{0, 1};
  REQUIRE((uint8_t{0} == f6) == true);
}  // TEST_CASE "uint8_t fraction equals"

TEST_CASE("fraction equals uint8_t", "[utils][fraction]") {
  // positive positive equal
  fishbait::Fraction f0{88, 1};
  REQUIRE((f0 == uint8_t{88}) == true);
  // positive positive not equal
  fishbait::Fraction f1{1054969, 2394886};
  REQUIRE((f1 == uint8_t{83}) == false);
  // positive 0
  fishbait::Fraction f2{8514103, 2346648};
  REQUIRE((f2 == uint8_t{0}) == false);
  // negative positive
  fishbait::Fraction f3{-4603308, 8011867};
  REQUIRE((f3 == uint8_t{244}) == false);
  // negative 0
  fishbait::Fraction f4{-248126, 6352211};
  REQUIRE((f4 == uint8_t{0}) == false);
  // 0 positive
  fishbait::Fraction f5{0, 1};
  REQUIRE((f5 == uint8_t{147}) == false);
  // 0 0
  fishbait::Fraction f6{0, 1};
  REQUIRE((f6 == uint8_t{0}) == true);
}  // TEST_CASE "fraction equals uint8_t"

TEST_CASE("int16_t fraction equals", "[utils][fraction]") {
  // positive positive equal
  fishbait::Fraction f0{14299, 1};
  REQUIRE((int16_t{14299} == f0) == true);
  // positive positive not equal
  fishbait::Fraction f1{9109292, 9157213};
  REQUIRE((int16_t{25910} == f1) == false);
  // positive negative
  fishbait::Fraction f2{-2687079, 4199684};
  REQUIRE((int16_t{14067} == f2) == false);
  // positive 0
  fishbait::Fraction f3{0, 1};
  REQUIRE((int16_t{197} == f3) == false);
  // negative positive
  fishbait::Fraction f4{3963839, 7328577};
  REQUIRE((int16_t{-17818} == f4) == false);
  // negative negative equal
  fishbait::Fraction f5{-16050, 1};
  REQUIRE((int16_t{-16050} == f5) == true);
  // negative negative not equal
  fishbait::Fraction f6{-9910639, 4186529};
  REQUIRE((int16_t{-15280} == f6) == false);
  // negative 0
  fishbait::Fraction f7{0, 1};
  REQUIRE((int16_t{-12855} == f7) == false);
  // 0 positive
  fishbait::Fraction f8{8857858, 8239871};
  REQUIRE((int16_t{0} == f8) == false);
  // 0 negative
  fishbait::Fraction f9{-7530752, 8742217};
  REQUIRE((int16_t{0} == f9) == false);
  // 0 0
  fishbait::Fraction f10{0, 1};
  REQUIRE((int16_t{0} == f10) == true);
}  // TEST_CASE "int16_t fraction equals"

TEST_CASE("fraction equals int16_t", "[utils][fraction]") {
  // positive positive equal
  fishbait::Fraction f0{8732, 1};
  REQUIRE((f0 == int16_t{8732}) == true);
  // positive positive not equal
  fishbait::Fraction f1{3416069, 4140162};
  REQUIRE((f1 == int16_t{31930}) == false);
  // positive negative
  fishbait::Fraction f2{3341817, 4376024};
  REQUIRE((f2 == int16_t{-24876}) == false);
  // positive 0
  fishbait::Fraction f3{4129738, 4081451};
  REQUIRE((f3 == int16_t{0}) == false);
  // negative positive
  fishbait::Fraction f4{-297158, 1093085};
  REQUIRE((f4 == int16_t{16916}) == false);
  // negative negative equal
  fishbait::Fraction f5{-9582, 1};
  REQUIRE((f5 == int16_t{-9582}) == true);
  // negative negative not equal
  fishbait::Fraction f6{-23462, 14581};
  REQUIRE((f6 == int16_t{-22678}) == false);
  // negative 0
  fishbait::Fraction f7{-739502, 2767595};
  REQUIRE((f7 == int16_t{0}) == false);
  // 0 positive
  fishbait::Fraction f8{0, 1};
  REQUIRE((f8 == int16_t{9488}) == false);
  // 0 negative
  fishbait::Fraction f9{0, 1};
  REQUIRE((f9 == int16_t{-4171}) == false);
  // 0 0
  fishbait::Fraction f10{0, 1};
  REQUIRE((f10 == int16_t{0}) == true);
}  // TEST_CASE "fraction equals int16_t"

TEST_CASE("uint16_t fraction equals", "[utils][fraction]") {
  // positive positive equal
  fishbait::Fraction f0{6266, 1};
  REQUIRE((uint16_t{6266} == f0) == true);
  // positive positive not equal
  fishbait::Fraction f1{7162029, 8168249};
  REQUIRE((uint16_t{11101} == f1) == false);
  // positive negative
  fishbait::Fraction f2{-4679517, 5482366};
  REQUIRE((uint16_t{1637} == f2) == false);
  // positive 0
  fishbait::Fraction f3{0, 1};
  REQUIRE((uint16_t{18547} == f3) == false);
  // 0 positive
  fishbait::Fraction f4{1168507, 6957153};
  REQUIRE((uint16_t{0} == f4) == false);
  // 0 negative
  fishbait::Fraction f5{-1107567, 1344028};
  REQUIRE((uint16_t{0} == f5) == false);
  // 0 0
  fishbait::Fraction f6{0, 1};
  REQUIRE((uint16_t{0} == f6) == true);
}  // TEST_CASE "uint16_t fraction equals"

TEST_CASE("fraction equals uint16_t", "[utils][fraction]") {
  // positive positive equal
  fishbait::Fraction f0{15883, 1};
  REQUIRE((f0 == uint16_t{15883}) == true);
  // positive positive not equal
  fishbait::Fraction f1{6695767, 4010184};
  REQUIRE((f1 == uint16_t{55933}) == false);
  // positive 0
  fishbait::Fraction f2{758174, 1904539};
  REQUIRE((f2 == uint16_t{0}) == false);
  // negative positive
  fishbait::Fraction f3{-1311689, 1905387};
  REQUIRE((f3 == uint16_t{29959}) == false);
  // negative 0
  fishbait::Fraction f4{-5242640, 9251919};
  REQUIRE((f4 == uint16_t{0}) == false);
  // 0 positive
  fishbait::Fraction f5{0, 1};
  REQUIRE((f5 == uint16_t{6828}) == false);
  // 0 0
  fishbait::Fraction f6{0, 1};
  REQUIRE((f6 == uint16_t{0}) == true);
}  // TEST_CASE "fraction equals uint16_t"

TEST_CASE("int32_t fraction equals", "[utils][fraction]") {
  // positive positive equal
  fishbait::Fraction f0{8004705, 1};
  REQUIRE((int32_t{8004705} == f0) == true);
  // positive positive not equal
  fishbait::Fraction f1{9386940, 7227869};
  REQUIRE((int32_t{2958603} == f1) == false);
  // positive negative
  fishbait::Fraction f2{-1616403, 1242526};
  REQUIRE((int32_t{2985898} == f2) == false);
  // positive 0
  fishbait::Fraction f3{0, 1};
  REQUIRE((int32_t{9073540} == f3) == false);
  // negative positive
  fishbait::Fraction f4{34289, 42813};
  REQUIRE((int32_t{-5634362} == f4) == false);
  // negative negative equal
  fishbait::Fraction f5{-9717027, 1};
  REQUIRE((int32_t{-9717027} == f5) == true);
  // negative negative not equal
  fishbait::Fraction f6{-8311623, 330985};
  REQUIRE((int32_t{-1275927} == f6) == false);
  // negative 0
  fishbait::Fraction f7{0, 1};
  REQUIRE((int32_t{-1972217} == f7) == false);
  // 0 positive
  fishbait::Fraction f8{8313939, 1469447};
  REQUIRE((int32_t{0} == f8) == false);
  // 0 negative
  fishbait::Fraction f9{-8274701, 5543031};
  REQUIRE((int32_t{0} == f9) == false);
  // 0 0
  fishbait::Fraction f10{0, 1};
  REQUIRE((int32_t{0} == f10) == true);
}  // TEST_CASE "int32_t fraction equals"

TEST_CASE("fraction equals int32_t", "[utils][fraction]") {
  // positive positive equal
  fishbait::Fraction f0{4046384, 1};
  REQUIRE((f0 == int32_t{4046384}) == true);
  // positive positive not equal
  fishbait::Fraction f1{270361, 4790199};
  REQUIRE((f1 == int32_t{8222265}) == false);
  // positive negative
  fishbait::Fraction f2{4216387, 5853580};
  REQUIRE((f2 == int32_t{-7473569}) == false);
  // positive 0
  fishbait::Fraction f3{2989530, 546067};
  REQUIRE((f3 == int32_t{0}) == false);
  // negative positive
  fishbait::Fraction f4{-1143282, 1629203};
  REQUIRE((f4 == int32_t{9994675}) == false);
  // negative negative equal
  fishbait::Fraction f5{-3887733, 1};
  REQUIRE((f5 == int32_t{-3887733}) == true);
  // negative negative not equal
  fishbait::Fraction f6{-197188, 28301};
  REQUIRE((f6 == int32_t{-4007092}) == false);
  // negative 0
  fishbait::Fraction f7{-5453881, 9130265};
  REQUIRE((f7 == int32_t{0}) == false);
  // 0 positive
  fishbait::Fraction f8{0, 1};
  REQUIRE((f8 == int32_t{384499}) == false);
  // 0 negative
  fishbait::Fraction f9{0, 1};
  REQUIRE((f9 == int32_t{-1853662}) == false);
  // 0 0
  fishbait::Fraction f10{0, 1};
  REQUIRE((f10 == int32_t{0}) == true);
}  // TEST_CASE "fraction equals int32_t"

TEST_CASE("uint32_t fraction equals", "[utils][fraction]") {
  // positive positive equal
  fishbait::Fraction f0{6012843, 1};
  REQUIRE((uint32_t{6012843} == f0) == true);
  // positive positive not equal
  fishbait::Fraction f1{2478201, 525542};
  REQUIRE((uint32_t{5567871} == f1) == false);
  // positive negative
  fishbait::Fraction f2{-5593955, 3645201};
  REQUIRE((uint32_t{8012397} == f2) == false);
  // positive 0
  fishbait::Fraction f3{0, 1};
  REQUIRE((uint32_t{832061} == f3) == false);
  // 0 positive
  fishbait::Fraction f4{2732633, 2328822};
  REQUIRE((uint32_t{0} == f4) == false);
  // 0 negative
  fishbait::Fraction f5{-1862348, 7218561};
  REQUIRE((uint32_t{0} == f5) == false);
  // 0 0
  fishbait::Fraction f6{0, 1};
  REQUIRE((uint32_t{0} == f6) == true);
}  // TEST_CASE "uint32_t fraction equals"

TEST_CASE("fraction equals uint32_t", "[utils][fraction]") {
  // positive positive equal
  fishbait::Fraction f0{5308606, 1};
  REQUIRE((f0 == uint32_t{5308606}) == true);
  // positive positive not equal
  fishbait::Fraction f1{6408298, 4663037};
  REQUIRE((f1 == uint32_t{5365928}) == false);
  // positive 0
  fishbait::Fraction f2{3889585, 1955384};
  REQUIRE((f2 == uint32_t{0}) == false);
  // negative positive
  fishbait::Fraction f3{-416481, 782117};
  REQUIRE((f3 == uint32_t{6635671}) == false);
  // negative 0
  fishbait::Fraction f4{-6321151, 3364326};
  REQUIRE((f4 == uint32_t{0}) == false);
  // 0 positive
  fishbait::Fraction f5{0, 1};
  REQUIRE((f5 == uint32_t{7339656}) == false);
  // 0 0
  fishbait::Fraction f6{0, 1};
  REQUIRE((f6 == uint32_t{0}) == true);
}  // TEST_CASE "fraction equals uint32_t"

TEST_CASE("int64_t fraction equals", "[utils][fraction]") {
  // positive positive equal
  fishbait::Fraction f0{8326188, 1};
  REQUIRE((int64_t{8326188} == f0) == true);
  // positive positive not equal
  fishbait::Fraction f1{6245875, 5809054};
  REQUIRE((int64_t{7911715} == f1) == false);
  // positive negative
  fishbait::Fraction f2{-1289486, 962975};
  REQUIRE((int64_t{7891245} == f2) == false);
  // positive 0
  fishbait::Fraction f3{0, 1};
  REQUIRE((int64_t{2583439} == f3) == false);
  // negative positive
  fishbait::Fraction f4{607461, 8416997};
  REQUIRE((int64_t{-9340597} == f4) == false);
  // negative negative equal
  fishbait::Fraction f5{-2333011, 1};
  REQUIRE((int64_t{-2333011} == f5) == true);
  // negative negative not equal
  fishbait::Fraction f6{-3472352, 9697825};
  REQUIRE((int64_t{-2668756} == f6) == false);
  // negative 0
  fishbait::Fraction f7{0, 1};
  REQUIRE((int64_t{-1252373} == f7) == false);
  // 0 positive
  fishbait::Fraction f8{465158, 3576737};
  REQUIRE((int64_t{0} == f8) == false);
  // 0 negative
  fishbait::Fraction f9{-1772799, 897356};
  REQUIRE((int64_t{0} == f9) == false);
  // 0 0
  fishbait::Fraction f10{0, 1};
  REQUIRE((int64_t{0} == f10) == true);
}  // TEST_CASE "int64_t fraction equals"

TEST_CASE("fraction equals int64_t", "[utils][fraction]") {
  // positive positive equal
  fishbait::Fraction f0{5491338, 1};
  REQUIRE((f0 == int64_t{5491338}) == true);
  // positive positive not equal
  fishbait::Fraction f1{3739007, 9071270};
  REQUIRE((f1 == int64_t{4617816}) == false);
  // positive negative
  fishbait::Fraction f2{5566306, 1001295};
  REQUIRE((f2 == int64_t{-3883077}) == false);
  // positive 0
  fishbait::Fraction f3{8677185, 6574148};
  REQUIRE((f3 == int64_t{0}) == false);
  // negative positive
  fishbait::Fraction f4{-6128263, 8862350};
  REQUIRE((f4 == int64_t{310511}) == false);
  // negative negative equal
  fishbait::Fraction f5{-7892018, 1};
  REQUIRE((f5 == int64_t{-7892018}) == true);
  // negative negative not equal
  fishbait::Fraction f6{-1139031, 880829};
  REQUIRE((f6 == int64_t{-4910053}) == false);
  // negative 0
  fishbait::Fraction f7{-578663, 599640};
  REQUIRE((f7 == int64_t{0}) == false);
  // 0 positive
  fishbait::Fraction f8{0, 1};
  REQUIRE((f8 == int64_t{992687}) == false);
  // 0 negative
  fishbait::Fraction f9{0, 1};
  REQUIRE((f9 == int64_t{-9289313}) == false);
  // 0 0
  fishbait::Fraction f10{0, 1};
  REQUIRE((f10 == int64_t{0}) == true);
}  // TEST_CASE "fraction equals int64_t"

TEST_CASE("double fraction equals", "[utils][fraction]") {
  // positive positive equal
  fishbait::Fraction f0{1254184459227837, 2147483648};
  REQUIRE((584025.1498053954 == f0) == true);
  // positive positive not equal
  fishbait::Fraction f1{5946679, 4381008};
  REQUIRE((3026976.596261782 == f1) == false);
  // positive negative
  fishbait::Fraction f2{-8302605, 7683536};
  REQUIRE((769810.6452040143 == f2) == false);
  // positive 0
  fishbait::Fraction f3{0, 1};
  REQUIRE((9184748.499422569 == f3) == false);
  // negative positive
  fishbait::Fraction f4{3255554, 4912423};
  REQUIRE((-3217949.2464841884 == f4) == false);
  // negative negative equal
  fishbait::Fraction f5{-5491964806049201, 2147483648};
  REQUIRE((-2557395.4014336695 == f5) == true);
  // negative negative not equal
  fishbait::Fraction f6{-40875, 3998528};
  REQUIRE((-860280.4183938077 == f6) == false);
  // negative 0
  fishbait::Fraction f7{0, 1};
  REQUIRE((-4172657.8020866686 == f7) == false);
  // 0 positive
  fishbait::Fraction f8{1815131, 957731};
  REQUIRE((0.0 == f8) == false);
  // 0 negative
  fishbait::Fraction f9{-2147033, 1472994};
  REQUIRE((0.0 == f9) == false);
  // 0 0
  fishbait::Fraction f10{0, 1};
  REQUIRE((0.0 == f10) == true);
}  // TEST_CASE "double fraction equals"

TEST_CASE("fraction double equals", "[utils][fraction]") {
  // positive positive equal
  fishbait::Fraction f0{3653503836092857, 1073741824};
  REQUIRE((f0 == 3402590.598997527) == true);
  // positive positive not equal
  fishbait::Fraction f1{4807732, 1462449};
  REQUIRE((f1 == 9365301.769976139) == false);
  // positive negative
  fishbait::Fraction f2{6992843, 3569389};
  REQUIRE((f2 == -7965930.252493443) == false);
  // positive 0
  fishbait::Fraction f3{8541965, 5503668};
  REQUIRE((f3 == 0.0) == false);
  // negative positive
  fishbait::Fraction f4{-245564, 176733};
  REQUIRE((f4 == 8868685.441901067) == false);
  // negative negative equal
  fishbait::Fraction f5{-8579739230697913, 2147483648};
  REQUIRE((f5 == -3995252.4149315027) == true);
  // negative negative not equal
  fishbait::Fraction f6{-549681, 59416};
  REQUIRE((f6 == -7497956.29958038) == false);
  // negative 0
  fishbait::Fraction f7{-819669, 3820160};
  REQUIRE((f7 == 0.0) == false);
  // 0 positive
  fishbait::Fraction f8{0, 1};
  REQUIRE((f8 == 715480.6240489782) == false);
  // 0 negative
  fishbait::Fraction f9{0, 1};
  REQUIRE((f9 == -5802592.277093917) == false);
  // 0 0
  fishbait::Fraction f10{0, 1};
  REQUIRE((f10 == 0.0) == true);
}  // TEST_CASE "fraction double equals"

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
/* ------------------------------ Not Equals -------------------------------- */
/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */

TEST_CASE("fraction fraction not equals", "[utils][fraction]") {
  // positive positive equal
  fishbait::Fraction f0{4784863, 5579732};
  fishbait::Fraction f1{4784863, 5579732};
  REQUIRE((f0 != f1) == false);
  // positive positive not equal
  fishbait::Fraction f2{1308302, 7994741};
  fishbait::Fraction f3{3446356, 7656447};
  REQUIRE((f2 != f3) == true);
  // positive negative
  fishbait::Fraction f4{4813924, 1638195};
  fishbait::Fraction f5{-4625168, 15569};
  REQUIRE((f4 != f5) == true);
  // positive 0
  fishbait::Fraction f6{8400731, 9692350};
  fishbait::Fraction f7{0, 1};
  REQUIRE((f6 != f7) == true);
  // negative positive
  fishbait::Fraction f8{-4265593, 1939401};
  fishbait::Fraction f9{3150767, 2114575};
  REQUIRE((f8 != f9) == true);
  // negative negative equal
  fishbait::Fraction f10{-2727191, 1855821};
  fishbait::Fraction f11{-2727191, 1855821};
  REQUIRE((f10 != f11) == false);
  // negative negative not equal
  fishbait::Fraction f12{-2390935, 615831};
  fishbait::Fraction f13{-249716, 4216729};
  REQUIRE((f12 != f13) == true);
  // negative 0
  fishbait::Fraction f14{-265051, 9539524};
  fishbait::Fraction f15{0, 1};
  REQUIRE((f14 != f15) == true);
  // 0 positive
  fishbait::Fraction f16{0, 1};
  fishbait::Fraction f17{1929077, 2800867};
  REQUIRE((f16 != f17) == true);
  // 0 negative
  fishbait::Fraction f18{0, 1};
  fishbait::Fraction f19{-1152403, 6771032};
  REQUIRE((f18 != f19) == true);
  // 0 0
  fishbait::Fraction f20{0, 1};
  fishbait::Fraction f21{0, 1};
  REQUIRE((f20 != f21) == false);
}  // TEST_CASE "fraction fraction not equals"

TEST_CASE("int8_t fraction not equals", "[utils][fraction]") {
  // positive positive equal
  fishbait::Fraction f0{63, 1};
  REQUIRE((int8_t{63} != f0) == false);
  // positive positive not equal
  fishbait::Fraction f1{1760445, 461227};
  REQUIRE((int8_t{121} != f1) == true);
  // positive negative
  fishbait::Fraction f2{-1657845, 1568159};
  REQUIRE((int8_t{103} != f2) == true);
  // positive 0
  fishbait::Fraction f3{0, 1};
  REQUIRE((int8_t{40} != f3) == true);
  // negative positive
  fishbait::Fraction f4{9673555, 1083359};
  REQUIRE((int8_t{-45} != f4) == true);
  // negative negative equal
  fishbait::Fraction f5{-53, 1};
  REQUIRE((int8_t{-53} != f5) == false);
  // negative negative not equal
  fishbait::Fraction f6{-2814289, 542088};
  REQUIRE((int8_t{-118} != f6) == true);
  // negative 0
  fishbait::Fraction f7{0, 1};
  REQUIRE((int8_t{-43} != f7) == true);
  // 0 positive
  fishbait::Fraction f8{6949375, 7057136};
  REQUIRE((int8_t{0} != f8) == true);
  // 0 negative
  fishbait::Fraction f9{-8778380, 6990739};
  REQUIRE((int8_t{0} != f9) == true);
  // 0 0
  fishbait::Fraction f10{0, 1};
  REQUIRE((int8_t{0} != f10) == false);
}  // TEST_CASE "int8_t fraction not equals"

TEST_CASE("fraction not equals int8_t", "[utils][fraction]") {
  // positive positive equal
  fishbait::Fraction f0{47, 1};
  REQUIRE((f0 != int8_t{47}) == false);
  // positive positive not equal
  fishbait::Fraction f1{728194, 2995707};
  REQUIRE((f1 != int8_t{29}) == true);
  // positive negative
  fishbait::Fraction f2{8113505, 8442673};
  REQUIRE((f2 != int8_t{-77}) == true);
  // positive 0
  fishbait::Fraction f3{3139612, 1729479};
  REQUIRE((f3 != int8_t{0}) == true);
  // negative positive
  fishbait::Fraction f4{-2016401, 2046742};
  REQUIRE((f4 != int8_t{86}) == true);
  // negative negative equal
  fishbait::Fraction f5{-69, 1};
  REQUIRE((f5 != int8_t{-69}) == false);
  // negative negative not equal
  fishbait::Fraction f6{-8795107, 6029427};
  REQUIRE((f6 != int8_t{-87}) == true);
  // negative 0
  fishbait::Fraction f7{-8623496, 3714169};
  REQUIRE((f7 != int8_t{0}) == true);
  // 0 positive
  fishbait::Fraction f8{0, 1};
  REQUIRE((f8 != int8_t{123}) == true);
  // 0 negative
  fishbait::Fraction f9{0, 1};
  REQUIRE((f9 != int8_t{-52}) == true);
  // 0 0
  fishbait::Fraction f10{0, 1};
  REQUIRE((f10 != int8_t{0}) == false);
}  // TEST_CASE "fraction not equals int8_t"

TEST_CASE("uint8_t fraction not equals", "[utils][fraction]") {
  // positive positive equal
  fishbait::Fraction f0{206, 1};
  REQUIRE((uint8_t{206} != f0) == false);
  // positive positive not equal
  fishbait::Fraction f1{815125, 3526332};
  REQUIRE((uint8_t{182} != f1) == true);
  // positive negative
  fishbait::Fraction f2{-2092946, 2487003};
  REQUIRE((uint8_t{138} != f2) == true);
  // positive 0
  fishbait::Fraction f3{0, 1};
  REQUIRE((uint8_t{117} != f3) == true);
  // 0 positive
  fishbait::Fraction f4{4547037, 9412082};
  REQUIRE((uint8_t{0} != f4) == true);
  // 0 negative
  fishbait::Fraction f5{-6589879, 8444374};
  REQUIRE((uint8_t{0} != f5) == true);
  // 0 0
  fishbait::Fraction f6{0, 1};
  REQUIRE((uint8_t{0} != f6) == false);
}  // TEST_CASE "uint8_t fraction not equals"

TEST_CASE("fraction not equals uint8_t", "[utils][fraction]") {
  // positive positive equal
  fishbait::Fraction f0{111, 1};
  REQUIRE((f0 != uint8_t{111}) == false);
  // positive positive not equal
  fishbait::Fraction f1{6824994, 9951467};
  REQUIRE((f1 != uint8_t{135}) == true);
  // positive 0
  fishbait::Fraction f2{31630, 622413};
  REQUIRE((f2 != uint8_t{0}) == true);
  // negative positive
  fishbait::Fraction f3{-8897591, 9429372};
  REQUIRE((f3 != uint8_t{82}) == true);
  // negative 0
  fishbait::Fraction f4{-9446328, 1710293};
  REQUIRE((f4 != uint8_t{0}) == true);
  // 0 positive
  fishbait::Fraction f5{0, 1};
  REQUIRE((f5 != uint8_t{55}) == true);
  // 0 0
  fishbait::Fraction f6{0, 1};
  REQUIRE((f6 != uint8_t{0}) == false);
}  // TEST_CASE "fraction not equals uint8_t"

TEST_CASE("int16_t fraction not equals", "[utils][fraction]") {
  // positive positive equal
  fishbait::Fraction f0{16350, 1};
  REQUIRE((int16_t{16350} != f0) == false);
  // positive positive not equal
  fishbait::Fraction f1{5083293, 2772518};
  REQUIRE((int16_t{19500} != f1) == true);
  // positive negative
  fishbait::Fraction f2{-4598258, 1064719};
  REQUIRE((int16_t{21797} != f2) == true);
  // positive 0
  fishbait::Fraction f3{0, 1};
  REQUIRE((int16_t{6234} != f3) == true);
  // negative positive
  fishbait::Fraction f4{953788, 367247};
  REQUIRE((int16_t{-30371} != f4) == true);
  // negative negative equal
  fishbait::Fraction f5{-749, 1};
  REQUIRE((int16_t{-749} != f5) == false);
  // negative negative not equal
  fishbait::Fraction f6{-3532765, 8727472};
  REQUIRE((int16_t{-10993} != f6) == true);
  // negative 0
  fishbait::Fraction f7{0, 1};
  REQUIRE((int16_t{-22009} != f7) == true);
  // 0 positive
  fishbait::Fraction f8{345267, 7750394};
  REQUIRE((int16_t{0} != f8) == true);
  // 0 negative
  fishbait::Fraction f9{-5045438, 8037977};
  REQUIRE((int16_t{0} != f9) == true);
  // 0 0
  fishbait::Fraction f10{0, 1};
  REQUIRE((int16_t{0} != f10) == false);
}  // TEST_CASE "int16_t fraction not equals"

TEST_CASE("fraction not equals int16_t", "[utils][fraction]") {
  // positive positive equal
  fishbait::Fraction f0{24514, 1};
  REQUIRE((f0 != int16_t{24514}) == false);
  // positive positive not equal
  fishbait::Fraction f1{8211865, 5676359};
  REQUIRE((f1 != int16_t{17771}) == true);
  // positive negative
  fishbait::Fraction f2{270461, 532224};
  REQUIRE((f2 != int16_t{-2152}) == true);
  // positive 0
  fishbait::Fraction f3{6841078, 9768049};
  REQUIRE((f3 != int16_t{0}) == true);
  // negative positive
  fishbait::Fraction f4{-9442327, 8532854};
  REQUIRE((f4 != int16_t{13913}) == true);
  // negative negative equal
  fishbait::Fraction f5{-24751, 1};
  REQUIRE((f5 != int16_t{-24751}) == false);
  // negative negative not equal
  fishbait::Fraction f6{-7151239, 5297165};
  REQUIRE((f6 != int16_t{-19737}) == true);
  // negative 0
  fishbait::Fraction f7{-8822595, 4606678};
  REQUIRE((f7 != int16_t{0}) == true);
  // 0 positive
  fishbait::Fraction f8{0, 1};
  REQUIRE((f8 != int16_t{20375}) == true);
  // 0 negative
  fishbait::Fraction f9{0, 1};
  REQUIRE((f9 != int16_t{-13289}) == true);
  // 0 0
  fishbait::Fraction f10{0, 1};
  REQUIRE((f10 != int16_t{0}) == false);
}  // TEST_CASE "fraction not equals int16_t"

TEST_CASE("uint16_t fraction not equals", "[utils][fraction]") {
  // positive positive equal
  fishbait::Fraction f0{16250, 1};
  REQUIRE((uint16_t{16250} != f0) == false);
  // positive positive not equal
  fishbait::Fraction f1{8334202, 271519};
  REQUIRE((uint16_t{37529} != f1) == true);
  // positive negative
  fishbait::Fraction f2{-1777815, 7207321};
  REQUIRE((uint16_t{3235} != f2) == true);
  // positive 0
  fishbait::Fraction f3{0, 1};
  REQUIRE((uint16_t{6594} != f3) == true);
  // 0 positive
  fishbait::Fraction f4{952553, 8659050};
  REQUIRE((uint16_t{0} != f4) == true);
  // 0 negative
  fishbait::Fraction f5{-367703, 725561};
  REQUIRE((uint16_t{0} != f5) == true);
  // 0 0
  fishbait::Fraction f6{0, 1};
  REQUIRE((uint16_t{0} != f6) == false);
}  // TEST_CASE "uint16_t fraction not equals"

TEST_CASE("fraction not equals uint16_t", "[utils][fraction]") {
  // positive positive equal
  fishbait::Fraction f0{45576, 1};
  REQUIRE((f0 != uint16_t{45576}) == false);
  // positive positive not equal
  fishbait::Fraction f1{3445655, 5205664};
  REQUIRE((f1 != uint16_t{12022}) == true);
  // positive 0
  fishbait::Fraction f2{1155747, 334753};
  REQUIRE((f2 != uint16_t{0}) == true);
  // negative positive
  fishbait::Fraction f3{-6290495, 5471118};
  REQUIRE((f3 != uint16_t{49274}) == true);
  // negative 0
  fishbait::Fraction f4{-1801916, 1496543};
  REQUIRE((f4 != uint16_t{0}) == true);
  // 0 positive
  fishbait::Fraction f5{0, 1};
  REQUIRE((f5 != uint16_t{24181}) == true);
  // 0 0
  fishbait::Fraction f6{0, 1};
  REQUIRE((f6 != uint16_t{0}) == false);
}  // TEST_CASE "fraction not equals uint16_t"

TEST_CASE("int32_t fraction not equals", "[utils][fraction]") {
  // positive positive equal
  fishbait::Fraction f0{6450621, 1};
  REQUIRE((int32_t{6450621} != f0) == false);
  // positive positive not equal
  fishbait::Fraction f1{9796116, 2620765};
  REQUIRE((int32_t{1601842} != f1) == true);
  // positive negative
  fishbait::Fraction f2{-2383485, 3158656};
  REQUIRE((int32_t{466989} != f2) == true);
  // positive 0
  fishbait::Fraction f3{0, 1};
  REQUIRE((int32_t{4757820} != f3) == true);
  // negative positive
  fishbait::Fraction f4{1671651, 1244477};
  REQUIRE((int32_t{-2420105} != f4) == true);
  // negative negative equal
  fishbait::Fraction f5{-9228110, 1};
  REQUIRE((int32_t{-9228110} != f5) == false);
  // negative negative not equal
  fishbait::Fraction f6{-2790383, 3314838};
  REQUIRE((int32_t{-5521282} != f6) == true);
  // negative 0
  fishbait::Fraction f7{0, 1};
  REQUIRE((int32_t{-1123086} != f7) == true);
  // 0 positive
  fishbait::Fraction f8{114569, 3040725};
  REQUIRE((int32_t{0} != f8) == true);
  // 0 negative
  fishbait::Fraction f9{-1445659, 3354522};
  REQUIRE((int32_t{0} != f9) == true);
  // 0 0
  fishbait::Fraction f10{0, 1};
  REQUIRE((int32_t{0} != f10) == false);
}  // TEST_CASE "int32_t fraction not equals"

TEST_CASE("fraction not equals int32_t", "[utils][fraction]") {
  // positive positive equal
  fishbait::Fraction f0{1396092, 1};
  REQUIRE((f0 != int32_t{1396092}) == false);
  // positive positive not equal
  fishbait::Fraction f1{8894893, 5789787};
  REQUIRE((f1 != int32_t{5711380}) == true);
  // positive negative
  fishbait::Fraction f2{2180453, 494664};
  REQUIRE((f2 != int32_t{-1250187}) == true);
  // positive 0
  fishbait::Fraction f3{1888843, 941913};
  REQUIRE((f3 != int32_t{0}) == true);
  // negative positive
  fishbait::Fraction f4{-636666, 1146665};
  REQUIRE((f4 != int32_t{1930684}) == true);
  // negative negative equal
  fishbait::Fraction f5{-4975878, 1};
  REQUIRE((f5 != int32_t{-4975878}) == false);
  // negative negative not equal
  fishbait::Fraction f6{-395183, 1872801};
  REQUIRE((f6 != int32_t{-1896270}) == true);
  // negative 0
  fishbait::Fraction f7{-7934282, 539787};
  REQUIRE((f7 != int32_t{0}) == true);
  // 0 positive
  fishbait::Fraction f8{0, 1};
  REQUIRE((f8 != int32_t{3418604}) == true);
  // 0 negative
  fishbait::Fraction f9{0, 1};
  REQUIRE((f9 != int32_t{-8457415}) == true);
  // 0 0
  fishbait::Fraction f10{0, 1};
  REQUIRE((f10 != int32_t{0}) == false);
}  // TEST_CASE "fraction not equals int32_t"

TEST_CASE("uint32_t fraction not equals", "[utils][fraction]") {
  // positive positive equal
  fishbait::Fraction f0{5563522, 1};
  REQUIRE((uint32_t{5563522} != f0) == false);
  // positive positive not equal
  fishbait::Fraction f1{3151475, 408136};
  REQUIRE((uint32_t{7472871} != f1) == true);
  // positive negative
  fishbait::Fraction f2{-3312928, 3241323};
  REQUIRE((uint32_t{3913709} != f2) == true);
  // positive 0
  fishbait::Fraction f3{0, 1};
  REQUIRE((uint32_t{574345} != f3) == true);
  // 0 positive
  fishbait::Fraction f4{4893291, 298703};
  REQUIRE((uint32_t{0} != f4) == true);
  // 0 negative
  fishbait::Fraction f5{-1509221, 4073162};
  REQUIRE((uint32_t{0} != f5) == true);
  // 0 0
  fishbait::Fraction f6{0, 1};
  REQUIRE((uint32_t{0} != f6) == false);
}  // TEST_CASE "uint32_t fraction not equals"

TEST_CASE("fraction not equals uint32_t", "[utils][fraction]") {
  // positive positive equal
  fishbait::Fraction f0{4964319, 1};
  REQUIRE((f0 != uint32_t{4964319}) == false);
  // positive positive not equal
  fishbait::Fraction f1{2617807, 9183075};
  REQUIRE((f1 != uint32_t{74089}) == true);
  // positive 0
  fishbait::Fraction f2{5254691, 5816550};
  REQUIRE((f2 != uint32_t{0}) == true);
  // negative positive
  fishbait::Fraction f3{-3113426, 885985};
  REQUIRE((f3 != uint32_t{6633685}) == true);
  // negative 0
  fishbait::Fraction f4{-9190648, 3104237};
  REQUIRE((f4 != uint32_t{0}) == true);
  // 0 positive
  fishbait::Fraction f5{0, 1};
  REQUIRE((f5 != uint32_t{3706473}) == true);
  // 0 0
  fishbait::Fraction f6{0, 1};
  REQUIRE((f6 != uint32_t{0}) == false);
}  // TEST_CASE "fraction not equals uint32_t"

TEST_CASE("int64_t fraction not equals", "[utils][fraction]") {
  // positive positive equal
  fishbait::Fraction f0{1167696, 1};
  REQUIRE((int64_t{1167696} != f0) == false);
  // positive positive not equal
  fishbait::Fraction f1{1510732, 1508755};
  REQUIRE((int64_t{3979851} != f1) == true);
  // positive negative
  fishbait::Fraction f2{-2795183, 1790341};
  REQUIRE((int64_t{5533328} != f2) == true);
  // positive 0
  fishbait::Fraction f3{0, 1};
  REQUIRE((int64_t{7559198} != f3) == true);
  // negative positive
  fishbait::Fraction f4{9314, 69507};
  REQUIRE((int64_t{-2793856} != f4) == true);
  // negative negative equal
  fishbait::Fraction f5{-2387417, 1};
  REQUIRE((int64_t{-2387417} != f5) == false);
  // negative negative not equal
  fishbait::Fraction f6{-3773079, 2219264};
  REQUIRE((int64_t{-5213889} != f6) == true);
  // negative 0
  fishbait::Fraction f7{0, 1};
  REQUIRE((int64_t{-673459} != f7) == true);
  // 0 positive
  fishbait::Fraction f8{1997401, 8366125};
  REQUIRE((int64_t{0} != f8) == true);
  // 0 negative
  fishbait::Fraction f9{-6230454, 2455241};
  REQUIRE((int64_t{0} != f9) == true);
  // 0 0
  fishbait::Fraction f10{0, 1};
  REQUIRE((int64_t{0} != f10) == false);
}  // TEST_CASE "int64_t fraction not equals"

TEST_CASE("fraction not equals int64_t", "[utils][fraction]") {
  // positive positive equal
  fishbait::Fraction f0{1086373, 1};
  REQUIRE((f0 != int64_t{1086373}) == false);
  // positive positive not equal
  fishbait::Fraction f1{1243279, 921932};
  REQUIRE((f1 != int64_t{1655786}) == true);
  // positive negative
  fishbait::Fraction f2{5921679, 3589703};
  REQUIRE((f2 != int64_t{-663184}) == true);
  // positive 0
  fishbait::Fraction f3{175735, 113507};
  REQUIRE((f3 != int64_t{0}) == true);
  // negative positive
  fishbait::Fraction f4{-5551878, 5215367};
  REQUIRE((f4 != int64_t{34322}) == true);
  // negative negative equal
  fishbait::Fraction f5{-8550176, 1};
  REQUIRE((f5 != int64_t{-8550176}) == false);
  // negative negative not equal
  fishbait::Fraction f6{-1153810, 1273191};
  REQUIRE((f6 != int64_t{-8135654}) == true);
  // negative 0
  fishbait::Fraction f7{-5058351, 8535164};
  REQUIRE((f7 != int64_t{0}) == true);
  // 0 positive
  fishbait::Fraction f8{0, 1};
  REQUIRE((f8 != int64_t{4035663}) == true);
  // 0 negative
  fishbait::Fraction f9{0, 1};
  REQUIRE((f9 != int64_t{-7091088}) == true);
  // 0 0
  fishbait::Fraction f10{0, 1};
  REQUIRE((f10 != int64_t{0}) == false);
}  // TEST_CASE "fraction not equals int64_t"

TEST_CASE("double fraction not equals", "[utils][fraction]") {
  // positive positive equal
  fishbait::Fraction f0{5133499974770585, 34359738368};
  REQUIRE((149404.51291536985 != f0) == false);
  // positive positive not equal
  fishbait::Fraction f1{8059762, 188615};
  REQUIRE((5362800.232690665 != f1) == true);
  // positive negative
  fishbait::Fraction f2{-4902649, 6702049};
  REQUIRE((8665515.00836767 != f2) == true);
  // positive 0
  fishbait::Fraction f3{0, 1};
  REQUIRE((6390471.586417233 != f3) == true);
  // negative positive
  fishbait::Fraction f4{4260741, 8412316};
  REQUIRE((-6294873.300062752 != f4) == true);
  // negative negative equal
  fishbait::Fraction f5{-2121792849246457, 536870912};
  REQUIRE((-3952147.1583218444 != f5) == false);
  // negative negative not equal
  fishbait::Fraction f6{-7027706, 6777077};
  REQUIRE((-8491979.624541113 != f6) == true);
  // negative 0
  fishbait::Fraction f7{0, 1};
  REQUIRE((-185726.15206693986 != f7) == true);
  // 0 positive
  fishbait::Fraction f8{4677616, 3286375};
  REQUIRE((0.0 != f8) == true);
  // 0 negative
  fishbait::Fraction f9{-9176744, 3473681};
  REQUIRE((0.0 != f9) == true);
  // 0 0
  fishbait::Fraction f10{0, 1};
  REQUIRE((0.0 != f10) == false);
}  // TEST_CASE "double fraction not equals"

TEST_CASE("fraction double not equals", "[utils][fraction]") {
  // positive positive equal
  fishbait::Fraction f0{5939754552283169, 8589934592};
  REQUIRE((f0 != 691478.4377770462) == false);
  // positive positive not equal
  fishbait::Fraction f1{4968715, 1513492};
  REQUIRE((f1 != 2062128.0540153321) == true);
  // positive negative
  fishbait::Fraction f2{1573199, 1514019};
  REQUIRE((f2 != -9359109.03770745) == true);
  // positive 0
  fishbait::Fraction f3{3850703, 1632702};
  REQUIRE((f3 != 0.0) == true);
  // negative positive
  fishbait::Fraction f4{-7901725, 8421009};
  REQUIRE((f4 != 7706090.605823248) == true);
  // negative negative equal
  fishbait::Fraction f5{-87831276472221, 33554432};
  REQUIRE((f5 != -2617576.0171479285) == false);
  // negative negative not equal
  fishbait::Fraction f6{-8277083, 579668};
  REQUIRE((f6 != -3113123.563958323) == true);
  // negative 0
  fishbait::Fraction f7{-7905317, 3014447};
  REQUIRE((f7 != 0.0) == true);
  // 0 positive
  fishbait::Fraction f8{0, 1};
  REQUIRE((f8 != 9986532.967339925) == true);
  // 0 negative
  fishbait::Fraction f9{0, 1};
  REQUIRE((f9 != -2971244.98770663) == true);
  // 0 0
  fishbait::Fraction f10{0, 1};
  REQUIRE((f10 != 0.0) == false);
}  // TEST_CASE "fraction double not equals"

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
/* ------------------------------- Less Than -------------------------------- */
/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */

TEST_CASE("fraction fraction less than", "[utils][fraction]") {
  // positive positive equal
  fishbait::Fraction f0{8540447, 3219370};
  fishbait::Fraction f1{8540447, 3219370};
  REQUIRE((f0 < f1) == false);
  // positive positive not equal
  fishbait::Fraction f2{362907, 565562};
  fishbait::Fraction f3{2443021, 4281737};
  REQUIRE((f2 < f3) == false);
  // positive negative
  fishbait::Fraction f4{116081, 75224};
  fishbait::Fraction f5{-1213180, 4830363};
  REQUIRE((f4 < f5) == false);
  // positive 0
  fishbait::Fraction f6{5748365, 3796257};
  fishbait::Fraction f7{0, 1};
  REQUIRE((f6 < f7) == false);
  // negative positive
  fishbait::Fraction f8{-7405364, 1738907};
  fishbait::Fraction f9{4154139, 3315332};
  REQUIRE((f8 < f9) == true);
  // negative negative equal
  fishbait::Fraction f10{-8111219, 5917911};
  fishbait::Fraction f11{-8111219, 5917911};
  REQUIRE((f10 < f11) == false);
  // negative negative not equal
  fishbait::Fraction f12{-1163783, 996192};
  fishbait::Fraction f13{-8466569, 5265962};
  REQUIRE((f12 < f13) == false);
  // negative 0
  fishbait::Fraction f14{-7898356, 1319409};
  fishbait::Fraction f15{0, 1};
  REQUIRE((f14 < f15) == true);
  // 0 positive
  fishbait::Fraction f16{0, 1};
  fishbait::Fraction f17{797034, 1674971};
  REQUIRE((f16 < f17) == true);
  // 0 negative
  fishbait::Fraction f18{0, 1};
  fishbait::Fraction f19{-2483077, 7867310};
  REQUIRE((f18 < f19) == false);
  // 0 0
  fishbait::Fraction f20{0, 1};
  fishbait::Fraction f21{0, 1};
  REQUIRE((f20 < f21) == false);
}  // TEST_CASE "fraction fraction less than"

TEST_CASE("int8_t fraction less than", "[utils][fraction]") {
  // positive positive equal
  fishbait::Fraction f0{61, 1};
  REQUIRE((int8_t{61} < f0) == false);
  // positive positive not equal
  fishbait::Fraction f1{1057403, 3267472};
  REQUIRE((int8_t{107} < f1) == false);
  // positive negative
  fishbait::Fraction f2{-985017, 522587};
  REQUIRE((int8_t{64} < f2) == false);
  // positive 0
  fishbait::Fraction f3{0, 1};
  REQUIRE((int8_t{67} < f3) == false);
  // negative positive
  fishbait::Fraction f4{9839115, 9856051};
  REQUIRE((int8_t{-105} < f4) == true);
  // negative negative equal
  fishbait::Fraction f5{-52, 1};
  REQUIRE((int8_t{-52} < f5) == false);
  // negative negative not equal
  fishbait::Fraction f6{-8612616, 1812959};
  REQUIRE((int8_t{-58} < f6) == true);
  // negative 0
  fishbait::Fraction f7{0, 1};
  REQUIRE((int8_t{-55} < f7) == true);
  // 0 positive
  fishbait::Fraction f8{7333444, 5362233};
  REQUIRE((int8_t{0} < f8) == true);
  // 0 negative
  fishbait::Fraction f9{-1051667, 1695235};
  REQUIRE((int8_t{0} < f9) == false);
  // 0 0
  fishbait::Fraction f10{0, 1};
  REQUIRE((int8_t{0} < f10) == false);
}  // TEST_CASE "int8_t fraction less than"

TEST_CASE("fraction less than int8_t", "[utils][fraction]") {
  // positive positive equal
  fishbait::Fraction f0{120, 1};
  REQUIRE((f0 < int8_t{120}) == false);
  // positive positive not equal
  fishbait::Fraction f1{8906819, 7139994};
  REQUIRE((f1 < int8_t{77}) == true);
  // positive negative
  fishbait::Fraction f2{1194967, 3226848};
  REQUIRE((f2 < int8_t{-15}) == false);
  // positive 0
  fishbait::Fraction f3{2754191, 3748174};
  REQUIRE((f3 < int8_t{0}) == false);
  // negative positive
  fishbait::Fraction f4{-2605963, 2804261};
  REQUIRE((f4 < int8_t{37}) == true);
  // negative negative equal
  fishbait::Fraction f5{-6, 1};
  REQUIRE((f5 < int8_t{-6}) == false);
  // negative negative not equal
  fishbait::Fraction f6{-740135, 802424};
  REQUIRE((f6 < int8_t{-22}) == false);
  // negative 0
  fishbait::Fraction f7{-4533371, 1716435};
  REQUIRE((f7 < int8_t{0}) == true);
  // 0 positive
  fishbait::Fraction f8{0, 1};
  REQUIRE((f8 < int8_t{62}) == true);
  // 0 negative
  fishbait::Fraction f9{0, 1};
  REQUIRE((f9 < int8_t{-64}) == false);
  // 0 0
  fishbait::Fraction f10{0, 1};
  REQUIRE((f10 < int8_t{0}) == false);
}  // TEST_CASE "fraction less than int8_t"

TEST_CASE("uint8_t fraction less than", "[utils][fraction]") {
  // positive positive equal
  fishbait::Fraction f0{131, 1};
  REQUIRE((uint8_t{131} < f0) == false);
  // positive positive not equal
  fishbait::Fraction f1{123640, 77749};
  REQUIRE((uint8_t{76} < f1) == false);
  // positive negative
  fishbait::Fraction f2{-2034643, 2455567};
  REQUIRE((uint8_t{41} < f2) == false);
  // positive 0
  fishbait::Fraction f3{0, 1};
  REQUIRE((uint8_t{30} < f3) == false);
  // 0 positive
  fishbait::Fraction f4{3763, 1656};
  REQUIRE((uint8_t{0} < f4) == true);
  // 0 negative
  fishbait::Fraction f5{-2527, 30550};
  REQUIRE((uint8_t{0} < f5) == false);
  // 0 0
  fishbait::Fraction f6{0, 1};
  REQUIRE((uint8_t{0} < f6) == false);
}  // TEST_CASE "uint8_t fraction less than"

TEST_CASE("fraction less than uint8_t", "[utils][fraction]") {
  // positive positive equal
  fishbait::Fraction f0{73, 1};
  REQUIRE((f0 < uint8_t{73}) == false);
  // positive positive not equal
  fishbait::Fraction f1{1752433, 1412412};
  REQUIRE((f1 < uint8_t{94}) == true);
  // positive 0
  fishbait::Fraction f2{270247, 471169};
  REQUIRE((f2 < uint8_t{0}) == false);
  // negative positive
  fishbait::Fraction f3{-6903737, 6191423};
  REQUIRE((f3 < uint8_t{247}) == true);
  // negative 0
  fishbait::Fraction f4{-572419, 1363606};
  REQUIRE((f4 < uint8_t{0}) == true);
  // 0 positive
  fishbait::Fraction f5{0, 1};
  REQUIRE((f5 < uint8_t{203}) == true);
  // 0 0
  fishbait::Fraction f6{0, 1};
  REQUIRE((f6 < uint8_t{0}) == false);
}  // TEST_CASE "fraction less than uint8_t"

TEST_CASE("int16_t fraction less than", "[utils][fraction]") {
  // positive positive equal
  fishbait::Fraction f0{10134, 1};
  REQUIRE((int16_t{10134} < f0) == false);
  // positive positive not equal
  fishbait::Fraction f1{9468314, 1232737};
  REQUIRE((int16_t{21704} < f1) == false);
  // positive negative
  fishbait::Fraction f2{-697143, 832978};
  REQUIRE((int16_t{17175} < f2) == false);
  // positive 0
  fishbait::Fraction f3{0, 1};
  REQUIRE((int16_t{25495} < f3) == false);
  // negative positive
  fishbait::Fraction f4{9258436, 1935357};
  REQUIRE((int16_t{-8866} < f4) == true);
  // negative negative equal
  fishbait::Fraction f5{-13281, 1};
  REQUIRE((int16_t{-13281} < f5) == false);
  // negative negative not equal
  fishbait::Fraction f6{-3082514, 2756435};
  REQUIRE((int16_t{-17767} < f6) == true);
  // negative 0
  fishbait::Fraction f7{0, 1};
  REQUIRE((int16_t{-4555} < f7) == true);
  // 0 positive
  fishbait::Fraction f8{1083658, 3279343};
  REQUIRE((int16_t{0} < f8) == true);
  // 0 negative
  fishbait::Fraction f9{-4121519, 3581390};
  REQUIRE((int16_t{0} < f9) == false);
  // 0 0
  fishbait::Fraction f10{0, 1};
  REQUIRE((int16_t{0} < f10) == false);
}  // TEST_CASE "int16_t fraction less than"

TEST_CASE("fraction less than int16_t", "[utils][fraction]") {
  // positive positive equal
  fishbait::Fraction f0{13079, 1};
  REQUIRE((f0 < int16_t{13079}) == false);
  // positive positive not equal
  fishbait::Fraction f1{6828599, 7380619};
  REQUIRE((f1 < int16_t{22122}) == true);
  // positive negative
  fishbait::Fraction f2{9517192, 9352149};
  REQUIRE((f2 < int16_t{-24507}) == false);
  // positive 0
  fishbait::Fraction f3{5142993, 2245807};
  REQUIRE((f3 < int16_t{0}) == false);
  // negative positive
  fishbait::Fraction f4{-1168873, 7330980};
  REQUIRE((f4 < int16_t{11933}) == true);
  // negative negative equal
  fishbait::Fraction f5{-6659, 1};
  REQUIRE((f5 < int16_t{-6659}) == false);
  // negative negative not equal
  fishbait::Fraction f6{-3399045, 8884271};
  REQUIRE((f6 < int16_t{-18194}) == false);
  // negative 0
  fishbait::Fraction f7{-1972161, 9858856};
  REQUIRE((f7 < int16_t{0}) == true);
  // 0 positive
  fishbait::Fraction f8{0, 1};
  REQUIRE((f8 < int16_t{23486}) == true);
  // 0 negative
  fishbait::Fraction f9{0, 1};
  REQUIRE((f9 < int16_t{-11356}) == false);
  // 0 0
  fishbait::Fraction f10{0, 1};
  REQUIRE((f10 < int16_t{0}) == false);
}  // TEST_CASE "fraction less than int16_t"

TEST_CASE("uint16_t fraction less than", "[utils][fraction]") {
  // positive positive equal
  fishbait::Fraction f0{2980, 1};
  REQUIRE((uint16_t{2980} < f0) == false);
  // positive positive not equal
  fishbait::Fraction f1{3646793, 830316};
  REQUIRE((uint16_t{28367} < f1) == false);
  // positive negative
  fishbait::Fraction f2{-1790000, 2271767};
  REQUIRE((uint16_t{54486} < f2) == false);
  // positive 0
  fishbait::Fraction f3{0, 1};
  REQUIRE((uint16_t{15549} < f3) == false);
  // 0 positive
  fishbait::Fraction f4{165959, 133837};
  REQUIRE((uint16_t{0} < f4) == true);
  // 0 negative
  fishbait::Fraction f5{-2365008, 2081059};
  REQUIRE((uint16_t{0} < f5) == false);
  // 0 0
  fishbait::Fraction f6{0, 1};
  REQUIRE((uint16_t{0} < f6) == false);
}  // TEST_CASE "uint16_t fraction less than"

TEST_CASE("fraction less than uint16_t", "[utils][fraction]") {
  // positive positive equal
  fishbait::Fraction f0{30252, 1};
  REQUIRE((f0 < uint16_t{30252}) == false);
  // positive positive not equal
  fishbait::Fraction f1{2385633, 1444493};
  REQUIRE((f1 < uint16_t{52309}) == true);
  // positive 0
  fishbait::Fraction f2{721882, 5042351};
  REQUIRE((f2 < uint16_t{0}) == false);
  // negative positive
  fishbait::Fraction f3{-3014167, 2805546};
  REQUIRE((f3 < uint16_t{36549}) == true);
  // negative 0
  fishbait::Fraction f4{-1988807, 1598002};
  REQUIRE((f4 < uint16_t{0}) == true);
  // 0 positive
  fishbait::Fraction f5{0, 1};
  REQUIRE((f5 < uint16_t{19103}) == true);
  // 0 0
  fishbait::Fraction f6{0, 1};
  REQUIRE((f6 < uint16_t{0}) == false);
}  // TEST_CASE "fraction less than uint16_t"

TEST_CASE("int32_t fraction less than", "[utils][fraction]") {
  // positive positive equal
  fishbait::Fraction f0{3207247, 1};
  REQUIRE((int32_t{3207247} < f0) == false);
  // positive positive not equal
  fishbait::Fraction f1{7892233, 2339491};
  REQUIRE((int32_t{9449106} < f1) == false);
  // positive negative
  fishbait::Fraction f2{-32812, 7170373};
  REQUIRE((int32_t{1519805} < f2) == false);
  // positive 0
  fishbait::Fraction f3{0, 1};
  REQUIRE((int32_t{5305083} < f3) == false);
  // negative positive
  fishbait::Fraction f4{58596, 9374629};
  REQUIRE((int32_t{-5585075} < f4) == true);
  // negative negative equal
  fishbait::Fraction f5{-608472, 1};
  REQUIRE((int32_t{-608472} < f5) == false);
  // negative negative not equal
  fishbait::Fraction f6{-1085379, 297397};
  REQUIRE((int32_t{-958717} < f6) == true);
  // negative 0
  fishbait::Fraction f7{0, 1};
  REQUIRE((int32_t{-6956751} < f7) == true);
  // 0 positive
  fishbait::Fraction f8{5189407, 8567444};
  REQUIRE((int32_t{0} < f8) == true);
  // 0 negative
  fishbait::Fraction f9{-443503, 853741};
  REQUIRE((int32_t{0} < f9) == false);
  // 0 0
  fishbait::Fraction f10{0, 1};
  REQUIRE((int32_t{0} < f10) == false);
}  // TEST_CASE "int32_t fraction less than"

TEST_CASE("fraction less than int32_t", "[utils][fraction]") {
  // positive positive equal
  fishbait::Fraction f0{9034175, 1};
  REQUIRE((f0 < int32_t{9034175}) == false);
  // positive positive not equal
  fishbait::Fraction f1{1977596, 5150383};
  REQUIRE((f1 < int32_t{2417311}) == true);
  // positive negative
  fishbait::Fraction f2{7882623, 4354832};
  REQUIRE((f2 < int32_t{-9245979}) == false);
  // positive 0
  fishbait::Fraction f3{2304247, 2814259};
  REQUIRE((f3 < int32_t{0}) == false);
  // negative positive
  fishbait::Fraction f4{-9968219, 3278035};
  REQUIRE((f4 < int32_t{7518104}) == true);
  // negative negative equal
  fishbait::Fraction f5{-4044422, 1};
  REQUIRE((f5 < int32_t{-4044422}) == false);
  // negative negative not equal
  fishbait::Fraction f6{-717091, 5009205};
  REQUIRE((f6 < int32_t{-9798911}) == false);
  // negative 0
  fishbait::Fraction f7{-3674459, 9474706};
  REQUIRE((f7 < int32_t{0}) == true);
  // 0 positive
  fishbait::Fraction f8{0, 1};
  REQUIRE((f8 < int32_t{9137370}) == true);
  // 0 negative
  fishbait::Fraction f9{0, 1};
  REQUIRE((f9 < int32_t{-8211559}) == false);
  // 0 0
  fishbait::Fraction f10{0, 1};
  REQUIRE((f10 < int32_t{0}) == false);
}  // TEST_CASE "fraction less than int32_t"

TEST_CASE("uint32_t fraction less than", "[utils][fraction]") {
  // positive positive equal
  fishbait::Fraction f0{6016604, 1};
  REQUIRE((uint32_t{6016604} < f0) == false);
  // positive positive not equal
  fishbait::Fraction f1{2809468, 2728221};
  REQUIRE((uint32_t{7010763} < f1) == false);
  // positive negative
  fishbait::Fraction f2{-4225048, 2078937};
  REQUIRE((uint32_t{5796594} < f2) == false);
  // positive 0
  fishbait::Fraction f3{0, 1};
  REQUIRE((uint32_t{2261799} < f3) == false);
  // 0 positive
  fishbait::Fraction f4{7539845, 5198117};
  REQUIRE((uint32_t{0} < f4) == true);
  // 0 negative
  fishbait::Fraction f5{-18797, 145849};
  REQUIRE((uint32_t{0} < f5) == false);
  // 0 0
  fishbait::Fraction f6{0, 1};
  REQUIRE((uint32_t{0} < f6) == false);
}  // TEST_CASE "uint32_t fraction less than"

TEST_CASE("fraction less than uint32_t", "[utils][fraction]") {
  // positive positive equal
  fishbait::Fraction f0{3014545, 1};
  REQUIRE((f0 < uint32_t{3014545}) == false);
  // positive positive not equal
  fishbait::Fraction f1{7053268, 9297953};
  REQUIRE((f1 < uint32_t{5001268}) == true);
  // positive 0
  fishbait::Fraction f2{9456485, 2316351};
  REQUIRE((f2 < uint32_t{0}) == false);
  // negative positive
  fishbait::Fraction f3{-2773235, 8455707};
  REQUIRE((f3 < uint32_t{5935514}) == true);
  // negative 0
  fishbait::Fraction f4{-5344745, 7256276};
  REQUIRE((f4 < uint32_t{0}) == true);
  // 0 positive
  fishbait::Fraction f5{0, 1};
  REQUIRE((f5 < uint32_t{1565602}) == true);
  // 0 0
  fishbait::Fraction f6{0, 1};
  REQUIRE((f6 < uint32_t{0}) == false);
}  // TEST_CASE "fraction less than uint32_t"

TEST_CASE("int64_t fraction less than", "[utils][fraction]") {
  // positive positive equal
  fishbait::Fraction f0{4117119, 1};
  REQUIRE((int64_t{4117119} < f0) == false);
  // positive positive not equal
  fishbait::Fraction f1{17295, 6520012};
  REQUIRE((int64_t{7243008} < f1) == false);
  // positive negative
  fishbait::Fraction f2{-632069, 9104927};
  REQUIRE((int64_t{1342446} < f2) == false);
  // positive 0
  fishbait::Fraction f3{0, 1};
  REQUIRE((int64_t{3219637} < f3) == false);
  // negative positive
  fishbait::Fraction f4{92245, 4202186};
  REQUIRE((int64_t{-4902600} < f4) == true);
  // negative negative equal
  fishbait::Fraction f5{-7779144, 1};
  REQUIRE((int64_t{-7779144} < f5) == false);
  // negative negative not equal
  fishbait::Fraction f6{-4541187, 2683027};
  REQUIRE((int64_t{-5060572} < f6) == true);
  // negative 0
  fishbait::Fraction f7{0, 1};
  REQUIRE((int64_t{-1295316} < f7) == true);
  // 0 positive
  fishbait::Fraction f8{9048083, 7196202};
  REQUIRE((int64_t{0} < f8) == true);
  // 0 negative
  fishbait::Fraction f9{-5666471, 8752889};
  REQUIRE((int64_t{0} < f9) == false);
  // 0 0
  fishbait::Fraction f10{0, 1};
  REQUIRE((int64_t{0} < f10) == false);
}  // TEST_CASE "int64_t fraction less than"

TEST_CASE("fraction less than int64_t", "[utils][fraction]") {
  // positive positive equal
  fishbait::Fraction f0{8147760, 1};
  REQUIRE((f0 < int64_t{8147760}) == false);
  // positive positive not equal
  fishbait::Fraction f1{1762307, 1690891};
  REQUIRE((f1 < int64_t{2601018}) == true);
  // positive negative
  fishbait::Fraction f2{1394922, 194015};
  REQUIRE((f2 < int64_t{-5495969}) == false);
  // positive 0
  fishbait::Fraction f3{6528835, 4565026};
  REQUIRE((f3 < int64_t{0}) == false);
  // negative positive
  fishbait::Fraction f4{-85952, 256437};
  REQUIRE((f4 < int64_t{5654123}) == true);
  // negative negative equal
  fishbait::Fraction f5{-712694, 1};
  REQUIRE((f5 < int64_t{-712694}) == false);
  // negative negative not equal
  fishbait::Fraction f6{-463616, 1198685};
  REQUIRE((f6 < int64_t{-4909575}) == false);
  // negative 0
  fishbait::Fraction f7{-465731, 3309};
  REQUIRE((f7 < int64_t{0}) == true);
  // 0 positive
  fishbait::Fraction f8{0, 1};
  REQUIRE((f8 < int64_t{2155190}) == true);
  // 0 negative
  fishbait::Fraction f9{0, 1};
  REQUIRE((f9 < int64_t{-4213611}) == false);
  // 0 0
  fishbait::Fraction f10{0, 1};
  REQUIRE((f10 < int64_t{0}) == false);
}  // TEST_CASE "fraction less than int64_t"

TEST_CASE("double fraction less than", "[utils][fraction]") {
  // positive positive equal
  fishbait::Fraction f0{2714023686310977, 4294967296};
  REQUIRE((631907.8817756327 < f0) == false);
  // positive positive not equal
  fishbait::Fraction f1{2293707, 730082};
  REQUIRE((8551371.46365554 < f1) == false);
  // positive negative
  fishbait::Fraction f2{-9759203, 5464488};
  REQUIRE((5197171.650309762 < f2) == false);
  // positive 0
  fishbait::Fraction f3{0, 1};
  REQUIRE((9876655.147697639 < f3) == false);
  // negative positive
  fishbait::Fraction f4{3499685, 7604277};
  REQUIRE((-6762481.577767846 < f4) == true);
  // negative negative equal
  fishbait::Fraction f5{-671404346248769, 134217728};
  REQUIRE((-5002352.194851406 < f5) == false);
  // negative negative not equal
  fishbait::Fraction f6{-9623750, 4072491};
  REQUIRE((-7066924.656570608 < f6) == true);
  // negative 0
  fishbait::Fraction f7{0, 1};
  REQUIRE((-2330608.7200199403 < f7) == true);
  // 0 positive
  fishbait::Fraction f8{658069, 1655447};
  REQUIRE((0.0 < f8) == true);
  // 0 negative
  fishbait::Fraction f9{-1787259, 163543};
  REQUIRE((0.0 < f9) == false);
  // 0 0
  fishbait::Fraction f10{0, 1};
  REQUIRE((0.0 < f10) == false);
}  // TEST_CASE "double fraction less than"

TEST_CASE("fraction double less than", "[utils][fraction]") {
  // positive positive equal
  fishbait::Fraction f0{3061389050079025, 536870912};
  REQUIRE((f0 < 5702281.464038463) == false);
  // positive positive not equal
  fishbait::Fraction f1{4662548, 6063049};
  REQUIRE((f1 < 3096757.626819014) == true);
  // positive negative
  fishbait::Fraction f2{3155499, 1306043};
  REQUIRE((f2 < -6960659.785650445) == false);
  // positive 0
  fishbait::Fraction f3{2183279, 1349786};
  REQUIRE((f3 < 0.0) == false);
  // negative positive
  fishbait::Fraction f4{-6455062, 5812399};
  REQUIRE((f4 < 7484126.118552723) == true);
  // negative negative equal
  fishbait::Fraction f5{-1357212609795887, 1073741824};
  REQUIRE((f5 < -1264002.7420556983) == false);
  // negative negative not equal
  fishbait::Fraction f6{-3525461, 5070218};
  REQUIRE((f6 < -454379.9661124032) == false);
  // negative 0
  fishbait::Fraction f7{-1160368, 471533};
  REQUIRE((f7 < 0.0) == true);
  // 0 positive
  fishbait::Fraction f8{0, 1};
  REQUIRE((f8 < 988881.2423591806) == true);
  // 0 negative
  fishbait::Fraction f9{0, 1};
  REQUIRE((f9 < -6021561.420920787) == false);
  // 0 0
  fishbait::Fraction f10{0, 1};
  REQUIRE((f10 < 0.0) == false);
}  // TEST_CASE "fraction double less than"

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
/* ----------------------------- Greater Than ------------------------------- */
/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */

TEST_CASE("fraction fraction greater than", "[utils][fraction]") {
  // positive positive equal
  fishbait::Fraction f0{9967595, 6189591};
  fishbait::Fraction f1{9967595, 6189591};
  REQUIRE((f0 > f1) == false);
  // positive positive not equal
  fishbait::Fraction f2{2664073, 1693021};
  fishbait::Fraction f3{2922779, 9076736};
  REQUIRE((f2 > f3) == true);
  // positive negative
  fishbait::Fraction f4{2573539, 2307641};
  fishbait::Fraction f5{-117466, 4038781};
  REQUIRE((f4 > f5) == true);
  // positive 0
  fishbait::Fraction f6{3220471, 8326864};
  fishbait::Fraction f7{0, 1};
  REQUIRE((f6 > f7) == true);
  // negative positive
  fishbait::Fraction f8{-1072411, 963001};
  fishbait::Fraction f9{5327717, 5507702};
  REQUIRE((f8 > f9) == false);
  // negative negative equal
  fishbait::Fraction f10{-1348727, 2150245};
  fishbait::Fraction f11{-1348727, 2150245};
  REQUIRE((f10 > f11) == false);
  // negative negative not equal
  fishbait::Fraction f12{-1386622, 2158625};
  fishbait::Fraction f13{-6611513, 93811};
  REQUIRE((f12 > f13) == true);
  // negative 0
  fishbait::Fraction f14{-5634589, 3952187};
  fishbait::Fraction f15{0, 1};
  REQUIRE((f14 > f15) == false);
  // 0 positive
  fishbait::Fraction f16{0, 1};
  fishbait::Fraction f17{1598313, 1451936};
  REQUIRE((f16 > f17) == false);
  // 0 negative
  fishbait::Fraction f18{0, 1};
  fishbait::Fraction f19{-2627840, 2533647};
  REQUIRE((f18 > f19) == true);
  // 0 0
  fishbait::Fraction f20{0, 1};
  fishbait::Fraction f21{0, 1};
  REQUIRE((f20 > f21) == false);
}  // TEST_CASE "fraction fraction greater than"

TEST_CASE("int8_t fraction greater than", "[utils][fraction]") {
  // positive positive equal
  fishbait::Fraction f0{72, 1};
  REQUIRE((int8_t{72} > f0) == false);
  // positive positive not equal
  fishbait::Fraction f1{3712351, 3804601};
  REQUIRE((int8_t{25} > f1) == true);
  // positive negative
  fishbait::Fraction f2{-2080189, 3292815};
  REQUIRE((int8_t{45} > f2) == true);
  // positive 0
  fishbait::Fraction f3{0, 1};
  REQUIRE((int8_t{24} > f3) == true);
  // negative positive
  fishbait::Fraction f4{4603087, 7983207};
  REQUIRE((int8_t{-73} > f4) == false);
  // negative negative equal
  fishbait::Fraction f5{-97, 1};
  REQUIRE((int8_t{-97} > f5) == false);
  // negative negative not equal
  fishbait::Fraction f6{-1624811, 823681};
  REQUIRE((int8_t{-68} > f6) == false);
  // negative 0
  fishbait::Fraction f7{0, 1};
  REQUIRE((int8_t{-48} > f7) == false);
  // 0 positive
  fishbait::Fraction f8{692855, 9636558};
  REQUIRE((int8_t{0} > f8) == false);
  // 0 negative
  fishbait::Fraction f9{-8039673, 9455035};
  REQUIRE((int8_t{0} > f9) == true);
  // 0 0
  fishbait::Fraction f10{0, 1};
  REQUIRE((int8_t{0} > f10) == false);
}  // TEST_CASE "int8_t fraction greater than"

TEST_CASE("fraction greater than int8_t", "[utils][fraction]") {
  // positive positive equal
  fishbait::Fraction f0{9, 1};
  REQUIRE((f0 > int8_t{9}) == false);
  // positive positive not equal
  fishbait::Fraction f1{557354, 872509};
  REQUIRE((f1 > int8_t{124}) == false);
  // positive negative
  fishbait::Fraction f2{9121865, 2716217};
  REQUIRE((f2 > int8_t{-91}) == true);
  // positive 0
  fishbait::Fraction f3{2005663, 980721};
  REQUIRE((f3 > int8_t{0}) == true);
  // negative positive
  fishbait::Fraction f4{-9986085, 2294713};
  REQUIRE((f4 > int8_t{26}) == false);
  // negative negative equal
  fishbait::Fraction f5{-22, 1};
  REQUIRE((f5 > int8_t{-22}) == false);
  // negative negative not equal
  fishbait::Fraction f6{-8149503, 1651724};
  REQUIRE((f6 > int8_t{-49}) == true);
  // negative 0
  fishbait::Fraction f7{-7613472, 7514465};
  REQUIRE((f7 > int8_t{0}) == false);
  // 0 positive
  fishbait::Fraction f8{0, 1};
  REQUIRE((f8 > int8_t{14}) == false);
  // 0 negative
  fishbait::Fraction f9{0, 1};
  REQUIRE((f9 > int8_t{-96}) == true);
  // 0 0
  fishbait::Fraction f10{0, 1};
  REQUIRE((f10 > int8_t{0}) == false);
}  // TEST_CASE "fraction greater than int8_t"

TEST_CASE("uint8_t fraction greater than", "[utils][fraction]") {
  // positive positive equal
  fishbait::Fraction f0{111, 1};
  REQUIRE((uint8_t{111} > f0) == false);
  // positive positive not equal
  fishbait::Fraction f1{4074053, 4028494};
  REQUIRE((uint8_t{237} > f1) == true);
  // positive negative
  fishbait::Fraction f2{-97091, 123974};
  REQUIRE((uint8_t{215} > f2) == true);
  // positive 0
  fishbait::Fraction f3{0, 1};
  REQUIRE((uint8_t{221} > f3) == true);
  // 0 positive
  fishbait::Fraction f4{1320353, 194656};
  REQUIRE((uint8_t{0} > f4) == false);
  // 0 negative
  fishbait::Fraction f5{-1807597, 1490011};
  REQUIRE((uint8_t{0} > f5) == true);
  // 0 0
  fishbait::Fraction f6{0, 1};
  REQUIRE((uint8_t{0} > f6) == false);
}  // TEST_CASE "uint8_t fraction greater than"

TEST_CASE("fraction greater than uint8_t", "[utils][fraction]") {
  // positive positive equal
  fishbait::Fraction f0{44, 1};
  REQUIRE((f0 > uint8_t{44}) == false);
  // positive positive not equal
  fishbait::Fraction f1{1836399, 1415543};
  REQUIRE((f1 > uint8_t{191}) == false);
  // positive 0
  fishbait::Fraction f2{9025088, 8435049};
  REQUIRE((f2 > uint8_t{0}) == true);
  // negative positive
  fishbait::Fraction f3{-1656437, 1237877};
  REQUIRE((f3 > uint8_t{30}) == false);
  // negative 0
  fishbait::Fraction f4{-65292, 720821};
  REQUIRE((f4 > uint8_t{0}) == false);
  // 0 positive
  fishbait::Fraction f5{0, 1};
  REQUIRE((f5 > uint8_t{28}) == false);
  // 0 0
  fishbait::Fraction f6{0, 1};
  REQUIRE((f6 > uint8_t{0}) == false);
}  // TEST_CASE "fraction greater than uint8_t"

TEST_CASE("int16_t fraction greater than", "[utils][fraction]") {
  // positive positive equal
  fishbait::Fraction f0{14810, 1};
  REQUIRE((int16_t{14810} > f0) == false);
  // positive positive not equal
  fishbait::Fraction f1{2070333, 2133127};
  REQUIRE((int16_t{28444} > f1) == true);
  // positive negative
  fishbait::Fraction f2{-4564042, 2594255};
  REQUIRE((int16_t{32698} > f2) == true);
  // positive 0
  fishbait::Fraction f3{0, 1};
  REQUIRE((int16_t{20254} > f3) == true);
  // negative positive
  fishbait::Fraction f4{893356, 5789555};
  REQUIRE((int16_t{-24758} > f4) == false);
  // negative negative equal
  fishbait::Fraction f5{-8102, 1};
  REQUIRE((int16_t{-8102} > f5) == false);
  // negative negative not equal
  fishbait::Fraction f6{-7425583, 2272461};
  REQUIRE((int16_t{-11308} > f6) == false);
  // negative 0
  fishbait::Fraction f7{0, 1};
  REQUIRE((int16_t{-24838} > f7) == false);
  // 0 positive
  fishbait::Fraction f8{4014267, 3638408};
  REQUIRE((int16_t{0} > f8) == false);
  // 0 negative
  fishbait::Fraction f9{-1285813, 738198};
  REQUIRE((int16_t{0} > f9) == true);
  // 0 0
  fishbait::Fraction f10{0, 1};
  REQUIRE((int16_t{0} > f10) == false);
}  // TEST_CASE "int16_t fraction greater than"

TEST_CASE("fraction greater than int16_t", "[utils][fraction]") {
  // positive positive equal
  fishbait::Fraction f0{31947, 1};
  REQUIRE((f0 > int16_t{31947}) == false);
  // positive positive not equal
  fishbait::Fraction f1{1564469, 2182793};
  REQUIRE((f1 > int16_t{6443}) == false);
  // positive negative
  fishbait::Fraction f2{3882416, 6892561};
  REQUIRE((f2 > int16_t{-20522}) == true);
  // positive 0
  fishbait::Fraction f3{7208843, 2612456};
  REQUIRE((f3 > int16_t{0}) == true);
  // negative positive
  fishbait::Fraction f4{-98481, 12584};
  REQUIRE((f4 > int16_t{1029}) == false);
  // negative negative equal
  fishbait::Fraction f5{-638, 1};
  REQUIRE((f5 > int16_t{-638}) == false);
  // negative negative not equal
  fishbait::Fraction f6{-4250731, 2216866};
  REQUIRE((f6 > int16_t{-25980}) == true);
  // negative 0
  fishbait::Fraction f7{-154171, 376395};
  REQUIRE((f7 > int16_t{0}) == false);
  // 0 positive
  fishbait::Fraction f8{0, 1};
  REQUIRE((f8 > int16_t{8050}) == false);
  // 0 negative
  fishbait::Fraction f9{0, 1};
  REQUIRE((f9 > int16_t{-10126}) == true);
  // 0 0
  fishbait::Fraction f10{0, 1};
  REQUIRE((f10 > int16_t{0}) == false);
}  // TEST_CASE "fraction greater than int16_t"

TEST_CASE("uint16_t fraction greater than", "[utils][fraction]") {
  // positive positive equal
  fishbait::Fraction f0{3310, 1};
  REQUIRE((uint16_t{3310} > f0) == false);
  // positive positive not equal
  fishbait::Fraction f1{6213096, 9926603};
  REQUIRE((uint16_t{12200} > f1) == true);
  // positive negative
  fishbait::Fraction f2{-5878376, 4271019};
  REQUIRE((uint16_t{2226} > f2) == true);
  // positive 0
  fishbait::Fraction f3{0, 1};
  REQUIRE((uint16_t{18437} > f3) == true);
  // 0 positive
  fishbait::Fraction f4{1901182, 208957};
  REQUIRE((uint16_t{0} > f4) == false);
  // 0 negative
  fishbait::Fraction f5{-4019812, 2366487};
  REQUIRE((uint16_t{0} > f5) == true);
  // 0 0
  fishbait::Fraction f6{0, 1};
  REQUIRE((uint16_t{0} > f6) == false);
}  // TEST_CASE "uint16_t fraction greater than"

TEST_CASE("fraction greater than uint16_t", "[utils][fraction]") {
  // positive positive equal
  fishbait::Fraction f0{39959, 1};
  REQUIRE((f0 > uint16_t{39959}) == false);
  // positive positive not equal
  fishbait::Fraction f1{5186598, 3555803};
  REQUIRE((f1 > uint16_t{41003}) == false);
  // positive 0
  fishbait::Fraction f2{2470742, 766491};
  REQUIRE((f2 > uint16_t{0}) == true);
  // negative positive
  fishbait::Fraction f3{-9532895, 8021366};
  REQUIRE((f3 > uint16_t{11241}) == false);
  // negative 0
  fishbait::Fraction f4{-2054347, 2029331};
  REQUIRE((f4 > uint16_t{0}) == false);
  // 0 positive
  fishbait::Fraction f5{0, 1};
  REQUIRE((f5 > uint16_t{26933}) == false);
  // 0 0
  fishbait::Fraction f6{0, 1};
  REQUIRE((f6 > uint16_t{0}) == false);
}  // TEST_CASE "fraction greater than uint16_t"

TEST_CASE("int32_t fraction greater than", "[utils][fraction]") {
  // positive positive equal
  fishbait::Fraction f0{9928155, 1};
  REQUIRE((int32_t{9928155} > f0) == false);
  // positive positive not equal
  fishbait::Fraction f1{3073551, 4881791};
  REQUIRE((int32_t{1032189} > f1) == true);
  // positive negative
  fishbait::Fraction f2{-363134, 9626291};
  REQUIRE((int32_t{2517877} > f2) == true);
  // positive 0
  fishbait::Fraction f3{0, 1};
  REQUIRE((int32_t{3532370} > f3) == true);
  // negative positive
  fishbait::Fraction f4{69476, 9287215};
  REQUIRE((int32_t{-1573993} > f4) == false);
  // negative negative equal
  fishbait::Fraction f5{-6758461, 1};
  REQUIRE((int32_t{-6758461} > f5) == false);
  // negative negative not equal
  fishbait::Fraction f6{-3726706, 4239871};
  REQUIRE((int32_t{-9063050} > f6) == false);
  // negative 0
  fishbait::Fraction f7{0, 1};
  REQUIRE((int32_t{-7946823} > f7) == false);
  // 0 positive
  fishbait::Fraction f8{5840482, 1165761};
  REQUIRE((int32_t{0} > f8) == false);
  // 0 negative
  fishbait::Fraction f9{-2608559, 2908729};
  REQUIRE((int32_t{0} > f9) == true);
  // 0 0
  fishbait::Fraction f10{0, 1};
  REQUIRE((int32_t{0} > f10) == false);
}  // TEST_CASE "int32_t fraction greater than"

TEST_CASE("fraction greater than int32_t", "[utils][fraction]") {
  // positive positive equal
  fishbait::Fraction f0{6383979, 1};
  REQUIRE((f0 > int32_t{6383979}) == false);
  // positive positive not equal
  fishbait::Fraction f1{5952236, 3194699};
  REQUIRE((f1 > int32_t{7200194}) == false);
  // positive negative
  fishbait::Fraction f2{4430229, 6557258};
  REQUIRE((f2 > int32_t{-2820456}) == true);
  // positive 0
  fishbait::Fraction f3{965489, 4136248};
  REQUIRE((f3 > int32_t{0}) == true);
  // negative positive
  fishbait::Fraction f4{-5261345, 4758188};
  REQUIRE((f4 > int32_t{4368700}) == false);
  // negative negative equal
  fishbait::Fraction f5{-310317, 1};
  REQUIRE((f5 > int32_t{-310317}) == false);
  // negative negative not equal
  fishbait::Fraction f6{-1056143, 1809342};
  REQUIRE((f6 > int32_t{-2386830}) == true);
  // negative 0
  fishbait::Fraction f7{-1894077, 5087215};
  REQUIRE((f7 > int32_t{0}) == false);
  // 0 positive
  fishbait::Fraction f8{0, 1};
  REQUIRE((f8 > int32_t{5372677}) == false);
  // 0 negative
  fishbait::Fraction f9{0, 1};
  REQUIRE((f9 > int32_t{-8830612}) == true);
  // 0 0
  fishbait::Fraction f10{0, 1};
  REQUIRE((f10 > int32_t{0}) == false);
}  // TEST_CASE "fraction greater than int32_t"

TEST_CASE("uint32_t fraction greater than", "[utils][fraction]") {
  // positive positive equal
  fishbait::Fraction f0{7418184, 1};
  REQUIRE((uint32_t{7418184} > f0) == false);
  // positive positive not equal
  fishbait::Fraction f1{901623, 9191554};
  REQUIRE((uint32_t{3664785} > f1) == true);
  // positive negative
  fishbait::Fraction f2{-8580020, 668783};
  REQUIRE((uint32_t{3928957} > f2) == true);
  // positive 0
  fishbait::Fraction f3{0, 1};
  REQUIRE((uint32_t{5891969} > f3) == true);
  // 0 positive
  fishbait::Fraction f4{9780072, 6937123};
  REQUIRE((uint32_t{0} > f4) == false);
  // 0 negative
  fishbait::Fraction f5{-1510472, 3440209};
  REQUIRE((uint32_t{0} > f5) == true);
  // 0 0
  fishbait::Fraction f6{0, 1};
  REQUIRE((uint32_t{0} > f6) == false);
}  // TEST_CASE "uint32_t fraction greater than"

TEST_CASE("fraction greater than uint32_t", "[utils][fraction]") {
  // positive positive equal
  fishbait::Fraction f0{1615579, 1};
  REQUIRE((f0 > uint32_t{1615579}) == false);
  // positive positive not equal
  fishbait::Fraction f1{2383244, 710225};
  REQUIRE((f1 > uint32_t{3702604}) == false);
  // positive 0
  fishbait::Fraction f2{2624223, 4043035};
  REQUIRE((f2 > uint32_t{0}) == true);
  // negative positive
  fishbait::Fraction f3{-2448917, 9791315};
  REQUIRE((f3 > uint32_t{1204216}) == false);
  // negative 0
  fishbait::Fraction f4{-9384600, 3720331};
  REQUIRE((f4 > uint32_t{0}) == false);
  // 0 positive
  fishbait::Fraction f5{0, 1};
  REQUIRE((f5 > uint32_t{4384566}) == false);
  // 0 0
  fishbait::Fraction f6{0, 1};
  REQUIRE((f6 > uint32_t{0}) == false);
}  // TEST_CASE "fraction greater than uint32_t"

TEST_CASE("int64_t fraction greater than", "[utils][fraction]") {
  // positive positive equal
  fishbait::Fraction f0{3399259, 1};
  REQUIRE((int64_t{3399259} > f0) == false);
  // positive positive not equal
  fishbait::Fraction f1{7952585, 3546579};
  REQUIRE((int64_t{9296678} > f1) == true);
  // positive negative
  fishbait::Fraction f2{-494068, 1451147};
  REQUIRE((int64_t{1000757} > f2) == true);
  // positive 0
  fishbait::Fraction f3{0, 1};
  REQUIRE((int64_t{451739} > f3) == true);
  // negative positive
  fishbait::Fraction f4{7471545, 7359238};
  REQUIRE((int64_t{-7197662} > f4) == false);
  // negative negative equal
  fishbait::Fraction f5{-937840, 1};
  REQUIRE((int64_t{-937840} > f5) == false);
  // negative negative not equal
  fishbait::Fraction f6{-8130049, 9257668};
  REQUIRE((int64_t{-2947418} > f6) == false);
  // negative 0
  fishbait::Fraction f7{0, 1};
  REQUIRE((int64_t{-8646519} > f7) == false);
  // 0 positive
  fishbait::Fraction f8{9080373, 7719787};
  REQUIRE((int64_t{0} > f8) == false);
  // 0 negative
  fishbait::Fraction f9{-2800135, 2526584};
  REQUIRE((int64_t{0} > f9) == true);
  // 0 0
  fishbait::Fraction f10{0, 1};
  REQUIRE((int64_t{0} > f10) == false);
}  // TEST_CASE "int64_t fraction greater than"

TEST_CASE("fraction greater than int64_t", "[utils][fraction]") {
  // positive positive equal
  fishbait::Fraction f0{2137138, 1};
  REQUIRE((f0 > int64_t{2137138}) == false);
  // positive positive not equal
  fishbait::Fraction f1{244987, 3196484};
  REQUIRE((f1 > int64_t{1541838}) == false);
  // positive negative
  fishbait::Fraction f2{7196639, 2312177};
  REQUIRE((f2 > int64_t{-5503663}) == true);
  // positive 0
  fishbait::Fraction f3{3592941, 7624658};
  REQUIRE((f3 > int64_t{0}) == true);
  // negative positive
  fishbait::Fraction f4{-2150725, 687062};
  REQUIRE((f4 > int64_t{8680752}) == false);
  // negative negative equal
  fishbait::Fraction f5{-4560139, 1};
  REQUIRE((f5 > int64_t{-4560139}) == false);
  // negative negative not equal
  fishbait::Fraction f6{-727126, 6039271};
  REQUIRE((f6 > int64_t{-926072}) == true);
  // negative 0
  fishbait::Fraction f7{-8117923, 654924};
  REQUIRE((f7 > int64_t{0}) == false);
  // 0 positive
  fishbait::Fraction f8{0, 1};
  REQUIRE((f8 > int64_t{3397017}) == false);
  // 0 negative
  fishbait::Fraction f9{0, 1};
  REQUIRE((f9 > int64_t{-9925695}) == true);
  // 0 0
  fishbait::Fraction f10{0, 1};
  REQUIRE((f10 > int64_t{0}) == false);
}  // TEST_CASE "fraction greater than int64_t"

TEST_CASE("double fraction greater than", "[utils][fraction]") {
  // positive positive equal
  fishbait::Fraction f0{240909929199551, 134217728};
  REQUIRE((1794918.8441004679 > f0) == false);
  // positive positive not equal
  fishbait::Fraction f1{3325918, 3407157};
  REQUIRE((4229827.585698447 > f1) == true);
  // positive negative
  fishbait::Fraction f2{-7014801, 1022575};
  REQUIRE((5215326.5987070855 > f2) == true);
  // positive 0
  fishbait::Fraction f3{0, 1};
  REQUIRE((4705655.145330848 > f3) == true);
  // negative positive
  fishbait::Fraction f4{550502, 1823317};
  REQUIRE((-5755912.976165803 > f4) == false);
  // negative negative equal
  fishbait::Fraction f5{-6262903176449291, 2147483648};
  REQUIRE((-2916391.555429106 > f5) == false);
  // negative negative not equal
  fishbait::Fraction f6{-4415803, 1667649};
  REQUIRE((-248338.3012445136 > f6) == false);
  // negative 0
  fishbait::Fraction f7{0, 1};
  REQUIRE((-3633962.9005279047 > f7) == false);
  // 0 positive
  fishbait::Fraction f8{2205923, 2794956};
  REQUIRE((0.0 > f8) == false);
  // 0 negative
  fishbait::Fraction f9{-9964193, 2017823};
  REQUIRE((0.0 > f9) == true);
  // 0 0
  fishbait::Fraction f10{0, 1};
  REQUIRE((0.0 > f10) == false);
}  // TEST_CASE "double fraction greater than"

TEST_CASE("fraction double greater than", "[utils][fraction]") {
  // positive positive equal
  fishbait::Fraction f0{4639766230518337, 8589934592};
  REQUIRE((f0 > 540139.8789275364) == false);
  // positive positive not equal
  fishbait::Fraction f1{8543101, 5507658};
  REQUIRE((f1 > 4787784.486846527) == false);
  // positive negative
  fishbait::Fraction f2{686244, 2080895};
  REQUIRE((f2 > -3304423.5235274276) == true);
  // positive 0
  fishbait::Fraction f3{2842673, 8969520};
  REQUIRE((f3 > 0.0) == true);
  // negative positive
  fishbait::Fraction f4{-570798, 1471933};
  REQUIRE((f4 > 900097.0324797522) == false);
  // negative negative equal
  fishbait::Fraction f5{-1487394666958409, 268435456};
  REQUIRE((f5 > -5540976.922804151) == false);
  // negative negative not equal
  fishbait::Fraction f6{-224556, 572981};
  REQUIRE((f6 > -2900181.5245582405) == true);
  // negative 0
  fishbait::Fraction f7{-3291593, 7725586};
  REQUIRE((f7 > 0.0) == false);
  // 0 positive
  fishbait::Fraction f8{0, 1};
  REQUIRE((f8 > 8713046.983147781) == false);
  // 0 negative
  fishbait::Fraction f9{0, 1};
  REQUIRE((f9 > -5909340.179402849) == true);
  // 0 0
  fishbait::Fraction f10{0, 1};
  REQUIRE((f10 > 0.0) == false);
}  // TEST_CASE "fraction double greater than"

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
/* -------------------------- Less Than Equal To ---------------------------- */
/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */

TEST_CASE("fraction fraction less than equal to", "[utils][fraction]") {
  // positive positive equal
  fishbait::Fraction f0{6631960, 6091141};
  fishbait::Fraction f1{6631960, 6091141};
  REQUIRE((f0 <= f1) == true);
  // positive positive not equal
  fishbait::Fraction f2{65709, 9524611};
  fishbait::Fraction f3{43599, 49496};
  REQUIRE((f2 <= f3) == true);
  // positive negative
  fishbait::Fraction f4{2336801, 3496762};
  fishbait::Fraction f5{-8019797, 9823918};
  REQUIRE((f4 <= f5) == false);
  // positive 0
  fishbait::Fraction f6{1819918, 3276331};
  fishbait::Fraction f7{0, 1};
  REQUIRE((f6 <= f7) == false);
  // negative positive
  fishbait::Fraction f8{-210539, 2865453};
  fishbait::Fraction f9{3013102, 7868451};
  REQUIRE((f8 <= f9) == true);
  // negative negative equal
  fishbait::Fraction f10{-4752698, 3121611};
  fishbait::Fraction f11{-4752698, 3121611};
  REQUIRE((f10 <= f11) == true);
  // negative negative not equal
  fishbait::Fraction f12{-5998921, 8716915};
  fishbait::Fraction f13{-7280574, 8332439};
  REQUIRE((f12 <= f13) == false);
  // negative 0
  fishbait::Fraction f14{-892322, 1486111};
  fishbait::Fraction f15{0, 1};
  REQUIRE((f14 <= f15) == true);
  // 0 positive
  fishbait::Fraction f16{0, 1};
  fishbait::Fraction f17{5823107, 4079786};
  REQUIRE((f16 <= f17) == true);
  // 0 negative
  fishbait::Fraction f18{0, 1};
  fishbait::Fraction f19{-9829381, 4082159};
  REQUIRE((f18 <= f19) == false);
  // 0 0
  fishbait::Fraction f20{0, 1};
  fishbait::Fraction f21{0, 1};
  REQUIRE((f20 <= f21) == true);
}  // TEST_CASE "fraction fraction less than equal to"

TEST_CASE("int8_t fraction less than equal to", "[utils][fraction]") {
  // positive positive equal
  fishbait::Fraction f0{43, 1};
  REQUIRE((int8_t{43} <= f0) == true);
  // positive positive not equal
  fishbait::Fraction f1{3746045, 2514741};
  REQUIRE((int8_t{56} <= f1) == false);
  // positive negative
  fishbait::Fraction f2{-8488441, 1032060};
  REQUIRE((int8_t{20} <= f2) == false);
  // positive 0
  fishbait::Fraction f3{0, 1};
  REQUIRE((int8_t{62} <= f3) == false);
  // negative positive
  fishbait::Fraction f4{293414, 255897};
  REQUIRE((int8_t{-27} <= f4) == true);
  // negative negative equal
  fishbait::Fraction f5{-74, 1};
  REQUIRE((int8_t{-74} <= f5) == true);
  // negative negative not equal
  fishbait::Fraction f6{-9807161, 12461};
  REQUIRE((int8_t{-90} <= f6) == false);
  // negative 0
  fishbait::Fraction f7{0, 1};
  REQUIRE((int8_t{-113} <= f7) == true);
  // 0 positive
  fishbait::Fraction f8{252636, 54949};
  REQUIRE((int8_t{0} <= f8) == true);
  // 0 negative
  fishbait::Fraction f9{-234221, 4874713};
  REQUIRE((int8_t{0} <= f9) == false);
  // 0 0
  fishbait::Fraction f10{0, 1};
  REQUIRE((int8_t{0} <= f10) == true);
}  // TEST_CASE "int8_t fraction less than equal to"

TEST_CASE("fraction less than equal to int8_t", "[utils][fraction]") {
  // positive positive equal
  fishbait::Fraction f0{116, 1};
  REQUIRE((f0 <= int8_t{116}) == true);
  // positive positive not equal
  fishbait::Fraction f1{701852, 3204291};
  REQUIRE((f1 <= int8_t{10}) == true);
  // positive negative
  fishbait::Fraction f2{6228099, 3180925};
  REQUIRE((f2 <= int8_t{-105}) == false);
  // positive 0
  fishbait::Fraction f3{1030043, 255170};
  REQUIRE((f3 <= int8_t{0}) == false);
  // negative positive
  fishbait::Fraction f4{-3758861, 7494501};
  REQUIRE((f4 <= int8_t{41}) == true);
  // negative negative equal
  fishbait::Fraction f5{-48, 1};
  REQUIRE((f5 <= int8_t{-48}) == true);
  // negative negative not equal
  fishbait::Fraction f6{-6528171, 7365641};
  REQUIRE((f6 <= int8_t{-115}) == false);
  // negative 0
  fishbait::Fraction f7{-2341547, 1105071};
  REQUIRE((f7 <= int8_t{0}) == true);
  // 0 positive
  fishbait::Fraction f8{0, 1};
  REQUIRE((f8 <= int8_t{41}) == true);
  // 0 negative
  fishbait::Fraction f9{0, 1};
  REQUIRE((f9 <= int8_t{-61}) == false);
  // 0 0
  fishbait::Fraction f10{0, 1};
  REQUIRE((f10 <= int8_t{0}) == true);
}  // TEST_CASE "fraction less than equal to int8_t"

TEST_CASE("uint8_t fraction less than equal to", "[utils][fraction]") {
  // positive positive equal
  fishbait::Fraction f0{1, 1};
  REQUIRE((uint8_t{1} <= f0) == true);
  // positive positive not equal
  fishbait::Fraction f1{3300089, 72413};
  REQUIRE((uint8_t{189} <= f1) == false);
  // positive negative
  fishbait::Fraction f2{-1305462, 6338333};
  REQUIRE((uint8_t{233} <= f2) == false);
  // positive 0
  fishbait::Fraction f3{0, 1};
  REQUIRE((uint8_t{20} <= f3) == false);
  // 0 positive
  fishbait::Fraction f4{1836454, 9289927};
  REQUIRE((uint8_t{0} <= f4) == true);
  // 0 negative
  fishbait::Fraction f5{-7712865, 1328312};
  REQUIRE((uint8_t{0} <= f5) == false);
  // 0 0
  fishbait::Fraction f6{0, 1};
  REQUIRE((uint8_t{0} <= f6) == true);
}  // TEST_CASE "uint8_t fraction less than equal to"

TEST_CASE("fraction less than equal to uint8_t", "[utils][fraction]") {
  // positive positive equal
  fishbait::Fraction f0{191, 1};
  REQUIRE((f0 <= uint8_t{191}) == true);
  // positive positive not equal
  fishbait::Fraction f1{5977774, 3498795};
  REQUIRE((f1 <= uint8_t{197}) == true);
  // positive 0
  fishbait::Fraction f2{8215127, 4459119};
  REQUIRE((f2 <= uint8_t{0}) == false);
  // negative positive
  fishbait::Fraction f3{-1284319, 5008016};
  REQUIRE((f3 <= uint8_t{87}) == true);
  // negative 0
  fishbait::Fraction f4{-1122035, 250958};
  REQUIRE((f4 <= uint8_t{0}) == true);
  // 0 positive
  fishbait::Fraction f5{0, 1};
  REQUIRE((f5 <= uint8_t{127}) == true);
  // 0 0
  fishbait::Fraction f6{0, 1};
  REQUIRE((f6 <= uint8_t{0}) == true);
}  // TEST_CASE "fraction less than equal to uint8_t"

TEST_CASE("int16_t fraction less than equal to", "[utils][fraction]") {
  // positive positive equal
  fishbait::Fraction f0{32134, 1};
  REQUIRE((int16_t{32134} <= f0) == true);
  // positive positive not equal
  fishbait::Fraction f1{701257, 1256417};
  REQUIRE((int16_t{16460} <= f1) == false);
  // positive negative
  fishbait::Fraction f2{-3688040, 1580231};
  REQUIRE((int16_t{29139} <= f2) == false);
  // positive 0
  fishbait::Fraction f3{0, 1};
  REQUIRE((int16_t{18478} <= f3) == false);
  // negative positive
  fishbait::Fraction f4{1000613, 148339};
  REQUIRE((int16_t{-15655} <= f4) == true);
  // negative negative equal
  fishbait::Fraction f5{-18673, 1};
  REQUIRE((int16_t{-18673} <= f5) == true);
  // negative negative not equal
  fishbait::Fraction f6{-451331, 7082284};
  REQUIRE((int16_t{-13513} <= f6) == true);
  // negative 0
  fishbait::Fraction f7{0, 1};
  REQUIRE((int16_t{-26060} <= f7) == true);
  // 0 positive
  fishbait::Fraction f8{3364967, 9940395};
  REQUIRE((int16_t{0} <= f8) == true);
  // 0 negative
  fishbait::Fraction f9{-220861, 363390};
  REQUIRE((int16_t{0} <= f9) == false);
  // 0 0
  fishbait::Fraction f10{0, 1};
  REQUIRE((int16_t{0} <= f10) == true);
}  // TEST_CASE "int16_t fraction less than equal to"

TEST_CASE("fraction less than equal to int16_t", "[utils][fraction]") {
  // positive positive equal
  fishbait::Fraction f0{15702, 1};
  REQUIRE((f0 <= int16_t{15702}) == true);
  // positive positive not equal
  fishbait::Fraction f1{5163833, 2334449};
  REQUIRE((f1 <= int16_t{10082}) == true);
  // positive negative
  fishbait::Fraction f2{9886957, 4777613};
  REQUIRE((f2 <= int16_t{-3932}) == false);
  // positive 0
  fishbait::Fraction f3{383417, 7276};
  REQUIRE((f3 <= int16_t{0}) == false);
  // negative positive
  fishbait::Fraction f4{-1869541, 402384};
  REQUIRE((f4 <= int16_t{18024}) == true);
  // negative negative equal
  fishbait::Fraction f5{-23788, 1};
  REQUIRE((f5 <= int16_t{-23788}) == true);
  // negative negative not equal
  fishbait::Fraction f6{-4664747, 799918};
  REQUIRE((f6 <= int16_t{-6292}) == false);
  // negative 0
  fishbait::Fraction f7{-310006, 4029011};
  REQUIRE((f7 <= int16_t{0}) == true);
  // 0 positive
  fishbait::Fraction f8{0, 1};
  REQUIRE((f8 <= int16_t{1685}) == true);
  // 0 negative
  fishbait::Fraction f9{0, 1};
  REQUIRE((f9 <= int16_t{-19853}) == false);
  // 0 0
  fishbait::Fraction f10{0, 1};
  REQUIRE((f10 <= int16_t{0}) == true);
}  // TEST_CASE "fraction less than equal to int16_t"

TEST_CASE("uint16_t fraction less than equal to", "[utils][fraction]") {
  // positive positive equal
  fishbait::Fraction f0{49616, 1};
  REQUIRE((uint16_t{49616} <= f0) == true);
  // positive positive not equal
  fishbait::Fraction f1{1911577, 3751989};
  REQUIRE((uint16_t{48898} <= f1) == false);
  // positive negative
  fishbait::Fraction f2{-9022659, 7495741};
  REQUIRE((uint16_t{39122} <= f2) == false);
  // positive 0
  fishbait::Fraction f3{0, 1};
  REQUIRE((uint16_t{51058} <= f3) == false);
  // 0 positive
  fishbait::Fraction f4{2058959, 646855};
  REQUIRE((uint16_t{0} <= f4) == true);
  // 0 negative
  fishbait::Fraction f5{-1041749, 3842350};
  REQUIRE((uint16_t{0} <= f5) == false);
  // 0 0
  fishbait::Fraction f6{0, 1};
  REQUIRE((uint16_t{0} <= f6) == true);
}  // TEST_CASE "uint16_t fraction less than equal to"

TEST_CASE("fraction less than equal to uint16_t", "[utils][fraction]") {
  // positive positive equal
  fishbait::Fraction f0{32218, 1};
  REQUIRE((f0 <= uint16_t{32218}) == true);
  // positive positive not equal
  fishbait::Fraction f1{790137, 996652};
  REQUIRE((f1 <= uint16_t{63922}) == true);
  // positive 0
  fishbait::Fraction f2{123869, 79316};
  REQUIRE((f2 <= uint16_t{0}) == false);
  // negative positive
  fishbait::Fraction f3{-93197, 360310};
  REQUIRE((f3 <= uint16_t{16802}) == true);
  // negative 0
  fishbait::Fraction f4{-4101267, 3221545};
  REQUIRE((f4 <= uint16_t{0}) == true);
  // 0 positive
  fishbait::Fraction f5{0, 1};
  REQUIRE((f5 <= uint16_t{13112}) == true);
  // 0 0
  fishbait::Fraction f6{0, 1};
  REQUIRE((f6 <= uint16_t{0}) == true);
}  // TEST_CASE "fraction less than equal to uint16_t"

TEST_CASE("int32_t fraction less than equal to", "[utils][fraction]") {
  // positive positive equal
  fishbait::Fraction f0{657540, 1};
  REQUIRE((int32_t{657540} <= f0) == true);
  // positive positive not equal
  fishbait::Fraction f1{6831335, 3137562};
  REQUIRE((int32_t{2691365} <= f1) == false);
  // positive negative
  fishbait::Fraction f2{-908297, 1094836};
  REQUIRE((int32_t{3134328} <= f2) == false);
  // positive 0
  fishbait::Fraction f3{0, 1};
  REQUIRE((int32_t{9113019} <= f3) == false);
  // negative positive
  fishbait::Fraction f4{2723318, 5157619};
  REQUIRE((int32_t{-7241392} <= f4) == true);
  // negative negative equal
  fishbait::Fraction f5{-4460436, 1};
  REQUIRE((int32_t{-4460436} <= f5) == true);
  // negative negative not equal
  fishbait::Fraction f6{-4785142, 6046209};
  REQUIRE((int32_t{-52712} <= f6) == true);
  // negative 0
  fishbait::Fraction f7{0, 1};
  REQUIRE((int32_t{-6546250} <= f7) == true);
  // 0 positive
  fishbait::Fraction f8{8528517, 2604674};
  REQUIRE((int32_t{0} <= f8) == true);
  // 0 negative
  fishbait::Fraction f9{-2157990, 479711};
  REQUIRE((int32_t{0} <= f9) == false);
  // 0 0
  fishbait::Fraction f10{0, 1};
  REQUIRE((int32_t{0} <= f10) == true);
}  // TEST_CASE "int32_t fraction less than equal to"

TEST_CASE("fraction less than equal to int32_t", "[utils][fraction]") {
  // positive positive equal
  fishbait::Fraction f0{58114, 1};
  REQUIRE((f0 <= int32_t{58114}) == true);
  // positive positive not equal
  fishbait::Fraction f1{4683217, 1828621};
  REQUIRE((f1 <= int32_t{9369050}) == true);
  // positive negative
  fishbait::Fraction f2{8956553, 2027689};
  REQUIRE((f2 <= int32_t{-6048959}) == false);
  // positive 0
  fishbait::Fraction f3{1126321, 7822768};
  REQUIRE((f3 <= int32_t{0}) == false);
  // negative positive
  fishbait::Fraction f4{-2674133, 3136979};
  REQUIRE((f4 <= int32_t{1044544}) == true);
  // negative negative equal
  fishbait::Fraction f5{-2476009, 1};
  REQUIRE((f5 <= int32_t{-2476009}) == true);
  // negative negative not equal
  fishbait::Fraction f6{-4105775, 4567061};
  REQUIRE((f6 <= int32_t{-6512594}) == false);
  // negative 0
  fishbait::Fraction f7{-3844993, 4844885};
  REQUIRE((f7 <= int32_t{0}) == true);
  // 0 positive
  fishbait::Fraction f8{0, 1};
  REQUIRE((f8 <= int32_t{2238398}) == true);
  // 0 negative
  fishbait::Fraction f9{0, 1};
  REQUIRE((f9 <= int32_t{-773948}) == false);
  // 0 0
  fishbait::Fraction f10{0, 1};
  REQUIRE((f10 <= int32_t{0}) == true);
}  // TEST_CASE "fraction less than equal to int32_t"

TEST_CASE("uint32_t fraction less than equal to", "[utils][fraction]") {
  // positive positive equal
  fishbait::Fraction f0{6673212, 1};
  REQUIRE((uint32_t{6673212} <= f0) == true);
  // positive positive not equal
  fishbait::Fraction f1{4919755, 2433223};
  REQUIRE((uint32_t{2198123} <= f1) == false);
  // positive negative
  fishbait::Fraction f2{-760403, 2218642};
  REQUIRE((uint32_t{1148130} <= f2) == false);
  // positive 0
  fishbait::Fraction f3{0, 1};
  REQUIRE((uint32_t{6748571} <= f3) == false);
  // 0 positive
  fishbait::Fraction f4{5267521, 4025828};
  REQUIRE((uint32_t{0} <= f4) == true);
  // 0 negative
  fishbait::Fraction f5{-8728712, 9143315};
  REQUIRE((uint32_t{0} <= f5) == false);
  // 0 0
  fishbait::Fraction f6{0, 1};
  REQUIRE((uint32_t{0} <= f6) == true);
}  // TEST_CASE "uint32_t fraction less than equal to"

TEST_CASE("fraction less than equal to uint32_t", "[utils][fraction]") {
  // positive positive equal
  fishbait::Fraction f0{1873376, 1};
  REQUIRE((f0 <= uint32_t{1873376}) == true);
  // positive positive not equal
  fishbait::Fraction f1{965213, 9158465};
  REQUIRE((f1 <= uint32_t{2445400}) == true);
  // positive 0
  fishbait::Fraction f2{2662403, 848015};
  REQUIRE((f2 <= uint32_t{0}) == false);
  // negative positive
  fishbait::Fraction f3{-1452901, 7692229};
  REQUIRE((f3 <= uint32_t{7182084}) == true);
  // negative 0
  fishbait::Fraction f4{-1324798, 1459379};
  REQUIRE((f4 <= uint32_t{0}) == true);
  // 0 positive
  fishbait::Fraction f5{0, 1};
  REQUIRE((f5 <= uint32_t{9590680}) == true);
  // 0 0
  fishbait::Fraction f6{0, 1};
  REQUIRE((f6 <= uint32_t{0}) == true);
}  // TEST_CASE "fraction less than equal to uint32_t"

TEST_CASE("int64_t fraction less than equal to", "[utils][fraction]") {
  // positive positive equal
  fishbait::Fraction f0{1251116, 1};
  REQUIRE((int64_t{1251116} <= f0) == true);
  // positive positive not equal
  fishbait::Fraction f1{5383733, 6939045};
  REQUIRE((int64_t{9447009} <= f1) == false);
  // positive negative
  fishbait::Fraction f2{-8089643, 361287};
  REQUIRE((int64_t{6613016} <= f2) == false);
  // positive 0
  fishbait::Fraction f3{0, 1};
  REQUIRE((int64_t{8899070} <= f3) == false);
  // negative positive
  fishbait::Fraction f4{4544540, 3190287};
  REQUIRE((int64_t{-5106454} <= f4) == true);
  // negative negative equal
  fishbait::Fraction f5{-8516547, 1};
  REQUIRE((int64_t{-8516547} <= f5) == true);
  // negative negative not equal
  fishbait::Fraction f6{-8679651, 8434156};
  REQUIRE((int64_t{-9156321} <= f6) == true);
  // negative 0
  fishbait::Fraction f7{0, 1};
  REQUIRE((int64_t{-6138203} <= f7) == true);
  // 0 positive
  fishbait::Fraction f8{5235396, 3880445};
  REQUIRE((int64_t{0} <= f8) == true);
  // 0 negative
  fishbait::Fraction f9{-1987401, 1233016};
  REQUIRE((int64_t{0} <= f9) == false);
  // 0 0
  fishbait::Fraction f10{0, 1};
  REQUIRE((int64_t{0} <= f10) == true);
}  // TEST_CASE "int64_t fraction less than equal to"

TEST_CASE("fraction less than equal to int64_t", "[utils][fraction]") {
  // positive positive equal
  fishbait::Fraction f0{7979223, 1};
  REQUIRE((f0 <= int64_t{7979223}) == true);
  // positive positive not equal
  fishbait::Fraction f1{745803, 8633233};
  REQUIRE((f1 <= int64_t{8348339}) == true);
  // positive negative
  fishbait::Fraction f2{8465758, 5239777};
  REQUIRE((f2 <= int64_t{-2876179}) == false);
  // positive 0
  fishbait::Fraction f3{830607, 384718};
  REQUIRE((f3 <= int64_t{0}) == false);
  // negative positive
  fishbait::Fraction f4{-1210283, 1051837};
  REQUIRE((f4 <= int64_t{7511176}) == true);
  // negative negative equal
  fishbait::Fraction f5{-45239, 1};
  REQUIRE((f5 <= int64_t{-45239}) == true);
  // negative negative not equal
  fishbait::Fraction f6{-8236309, 9929903};
  REQUIRE((f6 <= int64_t{-8097962}) == false);
  // negative 0
  fishbait::Fraction f7{-5136128, 5517161};
  REQUIRE((f7 <= int64_t{0}) == true);
  // 0 positive
  fishbait::Fraction f8{0, 1};
  REQUIRE((f8 <= int64_t{5425171}) == true);
  // 0 negative
  fishbait::Fraction f9{0, 1};
  REQUIRE((f9 <= int64_t{-6551270}) == false);
  // 0 0
  fishbait::Fraction f10{0, 1};
  REQUIRE((f10 <= int64_t{0}) == true);
}  // TEST_CASE "fraction less than equal to int64_t"

TEST_CASE("double fraction less than equal to", "[utils][fraction]") {
  // positive positive equal
  fishbait::Fraction f0{7358425574038373, 1073741824};
  REQUIRE((6853067.850729798 <= f0) == true);
  // positive positive not equal
  fishbait::Fraction f1{7198154, 9215031};
  REQUIRE((2559902.1994168344 <= f1) == false);
  // positive negative
  fishbait::Fraction f2{-3839885, 6077066};
  REQUIRE((944508.2688977061 <= f2) == false);
  // positive 0
  fishbait::Fraction f3{0, 1};
  REQUIRE((6878227.530138672 <= f3) == false);
  // negative positive
  fishbait::Fraction f4{271120, 66527};
  REQUIRE((-3346593.035167956 <= f4) == true);
  // negative negative equal
  fishbait::Fraction f5{-5127499893230001, 536870912};
  REQUIRE((-9550712.803807111 <= f5) == true);
  // negative negative not equal
  fishbait::Fraction f6{-4193743, 6856805};
  REQUIRE((-5596053.042530582 <= f6) == true);
  // negative 0
  fishbait::Fraction f7{0, 1};
  REQUIRE((-6659755.483018812 <= f7) == true);
  // 0 positive
  fishbait::Fraction f8{7558449, 7319999};
  REQUIRE((0.0 <= f8) == true);
  // 0 negative
  fishbait::Fraction f9{-3698465, 7607424};
  REQUIRE((0.0 <= f9) == false);
  // 0 0
  fishbait::Fraction f10{0, 1};
  REQUIRE((0.0 <= f10) == true);
}  // TEST_CASE "double fraction less than equal to"

TEST_CASE("fraction double less than equal to", "[utils][fraction]") {
  // positive positive equal
  fishbait::Fraction f0{4566614974472407, 536870912};
  REQUIRE((f0 <= 8505983.23060648) == true);
  // positive positive not equal
  fishbait::Fraction f1{1432074, 1158713};
  REQUIRE((f1 <= 5594920.075434088) == true);
  // positive negative
  fishbait::Fraction f2{368141, 71240};
  REQUIRE((f2 <= -4791157.179966361) == false);
  // positive 0
  fishbait::Fraction f3{7104, 400777};
  REQUIRE((f3 <= 0.0) == false);
  // negative positive
  fishbait::Fraction f4{-6389607, 8641543};
  REQUIRE((f4 <= 6351056.052238692) == true);
  // negative negative equal
  fishbait::Fraction f5{-8408403464740445, 1073741824};
  REQUIRE((f5 <= -7830935.963187781) == true);
  // negative negative not equal
  fishbait::Fraction f6{-2949875, 6986311};
  REQUIRE((f6 <= -6595182.953199975) == false);
  // negative 0
  fishbait::Fraction f7{-9919808, 244071};
  REQUIRE((f7 <= 0.0) == true);
  // 0 positive
  fishbait::Fraction f8{0, 1};
  REQUIRE((f8 <= 7800407.631176228) == true);
  // 0 negative
  fishbait::Fraction f9{0, 1};
  REQUIRE((f9 <= -4755544.328571478) == false);
  // 0 0
  fishbait::Fraction f10{0, 1};
  REQUIRE((f10 <= 0.0) == true);
}  // TEST_CASE "fraction double less than equal to"

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
/* ------------------------- Greater Than Equal To -------------------------- */
/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */

TEST_CASE("fraction fraction greater than equal to", "[utils][fraction]") {
  // positive positive equal
  fishbait::Fraction f0{9321494, 9105791};
  fishbait::Fraction f1{9321494, 9105791};
  REQUIRE((f0 >= f1) == true);
  // positive positive not equal
  fishbait::Fraction f2{5278897, 887625};
  fishbait::Fraction f3{4762152, 1749781};
  REQUIRE((f2 >= f3) == true);
  // positive negative
  fishbait::Fraction f4{1179763, 2377213};
  fishbait::Fraction f5{-299251, 562466};
  REQUIRE((f4 >= f5) == true);
  // positive 0
  fishbait::Fraction f6{348389, 6265691};
  fishbait::Fraction f7{0, 1};
  REQUIRE((f6 >= f7) == true);
  // negative positive
  fishbait::Fraction f8{-7106420, 859979};
  fishbait::Fraction f9{12537, 3191521};
  REQUIRE((f8 >= f9) == false);
  // negative negative equal
  fishbait::Fraction f10{-7226069, 2733045};
  fishbait::Fraction f11{-7226069, 2733045};
  REQUIRE((f10 >= f11) == true);
  // negative negative not equal
  fishbait::Fraction f12{-4016409, 2980699};
  fishbait::Fraction f13{-9636398, 5567961};
  REQUIRE((f12 >= f13) == true);
  // negative 0
  fishbait::Fraction f14{-2993582, 2543413};
  fishbait::Fraction f15{0, 1};
  REQUIRE((f14 >= f15) == false);
  // 0 positive
  fishbait::Fraction f16{0, 1};
  fishbait::Fraction f17{8382071, 5220099};
  REQUIRE((f16 >= f17) == false);
  // 0 negative
  fishbait::Fraction f18{0, 1};
  fishbait::Fraction f19{-4920697, 8102403};
  REQUIRE((f18 >= f19) == true);
  // 0 0
  fishbait::Fraction f20{0, 1};
  fishbait::Fraction f21{0, 1};
  REQUIRE((f20 >= f21) == true);
}  // TEST_CASE "fraction fraction greater than equal to"

TEST_CASE("int8_t fraction greater than equal to", "[utils][fraction]") {
  // positive positive equal
  fishbait::Fraction f0{4, 1};
  REQUIRE((int8_t{4} >= f0) == true);
  // positive positive not equal
  fishbait::Fraction f1{9867403, 9204480};
  REQUIRE((int8_t{14} >= f1) == true);
  // positive negative
  fishbait::Fraction f2{-1756131, 3359245};
  REQUIRE((int8_t{99} >= f2) == true);
  // positive 0
  fishbait::Fraction f3{0, 1};
  REQUIRE((int8_t{94} >= f3) == true);
  // negative positive
  fishbait::Fraction f4{5546245, 5011737};
  REQUIRE((int8_t{-74} >= f4) == false);
  // negative negative equal
  fishbait::Fraction f5{-63, 1};
  REQUIRE((int8_t{-63} >= f5) == true);
  // negative negative not equal
  fishbait::Fraction f6{-1008265, 7781802};
  REQUIRE((int8_t{-83} >= f6) == false);
  // negative 0
  fishbait::Fraction f7{0, 1};
  REQUIRE((int8_t{-6} >= f7) == false);
  // 0 positive
  fishbait::Fraction f8{4197143, 984318};
  REQUIRE((int8_t{0} >= f8) == false);
  // 0 negative
  fishbait::Fraction f9{-4427187, 1023283};
  REQUIRE((int8_t{0} >= f9) == true);
  // 0 0
  fishbait::Fraction f10{0, 1};
  REQUIRE((int8_t{0} >= f10) == true);
}  // TEST_CASE "int8_t fraction greater than equal to"

TEST_CASE("fraction greater than equal to int8_t", "[utils][fraction]") {
  // positive positive equal
  fishbait::Fraction f0{28, 1};
  REQUIRE((f0 >= int8_t{28}) == true);
  // positive positive not equal
  fishbait::Fraction f1{4562891, 4149428};
  REQUIRE((f1 >= int8_t{42}) == false);
  // positive negative
  fishbait::Fraction f2{199961, 3040388};
  REQUIRE((f2 >= int8_t{-72}) == true);
  // positive 0
  fishbait::Fraction f3{472987, 211928};
  REQUIRE((f3 >= int8_t{0}) == true);
  // negative positive
  fishbait::Fraction f4{-1097209, 2834683};
  REQUIRE((f4 >= int8_t{90}) == false);
  // negative negative equal
  fishbait::Fraction f5{-70, 1};
  REQUIRE((f5 >= int8_t{-70}) == true);
  // negative negative not equal
  fishbait::Fraction f6{-228245, 993884};
  REQUIRE((f6 >= int8_t{-118}) == true);
  // negative 0
  fishbait::Fraction f7{-3239858, 2202999};
  REQUIRE((f7 >= int8_t{0}) == false);
  // 0 positive
  fishbait::Fraction f8{0, 1};
  REQUIRE((f8 >= int8_t{20}) == false);
  // 0 negative
  fishbait::Fraction f9{0, 1};
  REQUIRE((f9 >= int8_t{-36}) == true);
  // 0 0
  fishbait::Fraction f10{0, 1};
  REQUIRE((f10 >= int8_t{0}) == true);
}  // TEST_CASE "fraction greater than equal to int8_t"

TEST_CASE("uint8_t fraction greater than equal to", "[utils][fraction]") {
  // positive positive equal
  fishbait::Fraction f0{104, 1};
  REQUIRE((uint8_t{104} >= f0) == true);
  // positive positive not equal
  fishbait::Fraction f1{375339, 165644};
  REQUIRE((uint8_t{220} >= f1) == true);
  // positive negative
  fishbait::Fraction f2{-4737589, 9836089};
  REQUIRE((uint8_t{184} >= f2) == true);
  // positive 0
  fishbait::Fraction f3{0, 1};
  REQUIRE((uint8_t{84} >= f3) == true);
  // 0 positive
  fishbait::Fraction f4{5526674, 9089585};
  REQUIRE((uint8_t{0} >= f4) == false);
  // 0 negative
  fishbait::Fraction f5{-428090, 4331559};
  REQUIRE((uint8_t{0} >= f5) == true);
  // 0 0
  fishbait::Fraction f6{0, 1};
  REQUIRE((uint8_t{0} >= f6) == true);
}  // TEST_CASE "uint8_t fraction greater than equal to"

TEST_CASE("fraction greater than equal to uint8_t", "[utils][fraction]") {
  // positive positive equal
  fishbait::Fraction f0{141, 1};
  REQUIRE((f0 >= uint8_t{141}) == true);
  // positive positive not equal
  fishbait::Fraction f1{1753889, 4288441};
  REQUIRE((f1 >= uint8_t{69}) == false);
  // positive 0
  fishbait::Fraction f2{1529314, 231255};
  REQUIRE((f2 >= uint8_t{0}) == true);
  // negative positive
  fishbait::Fraction f3{-1472707, 1454375};
  REQUIRE((f3 >= uint8_t{208}) == false);
  // negative 0
  fishbait::Fraction f4{-2933837, 1471451};
  REQUIRE((f4 >= uint8_t{0}) == false);
  // 0 positive
  fishbait::Fraction f5{0, 1};
  REQUIRE((f5 >= uint8_t{130}) == false);
  // 0 0
  fishbait::Fraction f6{0, 1};
  REQUIRE((f6 >= uint8_t{0}) == true);
}  // TEST_CASE "fraction greater than equal to uint8_t"

TEST_CASE("int16_t fraction greater than equal to", "[utils][fraction]") {
  // positive positive equal
  fishbait::Fraction f0{17607, 1};
  REQUIRE((int16_t{17607} >= f0) == true);
  // positive positive not equal
  fishbait::Fraction f1{9223603, 7758000};
  REQUIRE((int16_t{25296} >= f1) == true);
  // positive negative
  fishbait::Fraction f2{-2528992, 2081921};
  REQUIRE((int16_t{11212} >= f2) == true);
  // positive 0
  fishbait::Fraction f3{0, 1};
  REQUIRE((int16_t{14904} >= f3) == true);
  // negative positive
  fishbait::Fraction f4{940091, 4698363};
  REQUIRE((int16_t{-2774} >= f4) == false);
  // negative negative equal
  fishbait::Fraction f5{-27902, 1};
  REQUIRE((int16_t{-27902} >= f5) == true);
  // negative negative not equal
  fishbait::Fraction f6{-430669, 9512976};
  REQUIRE((int16_t{-15369} >= f6) == false);
  // negative 0
  fishbait::Fraction f7{0, 1};
  REQUIRE((int16_t{-9768} >= f7) == false);
  // 0 positive
  fishbait::Fraction f8{2645493, 7692560};
  REQUIRE((int16_t{0} >= f8) == false);
  // 0 negative
  fishbait::Fraction f9{-4092649, 4089062};
  REQUIRE((int16_t{0} >= f9) == true);
  // 0 0
  fishbait::Fraction f10{0, 1};
  REQUIRE((int16_t{0} >= f10) == true);
}  // TEST_CASE "int16_t fraction greater than equal to"

TEST_CASE("fraction greater than equal to int16_t", "[utils][fraction]") {
  // positive positive equal
  fishbait::Fraction f0{24941, 1};
  REQUIRE((f0 >= int16_t{24941}) == true);
  // positive positive not equal
  fishbait::Fraction f1{701799, 309161};
  REQUIRE((f1 >= int16_t{6759}) == false);
  // positive negative
  fishbait::Fraction f2{6571208, 386311};
  REQUIRE((f2 >= int16_t{-24834}) == true);
  // positive 0
  fishbait::Fraction f3{379319, 881942};
  REQUIRE((f3 >= int16_t{0}) == true);
  // negative positive
  fishbait::Fraction f4{-9451279, 8349344};
  REQUIRE((f4 >= int16_t{27798}) == false);
  // negative negative equal
  fishbait::Fraction f5{-5676, 1};
  REQUIRE((f5 >= int16_t{-5676}) == true);
  // negative negative not equal
  fishbait::Fraction f6{-195076, 56333};
  REQUIRE((f6 >= int16_t{-26230}) == true);
  // negative 0
  fishbait::Fraction f7{-2018773, 7013920};
  REQUIRE((f7 >= int16_t{0}) == false);
  // 0 positive
  fishbait::Fraction f8{0, 1};
  REQUIRE((f8 >= int16_t{6511}) == false);
  // 0 negative
  fishbait::Fraction f9{0, 1};
  REQUIRE((f9 >= int16_t{-22055}) == true);
  // 0 0
  fishbait::Fraction f10{0, 1};
  REQUIRE((f10 >= int16_t{0}) == true);
}  // TEST_CASE "fraction greater than equal to int16_t"

TEST_CASE("uint16_t fraction greater than equal to", "[utils][fraction]") {
  // positive positive equal
  fishbait::Fraction f0{9861, 1};
  REQUIRE((uint16_t{9861} >= f0) == true);
  // positive positive not equal
  fishbait::Fraction f1{2950910, 7456153};
  REQUIRE((uint16_t{62214} >= f1) == true);
  // positive negative
  fishbait::Fraction f2{-2192074, 1855935};
  REQUIRE((uint16_t{1504} >= f2) == true);
  // positive 0
  fishbait::Fraction f3{0, 1};
  REQUIRE((uint16_t{53029} >= f3) == true);
  // 0 positive
  fishbait::Fraction f4{5000057, 8398691};
  REQUIRE((uint16_t{0} >= f4) == false);
  // 0 negative
  fishbait::Fraction f5{-869789, 98372};
  REQUIRE((uint16_t{0} >= f5) == true);
  // 0 0
  fishbait::Fraction f6{0, 1};
  REQUIRE((uint16_t{0} >= f6) == true);
}  // TEST_CASE "uint16_t fraction greater than equal to"

TEST_CASE("fraction greater than equal to uint16_t", "[utils][fraction]") {
  // positive positive equal
  fishbait::Fraction f0{22163, 1};
  REQUIRE((f0 >= uint16_t{22163}) == true);
  // positive positive not equal
  fishbait::Fraction f1{8774597, 6318865};
  REQUIRE((f1 >= uint16_t{51256}) == false);
  // positive 0
  fishbait::Fraction f2{5811784, 3854685};
  REQUIRE((f2 >= uint16_t{0}) == true);
  // negative positive
  fishbait::Fraction f3{-3305458, 329161};
  REQUIRE((f3 >= uint16_t{2239}) == false);
  // negative 0
  fishbait::Fraction f4{-9768733, 1755230};
  REQUIRE((f4 >= uint16_t{0}) == false);
  // 0 positive
  fishbait::Fraction f5{0, 1};
  REQUIRE((f5 >= uint16_t{50005}) == false);
  // 0 0
  fishbait::Fraction f6{0, 1};
  REQUIRE((f6 >= uint16_t{0}) == true);
}  // TEST_CASE "fraction greater than equal to uint16_t"

TEST_CASE("int32_t fraction greater than equal to", "[utils][fraction]") {
  // positive positive equal
  fishbait::Fraction f0{4058866, 1};
  REQUIRE((int32_t{4058866} >= f0) == true);
  // positive positive not equal
  fishbait::Fraction f1{5665747, 384500};
  REQUIRE((int32_t{6864647} >= f1) == true);
  // positive negative
  fishbait::Fraction f2{-7899797, 7191741};
  REQUIRE((int32_t{4009663} >= f2) == true);
  // positive 0
  fishbait::Fraction f3{0, 1};
  REQUIRE((int32_t{4931319} >= f3) == true);
  // negative positive
  fishbait::Fraction f4{2607587, 2144002};
  REQUIRE((int32_t{-2587962} >= f4) == false);
  // negative negative equal
  fishbait::Fraction f5{-1339104, 1};
  REQUIRE((int32_t{-1339104} >= f5) == true);
  // negative negative not equal
  fishbait::Fraction f6{-8234888, 4805503};
  REQUIRE((int32_t{-8954998} >= f6) == false);
  // negative 0
  fishbait::Fraction f7{0, 1};
  REQUIRE((int32_t{-5084968} >= f7) == false);
  // 0 positive
  fishbait::Fraction f8{1545101, 1819974};
  REQUIRE((int32_t{0} >= f8) == false);
  // 0 negative
  fishbait::Fraction f9{-1048818, 825007};
  REQUIRE((int32_t{0} >= f9) == true);
  // 0 0
  fishbait::Fraction f10{0, 1};
  REQUIRE((int32_t{0} >= f10) == true);
}  // TEST_CASE "int32_t fraction greater than equal to"

TEST_CASE("fraction greater than equal to int32_t", "[utils][fraction]") {
  // positive positive equal
  fishbait::Fraction f0{6833539, 1};
  REQUIRE((f0 >= int32_t{6833539}) == true);
  // positive positive not equal
  fishbait::Fraction f1{2563221, 870490};
  REQUIRE((f1 >= int32_t{3830405}) == false);
  // positive negative
  fishbait::Fraction f2{6345677, 9204586};
  REQUIRE((f2 >= int32_t{-9641324}) == true);
  // positive 0
  fishbait::Fraction f3{1584616, 9930875};
  REQUIRE((f3 >= int32_t{0}) == true);
  // negative positive
  fishbait::Fraction f4{-3425729, 7800853};
  REQUIRE((f4 >= int32_t{6485552}) == false);
  // negative negative equal
  fishbait::Fraction f5{-4117691, 1};
  REQUIRE((f5 >= int32_t{-4117691}) == true);
  // negative negative not equal
  fishbait::Fraction f6{-4013179, 5053595};
  REQUIRE((f6 >= int32_t{-3228696}) == true);
  // negative 0
  fishbait::Fraction f7{-2004085, 85794};
  REQUIRE((f7 >= int32_t{0}) == false);
  // 0 positive
  fishbait::Fraction f8{0, 1};
  REQUIRE((f8 >= int32_t{726198}) == false);
  // 0 negative
  fishbait::Fraction f9{0, 1};
  REQUIRE((f9 >= int32_t{-4737182}) == true);
  // 0 0
  fishbait::Fraction f10{0, 1};
  REQUIRE((f10 >= int32_t{0}) == true);
}  // TEST_CASE "fraction greater than equal to int32_t"

TEST_CASE("uint32_t fraction greater than equal to", "[utils][fraction]") {
  // positive positive equal
  fishbait::Fraction f0{4907563, 1};
  REQUIRE((uint32_t{4907563} >= f0) == true);
  // positive positive not equal
  fishbait::Fraction f1{941733, 1595306};
  REQUIRE((uint32_t{3370347} >= f1) == true);
  // positive negative
  fishbait::Fraction f2{-1169601, 8465302};
  REQUIRE((uint32_t{8534811} >= f2) == true);
  // positive 0
  fishbait::Fraction f3{0, 1};
  REQUIRE((uint32_t{4108148} >= f3) == true);
  // 0 positive
  fishbait::Fraction f4{6070311, 6726274};
  REQUIRE((uint32_t{0} >= f4) == false);
  // 0 negative
  fishbait::Fraction f5{-763689, 4987940};
  REQUIRE((uint32_t{0} >= f5) == true);
  // 0 0
  fishbait::Fraction f6{0, 1};
  REQUIRE((uint32_t{0} >= f6) == true);
}  // TEST_CASE "uint32_t fraction greater than equal to"

TEST_CASE("fraction greater than equal to uint32_t", "[utils][fraction]") {
  // positive positive equal
  fishbait::Fraction f0{8028078, 1};
  REQUIRE((f0 >= uint32_t{8028078}) == true);
  // positive positive not equal
  fishbait::Fraction f1{267707, 310613};
  REQUIRE((f1 >= uint32_t{2403591}) == false);
  // positive 0
  fishbait::Fraction f2{9718873, 7525113};
  REQUIRE((f2 >= uint32_t{0}) == true);
  // negative positive
  fishbait::Fraction f3{-3163843, 2512275};
  REQUIRE((f3 >= uint32_t{8989168}) == false);
  // negative 0
  fishbait::Fraction f4{-6998190, 1633241};
  REQUIRE((f4 >= uint32_t{0}) == false);
  // 0 positive
  fishbait::Fraction f5{0, 1};
  REQUIRE((f5 >= uint32_t{6168213}) == false);
  // 0 0
  fishbait::Fraction f6{0, 1};
  REQUIRE((f6 >= uint32_t{0}) == true);
}  // TEST_CASE "fraction greater than equal to uint32_t"

TEST_CASE("int64_t fraction greater than equal to", "[utils][fraction]") {
  // positive positive equal
  fishbait::Fraction f0{3362772, 1};
  REQUIRE((int64_t{3362772} >= f0) == true);
  // positive positive not equal
  fishbait::Fraction f1{5561837, 5329884};
  REQUIRE((int64_t{749388} >= f1) == true);
  // positive negative
  fishbait::Fraction f2{-116241, 74401};
  REQUIRE((int64_t{4672759} >= f2) == true);
  // positive 0
  fishbait::Fraction f3{0, 1};
  REQUIRE((int64_t{8874659} >= f3) == true);
  // negative positive
  fishbait::Fraction f4{9946542, 94861};
  REQUIRE((int64_t{-5562451} >= f4) == false);
  // negative negative equal
  fishbait::Fraction f5{-6475184, 1};
  REQUIRE((int64_t{-6475184} >= f5) == true);
  // negative negative not equal
  fishbait::Fraction f6{-1875333, 3595358};
  REQUIRE((int64_t{-8171371} >= f6) == false);
  // negative 0
  fishbait::Fraction f7{0, 1};
  REQUIRE((int64_t{-395380} >= f7) == false);
  // 0 positive
  fishbait::Fraction f8{198623, 104269};
  REQUIRE((int64_t{0} >= f8) == false);
  // 0 negative
  fishbait::Fraction f9{-7178536, 2773317};
  REQUIRE((int64_t{0} >= f9) == true);
  // 0 0
  fishbait::Fraction f10{0, 1};
  REQUIRE((int64_t{0} >= f10) == true);
}  // TEST_CASE "int64_t fraction greater than equal to"

TEST_CASE("fraction greater than equal to int64_t", "[utils][fraction]") {
  // positive positive equal
  fishbait::Fraction f0{2534212, 1};
  REQUIRE((f0 >= int64_t{2534212}) == true);
  // positive positive not equal
  fishbait::Fraction f1{356939, 957692};
  REQUIRE((f1 >= int64_t{4880885}) == false);
  // positive negative
  fishbait::Fraction f2{6443865, 9468706};
  REQUIRE((f2 >= int64_t{-2728939}) == true);
  // positive 0
  fishbait::Fraction f3{801796, 3865927};
  REQUIRE((f3 >= int64_t{0}) == true);
  // negative positive
  fishbait::Fraction f4{-941195, 479207};
  REQUIRE((f4 >= int64_t{1575835}) == false);
  // negative negative equal
  fishbait::Fraction f5{-527614, 1};
  REQUIRE((f5 >= int64_t{-527614}) == true);
  // negative negative not equal
  fishbait::Fraction f6{-1410439, 548299};
  REQUIRE((f6 >= int64_t{-5108907}) == true);
  // negative 0
  fishbait::Fraction f7{-295159, 3207658};
  REQUIRE((f7 >= int64_t{0}) == false);
  // 0 positive
  fishbait::Fraction f8{0, 1};
  REQUIRE((f8 >= int64_t{4872690}) == false);
  // 0 negative
  fishbait::Fraction f9{0, 1};
  REQUIRE((f9 >= int64_t{-8213495}) == true);
  // 0 0
  fishbait::Fraction f10{0, 1};
  REQUIRE((f10 >= int64_t{0}) == true);
}  // TEST_CASE "fraction greater than equal to int64_t"

TEST_CASE("double fraction greater than equal to", "[utils][fraction]") {
  // positive positive equal
  fishbait::Fraction f0{3473482516254713, 2147483648};
  REQUIRE((1617466.339960095 >= f0) == true);
  // positive positive not equal
  fishbait::Fraction f1{5653882, 9017863};
  REQUIRE((7237395.605435523 >= f1) == true);
  // positive negative
  fishbait::Fraction f2{-8127181, 9997456};
  REQUIRE((9992184.546403065 >= f2) == true);
  // positive 0
  fishbait::Fraction f3{0, 1};
  REQUIRE((4929100.53539204 >= f3) == true);
  // negative positive
  fishbait::Fraction f4{8309108, 3680587};
  REQUIRE((-7565267.031707894 >= f4) == false);
  // negative negative equal
  fishbait::Fraction f5{-1848641185087115, 268435456};
  REQUIRE((-6886725.072142165 >= f5) == true);
  // negative negative not equal
  fishbait::Fraction f6{-5731079, 1533330};
  REQUIRE((-3836358.1900747963 >= f6) == false);
  // negative 0
  fishbait::Fraction f7{0, 1};
  REQUIRE((-9883320.522670005 >= f7) == false);
  // 0 positive
  fishbait::Fraction f8{1686761, 8335551};
  REQUIRE((0.0 >= f8) == false);
  // 0 negative
  fishbait::Fraction f9{-4099067, 3046124};
  REQUIRE((0.0 >= f9) == true);
  // 0 0
  fishbait::Fraction f10{0, 1};
  REQUIRE((0.0 >= f10) == true);
}  // TEST_CASE "double fraction greater than equal to"

TEST_CASE("fraction double greater than equal to", "[utils][fraction]") {
  // positive positive equal
  fishbait::Fraction f0{2861299648405027, 536870912};
  REQUIRE((f0 >= 5329585.910597866) == true);
  // positive positive not equal
  fishbait::Fraction f1{3374957, 3050789};
  REQUIRE((f1 >= 6862425.80559151) == false);
  // positive negative
  fishbait::Fraction f2{1921954, 6411295};
  REQUIRE((f2 >= -7246612.57464124) == true);
  // positive 0
  fishbait::Fraction f3{21738, 413381};
  REQUIRE((f3 >= 0.0) == true);
  // negative positive
  fishbait::Fraction f4{-3515396, 4672423};
  REQUIRE((f4 >= 7820810.861228255) == false);
  // negative negative equal
  fishbait::Fraction f5{-2085483905792353, 2147483648};
  REQUIRE((f5 >= -971129.1202308391) == true);
  // negative negative not equal
  fishbait::Fraction f6{-6557635, 2620244};
  REQUIRE((f6 >= -1261455.3147162215) == true);
  // negative 0
  fishbait::Fraction f7{-1746047, 8008616};
  REQUIRE((f7 >= 0.0) == false);
  // 0 positive
  fishbait::Fraction f8{0, 1};
  REQUIRE((f8 >= 4321584.470833904) == false);
  // 0 negative
  fishbait::Fraction f9{0, 1};
  REQUIRE((f9 >= -5008968.000657601) == true);
  // 0 0
  fishbait::Fraction f10{0, 1};
  REQUIRE((f10 >= 0.0) == true);
}  // TEST_CASE "fraction double greater than equal to"

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
/* -------------------------- Conversion Operators -------------------------- */
/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */

TEST_CASE("int8_t conversion", "[utils][fraction]") {
  // positive
  fishbait::Fraction f0{55, 37};
  REQUIRE(static_cast<int8_t>(f0) == 1);
  // negative
  fishbait::Fraction f1{-30, 7};
  REQUIRE(static_cast<int8_t>(f1) == -4);
  // 0
  fishbait::Fraction f2{0, 1};
  REQUIRE(static_cast<int8_t>(f2) == 0);
}  // TEST_CASE "int8_t conversion"

TEST_CASE("uint8_t conversion", "[utils][fraction]") {
  // positive
  fishbait::Fraction f0{99, 190};
  REQUIRE(static_cast<uint8_t>(f0) == 0);
  // 0
  fishbait::Fraction f1{0, 1};
  REQUIRE(static_cast<uint8_t>(f1) == 0);
}  // TEST_CASE "uint8_t conversion"

TEST_CASE("int16_t conversion", "[utils][fraction]") {
  // positive
  fishbait::Fraction f0{5465, 18417};
  REQUIRE(static_cast<int16_t>(f0) == 0);
  // negative
  fishbait::Fraction f1{-8135, 2342};
  REQUIRE(static_cast<int16_t>(f1) == -3);
  // 0
  fishbait::Fraction f2{0, 1};
  REQUIRE(static_cast<int16_t>(f2) == 0);
}  // TEST_CASE "int16_t conversion"

TEST_CASE("uint16_t conversion", "[utils][fraction]") {
  // positive
  fishbait::Fraction f0{48089, 39916};
  REQUIRE(static_cast<uint16_t>(f0) == 1);
  // 0
  fishbait::Fraction f1{0, 1};
  REQUIRE(static_cast<uint16_t>(f1) == 0);
}  // TEST_CASE "uint16_t conversion"

TEST_CASE("int32_t conversion", "[utils][fraction]") {
  // positive
  fishbait::Fraction f0{4649679, 2509195};
  REQUIRE(static_cast<int32_t>(f0) == 1);
  // negative
  fishbait::Fraction f1{-3101001, 938012};
  REQUIRE(static_cast<int32_t>(f1) == -3);
  // 0
  fishbait::Fraction f2{0, 1};
  REQUIRE(static_cast<int32_t>(f2) == 0);
}  // TEST_CASE "int32_t conversion"

TEST_CASE("uint32_t conversion", "[utils][fraction]") {
  // positive
  fishbait::Fraction f0{9398404, 1261649};
  REQUIRE(static_cast<uint32_t>(f0) == 7);
  // 0
  fishbait::Fraction f1{0, 1};
  REQUIRE(static_cast<uint32_t>(f1) == 0);
}  // TEST_CASE "uint32_t conversion"

TEST_CASE("int64_t conversion", "[utils][fraction]") {
  // positive
  fishbait::Fraction f0{5429729, 5477161};
  REQUIRE(static_cast<int64_t>(f0) == 0);
  // negative
  fishbait::Fraction f1{-1236355, 1353767};
  REQUIRE(static_cast<int64_t>(f1) == 0);
  // 0
  fishbait::Fraction f2{0, 1};
  REQUIRE(static_cast<int64_t>(f2) == 0);
}  // TEST_CASE "int64_t conversion"

TEST_CASE("bool conversion", "[utils][fraction]") {
  // positive
  fishbait::Fraction f0{9814312, 1974185};
  REQUIRE(static_cast<bool>(f0) == true);
  // negative
  fishbait::Fraction f1{-5042015, 7494188};
  REQUIRE(static_cast<bool>(f1) == true);
  // 0
  fishbait::Fraction f2;
  REQUIRE(static_cast<bool>(f2) == false);
}  // TEST_CASE "bool conversion"

TEST_CASE("double conversion operator", "[utils][fraction]") {
  // positive
  fishbait::Fraction f0{644327, 327200};
  REQUIRE(static_cast<double>(f0) == 1.9692145476772616);
  // negative
  fishbait::Fraction f1{-447817, 289347};
  REQUIRE(static_cast<double>(f1) == -1.547681503523451);
  // 0
  fishbait::Fraction f2;
  REQUIRE(static_cast<double>(f2) == 0.0);
  // rational
  fishbait::Fraction f3{500, 250};
  REQUIRE(static_cast<double>(f3) == 2.0);
  // irrational
  fishbait::Fraction f4{10, 9};
  REQUIRE(static_cast<double>(f4) == 1.1111111111111112);
}  // TEST_CASE "double conversion operator"

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
/* --------------------------------- Other ---------------------------------- */
/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */

TEST_CASE("fraction inversion", "[utils][fraction]") {
  // 0
  fishbait::Fraction f0{0};
  REQUIRE_THROWS(f0.Invert());
  // 1
  fishbait::Fraction f1{1};
  f1.Invert();
  REQUIRE(f1.numerator() == 1);
  REQUIRE(f1.denominator() == 1);
  // -1
  fishbait::Fraction f2{-1};
  f2.Invert();
  REQUIRE(f2.numerator() == -1);
  REQUIRE(f2.denominator() == 1);
  // positive
  fishbait::Fraction f3{8, 9};
  f3.Invert();
  REQUIRE(f3.numerator() == 9);
  REQUIRE(f3.denominator() == 8);
  // negative
  fishbait::Fraction f4{-34, 78};
  f4.Invert();
  REQUIRE(f4.numerator() == -39);
  REQUIRE(f4.denominator() == 17);
}  // TEST_CASE "fraction inversion"

TEST_CASE("fraction print operator", "[utils][fraction]") {
  std::stringstream ss;

  // positive
  fishbait::Fraction f0{486700, 6015407};
  ss << f0;
  REQUIRE(ss.str() == "486700/6015407");
  ss.str(std::string());
  // negative
  fishbait::Fraction f1{-855137, 4071620};
  ss << f1;
  REQUIRE(ss.str() == "-855137/4071620");
  ss.str(std::string());
  // 0
  fishbait::Fraction f2;
  ss << f2;
  REQUIRE(ss.str() == "0/1");
  ss.str(std::string());
  // rational
  fishbait::Fraction f3{16, 4};
  ss << f3;
  REQUIRE(ss.str() == "4/1");
  ss.str(std::string());
  // irrational
  fishbait::Fraction f4{16, 18};
  ss << f4;
  REQUIRE(ss.str() == "8/9");
  ss.str(std::string());
}  // TEST_CASE "fraction print operator"

TEST_CASE("negative fraction", "[utils][fraction]") {
  // positive
  fishbait::Fraction f0{9975443, 2139705};
  f0 = -f0;
  REQUIRE(f0.numerator() == -9975443);
  REQUIRE(f0.denominator() == 2139705);
  // negative
  fishbait::Fraction f1{-8067385, 6090897};
  f1 = -f1;
  REQUIRE(f1.numerator() == 8067385);
  REQUIRE(f1.denominator() == 6090897);
  // 0
  fishbait::Fraction f2;
  f2 = -f2;
  REQUIRE(f2.numerator() == 0);
  REQUIRE(f2.denominator() == 1);
  // rational
  fishbait::Fraction f3{777, 111};
  f3 = -f3;
  REQUIRE(f3.numerator() == -7);
  REQUIRE(f3.denominator() == 1);
  // irrational
  fishbait::Fraction f4{778, 111};
  f4 = -f4;
  REQUIRE(f4.numerator() == -778);
  REQUIRE(f4.denominator() == 111);
}  // TEST_CASE "negative fraction"

TEST_CASE("fraction GCD", "[utils][fraction]") {
  // GCD equals itself
  REQUIRE(fishbait::Fraction::GCD(8, 32) == fishbait::Fraction::GCD(32, 8));
  // 8, 32
  REQUIRE(fishbait::Fraction::GCD(8, 32) == 8);
  // 8, 12
  REQUIRE(fishbait::Fraction::GCD(8, 12) == 4);
  // 54827, 3716
  REQUIRE(fishbait::Fraction::GCD(54827, 3716) == 1);
  // 7, 19
  REQUIRE(fishbait::Fraction::GCD(7, 19) == 1);
  // 0, 0
  REQUIRE(fishbait::Fraction::GCD(0, 0) == 0);
  // 0, 10
  REQUIRE(fishbait::Fraction::GCD(0, 10) == 10);
  // 10, 0
  REQUIRE(fishbait::Fraction::GCD(10, 0) == 10);
}  // TEST_CASE "fraction GCD"

TEST_CASE("fraction LCM", "[utils][fraction]") {
  // LCM equals itself
  REQUIRE(fishbait::Fraction::LCM(8, 32) == fishbait::Fraction::LCM(32, 8));
  // 8, 32
  REQUIRE(fishbait::Fraction::LCM(8, 32) == 32);
  // 8, 12
  REQUIRE(fishbait::Fraction::LCM(8, 12) == 24);
  // 54827, 3716
  REQUIRE(fishbait::Fraction::LCM(54827, 3716) == 203737132);
  // 7, 19
  REQUIRE(fishbait::Fraction::LCM(7, 19) == 133);
  // 0, 0
  REQUIRE(fishbait::Fraction::LCM(0, 0) == 0);
  // 0, 10
  REQUIRE(fishbait::Fraction::LCM(0, 10) == 0);
  // 10, 0
  REQUIRE(fishbait::Fraction::LCM(10, 0) == 0);
}  // TEST_CASE "fraction LCM"
