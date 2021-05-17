// Copyright 2021 Marzuk Rashid

#include <cstdint>
#include <random>

#include "catch2/catch.hpp"
#include "utils/random.h"

TEST_CASE("Random doubles with same seed", "[utils][random]") {
  utils::Random r1(utils::Random::Seed(4567));
  utils::Random r2(utils::Random::Seed(4567));
  std::uniform_real_distribution<> std_unif(0.0, 1.0);
  REQUIRE(std_unif(r1()) == std_unif(r2()));
}  // TEST_CASE "Random doubles with same seed"

TEST_CASE("Random ints with same seed", "[utils][random]") {
  utils::Random r1(utils::Random::Seed(4));
  utils::Random r2(utils::Random::Seed(4));
  std::uniform_int_distribution<uint32_t> thousand(0, 999);
  REQUIRE(thousand(r1()) == thousand(r2()));
}  // TEST_CASE "Random ints with same seed"

TEST_CASE("Random ints with different seed", "[utils][random]") {
  utils::Random r1(utils::Random::Seed(4));
  utils::Random r2(utils::Random::Seed(5));
  std::uniform_int_distribution<uint32_t> thousand(0, 999);
  REQUIRE(thousand(r1()) != thousand(r2()));
}  // TEST_CASE "Random ints with same seed"

TEST_CASE("Random ints with random seeds", "[utils][random]") {
  utils::Random r1(utils::Random::Seed{});
  utils::Random r2(utils::Random::Seed{});
  std::uniform_int_distribution<uint32_t> thousand(0, 999);
  if (thousand(r1()) == thousand(r2())) {
    FAIL("The random seeds generated the same integer. This is unlikely but "
        "not impossible. If this test consistently fails then there is a "
        "problem.");
  }
}  // TEST_CASE "Random ints with same seed"

TEST_CASE("Random ints with random seeds 2", "[utils][random]") {
  utils::Random r1;
  utils::Random r2;
  std::uniform_int_distribution<uint32_t> thousand(0, 999);
  if (thousand(r1()) == thousand(r2())) {
    FAIL("The random seeds generated the same integer. This is unlikely but "
        "not impossible. If this test consistently fails then there is a "
        "problem.");
  }
}  // TEST_CASE "Random ints with same seed"
