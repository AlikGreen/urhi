#pragma once
#include <slang.h>

#include "descriptions/shaderReflection.h"
#include "enums/shaderStage.h"

namespace urhi::slang
{
class Convert
{
public:
    static ShaderReflection::DataType dataType(::slang::TypeLayoutReflection *typeLayout);
    static ShaderStage shaderStage(SlangStage slangStage);
    static TextureType textureType(SlangResourceShape shape);
    static uint32_t dataTypeByteWidth(ShaderReflection::DataType type);
};
}
