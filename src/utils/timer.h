// Copyright 2021 Marzuk Rashid

#ifndef SRC_UTILS_TIMER_H_
#define SRC_UTILS_TIMER_H_

#include <chrono>  // NOLINT(build/c++11)
#include <iostream>
#include <ratio>  // NOLINT(build/c++11)
#include <string_view>
#include <string>

namespace fishbait {

class Timer {
 public:
  using Milliseconds = std::chrono::duration<double, std::milli>;
  using Seconds = std::chrono::duration<double, std::ratio<1>>;
  using Minutes = std::chrono::duration<double, std::ratio<60, 1>>;

  template <typename Unit> static constexpr std::string_view Suffix() {
    if constexpr (std::is_same<Unit, Milliseconds>::value) {
      return "ms";
    } else if (std::is_same<Unit, Seconds>::value) {
      return "s";
    } else if (std::is_same<Unit, Minutes>::value) {
      return "min";
    }
  }

  Timer() : start_(std::chrono::high_resolution_clock::now()) {}

  Timer(const Timer& other) = default;
  Timer& operator=(const Timer& other) = default;
  Timer(Timer&&) = delete;
  Timer& operator=(Timer&&) = delete;

  /* @brief Returns the duration since the timer was last reset. */
  template <typename Unit = Milliseconds>
  double Check() {
    auto stop = std::chrono::high_resolution_clock::now();
    Unit duration = stop - start_;
    return duration.count();
  }

  /* @brief Resets timer and returns the duration since it was last reset. */
  template <typename Unit = Milliseconds>
  double Reset() {
    double ret = Check<Unit>();
    start_ = std::chrono::high_resolution_clock::now();
    return ret;
  }

  /* @brief Resets timer and prints the duration since it was last reset. */
  template <typename Unit = Seconds>
  inline std::ostream& Reset(std::ostream& os) {
    os << Reset<Unit>() << " " << Suffix<Unit>();
    return os;
  }

  /* @brief Prints the duration since the timer was last reset. */
  template <typename Unit = Seconds>
  inline std::ostream& Check(std::ostream& os) {
    os << Check<Unit>() << " " << Suffix<Unit>();
    return os;
  }

 private:
  std::chrono::time_point<std::chrono::high_resolution_clock> start_;
};  // Timer

}  // namespace fishbait

#endif  // SRC_UTILS_TIMER_H_
