#include <cstdint>
#include <vector>

#include "catch2/catch.hpp"
#include "clustering/distance.h"

TEST_CASE("EMD identical test", "[clustering][distance]") {
  std::vector<uint16_t> p1(50, 0);  // flops _id: 302847
  p1[49] = 1081;
  std::vector<uint16_t> p2(50, 0);  // flops _id: 91636
  p2[49] = 1081;

  double dist = fishbait::EarthMoverDistance<uint16_t, uint16_t>::Compute(
      nda::vector_ref<uint16_t>{p1}, nda::vector_ref<uint16_t>{p2});

  REQUIRE(dist == 0);
}

TEST_CASE("EMD flops data test", "[clustering][distance]") {
  // flops _id: 315
  std::vector<uint16_t> p1{0, 4, 53, 22, 56, 75, 10, 160, 218, 42, 0, 62, 96, 0,
      4, 8, 0, 0, 1, 0, 14, 1, 3, 2, 6, 1, 0, 1, 0, 0, 0, 0, 7, 44, 13, 38, 17,
      34, 26, 22, 10, 10, 0, 6, 9, 0, 0, 0, 6, 0};

  std::vector<uint16_t> p2(50, 0);  // flops _id: 302769
  p2[49] = 1081;

  double dist = fishbait::EarthMoverDistance<uint16_t, uint16_t>::Compute(
      nda::vector_ref<uint16_t>{p1}, nda::vector_ref<uint16_t>{p2});

  REQUIRE(dist == 37339);
}

TEST_CASE("EMD int double test", "[clustering][distance]") {
  std::vector<uint16_t> p1{2, 5, 15};
  std::vector<double> p2{10.4, 1.1, 10.5};

  double int_double = fishbait::EarthMoverDistance<uint16_t, double>::Compute(
      nda::vector_ref<uint16_t>{p1}, nda::vector_ref<double>{p2});

  REQUIRE(int_double == 12.9);

  double double_int = fishbait::EarthMoverDistance<double, uint16_t>::Compute(
      nda::vector_ref<double>{p2}, nda::vector_ref<uint16_t>{p1});

  REQUIRE(double_int == 12.9);
}

TEST_CASE("Euclidean distance identical test", "[clustering][distance]") {
  std::vector<int16_t> p1{6, -4, 10, -50, -247, 4};
  std::vector<int16_t> p2{6, -4, 10, -50, -247, 4};

  double dist = fishbait::EuclideanDistance<int16_t, int16_t>::Compute(
      nda::vector_ref<int16_t>{p1}, nda::vector_ref<int16_t>{p2});

  REQUIRE(dist == 0.0);
}

TEST_CASE("Euclidean distance int test", "[clustering][distance]") {
  std::vector<int16_t> p1{2, 4, 5, 3, 8, 2};
  std::vector<int16_t> p2{3, 1, 5, -3, 7, 2};

  double dist = fishbait::EuclideanDistance<int16_t, int16_t>::Compute(
      nda::vector_ref<int16_t>{p1}, nda::vector_ref<int16_t>{p2});

  REQUIRE(dist == 6.8556546004010439077092087245546281337738037109375);
}

TEST_CASE("Euclidean int double test", "[clustering][distance]") {
  std::vector<int16_t> p1{0, 4, -4, 6, -50, 23};
  std::vector<double> p2{0.8, 10.3, -100.56, 7.2, 15.4, 75.8};

  double int_double = fishbait::EuclideanDistance<int16_t, double>::Compute(
      nda::vector_ref<int16_t>{p1}, nda::vector_ref<double>{p2});

  REQUIRE(int_double == 128.181916041226344304959638975560665130615234375);

  double double_int = fishbait::EuclideanDistance<double, int16_t>::Compute(
      nda::vector_ref<double>{p2}, nda::vector_ref<int16_t>{p1});

  REQUIRE(double_int == 128.181916041226344304959638975560665130615234375);
}
