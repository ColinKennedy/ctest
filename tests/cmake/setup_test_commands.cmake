set(ALL_COMMANDS_TO_TEST)

string(REGEX MATCH ".*" IS_MATCH "tests/test_main.cpp")
if (IS_MATCH)
    include(cmake/main_tests.cmake)
    list(APPEND ALL_COMMANDS_TO_TEST "run_it_command")
endif()

string(REGEX MATCH ".*" IS_MATCH "tests/test_another.cpp")
if (IS_MATCH)
    include(cmake/another_test.cmake)
    list(APPEND ALL_COMMANDS_TO_TEST "another_command")
endif()

string(JOIN " " ALL_TESTS "${ALL_COMMANDS_TO_TEST}")
