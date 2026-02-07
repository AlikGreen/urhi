#pragma once
#include <string>

namespace urhi
{
class String
{
public:
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
};
}
