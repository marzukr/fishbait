// Copyright 2021 Marzuk Rashid

#include "Catch2/single_include/catch2/catch.hpp"

#include <array>
#include <cstdint>

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
