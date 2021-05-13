add_library(
  hand_strengths SHARED
  lut_generators.cc
  lut_generators.h
  ochs.cc
  ochs.h
  )
target_link_libraries(hand_strengths handisomorphism deck)

add_executable(hand_strengths.out main.cc)
target_link_libraries(hand_strengths.out hand_strengths deck)

file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/luts)
