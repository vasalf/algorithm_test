cmake_minimum_required (VERSION 2.8.11)
project(algorithm_test)

set(CMAKE_CXX_STANDARD 14)

# Library for printing ASCII tables
add_subdirectory(ascii_table)

# Basic library for testing
add_subdirectory(speedtest)

# Algorithms
add_subdirectory(aplusb)
add_subdirectory(segment_tree)
add_subdirectory(sufarray)
add_subdirectory(rope)
