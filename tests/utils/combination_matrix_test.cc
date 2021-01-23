// Copyright 2021 Marzuk Rashid

#include "catch2/catch.hpp"

#include "utils/combination_matrix.h"

TEST_CASE("CombinationMatrix", "[utils][combinationmatrix]") {
  const int n_items = 50;
  const double cm_filler = 5;

  utils::CombinationMatrix<double> cm(n_items, cm_filler);

  REQUIRE(cm.n() == n_items);

  for (int i = 0; i < cm.n(); ++i) {
    for (int j = 0; j < cm.n(); ++j) {
      if (i == j) {
        continue;
      }
      double id = i, jd = j;
      cm(i, j) = id / jd;
    }
  }

  for (int i = 0; i < cm.n(); ++i) {
    for (int j = i + 1; j < cm.n(); ++j) {
      REQUIRE(cm(i, j) == cm(j, i));
    }
  }
}  // TEST_CASE("CombinationMatrix", "[utils][combinationmatrix]")
