add_executable(tests.out main.cc
  ${CMAKE_BINARY_DIR}/out/tests

  external/array/array_test.cc

  src/mccfr/sequence_table_test.cc
  src/mccfr/strategy_test.cc

  src/clustering/cluster_table_test.cc
  src/clustering/distance_test.cc
  src/clustering/k_means_test.cc

  src/hand_strengths/card_combinations_test.cc
  src/hand_strengths/lut_generators_test.cc

  src/poker/card_utils_test.cc
  src/poker/indexer_test.cc
  src/poker/node_test.cc

  src/relay/commander_test.cc
  src/relay/scribe_test.cc

  src/utils/array_test.cc
  src/utils/cereal_test.cc
  src/utils/combination_matrix_test.cc
  src/utils/fraction_test.cc
  src/utils/loop_iterator_test.cc
  src/utils/math_test.cc
  src/utils/random_test.cc
  src/utils/timer_test.cc
  )
target_link_libraries(tests.out blueprint clustering hand_strengths poker relay
                      utils)

add_custom_command(OUTPUT ${CMAKE_BINARY_DIR}/out/tests
  COMMAND ${CMAKE_COMMAND} -E make_directory
          ${CMAKE_BINARY_DIR}/out/tests
  )
