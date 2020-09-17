cmake_minimum_required(VERSION 3.18.2)
project(omp)

add_library(
    omp_lib STATIC
    CardRange.cpp
    CardRange.h
    CombinedRange.cpp
    CombinedRange.h
    Constants.h
    EquityCalculator.cpp
    EquityCalculator.h
    Hand.h
    HandEvaluator.cpp
    HandEvaluator.h
    OffsetTable.hxx
    Random.h
    Util.h
    )

target_include_directories(omp_lib INTERFACE ./)

set_property(TARGET omp_lib PROPERTY CXX_STANDARD 17)
