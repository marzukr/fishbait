// Copyright 2021 Marzuk Rashid

#ifndef SRC_UTILS_MATRIX_H_
#define SRC_UTILS_MATRIX_H_

#include <stdint.h>

#include <cassert>
#include <vector>
#include <algorithm>
#include <functional>
#include <iostream>
#include <iterator>

#include "utils/vector_view.h"

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

  const T& Access(uint32_t i, uint32_t j) const {
    assert(i < n_);
    assert(j < m_);
    return data_[i*m_ + j];
  }
  T& operator()(uint32_t i, uint32_t j) {
    return const_cast<T&>(const_cast<const Matrix<T>*>(this)->Access(i, j));
  }
  const T& operator()(uint32_t i, uint32_t j) const { return Access(i, j); }

  VectorView<T> operator()(uint32_t i) const {
    assert(i < n_);
    return VectorView<T>(&data_[i*m_], m_);
  }

  bool operator==(const Matrix<T>& other) const {
    return n_ == other.n_ && m_ == other.m_ && data_ == other.data_;
  }

  template <typename U>
  void SetRow(uint32_t i, VectorView<U> r) {
    assert(i < n_);
    assert(r.n() == m_);
    std::copy(r.begin(), r.end(), &(*this)(i, 0));
  }

  template <typename U>
  void AddToRow(uint32_t i, VectorView<U> r) {
    assert(i < n_);
    assert(r.n() == m_);
    T* row_begin = &(*this)(i, 0);
    std::transform(r.begin(), r.end(), row_begin, row_begin,
        [](U a, T b) -> T { return b + a; });
  }

  template <typename U>
  void SubtractFromRow(uint32_t i, VectorView<U> r) {
    assert(i < n_);
    assert(r.n() == m_);
    T* row_begin = &(*this)(i, 0);
    std::transform(r.begin(), r.end(), row_begin, row_begin,
        [](U a, T b) -> T { return b - a; });
  }

  template <typename U>
  void Divide(VectorView<U> a) {
    assert(a.n() == n_);
    for (uint32_t i = 0; i < n_; ++i) {
      T* row_begin = &(*this)(i, 0);
      U divisor = a(i);
      assert(divisor != 0);
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

  template <class Archive>
  void serialize(Archive& ar) {  // NOLINT(runtime/references)
    ar(n_, m_, data_);
  }

  friend std::ostream& operator<<(std::ostream& os, const Matrix<T>& mx) {
    for (uint32_t i = 0; i < mx.n_; ++i) {
      os << mx(i, 0);
      for (uint32_t j = 1; j < mx.m_; ++j) {
        os << "," << mx(i, j);
      }
      os << std::endl;
    }
    return os;
  }

  using data_iter = typename std::vector<T>::iterator;
  using const_data_iter = typename std::vector<T>::const_iterator;
  using data_iter_diff = typename std::iterator_traits<data_iter>
                                     ::difference_type;

  data_iter begin() { return data_.begin(); }
  const_data_iter begin() const { return data_.begin(); }
  data_iter end() { return data_.end(); }
  const_data_iter end() const { return data_.end(); }

  data_iter_diff pos(const_data_iter it) const { return it - begin(); }
  data_iter_diff pos(const T* it) const { return it - data_.data(); }

  uint32_t row(const_data_iter it) const { return pos(it) / m_; }
  uint32_t row(const T* it) const { return pos(it) / m_; }
  uint32_t row(data_iter_diff pos) const { return pos / m_; }

  uint32_t col(const_data_iter it) const { return pos(it) % m_; }
  uint32_t col(const T* it) const { return pos(it) % m_; }
  uint32_t col(data_iter_diff pos) const { return pos % m_; }

 private:
  uint32_t n_;  // rows
  uint32_t m_;  // cols
  std::vector<T> data_;
};  // Matrix

}  // namespace utils

#endif  // SRC_UTILS_MATRIX_H_
