// Copyright 2021 Marzuk Rashid

#ifndef SRC_UTILS_COMBINATION_MATRIX_H_
#define SRC_UTILS_COMBINATION_MATRIX_H_

#include <stdint.h>

#include <cassert>
#include <vector>
#include <iostream>

namespace utils {

template <typename T>
class CombinationMatrix {
 public:
  explicit CombinationMatrix(uint32_t n, T filler = 0)
      : n_(n), data_((n*n-n)/2, filler) {}
  ~CombinationMatrix() {}

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

}  // namespace utils

#endif  // SRC_UTILS_COMBINATION_MATRIX_H_
