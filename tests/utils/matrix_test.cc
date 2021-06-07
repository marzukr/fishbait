// Copyright 2021 Marzuk Rashid

#include <cstdint>
#include <numeric>
#include <sstream>
#include <vector>

#include "catch2/catch.hpp"
#include "utils/matrix.h"
#include "utils/vector_view.h"

TEST_CASE("Test Matrix", "[utils][matrix]") {
  const int points_n = 1000;
  const int points_m = 50;
  const int points_val = 50;

  const int c_points_n = 200;
  const int c_points_m = 1000;
  const double c_points_val = 4.75;

  utils::Matrix<int> points(points_n, points_m, points_val);
  const utils::Matrix<double> const_points(c_points_n, c_points_m,
                                           c_points_val);

  // Main Matrix Tests
  SECTION("Verify that the dimensions are correct") {
    REQUIRE(points.n() == points_n);
    REQUIRE(points.m() == points_m);
    REQUIRE(const_points.n() == c_points_n);
    REQUIRE(const_points.m() == c_points_m);
  }  // SECTION("Verify that the dimensions are correct")
  SECTION("Verify that modifying values works") {
    const int points_i_new = 475;
    const int points_j_new = 30;
    const int points_val_new = 500;
    points(points_i_new, points_j_new) = points_val_new;

    SECTION("Verify that Access and () work on a Matrix") {
      for (uint64_t i = 0; i < points.n(); ++i) {
        for (uint64_t j = 0; j < points.m(); ++j) {
          if (i == points_i_new && j == points_j_new) {
            REQUIRE(points.Access(i, j) == points_val_new);
            REQUIRE(points(i, j) == points_val_new);
          } else {
            REQUIRE(points.Access(i, j) == points_val);
            REQUIRE(points(i, j) == points_val);
          }
        }
      }
    }  // SECTION("Verify that Access and () work on a Matrix")
    SECTION("Verify that row access works with VectorView") {
      utils::VectorView<int> v = points(points_i_new);
      for (uint64_t j = 0; j < points.m(); ++j) {
        if (j == points_j_new) {
          REQUIRE(v(j) == points_val_new);
        } else {
          REQUIRE(v(j) == points_val);
        }
      }
    }  // SECTION("Verify that row access works with VectorView")
    SECTION("Verify that equal matrixes are equal") {
      utils::Matrix<int> temp_points(points_n, points_m, points_val);
      temp_points(points_i_new, points_j_new) = points_val_new;
      REQUIRE(points == temp_points);
      REQUIRE(temp_points == points);
    }  // SECTION("Verify that equal matrixes are equal")
    SECTION("Verify that unequal matrixes are not equal") {
      utils::Matrix<int> temp_points(points_m, points_n, points_val);
      temp_points(points_j_new, points_i_new) = points_val_new;
      REQUIRE(!(points == temp_points));
      REQUIRE(!(temp_points == points));
    }  // SECTION("Verify that unequal matrixes are not equal")
    SECTION("Verify SetRow") {
      const double new_row_val = 6.99;
      const int row_number = 578;

      std::vector<double> row(points_m, new_row_val);
      points.SetRow(row_number, utils::VectorView(row));

      for (uint64_t i = 0; i < points.n(); ++i) {
        for (uint64_t j = 0; j < points.m(); ++j) {
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
      points.AddToRow(row_number, utils::VectorView(row));

      for (uint64_t i = 0; i < points.n(); ++i) {
        for (uint64_t j = 0; j < points.m(); ++j) {
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
      points.SubtractFromRow(row_number, utils::VectorView(row));

      for (uint64_t i = 0; i < points.n(); ++i) {
        for (uint64_t j = 0; j < points.m(); ++j) {
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
    SECTION("Verify Divide") {
      const double divide_val = 6;
      const double divide_val_2 = 10;
      const int divide_row = 80;

      std::vector<double> row(points_n, divide_val);
      row[divide_row] = divide_val_2;
      points.Divide(utils::VectorView(row));

      for (uint64_t i = 0; i < points.n(); ++i) {
        for (uint64_t j = 0; j < points.m(); ++j) {
          if (i == points_i_new && j == points_j_new) {
            REQUIRE(points(i, j) == (int)(points_val_new / divide_val));
          } else if (i == divide_row) {
            REQUIRE(points(i, j) == (int)(points_val / divide_val_2));
          } else {
            REQUIRE(points(i, j) == (int)(points_val / divide_val));
          }
        }
      }
    }  // SECTION("Verify Divide")
    SECTION("Verify Fill") {
      const int fill_val = 256;

      points.Fill(fill_val);

      for (uint64_t i = 0; i < points.n(); ++i) {
        for (uint64_t j = 0; j < points.m(); ++j) {
          REQUIRE(points(i, j) == fill_val);
        }
      }
    }  // SECTION("Verify Fill")
    SECTION("Verify iterators") {
      for (auto it = points.begin(); it != points.end(); ++it) {
        auto pos1 = points.pos(it);
        auto pos2 = points.pos(&*it);
        REQUIRE(pos1 == pos2);

        uint64_t row1 = points.row(it);
        uint64_t row2 = points.row(pos1);
        uint64_t row3 = points.row(&*it);
        REQUIRE(row1 == row2);
        REQUIRE(row1 == row3);

        uint64_t col1 = points.col(it);
        uint64_t col2 = points.col(pos1);
        uint64_t col3 = points.col(&*it);
        REQUIRE(col1 == col2);
        REQUIRE(col1 == col3);

        if (row1 == points_i_new && col1 == points_j_new) {
          REQUIRE(*it == points_val_new);
        } else {
          REQUIRE(*it == points_val);
        }
      }  // for it
    }  // SECTION("Verify iterators")
  }  // SECTION("Verify that modifying values works")

  // Const Matrix Tests
  SECTION("Verify () and Access on const Matrix") {
    for (uint64_t i = 0; i < const_points.n(); ++i) {
      for (uint64_t j = 0; j < const_points.m(); ++j) {
        REQUIRE(const_points.Access(i, j) == c_points_val);
        REQUIRE(const_points(i, j) == c_points_val);
      }
    }
  }  // SECTION("Verify () and Access on const Matrix")
  SECTION("Verify that row access works with VectorView on const Matrix") {
    const int row_number = 178;
    utils::VectorView<double> v = const_points(row_number);
    for (const double* it = v.begin(); it < v.end(); ++it) {
      REQUIRE(*it == c_points_val);
    }
  }  // SECTION("Verify that row access works with VectorView on const Matrix")
  SECTION("Verify that equal const Matrixes are equal") {
    utils::Matrix<double> temp_points(c_points_n, c_points_m, c_points_val);
    REQUIRE(const_points == temp_points);
    REQUIRE(temp_points == const_points);
  }  // SECTION("Verify that equal const Matrixes are equal")
  SECTION("Verify that unequal const Matrixes are not equal") {
    utils::Matrix<double> temp_points(c_points_m, c_points_n, c_points_val);
    REQUIRE(!(const_points == temp_points));
    REQUIRE(!(temp_points == const_points));
  }  // SECTION("Verify that unequal const Matrixes are not equal")
  SECTION("Verify iterators on const Matrix") {
    for (auto it = const_points.begin(); it != const_points.end(); ++it) {
      auto pos1 = const_points.pos(it);
      auto pos2 = const_points.pos(&*it);
      REQUIRE(pos1 == pos2);

      uint64_t row1 = const_points.row(it);
      uint64_t row2 = const_points.row(pos1);
      uint64_t row3 = const_points.row(&*it);
      REQUIRE(row1 == row2);
      REQUIRE(row1 == row3);

      uint64_t col1 = const_points.col(it);
      uint64_t col2 = const_points.col(pos1);
      uint64_t col3 = const_points.col(&*it);
      REQUIRE(col1 == col2);
      REQUIRE(col1 == col3);

      REQUIRE(*it == c_points_val);
    }  // for it
  }  // SECTION("Verify iterators on const Matrix")

  // Copy Constructor Test
  SECTION("Copy constructor") {
    const int change_val = 7;

    utils::Matrix<int> points2(points);
    points2.Fill(change_val);

    REQUIRE(points2.n() == points.n());
    REQUIRE(points2.m() == points.m());

    for (uint64_t i = 0; i < points.n(); ++i) {
      for (uint64_t j = 0; j < points.m(); ++j) {
        REQUIRE(points(i, j) == points_val);
        REQUIRE(points2(i, j) == change_val);
      }
    }
  }  // SECTION("Copy constructor")

  // Assignment Operator Test
  SECTION("Assignment operator") {
    const int change_val = 7;

    utils::Matrix<int> points2(1, 1);
    points2 = points;
    points2.Fill(change_val);

    REQUIRE(points2.n() == points.n());
    REQUIRE(points2.m() == points.m());

    for (uint64_t i = 0; i < points.n(); ++i) {
      for (uint64_t j = 0; j < points.m(); ++j) {
        REQUIRE(points(i, j) == points_val);
        REQUIRE(points2(i, j) == change_val);
      }
    }
  }  // SECTION("Assignment operator")
}  // TEST_CASE("Test Matrix", "[utils][matrix]")

TEST_CASE("Test Matrix print", "[utils][matrix]") {
  utils::Matrix<int> test_mat(5, 5);
  std::iota(test_mat.begin(), test_mat.end(), 1);
  std::stringstream ss;
  ss << test_mat;
  REQUIRE(ss.str() == "1,2,3,4,5\n"
                      "6,7,8,9,10\n"
                      "11,12,13,14,15\n"
                      "16,17,18,19,20\n"
                      "21,22,23,24,25\n");
}  // TEST_CASE("Test Matrix print", "[utils][matrix]")
