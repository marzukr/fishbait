add_library(
  utils SHARED
  cereal.h
  combination_matrix.h
  fraction.cc
  fraction.h
  loop_iterator.h
  "math.h"
  meta.h
  random.h
  timer.h
  )

if(NOT DEFINED CORES)
  set(CORES 1)
else()
  message(STATUS "Instructing program to use ${CORES} cores when possible")
endif()

file(WRITE config.h
"// Copyright 2021 Marzuk Rashid

#ifndef SRC_UTILS_CONFIG_H_
#define SRC_UTILS_CONFIG_H_

namespace fishbait {

constexpr int kThreads = ${CORES};

}  // namespace fishbait

#endif  // SRC_UTILS_CONFIG_H_
")
