// Copyright 2021 Marzuk Rashid

#include <stdint.h>

#include <vector>
#include <memory>
#include <iostream>

#include "Catch2/single_include/catch2/catch.hpp"

#include "utils/VectorView.h"

TEST_CASE("Test VectorView with heap array", "[utils][vectorview]") {
  std::unique_ptr<int[]> int_array(new int[100]);
  for (int i = 0; i < 100; ++i) {
    int_array[i] = 4;
  }
  utils::VectorView int_vv(int_array.get(), 100);

  SECTION("Verify that n is correct") {
    REQUIRE(int_vv.n() == 100);
  }
  SECTION("Verify that start is correct") {
    REQUIRE(int_vv.begin() == int_array.get());
  }
  SECTION("Verify that the () operator works") {
    for (uint32_t i = 0; i < int_vv.n(); ++i) {
      REQUIRE(int_vv(i) == 4);
    }
  }
  SECTION("Verify that the iterator works") {
    for (const int* it = int_vv.begin(); it < int_vv.end(); ++it) {
      REQUIRE(*it == 4);
    }
  }
}

TEST_CASE("Test VectorView with vector", "[utils][vectorview]") {
  std::vector<double> double_vector(100, 6.78);
  utils::VectorView double_vv(double_vector);

  SECTION("Verify that n is correct") {
    utils::VectorView double_vv(double_vector);
    REQUIRE(double_vv.n() == 100);
  }
  SECTION("Verify that start is correct") {
    REQUIRE(double_vv.begin() == double_vector.data());
  }
  SECTION("Verify that the () operator works") {
    for (uint32_t i = 0; i < double_vv.n(); ++i) {
      REQUIRE(double_vv(i) == 6.78);
    }
  }
  SECTION("Verify that the iterator works") {
    for (const double* it = double_vv.begin(); it < double_vv.end(); ++it) {
      REQUIRE(*it == 6.78);
    }
  }
}
