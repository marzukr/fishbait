// Copyright 2021 Marzuk Rashid

#include <cstdint>
#include <filesystem>
#include <iostream>
#include <vector>

#include "array/array.h"
#include "catch2/catch.hpp"
#include "utils/cereal.h"

TEST_CASE("Save and load vector test", "[utils][cereal]") {
  const uint32_t n = 100;

  std::vector<double> save_vect(n);
  for (uint32_t i = 0; i < n; ++i) {
    save_vect[i] = i + i / 10.0;
  }
  std::filesystem::remove("out/tests/vector_save_test.cereal");
  fishbait::Cereal(fishbait::FileAction::Save,
                   "out/tests/vector_save_test.cereal", &save_vect);

  std::vector<double> load_vect;
  fishbait::CerealLoad("out/tests/vector_save_test.cereal", &load_vect);
  REQUIRE(load_vect.size() == n);
  for (uint32_t i = 0; i < n; ++i) {
    REQUIRE(load_vect[i] == (i + i / 10.0));
  }
}  // TEST_CASE "Save and load vector test"

TEST_CASE("Save and load nda test", "[utils][cereal]") {
  const uint32_t n = 100;
  const uint32_t m = 100;

  using dense_col_t = nda::shape<nda::dim<>, nda::dense_dim<>>;
  nda::array<int, dense_col_t> save_matrix({n, m});
  for (uint32_t i = 0; i < n; ++i) {
    for (uint32_t j = 0; j < m; ++j) {
      save_matrix(i, j) = i + j;
    }
  }
  std::filesystem::remove("out/tests/nda_save_test.cereal");
  fishbait::CerealSave("out/tests/nda_save_test.cereal", &save_matrix);

  nda::array<int, dense_col_t> load_matrix({1, 1});
  fishbait::Cereal(fishbait::FileAction::Load, "out/tests/nda_save_test.cereal",
                   &load_matrix);
  REQUIRE(load_matrix.rows() == n);
  REQUIRE(load_matrix.columns() == m);
  for (uint32_t i = 0; i < n; ++i) {
    for (uint32_t j = 0; j < m; ++j) {
      REQUIRE(load_matrix(i, j) == (int)(i + j));
    }
  }
  REQUIRE(load_matrix == save_matrix);
}  // TEST_CASE "Save and load nda test"
