#include <algorithm>
#include <vector>

#include "catch2/catch.hpp"
#include "utils/loop_iterator.h"

TEST_CASE("LoopIterator 100 in order", "[utils][loopiterator]") {
  std::vector<uint8_t> visited(100, 0);
  uint8_t start = 0;
  uint8_t end = 100;
  std::for_each(fishbait::LoopIterator(start), fishbait::LoopIterator(end),
      [&](uint8_t i) {
        visited[i] += 1;
      });

  for (uint8_t i = 0; i < 100; ++i) {
    REQUIRE(visited[i] == 1);
  }
}  // TEST_CASE("LoopIterator 100 in order", "[utils][loopiterator]")

TEST_CASE("LoopIterator 10000 random start", "[utils][loopiterator]") {
  std::vector<uint8_t> visited(10000, 0);
  uint16_t start = 4837;
  uint16_t end = 14837;
  std::for_each(fishbait::LoopIterator(start), fishbait::LoopIterator(end),
      [&](uint16_t i) {
        visited[i - start] += 1;
      });

  for (uint16_t i = 0; i < 10000; ++i) {
    REQUIRE(visited[i] == 1);
  }
}  // TEST_CASE("LoopIterator 10000 random start", "[utils][loopiterator]")
