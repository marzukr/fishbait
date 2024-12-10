#ifndef AI_SRC_UTILS_COMBINATION_MATRIX_H_
#define AI_SRC_UTILS_COMBINATION_MATRIX_H_

#include <cassert>
#include <cstdint>
#include <iostream>
#include <vector>

namespace fishbait {

template <typename T>
class CombinationMatrix {
 public:
  explicit CombinationMatrix(uint32_t n, T filler = 0)
      : n_(n), data_((n*n-n)/2, filler) {}

  CombinationMatrix<T>(const CombinationMatrix<T>& other) = default;
  CombinationMatrix<T>& operator=(const CombinationMatrix<T>& other) = default;

  T& operator()(uint32_t i, uint32_t j) {
    assert(i != j);
    assert(i < n_);
    assert(j < n_);
    if (i > j) {
      uint32_t temp = i;
      i = j;
      j = temp;
    }
    uint32_t idx = i*this->n_ - i*(i+1)/2 + j - i - 1;
    return this->data_[idx];
  }

  uint32_t n() { return n_; }

 private:
  const uint32_t n_;  // side length
  std::vector<T> data_;
};  // CombinationMatrix

}  // namespace fishbait

#endif  // AI_SRC_UTILS_COMBINATION_MATRIX_H_
