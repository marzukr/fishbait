// Copyright 2021 Marzuk Rashid

#ifndef SRC_CLUSTERING_ARRAY_H_
#define SRC_CLUSTERING_ARRAY_H_

#include <stdint.h>

#include <cassert>

namespace clustering {

template <typename T>
class Array {
 public:
  explicit Array(uint32_t n) : n_(n) { data_ = new T[n]; }
  ~Array() {
    delete[] data_;
  }

  T& operator()(uint32_t i) {
    assert(i < n_);
    return data_[i];
  }
  const T& operator()(uint32_t i) const {
    assert(i < n_);
    return data_[i];
  }

  void Fill(T filler) {
    for (uint32_t i = 0; i < n_; ++i) {
      data_[i] = filler;
    }
  }

  T sum() const {
    T total = 0;
    for (uint32_t i = 0; i < n_; ++i) {
      total += data_[i];
    }
    return total;
  }

  uint32_t n() const { return n_; }

 private:
  const uint32_t n_;  // rows
  T* data_;
};  // Array

}  // namespace clustering

#endif  // SRC_CLUSTERING_ARRAY_H_
