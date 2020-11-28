cmake_minimum_required(VERSION 3.5.0)
project(hand_evaluator)

add_library(
    hand_evaluator_lib INTERFACE
    )

target_include_directories(hand_evaluator_lib INTERFACE ./)
