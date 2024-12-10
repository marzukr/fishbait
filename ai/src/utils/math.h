#ifndef AI_SRC_UTILS_MATH_H_
#define AI_SRC_UTILS_MATH_H_

#include <algorithm>
#include <cmath>
#include <functional>
#include <numeric>
#include <random>

#include "utils/random.h"

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

/*
  @brief Sample an index given an array of probabilities.

  @param data The list of probabilities to sample from.
  @param rng The random number generator to use to sample.
*/
template <typename ArrayT>
std::size_t Sample(const ArrayT& data, Random& rng) {
  std::uniform_real_distribution<double> sampler(0, 1);
  while (true) {
    double sampled = sampler(rng());
    double bound = 0;

    for (std::size_t i = 0; i < data.size(); ++i) {
      bound += data[i];
      if (sampled < bound) {
        return i;
      }
    }  // for i
  }
}

/*
  @brief Computed the chi-squared test statistic for the given data.

  @param observed The observed frequency of each class.
  @param expected The expected frequency of each class.
*/
template <typename ArrayT1, typename ArrayT2>
double ChiSqTestStat(const ArrayT1& observed, const ArrayT2& expected) {
  using T1 = typename ArrayT1::value_type;
  using T2 = typename ArrayT2::value_type;
  auto chi_transform = [](T1 o, T2 e) {
    double num = o - e;
    return (num * num) / e;
  };
  return std::transform_reduce(observed.begin(), observed.end(),
                               expected.begin(), 0.0, std::plus<>(),
                               chi_transform);
}

}  // namespace fishbait

#endif  // AI_SRC_UTILS_MATH_H_
