add_library(
  utils SHARED
  cereal.h
  combination_matrix.h
  fraction.cc
  fraction.h
  loop_iterator.h
  matrix.h
  random.h
  timer.h
  vector_view.h
  vector_view.cc
  )
coverage_counter_zero(utils)