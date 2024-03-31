function(create_cli_and_test)
    set(NAME ${ARGV0})
    set(NAME_TEST ${NAME}_test)

    add_executable(${NAME} ${NAME}.cpp)
    target_include_directories(${NAME}
        PRIVATE
            ../include
    )
    add_options(${NAME})
endfunction()


create_cli_and_test(test_arguments)
create_cli_and_test(test_empty)
create_cli_and_test(test_exception)
create_cli_and_test(test_mytests)
create_cli_and_test(test_single)


add_executable(run_it
    test_main.cpp
)

target_include_directories(run_it
    PRIVATE
        ../include
)

add_dependencies(
    run_it

    test_arguments
    test_empty
    test_exception
    test_single

    test_mytests
)

add_options(run_it)

if(NOT DEFINED TEST_SUITE)
    # TODO: Make sure these arguments works as expected
    set(TEST_SUITE "\"*\"" CACHE STRING "The suite(s) to run tests for.")
endif()

if(NOT DEFINED TEST_NAME)
    # TODO: Make sure these arguments works as expected
    set(TEST_NAME "\"*\"" CACHE STRING "The individual test(s) to run.")
endif()

add_custom_command(
    OUTPUT run_it_command
    COMMAND $<TARGET_FILE:run_it> "${TEST_SUITE}" "${TEST_NAME}"
)
