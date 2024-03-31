/* Copyright 2011-2023 Bas van den Berg
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef CTEST_H
#define CTEST_H

#if defined(__clang__)
// https://clang.llvm.org/docs/UsersManual.html#controlling-diagnostics-via-pragmas
#pragma clang system_header
#pragma clang diagnostic ignored "-Wunused-parameter"
#pragma clang diagnostic ignored "-Werror"
#elif defined(__GNUC__) || defined(__GNUG__)
#pragma GCC system_header
#elif defined(_MSC_VER)
// TODO: Finish this one?
#endif


#ifdef __cplusplus
#include <stdexcept> // std::exception

extern "C" {
#endif

#ifdef __GNUC__
#define CTEST_IMPL_FORMAT_PRINTF(a, b) __attribute__ ((format(printf, a, b)))
#else
#define CTEST_IMPL_FORMAT_PRINTF(a, b)
#endif

#include <inttypes.h> /* intmax_t, uintmax_t, PRI* */
#include <stdbool.h> /* bool, true, false */
#include <stddef.h> /* size_t */

#ifdef _WIN32
#define PRIdMAX "jd" /* intmax_t */
#define PRIuMAX "ju" /* uintmax_t */
#endif

typedef void (*ctest_nullary_run_func)(void);
typedef void (*ctest_unary_run_func)(void*);
typedef void (*ctest_setup_func)(void*);
typedef void (*ctest_teardown_func)(void*);

union ctest_run_func_union {
    ctest_nullary_run_func nullary;
    ctest_unary_run_func unary;
};

// Reference: https://stackoverflow.com/a/1486931
#define UNUSED(expr) do { (void)(expr); } while (0)

#define CTEST_IMPL_PRAGMA(x) _Pragma (#x)

#if defined(__GNUC__)
#if defined(__clang__) || __GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 6)
/* the GCC argument will work for both gcc and clang  */
#define CTEST_IMPL_DIAG_PUSH_IGNORED(w) \
    CTEST_IMPL_PRAGMA(GCC diagnostic push) \
    CTEST_IMPL_PRAGMA(GCC diagnostic ignored "-W" #w)
#define CTEST_IMPL_DIAG_POP() \
    CTEST_IMPL_PRAGMA(GCC diagnostic pop)
#else
/* the push/pop functionality wasn't in gcc until 4.6, fallback to "ignored"  */
#define CTEST_IMPL_DIAG_PUSH_IGNORED(w) \
    CTEST_IMPL_PRAGMA(GCC diagnostic ignored "-W" #w)
#define CTEST_IMPL_DIAG_POP()
#endif
#else
/* leave them out entirely for non-GNUC compilers  */
#define CTEST_IMPL_DIAG_PUSH_IGNORED(w)
#define CTEST_IMPL_DIAG_POP()
#endif

struct ctest {
    const char* ssname;  // suite name
    const char* ttname;  // test name
    union ctest_run_func_union run;

    void* data;
    ctest_setup_func* setup;
    ctest_teardown_func* teardown;

    int skip;

    unsigned int magic;
};

#define CTEST_IMPL_NAME(name) ctest_##name
#define CTEST_IMPL_FNAME(sname, tname) CTEST_IMPL_NAME(sname##_##tname##_run)
#define CTEST_IMPL_TNAME(sname, tname) CTEST_IMPL_NAME(sname##_##tname)
#define CTEST_IMPL_DATA_SNAME(sname) CTEST_IMPL_NAME(sname##_data)
#define CTEST_IMPL_DATA_TNAME(sname, tname) CTEST_IMPL_NAME(sname##_##tname##_data)
#define CTEST_IMPL_SETUP_FNAME(sname) CTEST_IMPL_NAME(sname##_setup)
#define CTEST_IMPL_SETUP_FPNAME(sname) CTEST_IMPL_NAME(sname##_setup_ptr)
#define CTEST_IMPL_SETUP_TPNAME(sname, tname) CTEST_IMPL_NAME(sname##_##tname##_setup_ptr)
#define CTEST_IMPL_TEARDOWN_FNAME(sname) CTEST_IMPL_NAME(sname##_teardown)
#define CTEST_IMPL_TEARDOWN_FPNAME(sname) CTEST_IMPL_NAME(sname##_teardown_ptr)
#define CTEST_IMPL_TEARDOWN_TPNAME(sname, tname) CTEST_IMPL_NAME(sname##_##tname##_teardown_ptr)

#define CTEST_IMPL_MAGIC (0xdeadbeef)
#ifdef __APPLE__
#define CTEST_IMPL_SECTION __attribute__ ((used, section ("__DATA, .ctest"), aligned(1)))
#else
#define CTEST_IMPL_SECTION __attribute__ ((used, section (".ctest"), aligned(1)))
#endif

#define CTEST_IMPL_STRUCT(sname, tname, tskip, tdata, tsetup, tteardown) \
    static struct ctest CTEST_IMPL_TNAME(sname, tname) CTEST_IMPL_SECTION = { \
        #sname, \
        #tname, \
        { (ctest_nullary_run_func) CTEST_IMPL_FNAME(sname, tname) }, \
        tdata, \
        (ctest_setup_func*) tsetup, \
        (ctest_teardown_func*) tteardown, \
        tskip, \
        CTEST_IMPL_MAGIC, \
    }

#ifdef __cplusplus

#define CTEST_SETUP(sname) \
    template <> void CTEST_IMPL_SETUP_FNAME(sname)(struct CTEST_IMPL_DATA_SNAME(sname)* data)

#define CTEST_TEARDOWN(sname) \
    template <> void CTEST_IMPL_TEARDOWN_FNAME(sname)(struct CTEST_IMPL_DATA_SNAME(sname)* data)

#define CTEST_DATA(sname) \
    template <typename T> void CTEST_IMPL_SETUP_FNAME(sname)(T* data) { } \
    template <typename T> void CTEST_IMPL_TEARDOWN_FNAME(sname)(T* data) { } \
    struct CTEST_IMPL_DATA_SNAME(sname)

#define CTEST_IMPL_CTEST(sname, tname, tskip) \
    static void CTEST_IMPL_FNAME(sname, tname)(void); \
    CTEST_IMPL_STRUCT(sname, tname, tskip, NULL, NULL, NULL); \
    static void CTEST_IMPL_FNAME(sname, tname)(void)

#define CTEST_IMPL_CTEST2(sname, tname, tskip) \
    static struct CTEST_IMPL_DATA_SNAME(sname) CTEST_IMPL_DATA_TNAME(sname, tname); \
    static void CTEST_IMPL_FNAME(sname, tname)(struct CTEST_IMPL_DATA_SNAME(sname)* data); \
    static void (*CTEST_IMPL_SETUP_TPNAME(sname, tname))(struct CTEST_IMPL_DATA_SNAME(sname)*) = &CTEST_IMPL_SETUP_FNAME(sname)<struct CTEST_IMPL_DATA_SNAME(sname)>; \
    static void (*CTEST_IMPL_TEARDOWN_TPNAME(sname, tname))(struct CTEST_IMPL_DATA_SNAME(sname)*) = &CTEST_IMPL_TEARDOWN_FNAME(sname)<struct CTEST_IMPL_DATA_SNAME(sname)>; \
    CTEST_IMPL_STRUCT(sname, tname, tskip, &CTEST_IMPL_DATA_TNAME(sname, tname), &CTEST_IMPL_SETUP_TPNAME(sname, tname), &CTEST_IMPL_TEARDOWN_TPNAME(sname, tname)); \
    static void CTEST_IMPL_FNAME(sname, tname)(struct CTEST_IMPL_DATA_SNAME(sname)* data)

#else

#define CTEST_SETUP(sname) \
    static void CTEST_IMPL_SETUP_FNAME(sname)(struct CTEST_IMPL_DATA_SNAME(sname)* data); \
    static void (*CTEST_IMPL_SETUP_FPNAME(sname))(struct CTEST_IMPL_DATA_SNAME(sname)*) = &CTEST_IMPL_SETUP_FNAME(sname); \
    static void CTEST_IMPL_SETUP_FNAME(sname)(struct CTEST_IMPL_DATA_SNAME(sname)* data)

#define CTEST_TEARDOWN(sname) \
    static void CTEST_IMPL_TEARDOWN_FNAME(sname)(struct CTEST_IMPL_DATA_SNAME(sname)* data); \
    static void (*CTEST_IMPL_TEARDOWN_FPNAME(sname))(struct CTEST_IMPL_DATA_SNAME(sname)*) = &CTEST_IMPL_TEARDOWN_FNAME(sname); \
    static void CTEST_IMPL_TEARDOWN_FNAME(sname)(struct CTEST_IMPL_DATA_SNAME(sname)* data)

#define CTEST_DATA(sname) \
    struct CTEST_IMPL_DATA_SNAME(sname); \
    static void (*CTEST_IMPL_SETUP_FPNAME(sname))(struct CTEST_IMPL_DATA_SNAME(sname)*); \
    static void (*CTEST_IMPL_TEARDOWN_FPNAME(sname))(struct CTEST_IMPL_DATA_SNAME(sname)*); \
    struct CTEST_IMPL_DATA_SNAME(sname)

#define CTEST_IMPL_CTEST(sname, tname, tskip) \
    static void CTEST_IMPL_FNAME(sname, tname)(void); \
    CTEST_IMPL_STRUCT(sname, tname, tskip, NULL, NULL, NULL); \
    static void CTEST_IMPL_FNAME(sname, tname)(void)

#define CTEST_IMPL_CTEST2(sname, tname, tskip) \
    static struct CTEST_IMPL_DATA_SNAME(sname) CTEST_IMPL_DATA_TNAME(sname, tname); \
    static void CTEST_IMPL_FNAME(sname, tname)(struct CTEST_IMPL_DATA_SNAME(sname)* data); \
    CTEST_IMPL_STRUCT(sname, tname, tskip, &CTEST_IMPL_DATA_TNAME(sname, tname), &CTEST_IMPL_SETUP_FPNAME(sname), &CTEST_IMPL_TEARDOWN_FPNAME(sname)); \
    static void CTEST_IMPL_FNAME(sname, tname)(struct CTEST_IMPL_DATA_SNAME(sname)* data)

#endif

void CTEST_LOG(const char* fmt, ...) CTEST_IMPL_FORMAT_PRINTF(1, 2);
void CTEST_ERR(const char* fmt, ...) CTEST_IMPL_FORMAT_PRINTF(1, 2);  // doesn't return

#define CTEST(sname, tname) CTEST_IMPL_CTEST(sname, tname, 0)
#define CTEST_SKIP(sname, tname) CTEST_IMPL_CTEST(sname, tname, 1)

#define CTEST2(sname, tname) CTEST_IMPL_CTEST2(sname, tname, 0)
#define CTEST2_SKIP(sname, tname) CTEST_IMPL_CTEST2(sname, tname, 1)


void assert_str(const char* cmp, const char* exp, const char* real, const char* caller, int line);
#define ASSERT_STR(exp, real) assert_str("==", exp, real, __FILE__, __LINE__)
#define ASSERT_NOT_STR(exp, real) assert_str("!=", exp, real, __FILE__, __LINE__)
#define ASSERT_STRSTR(str, substr) assert_str("=~", str, substr, __FILE__, __LINE__)
#define ASSERT_NOT_STRSTR(str, substr) assert_str("!~", str, substr, __FILE__, __LINE__)

void assert_wstr(const char* cmp, const wchar_t *exp, const wchar_t *real, const char* caller, int line);
#define ASSERT_WSTR(exp, real) assert_wstr("==", exp, real, __FILE__, __LINE__)
#define ASSERT_NOT_WSTR(exp, real) assert_wstr("!=", exp, real, __FILE__, __LINE__)
#define ASSERT_WSTRSTR(str, substr) assert_wstr("=~", str, substr, __FILE__, __LINE__)
#define ASSERT_NOT_WSTRSTR(str, substr) assert_wstr("!~", str, substr, __FILE__, __LINE__)

void assert_data(const unsigned char* exp, size_t expsize,
                 const unsigned char* real, size_t realsize,
                 const char* caller, int line);
#define ASSERT_DATA(exp, expsize, real, realsize) \
    assert_data(exp, expsize, real, realsize, __FILE__, __LINE__)

#define CTEST_FLT_EPSILON 1e-5
#define CTEST_DBL_EPSILON 1e-12

void assert_compare(const char* cmp, intmax_t exp, intmax_t real, const char* caller, int line);
#define ASSERT_EQUAL(exp, real) assert_compare("==", exp, real, __FILE__, __LINE__)
#define ASSERT_NOT_EQUAL(exp, real) assert_compare("!=", exp, real, __FILE__, __LINE__)

#define ASSERT_LT(v1, v2) assert_compare("<", v1, v2, __FILE__, __LINE__)
#define ASSERT_LE(v1, v2) assert_compare("<=", v1, v2, __FILE__, __LINE__)
#define ASSERT_GT(v1, v2) assert_compare(">", v1, v2, __FILE__, __LINE__)
#define ASSERT_GE(v1, v2) assert_compare(">=", v1, v2, __FILE__, __LINE__)

void assert_compare_u(const char* cmp, uintmax_t exp, uintmax_t real, const char* caller, int line);
#define ASSERT_EQUAL_U(exp, real) assert_compare_u("==", exp, real, __FILE__, __LINE__)
#define ASSERT_NOT_EQUAL_U(exp, real) assert_compare_u("!=", exp, real, __FILE__, __LINE__)

#define ASSERT_LT_U(v1, v2) assert_compare_u("<", v1, v2, __FILE__, __LINE__)
#define ASSERT_LE_U(v1, v2) assert_compare_u("<=", v1, v2, __FILE__, __LINE__)
#define ASSERT_GT_U(v1, v2) assert_compare_u(">", v1, v2, __FILE__, __LINE__)
#define ASSERT_GE_U(v1, v2) assert_compare_u(">=", v1, v2, __FILE__, __LINE__)

void assert_interval(intmax_t exp1, intmax_t exp2, intmax_t real, const char* caller, int line);
#define ASSERT_INTERVAL(exp1, exp2, real) assert_interval(exp1, exp2, real, __FILE__, __LINE__)

void assert_null(const void* real, const char* caller, int line);
#define ASSERT_NULL(real) assert_null((void*)real, __FILE__, __LINE__)

void assert_not_null(const void* real, const char* caller, int line);
#define ASSERT_NOT_NULL(real) assert_not_null(real, __FILE__, __LINE__)

void assert_true(int real, const char* caller, int line);
#define ASSERT_TRUE(real) assert_true(real, __FILE__, __LINE__)

void assert_false(int real, const char* caller, int line);
#define ASSERT_FALSE(real) assert_false(real, __FILE__, __LINE__)

void assert_fail(const char* caller, int line);
#define ASSERT_FAIL() assert_fail(__FILE__, __LINE__)

void assert_dbl_compare(const char* cmp, double exp, double real, double tol, const char* caller, int line);
#define ASSERT_DBL_NEAR(exp, real) assert_dbl_compare("==", exp, real, -CTEST_DBL_EPSILON, __FILE__, __LINE__)
#define ASSERT_DBL_NEAR_TOL(exp, real, tol) assert_dbl_compare("==", exp, real, tol, __FILE__, __LINE__)
#define ASSERT_DBL_FAR(exp, real) assert_dbl_compare("!=", exp, real, -CTEST_DBL_EPSILON, __FILE__, __LINE__)
#define ASSERT_DBL_FAR_TOL(exp, real, tol) assert_dbl_compare("!=", exp, real, tol, __FILE__, __LINE__)

#define ASSERT_FLT_NEAR(v1, v2) assert_dbl_compare("==", v1, v2, -CTEST_FLT_EPSILON, __FILE__, __LINE__)
#define ASSERT_FLT_FAR(v1, v2) assert_dbl_compare("!=", v1, v2, -CTEST_FLT_EPSILON, __FILE__, __LINE__)
#define ASSERT_DBL_LT(v1, v2) assert_dbl_compare("<", v1, v2, 0.0, __FILE__, __LINE__)
#define ASSERT_DBL_GT(v1, v2) assert_dbl_compare(">", v1, v2, 0.0, __FILE__, __LINE__)

#ifdef CTEST_MAIN

#include <setjmp.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#if !defined(_WIN32) || defined(__GNUC__)
#include <unistd.h>  // STDOUT_FILENO
#elif defined(_WIN32)
#include <io.h>
#endif
#include <stdint.h>
#include <stdlib.h>
#include <wchar.h>

static size_t CTEST_ERROR_SIZE;
static char* CTEST_ERROR_MESSAGE;
#define MSG_SIZE 4096
static char CTEST_ERROR_BUFFER[MSG_SIZE];
static jmp_buf CTEST_ERROR_;
static int CTEST_COLOR_OUTPUT = 1;

// We assume that a suite and test size are not > 200
#define MAX_SIZE 200
char CTEST_SUITE_NAME[MAX_SIZE];
char CTEST_TEST_EXPRESSION[MAX_SIZE];

typedef int (*ctest_filter_func)(const struct ctest*);

#define ANSI_BLACK    "\033[0;30m"
#define ANSI_RED      "\033[0;31m"
#define ANSI_GREEN    "\033[0;32m"
#define ANSI_YELLOW   "\033[0;33m"
#define ANSI_BLUE     "\033[0;34m"
#define ANSI_MAGENTA  "\033[0;35m"
#define ANSI_CYAN     "\033[0;36m"
#define ANSI_GREY     "\033[0;37m"
#define ANSI_DARKGREY "\033[01;30m"
#define ANSI_BRED     "\033[01;31m"
#define ANSI_BGREEN   "\033[01;32m"
#define ANSI_BYELLOW  "\033[01;33m"
#define ANSI_BBLUE    "\033[01;34m"
#define ANSI_BMAGENTA "\033[01;35m"
#define ANSI_BCYAN    "\033[01;36m"
#define ANSI_WHITE    "\033[01;37m"
#define ANSI_NORMAL   "\033[0m"

CTEST(suite, test) { }

static void vprint_errormsg(const char* const fmt, va_list ap) CTEST_IMPL_FORMAT_PRINTF(1, 0);
static void print_errormsg(const char* const fmt, ...) CTEST_IMPL_FORMAT_PRINTF(1, 2);

static void vprint_errormsg(const char* const fmt, va_list ap) {
    // (v)snprintf returns the number that would have been written
    const int ret = vsnprintf(CTEST_ERROR_MESSAGE, CTEST_ERROR_SIZE, fmt, ap);
    if (ret < 0) {
        CTEST_ERROR_MESSAGE[0] = 0x00;
    } else {
        const size_t size = (size_t) ret;
        const size_t s = (CTEST_ERROR_SIZE <= size ? size -CTEST_ERROR_SIZE : size);
        // CTEST_ERROR_SIZE may overflow at this point
        CTEST_ERROR_SIZE -= s;
        CTEST_ERROR_MESSAGE += s;
    }
}

static void print_errormsg(const char* const fmt, ...) {
    va_list argp;
    va_start(argp, fmt);
    vprint_errormsg(fmt, argp);
    va_end(argp);
}

static void msg_start(const char* color, const char* title) {
    if (CTEST_COLOR_OUTPUT) {
        print_errormsg("%s", color);
    }
    print_errormsg("  %s: ", title);
}

static void msg_end(void) {
    if (CTEST_COLOR_OUTPUT) {
        print_errormsg(ANSI_NORMAL);
    }
    print_errormsg("\n");
}

void CTEST_LOG(const char* fmt, ...)
{
    va_list argp;
    msg_start(ANSI_BLUE, "LOG");

    va_start(argp, fmt);
    vprint_errormsg(fmt, argp);
    va_end(argp);

    msg_end();
}

CTEST_IMPL_DIAG_PUSH_IGNORED(missing-noreturn)

void CTEST_ERR(const char* fmt, ...)
{
    va_list argp;
    msg_start(ANSI_YELLOW, "ERR");

    va_start(argp, fmt);
    vprint_errormsg(fmt, argp);
    va_end(argp);

    msg_end();
    longjmp(CTEST_ERROR_, 1);
}

CTEST_IMPL_DIAG_POP()

void assert_str(const char* cmp, const char* exp, const char*  real, const char* caller, int line) {
    if (
        ((!exp && real) || (exp && !real))
        ||
        (
            exp
            &&
            (
                (cmp[1] == '=' && ((cmp[0] == '=') ^ (strcmp(exp, real) == 0)))
                || (cmp[1] == '~' && ((cmp[0] == '=') ^ (strstr(exp, real) != NULL)))
            )
        )
    ) {
        CTEST_ERR("%s:%d  assertion failed, '%s' %s '%s'", caller, line, exp, cmp, real);
    }
}

void assert_wstr(const char* cmp, const wchar_t *exp, const wchar_t *real, const char* caller, int line) {
    if (
        ((!exp && real) || (exp && !real))
        ||
        (
            exp
            &&
            (
                (cmp[1] == '=' && ((cmp[0] == '=') ^ (wcscmp(exp, real) == 0)))
                || (cmp[1] == '~' && ((cmp[0] == '=') ^ (wcsstr(exp, real) != NULL)))
            )
        )
    )
    {
        CTEST_ERR("%s:%d  assertion failed, '%ls' %s '%ls'", caller, line, exp, cmp, real);
    }
}

void assert_data(const unsigned char* exp, size_t expsize,
                 const unsigned char* real, size_t realsize,
                 const char* caller, int line) {
    size_t i;
    if (expsize != realsize) {
        CTEST_ERR("%s:%d  expected %" PRIuMAX " bytes, got %" PRIuMAX, caller, line, (uintmax_t) expsize, (uintmax_t) realsize);
    }
    for (i=0; i<expsize; i++) {
        if (exp[i] != real[i]) {
            CTEST_ERR("%s:%d expected 0x%02x at offset %" PRIuMAX " got 0x%02x",
                caller, line, exp[i], (uintmax_t) i, real[i]);
        }
    }
}

static bool get_compare_result(const char* cmp, int c3, bool eq) {
    if (cmp[0] == '<')
        return c3 < 0 || ((cmp[1] == '=') && eq);
    if (cmp[0] == '>')
        return c3 > 0 || ((cmp[1] == '=') && eq);
    return (cmp[0] == '=') == eq;
}

void assert_compare(const char* cmp, intmax_t exp, intmax_t real, const char* caller, int line) {
    int c3 = (real < exp) - (exp < real);

    if (!get_compare_result(cmp, c3, c3 == 0)) {
        CTEST_ERR("%s:%d  assertion failed, %" PRIdMAX " %s %" PRIdMAX "", caller, line, exp, cmp, real);
    }
}

void assert_compare_u(const char* cmp, uintmax_t exp, uintmax_t real, const char* caller, int line) {
    int c3 = (real < exp) - (exp < real);

    if (!get_compare_result(cmp, c3, c3 == 0)) {
        CTEST_ERR("%s:%d  assertion failed, %" PRIuMAX " %s %" PRIuMAX, caller, line, exp, cmp, real);
    }
}

void assert_interval(intmax_t exp1, intmax_t exp2, intmax_t real, const char* caller, int line) {
    if (real < exp1 || real > exp2) {
        CTEST_ERR("%s:%d  expected %" PRIdMAX "-%" PRIdMAX ", got %" PRIdMAX, caller, line, exp1, exp2, real);
    }
}

static bool approximately_equal(double a, double b, double epsilon) {
    double d = a - b;
    if (d < 0) d = -d;
    if (a < 0) a = -a;
    if (b < 0) b = -b;
    return d <= (a > b ? a : b)*epsilon;     /* D.Knuth */
}

/* tol < 0 means it is an epsilon, else absolute error */
void assert_dbl_compare(const char* cmp, double exp, double real, double tol, const char* caller, int line) {
    double diff = exp - real;
    double absdiff = diff < 0 ? -diff : diff;
    int c3 = (real < exp) - (exp < real);
    bool eq = tol < 0 ? approximately_equal(exp, real, -tol) : absdiff <= tol;

    if (!get_compare_result(cmp, c3, eq)) {
        const char* tolstr = "tol";
        if (tol < 0) {
            tolstr = "eps";
            tol = -tol;
        }
        CTEST_ERR("%s:%d  assertion failed, %.8g %s %.8g (diff %.4g, %s %.4g)", caller, line, exp, cmp, real, diff, tolstr, tol);
    }
}

void assert_null(const void* real, const char* caller, int line) {
    if ((real) != NULL) {
        CTEST_ERR("%s:%d  should be NULL", caller, line);
    }
}

void assert_not_null(const void* real, const char* caller, int line) {
    if (real == NULL) {
        CTEST_ERR("%s:%d  should not be NULL", caller, line);
    }
}

void assert_true(int real, const char* caller, int line) {
    if ((real) == 0) {
        CTEST_ERR("%s:%d  should be true", caller, line);
    }
}

void assert_false(int real, const char* caller, int line) {
    if ((real) != 0) {
        CTEST_ERR("%s:%d  should be false", caller, line);
    }
}

void assert_fail(const char* caller, int line) {
    CTEST_ERR("%s:%d  shouldn't come here", caller, line);
}


static int suite_all(const struct ctest* t) {
    UNUSED(t);
    return 1;
}

/// Check if `pattern` is a substring of `candidate`.
///
/// @seealso https://stackoverflow.com/a/23457543
///
/// @param pattern A sub-string like `"foo*"`.
/// @param candidate A full string like `"football"`.
/// @param p The starting position for `pattern`. Used internally.
/// @param c The starting position for `pattern`. Used internally.
/// @return If there's a match, return `1`.
///
static int glob_text_(const char *pattern, const char *candidate, int p, int c) {
  if (pattern[p] == '\0') {
    return candidate[c] == '\0';
  } else if (pattern[p] == '*') {
    for (; candidate[c] != '\0'; c++) {
      if (glob_text_(pattern, candidate, p+1, c))
        return true;
    }
    return glob_text_(pattern, candidate, p+1, c);
  } else if (pattern[p] != '?' && pattern[p] != candidate[c]) {
    return false;
  }  else {
    return glob_text_(pattern, candidate, p+1, c+1);
  }
}


/// Check if `pattern` is a substring of `candidate`.
///
/// @param pattern A sub-string like `"foo*"`.
/// @param candidate A full string like `"football"`.
/// @return If there's a match, return `1`.
///
static int glob_text(const char *pattern, const char *candidate) {
    return glob_text_(pattern, candidate, 0, 0);
}

static int suite_filter(const struct ctest* t) {
    return (
        (
            // No suite filter
            CTEST_SUITE_NAME == NULL || CTEST_SUITE_NAME[0] == '\0'
            // ... Or a matching suite name
            || (glob_text(CTEST_SUITE_NAME, t->ssname) == 1)
        )
        && (
            // No test filter
            CTEST_TEST_EXPRESSION == NULL || CTEST_TEST_EXPRESSION[0] == '\0'
            // ... Or a matching test name
            || (glob_text(CTEST_TEST_EXPRESSION, t->ttname) == 1)
        )
    );
}

static void color_print(const char* color, const char* text) {
    if (CTEST_COLOR_OUTPUT)
        printf("%s%s" ANSI_NORMAL "\n", color, text);
    else
        printf("%s\n", text);
}

#ifdef CTEST_SEGFAULT
#include <signal.h>
static void sighandler(int signum)
{
    const char msg_color[] = ANSI_BRED "[SIGSEGV: Segmentation fault]" ANSI_NORMAL "\n";
    const char msg_nocolor[] = "[SIGSEGV: Segmentation fault]\n";

    const char* msg = CTEST_COLOR_OUTPUT ? msg_color : msg_nocolor;

#if defined(__unix__) || defined(__APPLE__)
    int stdout_file_descriptor = STDOUT_FILENO;
#elif defined(WIN32) || defined(_WIN32)
    int stdout_file_descriptor = _fileno(stdout);
#endif

    write(stdout_file_descriptor, msg, (unsigned int)strlen(msg));

    /* "Unregister" the signal handler and send the signal back to the process
     * so it can terminate as expected */
    signal(signum, SIG_DFL);
#if !defined(_WIN32) || defined(__CYGWIN__)
    kill(getpid(), signum);
#endif
}
#endif

int ctest_main(int argc, const char *argv[]);

/// Add `'*'` as a suffix to `text` in a new `result` buffer if it's needed.
///
/// @param text The string which may or may not end in `'*'`. e.g. `"foo"`.
/// @param result An return argument to save the new string out to.
///
void append_wild_char_suffix(const char *text, char *result) {
    int len = strlen(text);
    strcpy(result, text);

    if (len == 0 || text[len - 1] != '*') {
        strcat(result, "*");
    }
}

__attribute__((no_sanitize_address)) int ctest_main(int argc, const char *argv[])
{
    static int total = 0;
    static int num_ok = 0;
    static int num_errored = 0;
    static int num_fail = 0;
    static int num_skip = 0;
    static int idx = 1;
    static ctest_filter_func filter = suite_all;

#ifdef CTEST_SEGFAULT
    signal(SIGSEGV, sighandler);
#endif

    // TODO: Using suite_filter twice here is unclean. Separate to a different function, later
    if (argc == 2) {
        append_wild_char_suffix(argv[1], CTEST_SUITE_NAME);
        filter = suite_filter;
    } else if (argc == 3) {
        append_wild_char_suffix(argv[1], CTEST_SUITE_NAME);
        append_wild_char_suffix(argv[2], CTEST_TEST_EXPRESSION);
        filter = suite_filter;
    }
#ifdef CTEST_NO_COLORS
    CTEST_COLOR_OUTPUT = 0;
#else
    CTEST_COLOR_OUTPUT = isatty(1);
#endif
    clock_t t1 = clock();

    struct ctest* ctest_begin = &CTEST_IMPL_TNAME(suite, test);
    struct ctest* ctest_end = &CTEST_IMPL_TNAME(suite, test);
    // find begin and end of section by comparing magics
    while (1) {
        const struct ctest* t = ctest_begin-1;
        if (t->magic != CTEST_IMPL_MAGIC) break;
        ctest_begin--;
    }
    while (1) {
        const struct ctest* t = ctest_end+1;
        if (t->magic != CTEST_IMPL_MAGIC) break;
        ctest_end++;
    }
    ctest_end++;    // end after last one

    static struct ctest* test;
    for (test = ctest_begin; test != ctest_end; test++) {
        if (test == &CTEST_IMPL_TNAME(suite, test)) continue;
        if (filter(test)) total++;
    }

    for (test = ctest_begin; test != ctest_end; test++) {
        if (test == &CTEST_IMPL_TNAME(suite, test)) continue;
        if (filter(test)) {
            CTEST_ERROR_BUFFER[0] = 0;
            CTEST_ERROR_SIZE = MSG_SIZE-1;
            CTEST_ERROR_MESSAGE = CTEST_ERROR_BUFFER;
#ifdef CTEST_COLOR_OK
            printf("TEST %d/%d ", idx, total);
            printf("%s%s:%s " ANSI_NORMAL, ANSI_WHITE, test->ssname, test->ttname);
#else
            printf("TEST %d/%d %s:%s ", idx, total, test->ssname, test->ttname);
#endif
            fflush(stdout);
            if (test->skip) {
                color_print(ANSI_BYELLOW, "[SKIPPED]");
                num_skip++;
            } else {
                int has_exception = 0;  // Only used in C++
                int result = setjmp(CTEST_ERROR_);
                if (result == 0) {
                    if (test->setup && *test->setup) (*test->setup)(test->data);
                    if (test->data) {
#ifdef __cplusplus
                        try {
                            test->run.unary(test->data);
                        }
                        catch(std::exception const &error) {
                            has_exception = 1;

#ifdef CTEST_COLOR_OK
                            printf(
                                "%s[FAIL]\nException caught:\n%s\n" ANSI_NORMAL,
                                ANSI_BRED,
                                error.what()
                            );
#else
                            printf(
                                "[FAIL]\nException caught:\n%s\n",
                                error.what()
                            );
#endif
                        }
#else
                        test->run.unary(test->data);
#endif
                    } else {
#ifdef __cplusplus
                        try {
                            test->run.nullary();
                        }
                        catch(std::exception const &error) {
                            has_exception = 1;

#ifdef CTEST_COLOR_OK
                            printf(
                                "%s[FAIL]\nException caught:\n%s\n" ANSI_NORMAL,
                                ANSI_BRED,
                                error.what()
                            );
#else
                            printf(
                                "[FAIL]\nException caught:\n%s\n",
                                error.what()
                            );
#endif
                        }
#else
                        test->run.nullary();
#endif
                    }
                    if (test->teardown && *test->teardown) (*test->teardown)(test->data);

                    if (!has_exception) {
#ifdef CTEST_COLOR_OK
                        color_print(ANSI_BGREEN, "[OK]");
#else
                        printf("[OK]\n");
#endif
                        num_ok++;
                    } else {
                        num_errored++;
                    }

                } else {
#ifdef CTEST_COLOR_OK
                    color_print(ANSI_BRED, "[FAIL]");
#else
                    printf("[FAIL]\n");
#endif
                    num_fail++;
                }
                if (CTEST_ERROR_SIZE != MSG_SIZE-1) printf("%s", CTEST_ERROR_BUFFER);
            }
            idx++;
        }
    }
    clock_t t2 = clock();

    const char* color = (num_fail) ? ANSI_BRED : ANSI_GREEN;
    char results[92];
    snprintf(results, sizeof(results), "RESULTS: %d tests (%d ok, %d failed, %d errored, %d skipped) ran in %.1f ms",
             total, num_ok, num_fail, num_errored, num_skip, (double)(t2 - t1)*1000.0/CLOCKS_PER_SEC);
#ifdef CTEST_COLOR_OK
    color_print(color, results);
#else
    UNUSED(color);
    printf(results);
    printf("\n");
#endif
    return num_fail;
}

#endif

#ifdef __cplusplus
}
#endif

#if defined(__clang__)
// https://clang.llvm.org/docs/UsersManual.html#controlling-diagnostics-via-pragmas
#pragma clang diagnostic pop
#elif defined(__GNUC__) || defined(__GNUG__)
#pragma GCC diagnostic pop
#elif defined(_MSC_VER)
// TODO: Finish this one?
#endif

#endif

