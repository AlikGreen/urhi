#pragma once
#include <glad/gl.h>

#include "descriptions/shaderReflection.h"
#include "enums/bufferUsage.h"
#include "enums/compareOp.h"
#include "enums/mipmapFilter.h"
#include "enums/pixelFormat.h"
#include "enums/primitiveType.h"
#include "enums/shaderStage.h"
#include "enums/textureFilter.h"
#include "input/keyCodes.h"

namespace urhi
{
    enum class AddressMode;
}

namespace urhi
{
class GlDevice;
class GlConvert
{
public:
    static KeyCode     keyCode(int glfwKey);
    static KeyMod      keyMod(int glfwMods);
    static MouseButton mouseButton(int glfwButton);

    static GLenum pixelFormat(PixelFormat format);
    static GLenum internalFormat(PixelFormat format);
    static GLenum pixelType(PixelFormat format);

    static GLenum bufferUsage(BufferUsage usage);
    static GLenum bufferTarget(BufferUsage usage);

    static GLenum primitiveType(PrimitiveType type);

    static GLint componentCount(ShaderReflection::DataType type);
    static GLenum vertexBaseType(ShaderReflection::DataType type);
    static bool isIntegerType(ShaderReflection::DataType type);

    static GLenum filter(TextureFilter texFilter, MipmapFilter mipFilter);
    static GLenum filter(TextureFilter texFilter);

    static GLenum wrapMode(AddressMode mode);
    static GLenum compareFunc(CompareOp op);

    static GLenum shaderStage(ShaderStage stage);

    static GLenum textureType(TextureType type);
};
}
