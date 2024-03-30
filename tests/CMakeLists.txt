if(NOT DEFINED TEST_FILE)
    set(TEST_FILE ".*" CACHE STRING "The source code test(s) to run from.")
endif()

if(NOT DEFINED TEST_SUITE)
    set(TEST_SUITE "\"*\"" CACHE STRING "The suite(s) to run tests for.")
endif()

if(NOT DEFINED TEST_NAME)
    set(TEST_NAME "\"*\"" CACHE STRING "The individual test(s) to run.")
endif()

include(cmake/setup_test_commands.cmake)
add_custom_target(test DEPENDS ${ALL_TESTS})
