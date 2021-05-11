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
target_link_libraries(hand_strengths handisomorphism)

add_executable(hand_strengths.out main.cc)
target_link_libraries(hand_strengths.out handisomorphism hand_strengths)

add_custom_target(hand_strengths_gen ALL)
add_dependencies(hand_strengths_gen hand_strengths.out hand_strengths)

file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/out/hand_strengths)
