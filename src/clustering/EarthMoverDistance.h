#ifndef FISHBAIT_CLUSTERING_EARTHMOVERDISTANCE
#define FISHBAIT_CLUSTERING_EARTHMOVERDISTANCE

#include <stdint.h>

#include <cassert>
#include <cmath>

#include "clustering/Matrix.h"

template <typename T, typename U>
class EarthMoverDistance {
  public:
    static double Compute(typename Matrix<T>::Row p, 
                          typename Matrix<U>::Row q) {
      // The two distributions need to have the same number of buckets
      assert(p.length == q.length);

      double prev = 0;
      double sum = 0;
      for (uint8_t i = 1; i < p.length + 1; ++i) {
        // all flops have area of 1081
        T p_i = p.start[i-1];
        T q_i = q.start[i-1];
        double nxt = p_i + prev - q_i;
        sum += std::abs(nxt);
        prev = nxt;
      }
      return sum;
    }
};

#endif