// Copyright 2021 Marzuk Rashid

#include <cstdint>
#include <filesystem>
#include <iostream>
#include <vector>

#include "catch2/catch.hpp"
#include "utils/cereal.h"
#include "utils/matrix.h"

TEST_CASE("Save and load vector test", "[utils][cereal]") {
  const uint32_t n = 100;

  std::vector<double> save_vect(n);
  for (uint32_t i = 0; i < n; ++i) {
    save_vect[i] = i + i / 10.0;
  }
  std::filesystem::remove("out/tests/vector_save_test.cereal");
  utils::CerealSave("out/tests/vector_save_test.cereal", &save_vect);

  std::vector<double> load_vect;
  utils::CerealLoad("out/tests/vector_save_test.cereal", &load_vect);
  REQUIRE(load_vect.size() == n);
  for (uint32_t i = 0; i < n; ++i) {
    REQUIRE(load_vect[i] == (i + i / 10.0));
  }
}  // TEST_CASE "Save and load vector test"

TEST_CASE("Save and load matrix test", "[utils][cereal]") {
  const uint32_t n = 100;
  const uint32_t m = 100;

  utils::Matrix<int> save_matrix(n, m);
  for (uint32_t i = 0; i < n; ++i) {
    for (uint32_t j = 0; j < m; ++j) {
      save_matrix(i, j) = i + j;
    }
  }
  std::filesystem::remove("out/tests/matrix_save_test.cereal");
  utils::CerealSave("out/tests/matrix_save_test.cereal", &save_matrix);

  utils::Matrix<int> load_matrix(1, 1, 0);
  utils::CerealLoad("out/tests/matrix_save_test.cereal", &load_matrix);
  REQUIRE(load_matrix.n() == n);
  REQUIRE(load_matrix.m() == m);
  for (uint32_t i = 0; i < n; ++i) {
    for (uint32_t j = 0; j < m; ++j) {
      REQUIRE(load_matrix(i, j) == (int)(i + j));
    }
  }
}  // TEST_CASE "Save and load matrix test"
