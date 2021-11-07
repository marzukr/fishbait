// Copyright 2021 Marzuk Rashid

#ifndef SRC_UTILS_MATH_H_
#define SRC_UTILS_MATH_H_

#include <numeric>
#include <cmath>

namespace fishbait {

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

template <typename ArrayT>
double Mean(const ArrayT& data) {
  double sum = std::accumulate(data.begin(), data.end(), 0.0);
  return sum / data.size();
}

template <typename ArrayT>
double Std(const ArrayT& data, double mean) {
  double sum = 0;
  for (auto it = data.begin(); it != data.end(); ++it) {
    double diff = (*it - mean);
    sum += diff * diff;
  }
  return std::sqrt(sum / data.size());
}

}  // namespace fishbait

#endif  // SRC_UTILS_MATH_H_
