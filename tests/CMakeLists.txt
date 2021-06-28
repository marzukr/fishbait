add_executable(tests.out main.cc
  ${CMAKE_BINARY_DIR}/out/tests

  external/array/array_test.cc

  src/blueprint/sequence_table_test.cc

  src/clustering/distance_test.cc
  src/clustering/k_means_test.cc

  src/deck/card_combinations_test.cc
  src/deck/card_utils_test.cc
  src/deck/indexer_test.cc

  src/hand_strengths/lut_generators_test.cc

  src/poker_engine/node_test.cc

  src/utils/cereal_test.cc
  src/utils/combination_matrix_test.cc
  src/utils/fraction_test.cc
  src/utils/loop_iterator_test.cc
  src/utils/matrix_test.cc
  src/utils/random_test.cc
  src/utils/vector_view_test.cc

  )
target_link_libraries(tests.out blueprint clustering deck hand_strengths
                      poker_engine utils)
coverage_counter_zero(tests.out)

add_custom_command(OUTPUT ${CMAKE_BINARY_DIR}/out/tests
  COMMAND ${CMAKE_COMMAND} -E make_directory
          ${CMAKE_BINARY_DIR}/out/tests
  )
