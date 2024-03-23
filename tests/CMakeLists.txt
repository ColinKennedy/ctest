function(add_options)
    set(NAME ${ARGV0})

    # TODO: Add more rules (shadow, etc)
    target_compile_options(${NAME} PRIVATE -Wall -Wextra -Wpedantic -Werror)
    # TODO: What C++ standard is USD on again? Double-check
    target_compile_features(${NAME} PRIVATE cxx_std_20)
    # TODO: Add this back in, once I get Windows support
    # if(MSVC)
    #     target_compile_options(${NAME} PRIVATE /W4 /WX)
    # else()
    #     target_compile_options(${NAME} PRIVATE -Wall -Wextra -Wpedantic -Werror)
    # endif()
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


# add_executable(mytests
#     mytests.cpp  # Extra tests, for coverage
# )
#
# target_include_directories(mytests
#     PRIVATE
#         ../include
# )

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
