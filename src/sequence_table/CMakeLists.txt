add_executable(sequence_table.out main.cc SequenceTable.cc)
set_property(TARGET sequence_table.out PROPERTY CXX_STANDARD 17)

target_link_libraries(sequence_table.out poker_engine)

add_custom_target(sequence_table)
add_dependencies(sequence_table sequence_table.out)
