option(WARNINGS_AS_ERRORS "Treat Warnings As Errors" OFF)
include(cmake/StaticAnalyzers.cmake)

if(ENABLE_CPPCHECK)
    enable_cppcheck(${WARNINGS_AS_ERRORS} "")
endif()
