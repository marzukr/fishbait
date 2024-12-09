add_library(
  hand_strengths SHARED
  card_combinations.cc
  card_combinations.h
  definitions.h
  lut_files.h
  lut_generators.cc
  lut_generators.h
  ochs.cc
  ochs.h
  )
target_link_libraries(hand_strengths handisomorphism poker)

add_executable(hand_strengths.out main.cc
               ${CMAKE_BINARY_DIR}/out/ai/hand_strengths)
target_link_libraries(hand_strengths.out hand_strengths poker)

add_custom_command(OUTPUT ${CMAKE_BINARY_DIR}/out/ai/hand_strengths
  COMMAND ${CMAKE_COMMAND} -E make_directory
          ${CMAKE_BINARY_DIR}/out/ai/hand_strengths
  )
