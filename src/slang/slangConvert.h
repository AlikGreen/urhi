#pragma once
#include <slang.h>

#include "descriptions/shaderReflection.h"
#include "enums/shaderStage.h"
#include "enums/textureType.h"

namespace urhi
{
class SlangConvert
{
public:
    static refl::DataType dataType(slang::TypeLayoutReflection *typeLayout);
    static ShaderStage shaderStage(SlangStage slangStage);
    static TextureType textureType(SlangResourceShape shape);
    static uint32_t dataTypeByteWidth(refl::DataType type);
};
}
