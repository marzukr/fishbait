// Copyright 2021 Marzuk Rashid

#ifndef SRC_UTILS_MATRIX_H_
#define SRC_UTILS_MATRIX_H_

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <functional>
#include <iterator>
#include <ostream>
#include <vector>

#include "utils/vector_view.h"

namespace utils {

template <typename T>
class Matrix {
 public:
  Matrix(uint64_t l, uint64_t m, uint64_t n, T filler = 0)
      : l_(l), m_(m), n_(n), data_(l*m*n, filler) {}
  Matrix<T>(const Matrix<T>& other) = default;
  Matrix<T>& operator=(const Matrix<T>& other) = default;

  T& Access(uint64_t i, uint64_t j, uint64_t k) {
    assert(i < l_);
    assert(j < m_);
    assert(k < n_);
    return data_[i*m_*n_ + j*n_ + k];
  }
  const T& ConstAccess(uint64_t i, uint64_t j, uint64_t k) const {
    assert(i < l_);
    assert(j < m_);
    assert(k < n_);
    return data_[i*m_*n_ + j*n_ + k];
  }
  T& operator()(uint64_t i, uint64_t j, uint64_t k) {
    return Access(i, j, k);
  }
  const T& operator()(uint64_t i, uint64_t j, uint64_t k) const {
    return ConstAccess(i, j, k);
  }

  VectorView<T> operator()(uint64_t i, uint64_t j) const {
    assert(i < l_);
    assert(j < m_);
    return VectorView<T>(&data_[i*m_*n_ + j*n_], n_);
  }

  bool operator==(const Matrix<T>& other) const {
    return l_ == other.l_ && m_ == other.m_ && n_ == other.n_ &&
           data_ == other.data_;
  }

  template <typename U>
  void SetRow(uint64_t i, uint64_t j, VectorView<U> r) {
    assert(i < l_);
    assert(j < m_);
    assert(r.n() == n_);
    std::copy(r.begin(), r.end(), &(*this)(i, j, 0));
  }

  template <typename U>
  void AddToRow(uint64_t i, uint64_t j, VectorView<U> r) {
    assert(i < l_);
    assert(j < m_);
    assert(r.n() == n_);
    T* row_begin = &(*this)(i, j, 0);
    std::transform(r.begin(), r.end(), row_begin, row_begin,
        [](U a, T b) -> T { return b + a; });
  }

  template <typename U>
  void SubtractFromRow(uint64_t i, uint64_t j, VectorView<U> r) {
    assert(i < l_);
    assert(j < m_);
    assert(r.n() == n_);
    T* row_begin = &(*this)(i, j, 0);
    std::transform(r.begin(), r.end(), row_begin, row_begin,
        [](U a, T b) -> T { return b - a; });
  }

  template <typename U>
  void DivideEachRow(VectorView<U> a) {
    assert(a.n() == m_);
    for (uint64_t i = 0; i < l_; ++i) {
      for (uint64_t j = 0; j < m_; ++j) {
        T* row_begin = &(*this)(i, j, 0);
        U divisor = a(j);
        assert(divisor != 0);
        std::for_each(row_begin, row_begin+m_, [&divisor](T& elem) {
          elem /= divisor;
        });
      }
    }
  }

  void Fill(T filler) {
    std::fill(data_.begin(), data_.end(), filler);
  }

  uint64_t l() const { return l_; }
  uint64_t m() const { return m_; }
  uint64_t n() const { return n_; }

  template <class Archive>
  void serialize(Archive& ar) {  // NOLINT(runtime/references)
    ar(l_, m_, n_, data_);
  }

  friend std::ostream& operator<<(std::ostream& os, const Matrix<T>& mx) {
    for (uint64_t i = 0; i < mx.l_; ++i) {
      os << "Sheet " << i << ":" << std::endl;
      for (uint64_t j = 0; j < mx.m_; ++j) {
        os << mx(i, j, 0);
        for (uint64_t k = 1; k < mx.n_; ++k) {
          os << "," << mx(i, j, k);
        }
        if (j + 1 < mx.n_) {
          os << std::endl;
        }
      }
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

  uint64_t sheet(const_data_iter it) const { return pos(it) / (m_*n_); }
  uint64_t sheet(const T* it) const { return pos(it) / (m_*n_); }
  uint64_t sheet(data_iter_diff pos) const { return pos / (m_*n_); }

  uint64_t row(const_data_iter it) const { return pos(it) / n_; }
  uint64_t row(const T* it) const { return pos(it) / n_; }
  uint64_t row(data_iter_diff pos) const { return pos / n_; }

  uint64_t col(const_data_iter it) const { return pos(it) % n_; }
  uint64_t col(const T* it) const { return pos(it) % n_; }
  uint64_t col(data_iter_diff pos) const { return pos % n_; }

 private:
  uint64_t l_;  // sheets
  uint64_t m_;  // rows
  uint64_t n_;  // cols
  std::vector<T> data_;
};  // Matrix

}  // namespace utils

#endif  // SRC_UTILS_MATRIX_H_
