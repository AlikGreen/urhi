#pragma once
#include <string>

namespace NRHI
{
class String
{
public:
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
};
}
