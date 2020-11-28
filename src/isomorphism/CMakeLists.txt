cmake_minimum_required(VERSION 3.5.0)
project(isomorphism)

add_library(
    isomorphism_lib SHARED
    check-main.c
    deck.c
    deck.h
    hand_index-impl.h
    hand_index.c
    hand_index.h
    )

set(CMAKE_C_FLAGS "-std=c99")
