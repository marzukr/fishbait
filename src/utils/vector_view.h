// Copyright 2021 Marzuk Rashid

#ifndef SRC_UTILS_VECTOR_VIEW_H_
#define SRC_UTILS_VECTOR_VIEW_H_

#include <stdint.h>

#include <cassert>
#include <vector>
#include <array>
#include <initializer_list>
#include <ostream>

namespace utils {

template <typename T>
class VectorView {
 public:
  VectorView(const T* start, uint32_t n) : start_(start), n_(n) {}
  explicit VectorView(const std::vector<T>& v) : start_(v.data()),
                                                 n_(v.size()) {}
  template <std::size_t kN>
  explicit VectorView(const std::array<T, kN>& a) : start_(a.begin()),
                                                    n_(a.size()) {}
  ~VectorView() {}

  const T operator()(uint32_t i) const {
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

template <typename T>
std::ostream& operator<<(std::ostream& os, const VectorView<T>& v) {
  os << "[";
  for (uint32_t i = 0; i < v.n(); ++i) {
    os << v(i);
    if (i < v.n() - 1) {
      os << ", ";
    }
  }
  os << "]";
  return os;
}

}  // namespace utils

#endif  // SRC_UTILS_VECTOR_VIEW_H_
