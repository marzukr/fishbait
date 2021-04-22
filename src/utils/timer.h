// Copyright 2021 Marzuk Rashid

#ifndef SRC_UTILS_TIMER_H_
#define SRC_UTILS_TIMER_H_

#include <chrono>  // NOLINT(build/c++11)
#include <iostream>
#include <string>

namespace utils {

class Timer {
 public:
  Timer() : start_(std::chrono::high_resolution_clock::now()) {}

  int64_t StopAndReset() {
    using std::chrono::high_resolution_clock;
    using std::chrono::duration_cast;
    using std::chrono::milliseconds;

    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(stop - start_);
    start_ = high_resolution_clock::now();
    return duration.count();
  }

  void StopAndReset(const std::string& message) {
    std::cout << message << ": " << StopAndReset() / 1000.0 << "s"
              << std::endl;
  }

 private:
  std::chrono::time_point<std::chrono::high_resolution_clock> start_;
};  // Timer

}  // namespace utils

#endif  // SRC_UTILS_TIMER_H_
