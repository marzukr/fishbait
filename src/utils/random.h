// Copyright 2021 Marzuk Rashid

#ifndef SRC_UTILS_RANDOM_H_
#define SRC_UTILS_RANDOM_H_

#include <random>

#include "utils/cereal.h"

namespace fishbait {

class Random {
 public:
  class Seed {
   public:
    Seed() : seed_(GenerateSeed()) {}
    explicit Seed(const uint32_t seed) : seed_(seed) {}
    Seed(const Seed& other) = default;
    Seed& operator=(const Seed& other) = default;
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
  Random(const Random& other) = default;
  Random& operator=(const Random& other) = default;
  std::mt19937& operator()() { return rng_; }
  void seed(Seed seed) { rng_.seed(seed()); }

  /* @brief Cereal function. */
  template<class Archive>
  void serialize(Archive& archive) {
    archive(rng_);
  }

 private:
  std::mt19937 rng_;
};  // Random

}  // namespace fishbait

#endif  // SRC_UTILS_RANDOM_H_
