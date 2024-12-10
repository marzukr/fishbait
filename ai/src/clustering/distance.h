#ifndef AI_SRC_CLUSTERING_DISTANCE_H_
#define AI_SRC_CLUSTERING_DISTANCE_H_

#include <cassert>
#include <cmath>
#include <cstdint>
#include <functional>
#include <numeric>

#include "array/array.h"

namespace fishbait {

template <typename T, typename U>
class EarthMoverDistance {
 public:
  static double Compute(nda::vector_ref<T> p, nda::vector_ref<U> q) {
    // The two distributions need to have the same number of buckets
    assert(p.width() == q.width());

    double prev = 0;
    double sum = 0;
    for (nda::index_t i = 1; i < p.width() + 1; ++i) {
      // all flops have area of 1081
      T p_i = p(i-1);
      U q_i = q(i-1);
      double nxt = p_i + prev - q_i;
      sum += std::abs(nxt);
      prev = nxt;
    }
    return sum;
  }
};

template <typename T, typename U>
class EuclideanDistance {
 public:
  static double Compute(nda::vector_ref<T> p, nda::vector_ref<U> q) {
    // The two distributions need to have the same number of buckets
    assert(p.width() == q.width());

    double squared_dist = std::transform_reduce(p.base(), p.base() + p.width(),
        q.base(), 0.0, std::plus<double>(),
        [](T a, U b) -> double { return std::pow(b - a, 2); });

    return std::sqrt(squared_dist);
  }
};

}  // namespace fishbait

#endif  // AI_SRC_CLUSTERING_DISTANCE_H_
