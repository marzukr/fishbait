// Copyright 2021 Marzuk Rashid

#include <array>
#include <cstdint>

#include "catch2/catch.hpp"

#include "hand_strengths/card_utils.h"

TEST_CASE("Test SK to ISO", "[hand_strengths][cardutils]") {
  const uint8_t kDeckSize = 52;

  std::array<uint8_t, 52> converted;
  for (uint8_t i = 0; i < kDeckSize; ++i) {
    converted[i] = hand_strengths::ConvertSKtoISO(i);
  }
  std::array<uint8_t, 52> correct{48, 49, 50, 51, 44, 45, 46, 47, 40, 41, 42,
                                  43, 36, 37, 38, 39, 32, 33, 34, 35, 28, 29,
                                  30, 31, 24, 25, 26, 27, 20, 21, 22, 23, 16,
                                  17, 18, 19, 12, 13, 14, 15,  8,  9, 10, 11,
                                   4,  5,  6,  7,  0,  1,  2,  3};
  REQUIRE(converted == correct);
}  // TEST_CASE("Test SK to ISO", "[hand_strengths][cardutils]")

TEST_CASE("Test ISO to SK", "[hand_strengths][cardutils]") {
  const uint8_t kDeckSize = 52;

  std::array<uint8_t, 52> converted;
  for (uint8_t i = 0; i < kDeckSize; ++i) {
    converted[i] = hand_strengths::ConvertISOtoSK(i);
  }
  std::array<uint8_t, 52> correct{48, 49, 50, 51, 44, 45, 46, 47, 40, 41, 42,
                                  43, 36, 37, 38, 39, 32, 33, 34, 35, 28, 29,
                                  30, 31, 24, 25, 26, 27, 20, 21, 22, 23, 16,
                                  17, 18, 19, 12, 13, 14, 15,  8,  9, 10, 11,
                                   4,  5,  6,  7,  0,  1,  2,  3};
  REQUIRE(converted == correct);
}  // TEST_CASE("Test ISO to SK", "[hand_strengths][cardutils]")

TEST_CASE("SKCard Pretty Test", "[hand_strengths][cardutils]") {
  const uint8_t deck_size = 52;

  std::array<std::string, deck_size> converted;
  for (uint8_t i = 0; i < deck_size; ++i) {
    converted[i] = hand_strengths::SKCardPretty(i);
  }
  std::array<std::string, deck_size> correct{"A♠", "A♥", "A♦", "A♣", "K♠", "K♥",
                                             "K♦", "K♣", "Q♠", "Q♥", "Q♦", "Q♣",
                                             "J♠", "J♥", "J♦", "J♣", "T♠", "T♥",
                                             "T♦", "T♣", "9♠", "9♥", "9♦", "9♣",
                                             "8♠", "8♥", "8♦", "8♣", "7♠", "7♥", 
                                             "7♦", "7♣", "6♠", "6♥", "6♦", "6♣",
                                             "5♠", "5♥", "5♦", "5♣", "4♠", "4♥",
                                             "4♦", "4♣", "3♠", "3♥", "3♦", "3♣",
                                             "2♠", "2♥", "2♦", "2♣"};
  REQUIRE(converted == correct);
}  // TEST_CASE "SKCard Pretty Test"

TEST_CASE("ISOCardFromStr Test", "[hand_strengths][cardutils]") {
  const uint8_t deck_size = 52;

  std::array<std::string, deck_size> strings{"As", "Ah", "Ad", "Ac", "Ks", "Kh",
                                             "Kd", "Kc", "Qs", "Qh", "Qd", "Qc",
                                             "Js", "Jh", "Jd", "Jc", "Ts", "Th",
                                             "Td", "Tc", "9s", "9h", "9d", "9c",
                                             "8s", "8h", "8d", "8c", "7s", "7h",
                                             "7d", "7c", "6s", "6h", "6d", "6c",
                                             "5s", "5h", "5d", "5c", "4s", "4h",
                                             "4d", "4c", "3s", "3h", "3d", "3c",
                                             "2s", "2h", "2d", "2c"};
  std::array<uint8_t, deck_size> correct{48, 49, 50, 51, 44, 45, 46, 47, 40, 41,
                                         42, 43, 36, 37, 38, 39, 32, 33, 34, 35,
                                         28, 29, 30, 31, 24, 25, 26, 27, 20, 21,
                                         22, 23, 16, 17, 18, 19, 12, 13, 14, 15,
                                         8, 9, 10, 11, 4, 5, 6, 7, 0, 1, 2, 3};

  for (uint8_t i = 0; i < deck_size; ++i) {
    REQUIRE(+hand_strengths::ISOCardFromStr(strings[i]) == +correct[i]);
  }
}  // TEST_CASE "ISOCardFromStr Test"
