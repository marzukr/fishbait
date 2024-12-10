// Copyright 2021 Marzuk Rashid

#include <cstdint>
#include <numeric>
#include <sstream>
#include <vector>

#include "array/array.h"
#include "array/matrix.h"
#include "catch2/catch.hpp"

TEST_CASE("Test array", "[external][array]") {
  const int points_n = 1000;
  const int points_m = 50;
  const int points_val = 50;

  const int c_points_n = 200;
  const int c_points_m = 1000;
  const double c_points_val = 4.75;

  nda::matrix<int> points({points_n, points_m}, points_val);
  REQUIRE(points.size() == points_n * points_m);
  const nda::matrix<double> const_points({c_points_n, c_points_m},
                                         c_points_val);

  SECTION("Verify that modifying values works") {
    const int points_i_new = 475;
    const int points_j_new = 30;
    const int points_val_new = 500;
    points(points_i_new, points_j_new) = points_val_new;

    SECTION("Verify SetRow") {
      const double new_row_val = 6.99;
      const int row_number = 578;

      std::vector<double> row(points_m, new_row_val);
      points(row_number, nda::all).copy_elems(nda::vector_ref<double>{row});

      for (nda::index_t i = 0; i < points.rows(); ++i) {
        for (nda::index_t j = 0; j < points.columns(); ++j) {
          if (i == points_i_new && j == points_j_new) {
            REQUIRE(points(i, j) == points_val_new);
          } else if (i == row_number) {
            REQUIRE(points(i, j) == (int) new_row_val);
          } else {
            REQUIRE(points(i, j) == points_val);
          }
        }
      }
    }  // SECTION("Verify SetRow")
    SECTION("Verify AddToRow") {
      const double add_val = 6.99;
      const int row_number = 0;

      std::vector<double> row(points_m, add_val);
      points(row_number, nda::all) += nda::vector_ref<double>{row};

      for (nda::index_t i = 0; i < points.rows(); ++i) {
        for (nda::index_t j = 0; j < points.columns(); ++j) {
          if (i == points_i_new && j == points_j_new) {
            REQUIRE(points(i, j) == points_val_new);
          } else if (i == row_number) {
            REQUIRE(points(i, j) == points_val + static_cast<int>(add_val));
          } else {
            REQUIRE(points(i, j) == points_val);
          }
        }
      }
    }  // SECTION("Verify AddToRow")
    SECTION("Verify SubtractFromRow") {
      const int subtract_val = 101;
      const int row_number = points_n - 1;

      std::vector<double> row(points_m, subtract_val);
      points(row_number, nda::all) -= nda::vector_ref<double>{row};

      for (nda::index_t i = 0; i < points.rows(); ++i) {
        for (nda::index_t j = 0; j < points.columns(); ++j) {
          if (i == points_i_new && j == points_j_new) {
            REQUIRE(points(i, j) == points_val_new);
          } else if (i == row_number) {
            REQUIRE(points(i, j) == points_val - subtract_val);
          } else {
            REQUIRE(points(i, j) == points_val);
          }
        }
      }
    }  // SECTION("Verify SubtractFromRow")
    SECTION("Verify iterators") {
      nda::matrix_ref<int> points_ref = points.ref();
      int sum = 0;
      int its = 0;
      for (auto it = points_ref.begin(); it != points_ref.end(); ++it) {
        sum += *it;
        ++its;
        REQUIRE(its <= points_m * points_n);
      }
      REQUIRE(its == points_m * points_n);
      REQUIRE(sum == points_n * points_m * points_val + points_val_new -
              points_val);
    }  // SECTION("Verify iterators")
  }  // SECTION("Verify that modifying values works")

  SECTION("Verify iterators on const array") {
    nda::matrix_ref<const double> c_points_ref = const_points.ref();
    double sum = 0;
    int its = 0;
    for (auto it = c_points_ref.begin(); it != c_points_ref.end(); ++it) {
      sum += *it;
      REQUIRE(*it == c_points_val);
      ++its;
      REQUIRE(its <= c_points_m * c_points_n);
    }
    REQUIRE(its == c_points_m * c_points_n);
    REQUIRE(sum == c_points_n * c_points_m * c_points_val);
  }  // SECTION("Verify iterators on const array")
}  // TEST_CASE("Test array", "[external][array]")

TEST_CASE("Test array print", "[external][array]") {
  nda::matrix<int> test_mat({5, 5});
  std::iota(test_mat.begin(), test_mat.end(), 1);
  std::stringstream ss;
  ss << test_mat;
  REQUIRE(ss.str() == "1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, "
                      "17, 18, 19, 20, 21, 22, 23, 24, 25, ");
}  // TEST_CASE("Test array print", "[external][array]")

TEST_CASE("Test vector_ref with heap array", "[external][array]") {
  const int array_size = 100;
  const int array_filler = 4;

  std::unique_ptr<int[]> int_array(new int[array_size]);
  for (int i = 0; i < array_size; ++i) {
    int_array[i] = array_filler;
  }
  nda::vector_ref<int> int_vv{int_array.get(), array_size};

  SECTION("Verify that n is correct") {
    REQUIRE(int_vv.width() == array_size);
  }
  SECTION("Verify that the () operator works") {
    for (uint32_t i = 0; i < int_vv.width(); ++i) {
      REQUIRE(int_vv(i) == array_filler);
    }
  }
  SECTION("Verify that the iterator works") {
    for (auto it = int_vv.begin(); it != int_vv.end(); ++it) {
      REQUIRE(*it == array_filler);
    }
  }
}  // TEST_CASE("Test vector_ref with heap array", "[external][array]")

TEST_CASE("Test vector_ref with vector", "[external][array]") {
  const int array_size = 100;
  const double array_filler = 6.78;

  std::vector<double> double_vector(array_size, array_filler);
  nda::vector_ref<double> double_vv{double_vector};

  SECTION("Verify that n is correct") {
    REQUIRE(double_vv.width() == array_size);
  }
  SECTION("Verify that the () operator works") {
    for (uint32_t i = 0; i < double_vv.width(); ++i) {
      REQUIRE(double_vv(i) == array_filler);
    }
  }
  SECTION("Verify that the iterator works") {
    for (auto it = double_vv.begin(); it != double_vv.end(); ++it) {
      REQUIRE(*it == array_filler);
    }
  }
}  // TEST_CASE("Test vector_ref with vector", "[external][array]")

TEST_CASE("Test vector_ref with array", "[external][array]") {
  const std::size_t array_size = 10;
  const std::array<double, array_size> arr = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

  nda::const_vector_ref<double> double_vv{arr.data(), arr.size()};

  SECTION("Verify that n is correct") {
    REQUIRE(double_vv.width() == array_size);
  }
  SECTION("Verify that the () operator works") {
    for (uint32_t i = 0; i < double_vv.width(); ++i) {
      REQUIRE(double_vv(i) == i);
    }
  }
  SECTION("Verify that the iterator works") {
    uint32_t i = 0;
    for (auto it = double_vv.begin(); it != double_vv.end(); ++it) {
      REQUIRE(*it == i);
      ++i;
    }
  }
}  // TEST_CASE("Test vector_ref with array", "[external][array]")

TEST_CASE("Test vector_ref operators on different sizes", "[external][array]") {
  const std::size_t array_size_1 = 10;
  const std::array<double, array_size_1> arr_1 = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  nda::const_vector_ref<double> arr_1_view{arr_1.data(), arr_1.size()};

  const std::size_t array_size_2 = 5;
  const std::array<double, array_size_2> arr_2 = {0, 1, 2, 3, 4};
  nda::const_vector_ref<double> arr_2_view{arr_2.data(), arr_2.size()};

  REQUIRE(arr_2_view < arr_1_view);
  REQUIRE(!(arr_1_view < arr_2_view));
  REQUIRE(!(arr_1_view == arr_2_view));
}  // TEST_CASE "Test vector_ref operators on different sizes"

TEST_CASE("Test vector_ref operator same size arrays different contents",
          "[external][array]") {
  const std::size_t array_size_1 = 10;
  const std::array<double, array_size_1> arr_1 = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  nda::const_vector_ref<double> arr_1_view{arr_1.data(), arr_1.size()};

  const std::size_t array_size_2 = 10;
  const std::array<double, array_size_2> arr_2 = {1, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  nda::const_vector_ref<double> arr_2_view{arr_2.data(), arr_2.size()};

  REQUIRE(arr_1_view < arr_2_view);
  REQUIRE(!(arr_2_view < arr_1_view));
  REQUIRE(!(arr_2_view == arr_1_view));
}  // TEST_CASE "Test vector_ref operator same size arrays different contents"

TEST_CASE("Test vector_ref operator same size arrays same contents",
          "[external][array]") {
  const std::size_t array_size_1 = 10;
  const std::array<double, array_size_1> arr_1 = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  nda::const_vector_ref<double> arr_1_view{arr_1.data(), arr_1.size()};

  const std::size_t array_size_2 = 10;
  const std::array<double, array_size_2> arr_2 = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  nda::const_vector_ref<double> arr_2_view{arr_2.data(), arr_2.size()};

  REQUIRE(!(arr_1_view < arr_2_view));
  REQUIRE(!(arr_2_view < arr_1_view));
  REQUIRE(arr_2_view == arr_1_view);
}  // TEST_CASE "Test vector_ref operator same size arrays same contents"

TEST_CASE("vector_ref print operator", "[external][array]") {
  const std::size_t array_size_1 = 10;
  const std::array<double, array_size_1> arr_1 = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  nda::const_vector_ref<double> arr_1_view{arr_1};
  std::stringstream ss;
  ss << arr_1_view;
  REQUIRE(ss.str() == "0, 1, 2, 3, 4, 5, 6, 7, 8, 9, ");
}  // TEST_CASE "vector_ref print operator"
