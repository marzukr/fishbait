add_library(
  utils SHARED
  array.h
  cereal.h
  combination_matrix.h
  fraction.cc
  fraction.h
  loop_iterator.h
  "math.h"
  meta.h
  random.h
  thread.h
  timer.h
  )

if(NOT DEFINED CORES)
  set(CORES 1)
else()
  message(STATUS "Instructing program to use ${CORES} cores when possible")
endif()

file(WRITE config.h
"#ifndef SRC_AI_UTILS_CONFIG_H_
#define SRC_AI_UTILS_CONFIG_H_

namespace fishbait {

constexpr std::size_t kThreads = ${CORES};

}  // namespace fishbait

#endif  // SRC_AI_UTILS_CONFIG_H_
")
