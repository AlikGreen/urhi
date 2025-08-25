#include "file.h"

#include <fstream>
#include <sstream>
#include <filesystem>

namespace NRHI
{
    std::string File::readFileText(const char *filePath)
    {
        std::ifstream file(filePath);
        std::stringstream buffer;
        buffer << file.rdbuf();
        return buffer.str();
    }

    std::string File::getFileExtension(const std::string &filePath)
    {
        return std::filesystem::path(filePath).extension().string();
    }
}
