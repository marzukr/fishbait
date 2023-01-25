#ifndef SRC_AI_UTILS_META_H_
#define SRC_AI_UTILS_META_H_

namespace fishbait {

template <int kFirstNum = 0, int... kNums>
constexpr int PackSum(int depth = sizeof...(kNums)) {
  if (sizeof...(kNums) == 0 || depth == 0) {
    return kFirstNum;
  } else {
    return kFirstNum + PackSum<kNums...>(depth - 1);
  }
}

}  // namespace fishbait

#endif  // SRC_AI_UTILS_META_H_
