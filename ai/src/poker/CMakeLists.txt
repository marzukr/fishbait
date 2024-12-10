add_library(
  poker SHARED
  card_utils.cc
  card_utils.h
  definitions.h
  indexer.h
  node.h
  )
target_link_libraries(poker handisomorphism)
