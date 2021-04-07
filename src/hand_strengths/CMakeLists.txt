add_library(
  hand_strengths SHARED
  card_combinations.cc
  card_combinations.h
  card_utils.cc
  card_utils.h
  indexer.h
  lut_generators.cc
  lut_generators.h
  ochs.cc
  ochs.h
  )
set_property(TARGET hand_strengths PROPERTY CXX_STANDARD 17)
target_link_libraries(hand_strengths handisomorphism)

add_executable(hand_strengths.out main.cc)
set_property(TARGET hand_strengths.out PROPERTY CXX_STANDARD 17)
target_link_libraries(hand_strengths.out handisomorphism hand_strengths)

add_custom_target(hand_strengths_gen ALL)
add_dependencies(hand_strengths_gen hand_strengths.out hand_strengths)

file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/luts)
