#ifndef PARSER_HPP
#define PARSER_HPP

#include <regex>
#include <sstream>  // std::stringstream
#include <string>
#include <vector>

namespace parser {

namespace details
{
    // TODO: Remove the need for TEST_REGEX, later
    std::regex const TEST_REGEX {"TEST \\d+/\\d+ (\\w+):(\\w+).*"};
    std::regex const RESULTS_REGEX {
        "RESULTS: (\\d+) tests \\((\\d+) ok, (\\d+) failed, (\\d+) errored, (\\d+) skipped\\) ran in (\\d+\\.\\d+) ms"
    };

    bool ends_with(std::string const &full, std::string const &end) {
        if (full.length() < end.length())
        {
            return false;
        }

        return full.compare(full.length() - end.length(), end.length(), end) == 0;
    }

    bool starts_with(std::string const &full, std::string const &start) {
        if (full.length() < start.length())
        {
            return false;
        }

        return full.compare(0, start.length(), start) == 0;
    }

    bool is_errored(std::string const &text) { return ends_with(text, "[ERRORED]"); }

    bool is_failed(std::string const &text) { return ends_with(text, "[FAILED]"); }

    bool is_okay(std::string const &text) { return ends_with(text, "[OK]"); }

    bool is_skipped(std::string const &text) { return ends_with(text, "[SKIPPED]"); }

    bool is_testing_header(std::string const &text) { return starts_with(text, "TEST "); }
}

enum TestStatus
{
    TestStatus_ERRORED,
    TestStatus_FAILED,
    TestStatus_OK,
    TestStatus_SKIPPED,
};

struct SingleTestCase
{
    std::string suite_name;
    std::string test_name;
    TestStatus return_status;
};

struct TestResults
{
    std::vector<SingleTestCase> cases;

    unsigned int number_errored;
    unsigned int number_failed;
    unsigned int number_ok;
    unsigned int number_skipped;
    unsigned int number_total;
    unsigned int total_time;
};

TestResults parse_std_out(std::string const text)
{
    std::stringstream stream(text);
    std::string buffer;
    std::vector<SingleTestCase> cases;
    TestResults output;
    std::string previous;

    std::smatch matches;

    std::string suite_name;
    std::string test_name;
    TestStatus return_status;

    unsigned int number_total;
    unsigned int number_errored;
    unsigned int number_failed;
    unsigned int number_ok;
    unsigned int number_skipped;
    unsigned int total_time;

    // TODO: Replace with emplace_back
    auto add_to_cases = [&suite_name, &test_name, &return_status, &cases](){
        cases.push_back(SingleTestCase{suite_name, test_name, return_status});
    };

    while(std::getline(stream, buffer, '\n'))
    {
        previous = buffer;

        if (std::regex_search(buffer, matches, details::TEST_REGEX))
        {
            suite_name = matches[1];
            test_name = matches[2];

            if (details::is_okay(buffer))
            {
                return_status = TestStatus_OK;
                add_to_cases();
            }

            if (details::is_failed(buffer))
            {
                return_status = TestStatus_FAILED;
                add_to_cases();
            }
            else if (details::is_skipped(buffer))
            {
                return_status = TestStatus_SKIPPED;
                add_to_cases();
            }

            continue;
        }

        if (std::regex_search(buffer, matches, details::RESULTS_REGEX))
        {
            number_total = std::stoi(matches[1]);
            number_ok = std::stoi(matches[2]);
            number_failed = std::stoi(matches[3]);
            number_errored = std::stoi(matches[4]);
            number_skipped = std::stoi(matches[5]);
            total_time = std::stoi(matches[6]);

            output.cases = cases;
            output.number_errored = number_errored;
            output.number_failed = number_failed;
            output.number_ok = number_ok;
            output.number_skipped = number_skipped;
            output.number_total = number_total;
            output.total_time = total_time;

            break;
        }
    }

    return output;
}

}

#endif
