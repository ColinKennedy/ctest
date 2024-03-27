#include <sstream>
#include <stdio.h>

#define CTEST_MAIN

#define CTEST_SEGFAULT

#include <ctest.h>

#include "cli.hpp"
#include "parser.hpp"
#include "pather.hpp"


CTEST(simple, empty_no_tests)
{
    auto const raw = cli::execute_command(pather::make_absolute("empty"));

    ASSERT_EQUAL(cli::ExitCode_SUCCESS, raw.exit_code);
}


CTEST(simple, single_assert)
{
    auto const raw = cli::execute_command(pather::make_absolute("single"));

    ASSERT_EQUAL(cli::ExitCode_SUCCESS, raw.exit_code);
}


CTEST(arguments, no_arguments)
{
    auto const raw = cli::execute_command(pather::make_absolute("arguments"));
    auto const results = parser::parse_std_out(raw.std_out);

    ASSERT_EQUAL(4, results.cases.size());
    auto const test_result = results.cases[0];
    ASSERT_EQUAL(parser::TestStatus_OK, test_result.return_status);
}


CTEST(arguments, suite_argument)
{
    auto const raw = cli::execute_command(pather::make_absolute("arguments suitey"));
    auto const results = parser::parse_std_out(raw.std_out);
    auto& cases = results.cases;

    ASSERT_EQUAL(3, cases.size());
    ASSERT_TRUE(
        std::all_of(
            cases.begin(),
            cases.end(),
            [](auto test){ return test.return_status == parser::TestStatus_OK; }
        )
    );
}


CTEST(arguments, suite_and_name_argument)
{
    auto const raw = cli::execute_command(pather::make_absolute("arguments suitey test1"));
    auto const results = parser::parse_std_out(raw.std_out);
    auto& cases = results.cases;

    ASSERT_EQUAL(1, cases.size());
    auto const test_result = cases[0];
    ASSERT_EQUAL(parser::TestStatus_OK, test_result.return_status);
}


CTEST(invalids, unknown_suite)
{
    auto const raw = cli::execute_command(pather::make_absolute("arguments does_not_exist"));
    auto const results = parser::parse_std_out(raw.std_out);
    auto& cases = results.cases;

    ASSERT_EQUAL(0, cases.size());
}


CTEST(invalids, unknown_test)
{
    auto const raw = cli::execute_command(pather::make_absolute("arguments suitey missing"));
    auto const results = parser::parse_std_out(raw.std_out);
    auto& cases = results.cases;

    ASSERT_EQUAL(0, cases.size());
}


CTEST(miscellaneous, mytests)
{
    auto const raw = cli::execute_command(pather::make_absolute("mytests"));
    auto const results = parser::parse_std_out(raw.std_out);

    ASSERT_EQUAL(35, results.number_total);
    ASSERT_EQUAL(11, results.number_ok);
    ASSERT_EQUAL(22, results.number_failed);
    ASSERT_EQUAL(2, results.number_skipped);
}


int main(int argc, const char *argv[]) { return ctest_main(argc, argv); }
