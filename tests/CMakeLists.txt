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


create_cli_and_test(arguments)
create_cli_and_test(empty)
create_cli_and_test(single)
create_cli_and_test(mytests)


add_executable(run_it
    main.cpp  # Main "capabilities" tests
)

target_include_directories(run_it
    PRIVATE
        ../include
)

add_dependencies(
    run_it

    arguments
    empty
    single

    mytests
)

add_options(run_it)

# get_property(run_it_location TARGET run_it PROPERTY LOCATION)
# message (STATUS "run_it_location == ${run_it_location}")
# get_target_property(TEST_EXECUTABLE run_it RUNTIME_OUTPUT_DIRECTORY)
add_custom_target(
    test
    COMMAND $<TARGET_FILE:run_it>
)
add_dependencies(test run_it)
