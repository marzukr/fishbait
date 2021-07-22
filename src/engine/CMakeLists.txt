add_library(
  engine SHARED
  card_combinations.cc
  card_combinations.h
  card_utils.cc
  card_utils.h
  definitions.h
  indexer.h
  node.h
  )
target_link_libraries(engine handisomorphism)
