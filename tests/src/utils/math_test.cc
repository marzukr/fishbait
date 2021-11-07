// Copyright 2021 Marzuk Rashid

#include <array>
#include <vector>

#include "catch2/catch.hpp"
#include "utils/math.h"

TEST_CASE("stats test", "[utils][math]") {
  std::vector<int> int_data = {3, 4, 5, 6, 7, -5};
  double int_mean = fishbait::Mean(int_data);
  REQUIRE(int_mean == Approx(3.3333333333));
  REQUIRE(fishbait::Std(int_data, int_mean) == Approx(3.9440531887330774));

  std::array<double, 4> double_data = {7.3, 8.9, 10.4, -22};
  double double_mean = fishbait::Mean(double_data);
  REQUIRE(double_mean == Approx(1.15));
  REQUIRE(fishbait::Std(double_data, double_mean) == Approx(13.41053690200358));
}  // TEST_CASE "basic timer tests"
