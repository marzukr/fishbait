// Copyright 2021 Marzuk Rashid

#ifndef SRC_UTILS_MATH_H_
#define SRC_UTILS_MATH_H_

namespace utils {

constexpr uint32_t N_Choose_K(uint32_t n, uint32_t k) {
  if (k > n) return 0;
  if (k * 2 > n) k = n-k;
  if (k == 0) return 1;

  uint32_t result = n;
  for (uint32_t i = 2; i <= k; ++i) {
    result *= (n-i+1);
    result /= i;
  }
  return result;
}

}  // namespace utils

#endif  // SRC_UTILS_MATH_H_
