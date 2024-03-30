if(NOT DEFINED TEST_FILE)
    # TODO: Make sure these arguments works as expected
    set(TEST_FILE ".*" CACHE STRING "The source code test(s) to run from.")
endif()

if(NOT DEFINED TEST_SUITE)
    # TODO: Make sure these arguments works as expected
    set(TEST_SUITE "\"*\"" CACHE STRING "The suite(s) to run tests for.")
endif()

if(NOT DEFINED TEST_NAME)
    # TODO: Make sure these arguments works as expected
    set(TEST_NAME "\"*\"" CACHE STRING "The individual test(s) to run.")
endif()

include(cmake/setup_test_commands.cmake)
add_custom_target(test DEPENDS ${ALL_TESTS})
