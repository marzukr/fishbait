// Copyright 2021 Marzuk Rashid

#include "utils/vector_view.h"

#include <cstdint>
#include <ostream>

namespace utils {

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
