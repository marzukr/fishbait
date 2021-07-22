// Copyright 2021 Marzuk Rashid

#include <array>
#include <cstdint>

#include "catch2/catch.hpp"
#include "engine/card_utils.h"
#include "engine/definitions.h"

TEST_CASE("Test SK to ISO", "[engine][cardutils]") {
  std::array<engine::SK_Card, engine::kDeckSize> converted;
  for (engine::Card i = 0; i < engine::kDeckSize; ++i) {
    converted[i] = engine::ConvertSKtoISO(i);
  }
  std::array<engine::ISO_Card, engine::kDeckSize> correct{48, 49, 50, 51, 44,
      45, 46, 47, 40, 41, 42, 43, 36, 37, 38, 39, 32, 33, 34, 35, 28, 29, 30,
      31, 24, 25, 26, 27, 20, 21, 22, 23, 16, 17, 18, 19, 12, 13, 14, 15, 8, 9,
      10, 11, 4, 5, 6, 7, 0, 1, 2, 3};
  REQUIRE(converted == correct);
}  // TEST_CASE("Test SK to ISO", "[engine][cardutils]")

TEST_CASE("Test ISO to SK", "[engine][cardutils]") {
  std::array<engine::Card, engine::kDeckSize> converted;
  for (engine::ISO_Card i = 0; i < engine::kDeckSize; ++i) {
    converted[i] = engine::ConvertISOtoSK(i);
  }
  std::array<engine::SK_Card, engine::kDeckSize> correct{48, 49, 50, 51, 44, 45,
      46, 47, 40, 41, 42, 43, 36, 37, 38, 39, 32, 33, 34, 35, 28, 29, 30, 31,
      24, 25, 26, 27, 20, 21, 22, 23, 16, 17, 18, 19, 12, 13, 14, 15, 8, 9, 10,
      11, 4, 5, 6, 7, 0, 1, 2, 3};
  REQUIRE(converted == correct);
}  // TEST_CASE("Test ISO to SK", "[engine][cardutils]")

TEST_CASE("SKCard Pretty Test", "[engine][cardutils]") {
  std::array<std::string, engine::kDeckSize> converted;
  for (engine::SK_Card i = 0; i < engine::kDeckSize; ++i) {
    converted[i] = engine::SKCardPretty(i);
  }
  std::array<std::string, engine::kDeckSize> correct{"A♠", "A♥", "A♦", "A♣",
      "K♠", "K♥", "K♦", "K♣", "Q♠", "Q♥", "Q♦", "Q♣", "J♠", "J♥", "J♦", "J♣",
      "T♠", "T♥", "T♦", "T♣", "9♠", "9♥", "9♦", "9♣", "8♠", "8♥", "8♦", "8♣",
      "7♠", "7♥", "7♦", "7♣", "6♠", "6♥", "6♦", "6♣", "5♠", "5♥", "5♦", "5♣",
      "4♠", "4♥", "4♦", "4♣", "3♠", "3♥", "3♦", "3♣", "2♠", "2♥", "2♦", "2♣"};
  REQUIRE(converted == correct);
}  // TEST_CASE "SKCard Pretty Test"

TEST_CASE("ISOCardFromStr Test", "[engine][cardutils]") {
  std::array<std::string, engine::kDeckSize> strings{"As", "Ah", "Ad", "Ac",
      "Ks", "Kh", "Kd", "Kc", "Qs", "Qh", "Qd", "Qc", "Js", "Jh", "Jd", "Jc",
      "Ts", "Th", "Td", "Tc", "9s", "9h", "9d", "9c", "8s", "8h", "8d", "8c",
      "7s", "7h", "7d", "7c", "6s", "6h", "6d", "6c", "5s", "5h", "5d", "5c",
      "4s", "4h", "4d", "4c", "3s", "3h", "3d", "3c", "2s", "2h", "2d", "2c"};
  std::array<engine::ISO_Card, engine::kDeckSize> correct{48, 49, 50, 51, 44,
      45, 46, 47, 40, 41, 42, 43, 36, 37, 38, 39, 32, 33, 34, 35, 28, 29, 30,
      31, 24, 25, 26, 27, 20, 21, 22, 23, 16, 17, 18, 19, 12, 13, 14, 15, 8, 9,
      10, 11, 4, 5, 6, 7, 0, 1, 2, 3};

  for (engine::ISO_Card i = 0; i < engine::kDeckSize; ++i) {
    REQUIRE(+engine::ISOCardFromStr(strings[i]) == +correct[i]);
  }
  REQUIRE_THROWS(engine::ISOCardFromStr(""));
  REQUIRE_THROWS(engine::ISOCardFromStr("2cc"));
  REQUIRE_THROWS(engine::ISOCardFromStr("2l"));
  REQUIRE_THROWS(engine::ISOCardFromStr("cc"));
}  // TEST_CASE "ISOCardFromStr Test"

TEST_CASE("SKCardFromStr Test", "[engine][cardutils]") {
  std::array<std::string, engine::kDeckSize> strings{"As", "Ah", "Ad", "Ac",
      "Ks", "Kh", "Kd", "Kc", "Qs", "Qh", "Qd", "Qc", "Js", "Jh", "Jd", "Jc",
      "Ts", "Th", "Td", "Tc", "9s", "9h", "9d", "9c", "8s", "8h", "8d", "8c",
      "7s", "7h", "7d", "7c", "6s", "6h", "6d", "6c", "5s", "5h", "5d", "5c",
      "4s", "4h", "4d", "4c", "3s", "3h", "3d", "3c", "2s", "2h", "2d", "2c"};
  std::array<engine::SK_Card, engine::kDeckSize> correct{0, 1, 2, 3, 4, 5, 6, 7,
      8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26,
      27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44,
      45, 46, 47, 48, 49, 50, 51};

  for (engine::SK_Card i = 0; i < engine::kDeckSize; ++i) {
    REQUIRE(+engine::SKCardFromStr(strings[i]) == +correct[i]);
  }
  REQUIRE_THROWS(engine::SKCardFromStr(""));
  REQUIRE_THROWS(engine::SKCardFromStr("Tcc"));
  REQUIRE_THROWS(engine::SKCardFromStr("Al"));
  REQUIRE_THROWS(engine::SKCardFromStr("ss"));
}  // TEST_CASE "SKCardFromStr Test"
