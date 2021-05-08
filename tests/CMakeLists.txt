add_executable(tests.out main.cc

  clustering/distance_test.cc
  clustering/k_means_test.cc

  hand_strengths/card_utils_test.cc
  hand_strengths/card_combinations_test.cc
  hand_strengths/indexer_test.cc
  hand_strengths/lut_generators_test.cc

  utils/combination_matrix_test.cc
  utils/loop_iterator_test.cc
  utils/matrix_test.cc
  utils/random_test.cc
  utils/vector_view_test.cc

  )

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
target_link_libraries(tests.out hand_strengths Threads::Threads)

add_custom_target(tests ALL)
add_dependencies(tests tests.out)
