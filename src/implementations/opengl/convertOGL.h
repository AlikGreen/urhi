#pragma once
#include <typeinfo>
#include <glad/glad.h>

#include "enums/mipmapFilter.h"
#include "enums/pixelFormat.h"
#include "enums/pixelType.h"
#include "enums/shaderType.h"
#include "enums/textureFilter.h"
#include "enums/textureType.h"
#include "enums/textureWrap.h"
#include "input/keyCodes.h"

namespace Neon::RHI
{
class ConvertOGL
{
public:
    static GLenum typeinfoToGL(const std::type_info* type);
    static GLenum shaderTypeToGL(ShaderType type);
    static GLenum pixelFormatToGL(PixelFormat format);
    static GLenum pixelFormatToGLType(PixelFormat format);
    static uint32_t getComponentCount(const std::type_info* type);
    static KeyCode keyCodeFromGLFW(int keyCode);
    static KeyMod keyModFromGLFW(int mod);
    static MouseButton mouseButtonFromGLFW(int button);
    static GLenum textureWrapToGL(TextureWrap wrap);
    static GLenum textureFilterCombineToGL(TextureFilter filter, MipmapFilter mipmapFilter);
    static GLenum pixelTypeToGL(PixelType type);
    static GLenum textureTypeToGLType(TextureType type);
};
}
