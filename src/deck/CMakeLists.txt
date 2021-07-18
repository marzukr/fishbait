add_library(
  deck SHARED
  card_combinations.cc
  card_combinations.h
  card_utils.cc
  card_utils.h
  definitions.h
  indexer.h
  )
target_link_libraries(deck handisomorphism)
