// Copyright 2021 Marzuk Rashid

#ifndef SRC_UTILS_TIMER_H_
#define SRC_UTILS_TIMER_H_

#include <chrono>  // NOLINT(build/c++11)
#include <iostream>

namespace utils {

class Timer {
 public:
  Timer() : start_(std::chrono::high_resolution_clock::now()) {}

  void StopAndReset(bool log = false) {
    using std::chrono::high_resolution_clock;
    using std::chrono::duration_cast;
    using std::chrono::milliseconds;

    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(stop - start_);
    if (log) {
      std::cout << "Timer: " << duration.count() / 1000.0 << "s" << std::endl;
    }
    start_ = high_resolution_clock::now();
  }

 private:
  std::chrono::time_point<std::chrono::high_resolution_clock> start_;
};  // Timer

}  // namespace utils

#endif  // SRC_UTILS_TIMER_H_
