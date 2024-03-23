#include <stdio.h>

#define CTEST_MAIN

#define CTEST_SEGFAULT
#define CTEST_NO_COLORS

#include "ctest.h"

// Note: Intentionally no CTEST macros are defined here

int main(int argc, const char *argv[]) { return ctest_main(argc, argv); }
