#ifndef PATHER_H
#define PATHER_H

namespace pather {

#ifdef _WIN32
#include <windows.h>

char const PATH_SEPARATOR = '\\';

std::string get_current_directory()
{
    char buffer[MAX_PATH];
    GetModuleFileNameA(NULL, buffer, MAX_PATH);

    std::string path(buffer);

    return path.substr(0, path.find_last_of(PATH_SEPARATOR));
}
#else
#include <unistd.h>
#include <limits.h>

char const PATH_SEPARATOR = '/';

std::string get_current_directory()
{
    char buffer[PATH_MAX];
    ssize_t len = readlink("/proc/self/exe", buffer, sizeof(buffer)-1);

    if (len == -1) {
        return std::string{};
    }

    buffer[len] = '\0';

    std::string path(buffer);

    return path.substr(0, path.find_last_of(PATH_SEPARATOR));
}

#endif

std::string const CURRENT_DIRECTORY = get_current_directory();

std::string make_absolute(std::string const name)
{
    return CURRENT_DIRECTORY + PATH_SEPARATOR + name;
}

}

#endif
