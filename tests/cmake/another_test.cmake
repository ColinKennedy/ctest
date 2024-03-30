add_executable(another_test
    test_another.cpp
)

target_include_directories(another_test
    PRIVATE
        ../include
)

add_options(another_test)

add_custom_command(
    OUTPUT another_command
    COMMAND $<TARGET_FILE:another_test> "${TEST_SUITE}" "${TEST_NAME}"
)
