// Copyright 2021 Marzuk Rashid

#ifndef SRC_CLUSTERING_SYMMETRICMATRIX_H_
#define SRC_CLUSTERING_SYMMETRICMATRIX_H_

#include <stdint.h>

#include <cassert>

namespace clustering {

template <typename T>
class SymmetricMatrix {
 public:
  explicit SymmetricMatrix(uint32_t n) : n_(n) {
    data_ = new T[(n*n-n)/2];
  }
  ~SymmetricMatrix() {
    delete[] data_;
  }

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
  T* data_;
};

}  // namespace clustering

#endif  // SRC_CLUSTERING_SYMMETRICMATRIX_H_
