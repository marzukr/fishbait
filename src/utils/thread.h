// Copyright 2021 Marzuk Rashid

#ifndef SRC_UTILS_THREAD_H_
#define SRC_UTILS_THREAD_H_

#include <algorithm>
#include <array>
#include <thread>  // NOLINT(build/c++11)

#include "utils/config.h"

namespace fishbait {

template <typename ComputeFn>
inline void DivideWork(int work_n, ComputeFn&& worker) {
  std::array<std::thread, kThreads> threads;
  int work_per_thread = work_n / kThreads;
  int leftover = work_n % kThreads;
  int start = 0;
  for (int i = 0; i < kThreads; ++i) {
    int end = start + work_per_thread;
    if (i < leftover) end += 1;
    if (start == end) break;
    threads[i] = std::thread(worker, start, end);
    start = end;
  }
  for (int i = 0; i < std::min(kThreads, work_n); ++i) {
    threads[i].join();
  }
}

}  // namespace fishbait

#endif  // SRC_UTILS_THREAD_H_
