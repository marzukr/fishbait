// Copyright 2021 Marzuk Rashid

#include <stdint.h>

#include <vector>
#include <memory>
#include <array>

#include "Catch2/single_include/catch2/catch.hpp"

#include "utils/VectorView.h"

TEST_CASE("Test VectorView with heap array", "[utils][vectorview]") {
  const int array_size = 100;
  const int array_filler = 4;

  std::unique_ptr<int[]> int_array(new int[array_size]);
  for (int i = 0; i < array_size; ++i) {
    int_array[i] = array_filler;
  }
  utils::VectorView int_vv(int_array.get(), array_size);

  SECTION("Verify that n is correct") {
    REQUIRE(int_vv.n() == array_size);
  }
  SECTION("Verify that start is correct") {
    REQUIRE(int_vv.begin() == int_array.get());
  }
  SECTION("Verify that the () operator works") {
    for (uint32_t i = 0; i < int_vv.n(); ++i) {
      REQUIRE(int_vv(i) == array_filler);
    }
  }
  SECTION("Verify that the iterator works") {
    for (const int* it = int_vv.begin(); it < int_vv.end(); ++it) {
      REQUIRE(*it == array_filler);
    }
  }
}  // TEST_CASE("Test VectorView with heap array", "[utils][vectorview]")

TEST_CASE("Test VectorView with vector", "[utils][vectorview]") {
  const int array_size = 100;
  const double array_filler = 6.78;

  std::vector<double> double_vector(array_size, array_filler);
  utils::VectorView double_vv(double_vector);

  SECTION("Verify that n is correct") {
    REQUIRE(double_vv.n() == array_size);
  }
  SECTION("Verify that start is correct") {
    REQUIRE(double_vv.begin() == double_vector.data());
  }
  SECTION("Verify that the () operator works") {
    for (uint32_t i = 0; i < double_vv.n(); ++i) {
      REQUIRE(double_vv(i) == array_filler);
    }
  }
  SECTION("Verify that the iterator works") {
    for (const double* it = double_vv.begin(); it < double_vv.end(); ++it) {
      REQUIRE(*it == array_filler);
    }
  }
}  // TEST_CASE("Test VectorView with vector", "[utils][vectorview]")

TEST_CASE("Test VectorView with initializer list", "[utils][vectorview]") {
  const int list_size = 6;

  utils::VectorView int_vv({0, 1, 2, 3, 4, 5});

  SECTION("Verify that n is correct") {
    REQUIRE(int_vv.n() == list_size);
  }
  SECTION("Verify that the () operator works") {
    for (uint32_t i = 0; i < int_vv.n(); ++i) {
      REQUIRE(int_vv(i) == i);
    }
  }
  SECTION("Verify that the iterator works") {
    uint32_t i = 0;
    for (const int* it = int_vv.begin(); it < int_vv.end(); ++it) {
      REQUIRE(*it == i);
      ++i;
    }
  }
}  // TEST_CASE("Test VectorView with initializer list", "[utils][vectorview]")

TEST_CASE("Test VectorView with array", "[utils][vectorview]") {
  const std::size_t array_size = 10;
  const std::array<double, array_size> arr = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

  utils::VectorView double_vv(arr);

  SECTION("Verify that n is correct") {
    REQUIRE(double_vv.n() == array_size);
  }
  SECTION("Verify that start is correct") {
    REQUIRE(double_vv.begin() == arr.begin());
  }
  SECTION("Verify that the () operator works") {
    for (uint32_t i = 0; i < double_vv.n(); ++i) {
      REQUIRE(double_vv(i) == i);
    }
  }
  SECTION("Verify that the iterator works") {
    uint32_t i = 0;
    for (const double* it = double_vv.begin(); it < double_vv.end(); ++it) {
      REQUIRE(*it == i);
      ++i;
    }
  }
}  // TEST_CASE("Test VectorView with array", "[utils][vectorview]")
