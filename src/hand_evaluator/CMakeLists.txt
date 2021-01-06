find_package(Boost COMPONENTS python3 REQUIRED)
find_package(PythonLibs 3 REQUIRED)


add_library(hand_evaluator MODULE hand_evaluator.cc Indexer.cc OMPRanker.cc)
set_target_properties(hand_evaluator PROPERTIES PREFIX "")
set_property(TARGET hand_evaluator PROPERTY CXX_STANDARD 14)
target_include_directories(hand_evaluator PRIVATE 
  ${Boost_INCLUDE_DIRS} ${PYTHON_INCLUDE_DIRS}
)
target_link_libraries(hand_evaluator 
  ompeval handisomorphism ${Boost_LIBRARIES} ${PYTHON_LIBRARIES}
)

add_custom_target(distribution_gen)
add_dependencies(distribution_gen hand_evaluator)
