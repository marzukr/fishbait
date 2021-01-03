// Copyright 2021 Marzuk Rashid

#ifndef SRC_CLUSTERING_MATRIX_H_
#define SRC_CLUSTERING_MATRIX_H_

#include <stdint.h>

#include <cassert>
#include <iostream>
#include <string>
#include <sstream>

#include "clustering/Array.h"

namespace clustering {

template <typename T>
class Matrix {
 public:
  Matrix(uint32_t n, uint32_t m) : n_(n), m_(m) {
    data_ = new T[n*m];
  }
  ~Matrix() {
    delete[] data_;
  }
  Matrix(const Matrix<T>& other) : n_(other.n_), m_(other.m_) {
    data_ = new T[n_*m_];
    for (uint32_t i = 0; i < n_*m_; ++i) {
      data_[i] = other.data_[i];
    }
  }
  Matrix<T>& operator=(const Matrix<T>& other) {
    n_ = other.n_;
    m_ = other.m_;
    delete[] data_;
    data_ = new T[n_*m_];
    for (uint32_t i = 0; i < n_*m_; ++i) {
      data_[i] = other.data_[i];
    }
    return *this;
  }

  T& Access(uint32_t i, uint32_t j) {
    assert(i < n_);
    assert(j < m_);
    return data_[i*m_ + j];
  }
  T& operator()(uint32_t i, uint32_t j) { return Access(i, j); }
  const T& operator()(uint32_t i, uint32_t j) const { return Access(i, j); }

  struct Row {
    T* start;
    uint32_t length;

    std::string Print() const {
      std::ostringstream strm;
      strm << "(" << start[0];
      for (uint32_t j = 1; j < length; ++j) {
        strm << ", " << start[j];
      }
      strm << ")";
      return strm.str();
    }
  };
  Row operator()(uint32_t i) const {
    assert(i < n_);
    Row row { &data_[i*m_], m_ };
    return row;
  }

  bool operator==(const Matrix<T>& other) const {
    if (other.n_ != n_ || other.m_ != m_) return false;
    for (uint32_t i = 0; i < n_*m_; ++i) {
      if (data_[i] != other.data_[i]) return false;
    }
    return true;
  }

  template <typename U>
  void SetRow(uint32_t i, typename Matrix<U>::Row r) {
    assert(i < n_);
    assert(r.length == m_);
    for (uint32_t j = 0; j < m_; ++j) {
      operator()(i, j) = r.start[j];
    }
  }

  template <typename U>
  void AddToRow(uint32_t i, typename Matrix<U>::Row r) {
    assert(i < n_);
    assert(r.length == m_);
    for (uint32_t j = 0; j < m_; ++j) {
      operator()(i, j) += r.start[j];
    }
  }

  template <typename U>
  void SubtractFromRow(uint32_t i, typename Matrix<U>::Row r) {
    assert(i < n_);
    assert(r.length == m_);
    for (uint32_t j = 0; j < m_; ++j) {
      operator()(i, j) -= r.start[j];
    }
  }

  template <typename U>
  void Divide(const Array<U>& a) {
    assert(a.n() == n_);
    for (uint32_t i = 0; i < n_; ++i) {
      for (uint32_t j = 0; j < m_; ++j) {
        operator()(i, j) /= a(i);
      }
    }
  }

  void Fill(T filler) {
    for (uint32_t i = 0; i < n_*m_; ++i) {
      data_[i] = filler;
    }
  }

  uint32_t n() const { return n_; }
  uint32_t m() const { return m_; }

 private:
  const uint32_t n_;  // rows
  const uint32_t m_;  // cols
  T* data_;
};  // Matrix

}  // namespace clustering

#endif  // SRC_CLUSTERING_MATRIX_H_
