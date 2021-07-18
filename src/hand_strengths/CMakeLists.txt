add_library(
  hand_strengths SHARED
  definitions.h
  lut_files.h
  lut_generators.cc
  lut_generators.h
  ochs.cc
  ochs.h
  )
target_link_libraries(hand_strengths handisomorphism deck)
coverage_counter_zero(hand_strengths)

add_executable(hand_strengths.out main.cc
               ${CMAKE_BINARY_DIR}/out/hand_strengths)
target_link_libraries(hand_strengths.out hand_strengths deck)
coverage_counter_zero(hand_strengths.out)

add_custom_command(OUTPUT ${CMAKE_BINARY_DIR}/out/hand_strengths
  COMMAND ${CMAKE_COMMAND} -E make_directory
          ${CMAKE_BINARY_DIR}/out/hand_strengths
  )
