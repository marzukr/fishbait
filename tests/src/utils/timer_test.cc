#include <chrono>  // NOLINT(build/c++11)
#include <sstream>
#include <string>
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
  timer.Stop();
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  elapsed = timer.Check<fishbait::Timer::Seconds>();
  REQUIRE(elapsed == Approx(0.1).epsilon(0.75));
  timer.Start();
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  elapsed = timer.Check<fishbait::Timer::Seconds>();
  REQUIRE(elapsed == Approx(0.2).epsilon(0.75));

  timer.Reset();
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  elapsed = timer.Check();
  REQUIRE(elapsed == Approx(100).epsilon(0.75));
  timer.Reset();
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  timer.Stop();
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  elapsed = timer.Check();
  REQUIRE(elapsed == Approx(100).epsilon(0.75));
  timer.Start();
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  elapsed = timer.Check();
  REQUIRE(elapsed == Approx(200).epsilon(0.75));

  timer.Reset();
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  elapsed = timer.Check<fishbait::Timer::Milliseconds>();
  REQUIRE(elapsed == Approx(100).epsilon(0.75));
  timer.Reset();
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  timer.Stop();
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  elapsed = timer.Check<fishbait::Timer::Milliseconds>();
  REQUIRE(elapsed == Approx(100).epsilon(0.75));
  timer.Start();
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  elapsed = timer.Check<fishbait::Timer::Milliseconds>();
  REQUIRE(elapsed == Approx(200).epsilon(0.75));

  timer.Reset();
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  elapsed = timer.Check<fishbait::Timer::Minutes>();
  REQUIRE(elapsed == Approx(0.001666666667).epsilon(0.75));
  timer.Reset();
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  timer.Stop();
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  elapsed = timer.Check<fishbait::Timer::Minutes>();
  REQUIRE(elapsed == Approx(0.001666666667).epsilon(0.75));
  timer.Start();
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  elapsed = timer.Check<fishbait::Timer::Minutes>();
  REQUIRE(elapsed == Approx(0.003333333334).epsilon(0.75));
}  // TEST_CASE "timer check test"

TEST_CASE("timer reset test", "[utils][timer]") {
  fishbait::Timer timer;
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  double elapsed = timer.Reset<fishbait::Timer::Seconds>();
  REQUIRE(elapsed == Approx(0.1).epsilon(0.75));

  timer.Reset();
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  elapsed = timer.Reset();
  REQUIRE(elapsed == Approx(100).epsilon(0.75));

  timer.Reset();
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  elapsed = timer.Reset<fishbait::Timer::Milliseconds>();
  REQUIRE(elapsed == Approx(100).epsilon(0.75));

  timer.Reset();
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  elapsed = timer.Reset<fishbait::Timer::Minutes>();
  REQUIRE(elapsed == Approx(0.001666666667).epsilon(0.75));
}  // TEST_CASE "timer reset test"

TEST_CASE("timer check print test", "[utils][timer]") {
  std::stringstream ss;
  std::stringstream ss2;
  double printed_time;
  fishbait::Timer timer;

  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  timer.Check<fishbait::Timer::Seconds>(ss);
  printed_time = std::stod(ss.str());
  REQUIRE(printed_time == Approx(0.1).epsilon(0.75));
  ss2 << printed_time << " " << "s";
  REQUIRE(ss.str() == ss2.str());

  ss.str("");
  ss2.str("");
  timer.Reset();
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  timer.Check(ss);
  printed_time = std::stod(ss.str());
  REQUIRE(printed_time == Approx(0.1).epsilon(0.75));
  ss2 << printed_time << " " << "s";
  REQUIRE(ss.str() == ss2.str());

  ss.str("");
  ss2.str("");
  timer.Reset();
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  timer.Check<fishbait::Timer::Milliseconds>(ss);
  printed_time = std::stod(ss.str());
  REQUIRE(printed_time == Approx(100).epsilon(0.75));
  ss2 << printed_time << " " << "ms";
  REQUIRE(ss.str() == ss2.str());

  ss.str("");
  ss2.str("");
  timer.Reset();
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  timer.Check<fishbait::Timer::Minutes>(ss);
  printed_time = std::stod(ss.str());
  REQUIRE(printed_time == Approx(0.001666666667).epsilon(0.75));
  ss2 << printed_time << " " << "min";
  REQUIRE(ss.str() == ss2.str());
}  // TEST_CASE "timer check print test"

TEST_CASE("timer reset print test", "[utils][timer]") {
  std::stringstream ss;
  std::stringstream ss2;
  double printed_time;
  fishbait::Timer timer;

  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  timer.Reset<fishbait::Timer::Seconds>(ss);
  printed_time = std::stod(ss.str());
  REQUIRE(printed_time == Approx(0.1).epsilon(0.75));
  ss2 << printed_time << " " << "s";
  REQUIRE(ss.str() == ss2.str());

  ss.str("");
  ss2.str("");
  timer.Reset();
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  timer.Reset(ss);
  printed_time = std::stod(ss.str());
  REQUIRE(printed_time == Approx(0.1).epsilon(0.75));
  ss2 << printed_time << " " << "s";
  REQUIRE(ss.str() == ss2.str());

  ss.str("");
  ss2.str("");
  timer.Reset();
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  timer.Reset<fishbait::Timer::Milliseconds>(ss);
  printed_time = std::stod(ss.str());
  REQUIRE(printed_time == Approx(100).epsilon(0.75));
  ss2 << printed_time << " " << "ms";
  REQUIRE(ss.str() == ss2.str());

  ss.str("");
  ss2.str("");
  timer.Reset();
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  timer.Reset<fishbait::Timer::Minutes>(ss);
  printed_time = std::stod(ss.str());
  REQUIRE(printed_time == Approx(0.001666666667).epsilon(0.75));
  ss2 << printed_time << " " << "min";
  REQUIRE(ss.str() == ss2.str());
}  // TEST_CASE "timer reset print test"
