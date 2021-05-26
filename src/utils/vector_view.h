// Copyright 2021 Marzuk Rashid

#ifndef SRC_UTILS_VECTOR_VIEW_H_
#define SRC_UTILS_VECTOR_VIEW_H_

#include <array>
#include <cassert>
#include <cstdint>
#include <initializer_list>
#include <ostream>
#include <vector>

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

  bool operator<(const VectorView& other) const { return compare(other) < 0; }
  bool operator==(const VectorView& other) const { return compare(other) == 0; }

 private:
  int compare(const VectorView& other) const {
    /* If this has more elements, it's greater than other. If it has less
       elements then it is less than other. If they have the same number of
       elements, we compare the contents. */
    if (n() > other.n()) {
      return 1;
    } else if (n() < other.n()) {
      return -1;
    } else {
      /* If the first element of this is less than the first element of other,
         then this is less than other. If it is greater, then this is greater
         than other. If it's the same, check the next element. Repeat this for
         all elements. */
      for (uint32_t i = 0; i < n(); ++i) {
        if (operator()(i) < other(i)) {
          return -1;
        } else if (operator()(i) > other(i)) {
          return 1;
        } else {
          continue;
        }
      }  // for i
    }  // else

    /* If all elements are the same and they are of the same length, they are
       equal. */
    return 0;
  }

  const T* const start_;
  const uint32_t n_;
};  // VectorView

template <typename T>
std::ostream& operator<<(std::ostream& os, const VectorView<T>& v) {
  os << "[";
  for (uint32_t i = 0; i < v.n(); ++i) {
    os << +v(i);
    if (i < v.n() - 1) {
      os << ", ";
    }
  }
  os << "]";
  return os;
}

}  // namespace utils

#endif  // SRC_UTILS_VECTOR_VIEW_H_
