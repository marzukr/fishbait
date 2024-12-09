#ifndef AI_SRC_UTILS_THREAD_H_
#define AI_SRC_UTILS_THREAD_H_

#include <algorithm>
#include <array>
#include <thread>  // NOLINT(build/c++11)

#include "utils/config.h"

namespace fishbait {

template <typename ComputeFn>
inline void DivideWork(std::size_t work_n, ComputeFn&& worker) {
  std::array<std::thread, kThreads> threads;
  std::size_t work_per_thread = work_n / kThreads;
  std::size_t leftover = work_n % kThreads;
  std::size_t start = 0;
  for (std::size_t i = 0; i < kThreads; ++i) {
    std::size_t end = start + work_per_thread;
    if (i < leftover) end += 1;
    if (start == end) break;
    threads[i] = std::thread(worker, start, end);
    start = end;
  }
  for (std::size_t i = 0; i < std::min(kThreads, work_n); ++i) {
    threads[i].join();
  }
}

}  // namespace fishbait

#endif  // AI_SRC_UTILS_THREAD_H_
