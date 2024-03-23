#ifndef PARSER_HPP
#define PARSER_HPP

#include <regex>
#include <sstream>  // std::stringstream
#include <string>
#include <vector>

namespace parser {

namespace details
{
    std::regex const TEST_REGEX {"TEST \\d+/\\d+ (\\w+):(\\w+)"};
    std::regex const RESULTS_REGEX {
        "RESULTS: (\\d+) tests \\((\\d+) ok, (\\d+) failed, (\\d+) skipped\\) ran in (\\d+\\.\\d+) ms"
    };
}

enum TestStatus
{
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

    unsigned int number_ok;
    unsigned int number_failed;
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
    bool in_test {false};

    std::smatch matches;

    std::string suite_name;
    std::string test_name;
    TestStatus return_status;

    unsigned int number_total;
    unsigned int number_ok;
    unsigned int number_failed;
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
            in_test = true;
            suite_name = matches[1];
            test_name = matches[2];

            continue;
        }

        if (in_test)
        {
            if (buffer == "[OK]")
            {
                return_status = TestStatus_OK;
                add_to_cases();

                continue;
            }
            else if (buffer == "[FAILED]")
            {
                return_status = TestStatus_FAILED;
                add_to_cases();

                continue;
            }
            else if (buffer == "[SKIPPED]")
            {
                return_status = TestStatus_SKIPPED;
                add_to_cases();

                continue;
            }
        }

        if (in_test && std::regex_search(buffer, matches, details::RESULTS_REGEX))
        {
            number_total = std::stoi(matches[1]);
            number_ok = std::stoi(matches[2]);
            number_failed = std::stoi(matches[3]);
            number_skipped = std::stoi(matches[4]);
            total_time = std::stoi(matches[5]);

            in_test = false;

            output.cases = cases;
            output.number_ok = number_ok;
            output.number_failed = number_failed;
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
