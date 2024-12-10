#ifndef AI_SRC_UTILS_ARRAY_H_
#define AI_SRC_UTILS_ARRAY_H_

#include <array>

namespace fishbait {

template <class T, std::size_t N>
inline std::array<T, N> FilledArray(const T& filler) {
  std::array<T, N> ret_arr;
  ret_arr.fill(filler);
  return ret_arr;
}

}  // namespace fishbait

#endif  // AI_SRC_UTILS_ARRAY_H_
