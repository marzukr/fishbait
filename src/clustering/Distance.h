// Copyright 2021 Marzuk Rashid

#ifndef SRC_CLUSTERING_DISTANCE_H_
#define SRC_CLUSTERING_DISTANCE_H_

#include <stdint.h>

#include <cassert>
#include <cmath>
#include <numeric>
#include <functional>

#include "utils/VectorView.h"

namespace clustering {

template <typename T, typename U>
class EarthMoverDistance {
 public:
  static double Compute(typename utils::VectorView<T> p,
                        typename utils::VectorView<U> q) {
    // The two distributions need to have the same number of buckets
    assert(p.n() == q.n());

    double prev = 0;
    double sum = 0;
    for (uint8_t i = 1; i < p.n() + 1; ++i) {
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
  static double Compute(typename utils::VectorView<T> p,
                        typename utils::VectorView<U> q) {
    // The two distributions need to have the same number of buckets
    assert(p.n() == q.n());

    double squared_dist = std::transform_reduce(p.begin(), p.end(), q.begin(),
        0.0, std::plus<double>(),
        [](T a, U b) -> double { return std::pow(b - a, 2); });

    return std::sqrt(squared_dist);
  }
};

}  // namespace clustering

#endif  // SRC_CLUSTERING_DISTANCE_H_
