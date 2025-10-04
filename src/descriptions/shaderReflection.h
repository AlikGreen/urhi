#pragma once
#include <cstdint>
#include <string>
#include <vector>

namespace Neon::RHI
{
    enum class ShaderBaseType
    {
        Unknown,
        Float,
        Int,
        UInt,
        Bool,
        Double
    };

    struct ShaderUniformBlockProperty
    {
        std::string name;
        uint32_t offset;
        uint32_t size;
        ShaderBaseType baseType;
        uint32_t vectorComponentCount;
        uint32_t matrixColumnCount;
        bool isArray;
        uint32_t arraySize;
    };

    struct ShaderUniformBlock
    {
        std::string blockName;
        uint32_t blockSize;
        std::vector<ShaderUniformBlockProperty> members;
    };

    struct ShaderSampler
    {
        std::string name;
        uint32_t set = 0;
        uint32_t binding = 0;
        uint32_t arraySize = 0;
    };

    struct ShaderImage
    {
        std::string name;
        uint32_t set = 0;
        uint32_t binding = 0;
        bool writeable = true;
    };



    struct ShaderReflection
    {
        std::vector<ShaderUniformBlock> uniformBlocks{};
        std::vector<ShaderSampler> samplers{};
        std::vector<ShaderImage> images{};
    };
    ;
}
