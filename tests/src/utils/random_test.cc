#include <cstdint>
#include <random>

#include "catch2/catch.hpp"
#include "utils/random.h"

TEST_CASE("Random doubles with same seed", "[utils][random]") {
  fishbait::Random r1(fishbait::Random::Seed(4567));
  fishbait::Random r2(fishbait::Random::Seed(4567));
  std::uniform_real_distribution<> std_unif(0.0, 1.0);
  REQUIRE(std_unif(r1()) == std_unif(r2()));
}  // TEST_CASE "Random doubles with same seed"

TEST_CASE("Random ints with same seed", "[utils][random]") {
  fishbait::Random r1(fishbait::Random::Seed(4));
  fishbait::Random r2(fishbait::Random::Seed(4));
  std::uniform_int_distribution<uint32_t> thousand(0, 999);
  REQUIRE(thousand(r1()) == thousand(r2()));
}  // TEST_CASE "Random ints with same seed"

TEST_CASE("Random ints with different seed", "[utils][random]") {
  fishbait::Random r1(fishbait::Random::Seed(4));
  fishbait::Random r2(fishbait::Random::Seed(5));
  std::uniform_int_distribution<uint32_t> thousand(0, 999);
  REQUIRE(thousand(r1()) != thousand(r2()));
}  // TEST_CASE "Random ints with same seed"

TEST_CASE("Random ints with random seeds", "[utils][random]") {
  fishbait::Random r1(fishbait::Random::Seed{});
  fishbait::Random r2(fishbait::Random::Seed{});
  std::uniform_int_distribution<uint32_t> thousand(0, 999);
  if (thousand(r1()) == thousand(r2())) {
    WARN("The random seeds generated the same integer. This is unlikely but "
        "not impossible. If this test consistently fails then there is a "
        "problem.");
  }
}  // TEST_CASE "Random ints with same seed"

TEST_CASE("Random ints with random seeds 2", "[utils][random]") {
  fishbait::Random r1;
  fishbait::Random r2;
  std::uniform_int_distribution<uint32_t> thousand(0, 999);
  if (thousand(r1()) == thousand(r2())) {
    WARN("The random seeds generated the same integer. This is unlikely but "
        "not impossible. If this test consistently fails then there is a "
        "problem.");
  }
}  // TEST_CASE "Random ints with same seed"

TEST_CASE("Random seed produces different consecutive results",
          "[utils][random]") {
  fishbait::Random r1;
  std::uniform_int_distribution<uint32_t> thousand(0, 999);
  if (thousand(r1()) == thousand(r1())) {
    WARN("The random seed generated the same integer twice in a row. This is "
         "unlikely but not impossible. If this test consistently fails then "
         "there is a problem.");
  }
}  // TEST_CASE "Random ints with same seed"

TEST_CASE("changing seed produces the same results", "[utils][random]") {
  fishbait::Random r1(fishbait::Random::Seed(456));
  fishbait::Random r2;
  r2.seed(fishbait::Random::Seed(456));
  std::uniform_int_distribution<uint32_t> thousand(0, 999);
  REQUIRE(thousand(r1()) == thousand(r2()));
}  // TEST_CASE "changing seed produces the same results"
