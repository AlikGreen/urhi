#include "slangConvert.h"

namespace urhi
{
    refl::DataType SlangConvert::dataType(slang::TypeLayoutReflection* typeLayout)
    {
        slang::TypeReflection* type = typeLayout->getType();
        slang::TypeReflection::ScalarType scalarType;
        int elementCount = 1;

        if (type->getKind() == slang::TypeReflection::Kind::Matrix)
        {
            const uint32_t rows = type->getRowCount();
            const uint32_t cols = type->getColumnCount();

            if (rows == 4 && cols == 4)
                return refl::DataType::Float4x4;
            if (rows == 3 && cols == 3)
                return refl::DataType::Float3x3;

            return refl::DataType::Unknown;
        }

        if (type->getKind() == slang::TypeReflection::Kind::Vector)
        {
            elementCount = static_cast<int>(type->getElementCount());
            scalarType = type->getElementType()->getScalarType();
        }
        else
        {
            scalarType = type->getScalarType();
        }

        if (scalarType == slang::TypeReflection::ScalarType::Float32)
        {
            switch (elementCount)
            {
                case 1: return refl::DataType::Float;
                case 2: return refl::DataType::Float2;
                case 3: return refl::DataType::Float3;
                case 4: return refl::DataType::Float4;
                default: break;
            }
        }
        else if (scalarType == slang::TypeReflection::ScalarType::Int32)
        {
            switch (elementCount)
            {
                case 1: return refl::DataType::Int;
                case 2: return refl::DataType::Int2;
                case 3: return refl::DataType::Int3;
                case 4: return refl::DataType::Int4;
                default: break;
            }
        }
        else if (scalarType == slang::TypeReflection::ScalarType::UInt32)
        {
            switch (elementCount)
            {
                case 1: return refl::DataType::UInt;
                case 2: return refl::DataType::UInt2;
                case 3: return refl::DataType::UInt3;
                case 4: return refl::DataType::UInt4;
                default: break;
            }
        }

        return refl::DataType::Unknown;
    }

    ShaderStage SlangConvert::shaderStage(const SlangStage slangStage)
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

    TextureType SlangConvert::textureType(const SlangResourceShape shape)
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

    uint32_t SlangConvert::dataTypeByteWidth(const refl::DataType type)
    {
        switch (type)
        {
            case refl::DataType::Float:
            case refl::DataType::Int:
            case refl::DataType::UInt:
                return 4;

            case refl::DataType::Float2:
            case refl::DataType::Int2:
            case refl::DataType::UInt2:
                return 8;

            case refl::DataType::Float3:
            case refl::DataType::Int3:
            case refl::DataType::UInt3:
                return 12;

            case refl::DataType::Float4:
            case refl::DataType::Int4:
            case refl::DataType::UInt4:
                return 16;

            case refl::DataType::Float3x3: return 36;
            case refl::DataType::Float4x4: return 64;

            default:
                return 0;
        }
    }
}
