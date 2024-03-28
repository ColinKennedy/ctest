#include <stdio.h>

#define CTEST_MAIN

#define CTEST_SEGFAULT
#define CTEST_NO_COLORS

#include "ctest.h"

CTEST(single, empty) { }

CTEST(single, one_assert) { ASSERT_TRUE(true); }

int main(int argc, const char *argv[]) { return ctest_main(argc, argv); }
