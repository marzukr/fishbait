add_executable(tests.out main.cc
  ${CMAKE_BINARY_DIR}/out/tests

  clustering/distance_test.cc
  clustering/k_means_test.cc

  deck/card_combinations_test.cc
  deck/card_utils_test.cc
  deck/indexer_test.cc

  poker_engine/node_test.cc

  utils/cereal_test.cc
  utils/combination_matrix_test.cc
  utils/fraction_test.cc
  utils/loop_iterator_test.cc
  utils/matrix_test.cc
  utils/random_test.cc
  utils/vector_view_test.cc

  )
target_link_libraries(tests.out clustering deck hand_strengths poker_engine
                      utils)
coverage_counter_zero(tests.out)

add_custom_command(OUTPUT ${CMAKE_BINARY_DIR}/out/tests
  COMMAND ${CMAKE_COMMAND} -E make_directory
          ${CMAKE_BINARY_DIR}/out/tests
  )
