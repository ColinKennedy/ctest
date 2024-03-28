#include <stdio.h>

#define CTEST_MAIN

#define CTEST_SEGFAULT
#define CTEST_NO_COLORS

#include "ctest.h"

CTEST(suitey, test1) { ASSERT_TRUE(true); }

CTEST(suitey, test2) { ASSERT_TRUE(true); }

CTEST(suitey, test3) { ASSERT_TRUE(true); }

CTEST(another, testout) { ASSERT_TRUE(true); }

int main(int argc, const char *argv[]) { return ctest_main(argc, argv); }
