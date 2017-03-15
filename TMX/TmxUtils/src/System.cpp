/*
 * System.cpp
 *
 *  Created on: Sep 28, 2016
 *      Author: ivp
 */

#include "System.h"

using namespace std;

namespace tmx {
namespace utils {

std::string System::ExecCommand(const std::string& command)
{
    std::string result, file;
    FILE* pipe{popen(command.c_str(), "r")};
    char buffer[256];

    while (fgets(buffer, sizeof(buffer), pipe) != nullptr)
    {
        file = buffer;
        result += file.substr(0, file.size() - 1);
    }

    pclose(pipe);

    return result;
}

} /* namespace utils */
} /* namespace tmx */
