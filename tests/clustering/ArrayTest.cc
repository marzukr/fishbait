// // Copyright 2021 Marzuk Rashid

// #include <stdint.h>

// #include "Catch2/single_include/catch2/catch.hpp"

// #include "clustering/Array.h"

// TEST_CASE("Test Arrays", "[clustering][array]") {
//   clustering::Array<int> a(100);

//   REQUIRE(a.n() == 100);

//   SECTION("Fill function fills the array with the specified value") {
//     a.Fill(0);

//     for (uint32_t i = 0; i < a.n(); i++) {
//       REQUIRE(a(i) == 0);
//     }
//   }
//   SECTION("Sum computes the correct sum of elements") {
//     a.Fill(3);
//     REQUIRE(a.sum() == 3*a.n());
//   }
//   SECTION("Function call operator gets and sets values of the Array") {
//     for (uint32_t i = 0; i < a.n(); i++) {
//       a(i) = i;
//     }
//     for (uint32_t i = 0; i < a.n(); i++) {
//       REQUIRE(a(i) == i);
//     }
//   }
// }
