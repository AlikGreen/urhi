#pragma once
#include <cstdint>
#include <optional>
#include <string>
#include <vector>
#include <glm/glm.hpp>

#include "enums/resourceAccess.h"
#include "enums/shaderStage.h"
#include "enums/textureType.h"

namespace urhi
{
struct ShaderReflection
{
    enum class ResourceType
    {
        StorageBuffer,
        ConstantBuffer,
        Texture,
        Sampler,
        StorageImage,
    };

    enum class DataType
    {
        Float, Float2, Float3, Float4,
        Int, Int2, Int3, Int4,
        UInt, UInt2, UInt3, UInt4,
        Mat3, Mat4,
        Struct,
        Unknown
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
        ResourceType type = ResourceType::ConstantBuffer;
        uint32_t set = 0;
        uint32_t binding = 0;
        uint32_t arrayCount = 1;
        ResourceAccess access = ResourceAccess::ReadOnly;

        uint32_t totalSize = 0;
        std::vector<Member> members;

        TextureType dimension = TextureType::Texture2D;

        [[nodiscard]] bool isBuffer() const
        {
            return type == ResourceType::ConstantBuffer ||
                   type == ResourceType::StorageBuffer;
        }

        [[nodiscard]] bool isTexture() const
        {
            return type == ResourceType::Texture ||
                   type == ResourceType::StorageImage;
        }

        [[nodiscard]] const Member* findMember(const std::string& memberName) const
        {
            for (const auto& m : members)
            {
                if (m.name == memberName) return &m;
            }
            return nullptr;
        }
    };

    struct PushConstant
    {
        std::string name;
        uint32_t size;
        uint32_t offset;
    };

    struct VertexAttribute
    {
        std::string name;
        DataType type;
        uint32_t location;
        uint32_t offset;
    };


    struct VertexBinding
    {
        uint32_t binding = 0;
        uint32_t stride = 0;
        std::string structName;
        std::vector<VertexAttribute> attributes;
    };


    struct ComputeInfo
    {
        uint32_t workgroupSizeX = 1;
        uint32_t workgroupSizeY = 1;
        uint32_t workgroupSizeZ = 1;
    };

    std::vector<Resource> resources;
    std::vector<VertexBinding> vertexBindings;
    std::optional<PushConstant> pushConstant;
    std::optional<ComputeInfo> computeInfo;
};

}
