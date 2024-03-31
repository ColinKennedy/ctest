#include <stdexcept>  // std::runtime_error
#include <stdio.h>

#define CTEST_MAIN

#define CTEST_SEGFAULT
#define CTEST_NO_COLORS

#include "ctest.h"

CTEST(early_exit, exception)
{
    throw std::runtime_error {"I am a bad error!"};
}


int main(int argc, const char *argv[]) { return ctest_main(argc, argv); }
