cmake_minimum_required(VERSION 3.5)
project(ctest)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(APPLE AND "${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    # By default Apple GCC has position independent executables, which from
    # experimentation seems not to link well with CTest. So we've disabled the
    # functionality here.
    #
    # Maybe in the future this restriction can be lifted.
    #
    set(CMAKE_CXX_FLAGS "-no-pie")
endif()

include(cmake/ci.cmake)
include(cmake/common.cmake)

add_subdirectory(tests)
