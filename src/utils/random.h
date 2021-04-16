// Copyright 2021 Marzuk Rashid

#ifndef SRC_UTILS_RANDOM_H_
#define SRC_UTILS_RANDOM_H_

#include <random>

namespace utils {

class Random {
 public:
  explicit Random(int32_t seed = -1) : rng_(UnsignedSeed(seed)) {}
  std::mt19937& operator()() {
    return rng_;
  }

 private:
  uint16_t UnsignedSeed(int32_t seed = -1) {
    if (seed < 0) {
      std::random_device dev;
      seed = dev();
    }
    uint16_t unsigned_seed = seed;
    return unsigned_seed;
  }
  std::mt19937 rng_;
};  // Random

}  // namespace utils

#endif  // SRC_UTILS_RANDOM_H_
