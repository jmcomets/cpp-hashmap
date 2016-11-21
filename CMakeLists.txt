project(hash_map)

cmake_minimum_required(VERSION 2.6)

add_definitions(-W -Wall -Werror)
include_directories(${CMAKE_ROOT})

add_executable(hashmaps Main.cpp Person.cpp)
