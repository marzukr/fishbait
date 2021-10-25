// Copyright 2021 Marzuk Rashid

#include <chrono>  // NOLINT(build/c++11)
#include <thread>  // NOLINT(build/c++11)

#include "catch2/catch.hpp"
#include "utils/timer.h"

TEST_CASE("timer check test", "[utils][timer]") {
  fishbait::Timer timer;
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  double elapsed = timer.Check<fishbait::Timer::Seconds>();
  REQUIRE(elapsed == Approx(0.1).epsilon(0.75));

  timer.Reset();
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  elapsed = timer.Check<fishbait::Timer::Milliseconds>();
  REQUIRE(elapsed == Approx(100).epsilon(0.75));

  timer.Reset();
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  elapsed = timer.Check<fishbait::Timer::Minutes>();
  REQUIRE(elapsed == Approx(0.001666666667).epsilon(0.75));
}  // TEST_CASE "basic timer tests"

TEST_CASE("timer reset test", "[utils][timer]") {
  fishbait::Timer timer;
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  double elapsed = timer.Reset<fishbait::Timer::Seconds>();
  REQUIRE(elapsed == Approx(0.1).epsilon(0.75));

  timer.Reset();
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  elapsed = timer.Reset<fishbait::Timer::Milliseconds>();
  REQUIRE(elapsed == Approx(100).epsilon(0.75));

  timer.Reset();
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  elapsed = timer.Reset<fishbait::Timer::Minutes>();
  REQUIRE(elapsed == Approx(0.001666666667).epsilon(0.75));
}  // TEST_CASE "basic timer tests"
