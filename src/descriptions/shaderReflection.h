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

    struct ShaderUniformBlockMember
    {
        std::string name;
        uint32_t offset;
        uint32_t size;
        ShaderBaseType baseType;
        // uint32_t vectorComponentCount;
        // uint32_t matrixColumnCount;
        // bool isArray;
        // uint32_t arraySize;
    };

    struct ShaderUniformBlock
    {
        std::string name;
        uint32_t size;
        std::vector<ShaderUniformBlockMember> members{};
    };

    struct ShaderSampler
    {
        std::string name;
    };

    struct ShaderImage
    {
        std::string name;
        bool writeable;
    };



    struct ShaderReflection
    {
        std::vector<ShaderUniformBlock> uniformBlocks{};
        std::vector<ShaderSampler> samplers{};
        std::vector<ShaderImage> images{};
    };
    ;
}
