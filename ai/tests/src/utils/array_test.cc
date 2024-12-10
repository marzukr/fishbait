#include <array>

#include "catch2/catch.hpp"
#include "utils/array.h"

TEST_CASE("array fill test", "[utils][array]") {
  std::array<int, 15> test_arr = fishbait::FilledArray<int, 15>(256);
  for (auto it = test_arr.begin(); it != test_arr.end(); ++it) {
    REQUIRE(*it == 256);
  }
}  // TEST_CASE "array fill test"
