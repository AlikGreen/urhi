#pragma once
#include <array>
#include <cstdint>
#include <optional>
#include <string>
#include <vector>

#include "enums/resourceAccess.h"

namespace urhi::refl
{
enum class ResType  { CBuffer, Buffer, Texture, Sampler, Image };

enum class DataType
{
    Float, Float2, Float3, Float4,
    Int,   Int2, Int3, Int4,
    UInt,  UInt2, UInt3, UInt4,
    Float3x3, Float4x4, Struct, Unknown
};

struct Member
{
    std::string name;
    DataType    type   = DataType::Unknown;
    uint32_t    offset = 0;
    uint32_t    size   = 0;
};

struct Resource
{
    std::string         name;
    ResType             type    = ResType::CBuffer;
    ResourceAccess      access  = ResourceAccess::ReadWrite;
    uint32_t            set     = 0;
    uint32_t            binding = 0;
    uint32_t            count   = 1;    // >1 for arrays
    uint32_t            size    = 0;    // total size for buffers
    std::vector<Member> members;
};

struct PushConst
{
    uint32_t            size = 0;
    std::vector<Member> members;
};

struct VertexAttr
{
    std::string name;
    DataType    type;
    uint32_t    location;
    uint32_t    offset;
    uint32_t    stride;
    uint32_t    binding;
};

struct Data
{
    std::vector<Resource>    resources;
    std::vector<VertexAttr>  vertexAttrs;
    std::optional<PushConst> pushConst;
    std::array<uint32_t, 3>  workgroupSize = { 1, 1, 1 };
};
}