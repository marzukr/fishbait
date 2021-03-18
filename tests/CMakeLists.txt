add_executable(tests.out main.cc

  clustering/distance_test.cc
  clustering/k_means_test.cc

  hand_strengths/card_utils_test.cc
  hand_strengths/card_combinations_test.cc
  hand_strengths/indexer_test.cc

  utils/combination_matrix_test.cc
  utils/matrix_test.cc
  utils/vector_view_test.cc

  )

set_property(TARGET tests.out PROPERTY CXX_STANDARD 17)
target_link_libraries(tests.out hand_strengths)

add_custom_target(tests ALL)
add_dependencies(tests tests.out)
