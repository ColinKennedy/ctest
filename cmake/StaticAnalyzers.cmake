# Reference: https://github.com/cpp-best-practices/cmake_template/blob/8e1b9e02e8e87a6cffebb7507d41c7d787f1f207/cmake/StaticAnalyzers.cmake

macro(enable_cppcheck WARNINGS_AS_ERRORS CPPCHECK_OPTIONS)
    find_program(CPPCHECK cppcheck)

    if(NOT CPPCHECK)
        message(FATAL_ERROR "cppcheck requested but executable not found")

        return()
    endif()

    if(CMAKE_GENERATOR MATCHES ".*Visual Studio.*")
        set(CPPCHECK_TEMPLATE "vs")
    else()
        set(CPPCHECK_TEMPLATE "gcc")
    endif()

    if("${CPPCHECK_OPTIONS}" STREQUAL "")
        set(CMAKE_CXX_CPPCHECK
            ${CPPCHECK}
            --template=${CPPCHECK_TEMPLATE}
            --enable=style,performance,warning,portability
            --inline-suppr
            # We cannot act on a bug/missing feature of cppcheck
            --suppress=cppcheckError
            --suppress=internalAstError
            # if a file does not have an internalAstError, we get an unmatchedSuppression error
            --suppress=unmatchedSuppression
            # noisy and incorrect sometimes
            --suppress=passedByValue
            # ignores code that cppcheck thinks is invalid C++
            --suppress=syntaxError
            --suppress=preprocessorErrorDirective
            --inconclusive)
    else()
        # if the user provides a CPPCHECK_OPTIONS with a template specified, it will override this template
        set(CMAKE_CXX_CPPCHECK ${CPPCHECK} --template=${CPPCHECK_TEMPLATE} ${CPPCHECK_OPTIONS})
    endif()

    # NOTE: This block probably works in later cppcheck versions but mine's
    # old. It has no --std option
    #
    # if(NOT "${CMAKE_CXX_STANDARD}" STREQUAL "")
    #     set(CMAKE_CXX_CPPCHECK ${CMAKE_CXX_CPPCHECK} --std=c++${CMAKE_CXX_STANDARD})
    # endif()

    if(${WARNINGS_AS_ERRORS})
        list(APPEND CMAKE_CXX_CPPCHECK --error-exitcode=2)
    endif()
endmacro()
