// Copyright 2021 Marzuk Rashid

#include <stdint.h>

#include <vector>

#include "Catch2/single_include/catch2/catch.hpp"

#include "clustering/EarthMoverDistance.h"
#include "utils/VectorView.h"

TEST_CASE("EMD identical test", "[clustering][earthmoverdistance]") {
  std::vector<uint16_t> p1(50, 0);  // flops _id: 302847
  p1[49] = 1081;
  std::vector<uint16_t> p2(50, 0);  // flops _id: 91636
  p2[49] = 1081;

  double dist = clustering::EarthMoverDistance<uint16_t, uint16_t>::Compute(
      utils::VectorView(p1), utils::VectorView(p2));

  REQUIRE(dist == 0);
}

TEST_CASE("EMD flops data test", "[clustering][earthmoverdistance]") {
  // flops _id: 315
  std::vector<uint16_t> p1{0, 4, 53, 22, 56, 75, 10, 160, 218, 42, 0, 62, 96, 0,
      4, 8, 0, 0, 1, 0, 14, 1, 3, 2, 6, 1, 0, 1, 0, 0, 0, 0, 7, 44, 13, 38, 17,
      34, 26, 22, 10, 10, 0, 6, 9, 0, 0, 0, 6, 0};

  std::vector<uint16_t> p2(50, 0);  // flops _id: 302769
  p2[49] = 1081;

  double dist = clustering::EarthMoverDistance<uint16_t, uint16_t>::Compute(
      utils::VectorView(p1), utils::VectorView(p2));

  REQUIRE(dist == 37339);
}

TEST_CASE("EMD int double test", "[clustering][earthmoverdistance]") {
  std::vector<uint16_t> p1{2, 5, 15};
  std::vector<double> p2{10.4, 1.1, 10.5};

  double int_double = clustering::EarthMoverDistance<uint16_t, double>::Compute(
      utils::VectorView(p1), utils::VectorView(p2));

  REQUIRE(int_double == 12.9);

  double double_int = clustering::EarthMoverDistance<double, uint16_t>::Compute(
      utils::VectorView(p2), utils::VectorView(p1));

  REQUIRE(double_int == 12.9);
}
