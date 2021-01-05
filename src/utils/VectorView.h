// Copyright 2021 Marzuk Rashid

#ifndef SRC_UTILS_VECTORVIEW_H_
#define SRC_UTILS_VECTORVIEW_H_

#include <stdint.h>

#include <cassert>
#include <vector>

namespace utils {

template <typename T>
class VectorView {
 public:
  VectorView(const T* start, uint32_t n) : start_(start), n_(n) {}
  explicit VectorView(std::vector<T> v) : start_(v.data()), n_(v.size()) {}
  ~VectorView() {}

  const T operator()(uint32_t i) {
    assert(i < n_);
    return start_[i];
  }

  const T* begin() const {
    return start_;
  }

  const T* end() const {
    return start_ + n_;
  }

  uint32_t n() const { return n_; }

 private:
  const T* const start_;
  const uint32_t n_;
};  // VectorView

}  // namespace utils

#endif  // SRC_UTILS_VECTORVIEW_H_
