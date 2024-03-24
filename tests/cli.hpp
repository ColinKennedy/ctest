#ifndef CLI_HPP
#define CLI_HPP

#include <array> // std::array
#include <cstdio> // popen, pclose
#include <stdlib.h> // WIFEXITED, WEXITSTATUS
#include <iostream> // std::cout, std::cerr
#include <string>

namespace cli
{
    enum ExitCode
    {
        ExitCode_BAD_EXIT,
        ExitCode_PCLOSE_FAILED,
        ExitCode_POPEN_FAILED,
        ExitCode_SUCCESS,
    };

    struct Result {
        ExitCode exit_code;
        std::string std_out;
    };

#if defined(_WIN32)
int WIFEXITED(int status) { return (status == -1) ? 1 : 0; }
int WEXITSTATUS(int status) { return (status == 0) ? 1 : 0; }
#endif

#ifdef __MSC_VER
#define popen(...) _popen(__VA_ARGS__)
#define pclose(...) _pclose(__VA_ARGS__)
#endif


    Result execute_command(std::string const command)
    {
        FILE* pipe = popen(command.c_str(), "r");

        if (!pipe) {
            std::cerr << "popen() failed!" << std::endl;

            return Result{ExitCode_POPEN_FAILED, ""};
        }

        std::array<char, 128> buffer;
        std::string std_out;

        while (
            std::fgets(buffer.data(), static_cast<int>(buffer.size()), pipe) != nullptr
        )
        {
            std_out += buffer.data();
        }

        int status = pclose(pipe);

        if (status == -1) {
            std::cerr << "pclose() failed!" << std::endl;

            return Result{ExitCode_PCLOSE_FAILED, ""};
        }

        if (WIFEXITED(status) && WEXITSTATUS(status) != 0) {
            return Result{ExitCode_BAD_EXIT, std_out};
        }

        return Result{ExitCode_SUCCESS, std_out};
    }
}

#endif
