cmake_minimum_required(VERSION 3.15)
project(TwoSidedContextParser)

set(CMAKE_CXX_STANDARD 14)

add_executable(TwoSidedContextParser src/main.cpp src/grammar.h src/derivation_table.h src/derive.h src/grammar.cpp)