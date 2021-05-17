add_executable(tests.out main.cc

  clustering/distance_test.cc
  clustering/k_means_test.cc

  deck/card_combinations_test.cc
  deck/card_utils_test.cc
  deck/indexer_test.cc

  utils/cereal_test.cc
  utils/combination_matrix_test.cc
  utils/loop_iterator_test.cc
  utils/matrix_test.cc
  utils/random_test.cc
  utils/vector_view_test.cc

  )

target_link_libraries(tests.out clustering deck hand_strengths utils)
target_compile_options(tests.out PRIVATE -fprofile-arcs -ftest-coverage)
target_link_options(tests.out PRIVATE -fprofile-arcs)

add_custom_target(coverage
    COMMAND ./bin/tests.out
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    DEPENDS tests.out
    )
add_custom_command(TARGET coverage POST_BUILD
    COMMAND lcov --capture --no-external --directory . --base-directory ${CMAKE_SOURCE_DIR}/src --output-file test_coverage.info
    COMMAND genhtml test_coverage.info --output-directory lcov_html
    COMMAND lcov --zerocounters --directory .
    COMMAND echo "Coverage report generated at:" ${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/tests.out.dir/lcov_html/index.html
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/tests.out.dir
    )
