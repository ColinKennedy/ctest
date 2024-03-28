cmake_minimum_required(VERSION 3.5)
project(ctest)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_FLAGS "-no-pie")

add_subdirectory(tests)
