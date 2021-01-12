// Copyright 2021 Marzuk Rashid

#ifndef SRC_UTILS_MATRIX_H_
#define SRC_UTILS_MATRIX_H_

#include <stdint.h>

#include <cassert>
#include <vector>
#include <algorithm>
#include <functional>

#include "utils/VectorView.h"

namespace utils {

template <typename T>
class Matrix {
 public:
  Matrix(uint32_t n, uint32_t m, T filler = 0)
      : n_(n), m_(m), data_(n*m, filler) {}
  Matrix(const Matrix<T>& other)
      : n_(other.n_), m_(other.m_), data_(other.data_) {}
  ~Matrix() {}
  Matrix<T>& operator=(const Matrix<T>& other) {
    n_ = other.n_;
    m_ = other.m_;
    data_ = std::vector<T>(other.data_);
    return *this;
  }

  T& Access(uint32_t i, uint32_t j) {
    assert(i < n_);
    assert(j < m_);
    return data_[i*m_ + j];
  }
  T& operator()(uint32_t i, uint32_t j) { return Access(i, j); }
  const T& operator()(uint32_t i, uint32_t j) const { return Access(i, j); }

  VectorView<T> operator()(uint32_t i) const {
    assert(i < n_);
    return VectorView<T>(&data_[i*m_], m_);
  }

  bool operator==(const Matrix<T>& other) const {
    return data_ == other.data_;
  }

  template <typename U>
  void SetRow(uint32_t i, VectorView<U> r) {
    assert(i < n_);
    assert(r.n() == m_);
    std::copy(r.begin(), r.end(), &Access(i, 0));
  }

  template <typename U>
  void AddToRow(uint32_t i, VectorView<U> r) {
    assert(i < n_);
    assert(r.n() == m_);
    T* row_begin = &Access(i, 0);
    std::transform(r.begin(), r.end(), row_begin, row_begin,
        [](U a, T b) -> T { return b + a; });
  }

  template <typename U>
  void SubtractFromRow(uint32_t i, VectorView<U> r) {
    assert(i < n_);
    assert(r.n() == m_);
    T* row_begin = *Access(i, 0);
    std::transform(r.begin(), r.end(), row_begin, row_begin,
        [](U a, T b) -> T { return b - a; });
  }

  template <typename U>
  void Divide(VectorView<U> a) {
    assert(a.n() == n_);
    for (uint32_t i = 0; i < n_; ++i) {
      T* row_begin = &Access(i, 0);
      U divisor = a(i);
      std::for_each(row_begin, row_begin+m_, [&divisor](T& elem) {
        elem /= divisor;
      });
    }
  }

  void Fill(T filler) {
    std::fill(data_.begin(), data_.end(), filler);
  }

  uint32_t n() const { return n_; }
  uint32_t m() const { return m_; }

 private:
  const uint32_t n_;  // rows
  const uint32_t m_;  // cols
  std::vector<T> data_;
};  // Matrix

}  // namespace utils

#endif  // SRC_UTILS_MATRIX_H_