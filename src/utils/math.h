// Copyright 2021 Marzuk Rashid

#ifndef SRC_UTILS_MATH_H_
#define SRC_UTILS_MATH_H_

#include <algorithm>
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

/* @brief Population mean */
template <typename ArrayT>
double Mean(const ArrayT& data) {
  double sum = std::accumulate(data.begin(), data.end(), 0.0);
  return sum / data.size();
}

/* @brief Population standard deviation */
template <typename ArrayT>
double Std(const ArrayT& data, double mean) {
  double sum = 0;
  for (auto it = data.begin(); it != data.end(); ++it) {
    double diff = (*it - mean);
    sum += diff * diff;
  }
  return std::sqrt(sum / data.size());
}

/* @brief 95% confidence interval of normally distributed data */
template <typename ArrayT>
double CI95(const ArrayT& means, double std) {
  return 1.96 * std / std::sqrt(means.size());
}

/* @brief Normalize an array of probabilities. */
template <typename ArrayT>
void Normalize(ArrayT& data) {
  using T = typename ArrayT::value_type;
  T sum = std::accumulate(data.begin(), data.end(), T{0});
  std::for_each(data.begin(), data.end(), [=](T& item) {
    item /= sum;
  });
}

}  // namespace fishbait

#endif  // SRC_UTILS_MATH_H_
