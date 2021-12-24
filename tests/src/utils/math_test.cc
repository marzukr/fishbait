// Copyright 2021 Marzuk Rashid

#include <array>
#include <vector>

#include "catch2/catch.hpp"
#include "utils/math.h"

TEST_CASE("stats test", "[utils][math]") {
  std::vector<int> int_data = {3, 4, 5, 6, 7, -5};
  double int_mean = fishbait::Mean(int_data);
  double int_std = fishbait::Std(int_data, int_mean);
  REQUIRE(int_mean == Approx(3.3333333333));
  REQUIRE(int_std == Approx(3.9440531887330774));
  REQUIRE(fishbait::CI95(int_data, int_std) == Approx(3.1558998247));

  std::array<double, 4> double_data = {7.3, 8.9, 10.4, -22};
  double double_mean = fishbait::Mean(double_data);
  double double_std = fishbait::Std(double_data, double_mean);
  REQUIRE(double_mean == Approx(1.15));
  REQUIRE(double_std == Approx(13.41053690200358));
  REQUIRE(fishbait::CI95(double_data, double_std) == Approx(13.142326164));
}  // TEST_CASE "stats test"

TEST_CASE("normalize test", "[utils][math]") {
  std::vector<int> int_data = {3, 4, 5, 6, 7, -5};
  fishbait::Normalize(int_data);
  REQUIRE(int_data == std::vector<int>{0, 0, 0, 0, 0, 0});

  std::array<double, 4> double_data = {7.3, 8.9, 10.4, -22};
  fishbait::Normalize(double_data);
  std::array<double, 4> expected{7.3/4.6, 8.9/4.6, 10.4/4.6, -22/4.6};
  for (int i = 0; i < 4; ++i) {
    REQUIRE(double_data[i] == Approx(expected[i]));
  }
}  // TEST_CASE "normalize test"
