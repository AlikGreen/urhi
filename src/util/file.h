#pragma once
#include <string>

namespace NRHI
{
class File
{
public:
    static std::string readFileText(const char* filePath);
    static std::string getFileExtension(const std::string& filePath);
};
}
