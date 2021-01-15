add_library(
  hand_strengths SHARED
  CardCombinations.cc
  CardCombinations.h
  Indexer.h
  OCHS.h
  )
set_property(TARGET hand_strengths PROPERTY CXX_STANDARD 17)

add_executable(hand_strengths.out main.cc)
set_property(TARGET hand_strengths.out PROPERTY CXX_STANDARD 17)
target_link_libraries(hand_strengths.out handisomorphism hand_strengths)

add_custom_target(hand_strengths_gen ALL)
add_dependencies(hand_strengths_gen hand_strengths.out hand_strengths)
