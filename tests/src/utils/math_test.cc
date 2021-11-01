// Copyright 2021 Marzuk Rashid

#include <array>
#include <vector>

#include "catch2/catch.hpp"
#include "utils/math.h"

TEST_CASE("mean test", "[utils][math]") {
  std::vector<int> int_data = {3, 4, 5, 6, 7, -5};
  REQUIRE(fishbait::Mean(int_data) == Approx(3.3333333333));

  std::array<double, 4> double_data = {7.3, 8.9, 10.4, -22};
  REQUIRE(fishbait::Mean(double_data) == Approx(1.15));
}  // TEST_CASE "basic timer tests"
