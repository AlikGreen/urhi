#include "convert.h"

namespace urhi::slang
{
    using namespace ::slang;
    ShaderReflection::DataType Convert::dataType(TypeLayoutReflection* typeLayout)
    {
        TypeReflection* type = typeLayout->getType();
        TypeReflection::ScalarType scalarType;
        int elementCount = 1;

        if (type->getKind() == TypeReflection::Kind::Vector)
        {
            elementCount = static_cast<int>(type->getElementCount());
            scalarType = type->getElementType()->getScalarType();
        }
        else
        {
            scalarType = type->getScalarType();
        }

        if (scalarType == TypeReflection::ScalarType::Float32)
        {
            switch (elementCount)
            {
                case 1: return ShaderReflection::DataType::Float;
                case 2: return ShaderReflection::DataType::Float2;
                case 3: return ShaderReflection::DataType::Float3;
                case 4: return ShaderReflection::DataType::Float4;
                default: break;
            }
        }
        else if (scalarType == TypeReflection::ScalarType::Int32)
        {
            switch (elementCount)
            {
                case 1: return ShaderReflection::DataType::Int;
                case 2: return ShaderReflection::DataType::Int2;
                case 3: return ShaderReflection::DataType::Int3;
                case 4: return ShaderReflection::DataType::Int4;
                default: break;
            }
        }
        else if (scalarType == TypeReflection::ScalarType::UInt32)
        {
            switch (elementCount)
            {
                case 1: return ShaderReflection::DataType::UInt;
                case 2: return ShaderReflection::DataType::UInt2;
                case 3: return ShaderReflection::DataType::UInt3;
                case 4: return ShaderReflection::DataType::UInt4;
                default: break;
            }
        }

        return ShaderReflection::DataType::Unknown;
    }

    ShaderStage Convert::shaderStage(const SlangStage slangStage)
    {
        switch (slangStage)
        {
            case SLANG_STAGE_VERTEX:   return ShaderStage::Vertex;
            case SLANG_STAGE_FRAGMENT: return ShaderStage::Fragment;
            case SLANG_STAGE_COMPUTE:  return ShaderStage::Compute;
            case SLANG_STAGE_GEOMETRY: return ShaderStage::Geometry;
            // case SLANG_STAGE_HULL:     return ShaderStage::TessControl;
            // case SLANG_STAGE_DOMAIN:   return ShaderStage::TessEvaluation;
            default:                   return ShaderStage::Vertex;
        }
    }

    TextureType Convert::textureType(const SlangResourceShape shape)
    {
        const unsigned baseShape = shape & SLANG_RESOURCE_BASE_SHAPE_MASK;

        switch (baseShape)
        {
            case SLANG_TEXTURE_1D:   return TextureType::Texture1D;
            case SLANG_TEXTURE_2D:   return TextureType::Texture2D;
            case SLANG_TEXTURE_3D:   return TextureType::Texture3D;
            case SLANG_TEXTURE_CUBE: return TextureType::TextureCube;
            default:                 return TextureType::Texture2D;
        }
    }

    uint32_t Convert::dataTypeByteWidth(const ShaderReflection::DataType type)
    {
        switch (type)
        {
            case ShaderReflection::DataType::Float:
            case ShaderReflection::DataType::Int:
            case ShaderReflection::DataType::UInt:
                return 4;

            case ShaderReflection::DataType::Float2:
            case ShaderReflection::DataType::Int2:
            case ShaderReflection::DataType::UInt2:
                return 8;

            case ShaderReflection::DataType::Float3:
            case ShaderReflection::DataType::Int3:
            case ShaderReflection::DataType::UInt3:
                return 12;

            case ShaderReflection::DataType::Float4:
            case ShaderReflection::DataType::Int4:
            case ShaderReflection::DataType::UInt4:
                return 16;

            case ShaderReflection::DataType::Mat3: return 36; // 3x3 * 4 bytes
            case ShaderReflection::DataType::Mat4: return 64; // 4x4 * 4 bytes

            default: return 0;
        }
    }
}