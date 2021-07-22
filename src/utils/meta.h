// Copyright 2021 Marzuk Rashid

#ifndef SRC_UTILS_META_H_
#define SRC_UTILS_META_H_

namespace utils {

template <int kFirstNum = 0, int... kNums>
constexpr int PackSum(int depth = sizeof...(kNums)) {
  if (sizeof...(kNums) == 0 || depth == 0) {
    return kFirstNum;
  } else {
    return kFirstNum + PackSum<kNums...>(depth - 1);
  }
}

}  // namespace utils

#endif  // SRC_UTILS_META_H_
