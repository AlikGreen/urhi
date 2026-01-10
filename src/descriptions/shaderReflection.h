#pragma once
#include <cstdint>
#include <optional>
#include <string>
#include <vector>

namespace Neon::RHI
{
struct ShaderReflection
{
    enum class ResourceType
    {
        ConstantBuffer,
        Sampler,
        Image,
    };

    enum class DataType
    {
        Float, Float2, Float3, Float4,
        Int, Int2, Int3, Int4,
        UInt, UInt2, UInt3, UInt4,
        Mat3, Mat4,
        Struct
    };

    struct Member
    {
        std::string name;
        DataType type;
        uint32_t offset;
        uint32_t size;
        uint32_t arrayCount;
        std::vector<Member> members;
    };

    struct Resource
    {
        std::string name;
        ResourceType type;
        uint32_t arrayCount;

        std::vector<Member> members;
        uint32_t totalSize;
    };

    struct VertexAttribute
    {
        std::string name;
        DataType type;
        uint32_t location;
        uint32_t offset;
    };

    struct ComputeInfo
    {
        uint32_t workgroupSizeX;
        uint32_t workgroupSizeY;
        uint32_t workgroupSizeZ;
    };


    std::vector<Resource> resources;
    std::vector<VertexAttribute> vertexInputs;
    std::optional<ComputeInfo> computeInfo;
};
}
