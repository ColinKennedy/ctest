option(WARNINGS_AS_ERRORS "Treat Warnings As Errors" ON)
include(cmake/StaticAnalyzers.cmake)

if(ENABLE_CPPCHECK)
    enable_cppcheck(${WARNINGS_AS_ERRORS} "")
endif()
