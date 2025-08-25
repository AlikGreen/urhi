#include "string.h"
#include <algorithm>

namespace NRHI
{
    std::string String::trim(const std::string& str)
    {
        const size_t start = str.find_first_not_of(" \t\r\n");
        if (start == std::string::npos) return "";
        const size_t end = str.find_last_not_of(" \t\r\n");
        return str.substr(start, end - start + 1);
    }

    // Function to convert string to lowercase
    std::string String::toLower(const std::string& str)
    {
        std::string result = str;
        std::ranges::transform(result, result.begin(),
                               [](unsigned char c) { return std::tolower(c); });
        return result;
    }
}