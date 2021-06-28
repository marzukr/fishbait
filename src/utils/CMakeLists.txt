add_library(
  utils SHARED
  cereal.h
  combination_matrix.h
  fraction.cc
  fraction.h
  loop_iterator.h
  random.h
  timer.h
  )
coverage_counter_zero(utils)