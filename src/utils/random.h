// Copyright 2021 Marzuk Rashid

#ifndef SRC_UTILS_RANDOM_H_
#define SRC_UTILS_RANDOM_H_

#include <random>

namespace utils {

class Random {
 public:
  class Seed {
   public:
    Seed() : seed_(GenerateSeed()) {}
    explicit Seed(const uint32_t seed) : seed_(seed) {}
    uint32_t operator()() {
      return seed_;
    }
    static uint32_t GenerateSeed() {
      std::random_device dev;
      return dev();
    }
   private:
    uint32_t seed_;
  };  // Seed

  Random() : rng_(Seed::GenerateSeed()) {}
  explicit Random(Seed seed) : rng_(seed()) {}
  std::mt19937& operator()() {
    return rng_;
  }

 private:
  std::mt19937 rng_;
};  // Random

}  // namespace utils

#endif  // SRC_UTILS_RANDOM_H_
