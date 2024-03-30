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
