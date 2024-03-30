function(add_options)
    set(NAME ${ARGV0})

    target_compile_features(${NAME} PRIVATE cxx_std_20)

    # TODO: Add more rules (shadow, etc)
    # TODO: Change warnings into errors
    #
    if(MSVC)
        target_compile_options(${NAME} PRIVATE /W4 /WX)
    else()
        target_compile_options(${NAME} PRIVATE -Wall -Wextra -Wpedantic)
    endif()
endfunction()


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
create_cli_and_test(test_single)
create_cli_and_test(test_mytests)


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
    test_single

    test_mytests
)

add_options(run_it)

if(NOT DEFINED TEST_SUITE)
    set(TEST_SUITE "\"*\"" CACHE STRING "The suite(s) to run tests for.")
endif()

if(NOT DEFINED TEST_NAME)
    set(TEST_NAME "\"*\"" CACHE STRING "The individual test(s) to run.")
endif()

add_custom_target(
    test
    COMMAND $<TARGET_FILE:run_it> "${TEST_SUITE}" "${TEST_NAME}"
)
add_dependencies(test run_it)
