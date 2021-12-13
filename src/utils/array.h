// Copyright 2021 Marzuk Rashid

#ifndef SRC_UTILS_ARRAY_H_
#define SRC_UTILS_ARRAY_H_

#include <array>

namespace fishbait {

template <class T, std::size_t N>
inline std::array<T, N> FilledArray(const T& filler) {
  std::array<T, N> ret_arr;
  ret_arr.fill(filler);
  return ret_arr;
}

}  // namespace fishbait

#endif  // SRC_UTILS_ARRAY_H_
