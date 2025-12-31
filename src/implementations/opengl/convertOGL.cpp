#include "convertOGL.h"
#include <glm/glm.hpp>

#include "enums/pixelLayout.h"
#include "enums/shaderType.h"
#include "GLFW/glfw3.h"

namespace Neon::RHI
{
    GLenum ConvertOGL::typeinfoToGL(const std::type_info *type)
    {
        if (!type) return GL_INVALID_ENUM;

        static const std::unordered_map<size_t, GLenum> typeMap =
        {
            {typeid(float).hash_code(), GL_FLOAT},
            {typeid(double).hash_code(), GL_DOUBLE},

            {typeid(int8_t).hash_code(), GL_BYTE},
            {typeid(signed char).hash_code(), GL_BYTE},
            {typeid(int16_t).hash_code(), GL_SHORT},
            {typeid(short).hash_code(), GL_SHORT},
            {typeid(int32_t).hash_code(), GL_INT},
            {typeid(int).hash_code(), GL_INT},

            {typeid(uint8_t).hash_code(), GL_UNSIGNED_BYTE},
            {typeid(unsigned char).hash_code(), GL_UNSIGNED_BYTE},
            {typeid(uint16_t).hash_code(), GL_UNSIGNED_SHORT},
            {typeid(unsigned short).hash_code(), GL_UNSIGNED_SHORT},
            {typeid(uint32_t).hash_code(), GL_UNSIGNED_INT},
            {typeid(unsigned int).hash_code(), GL_UNSIGNED_INT},

            {typeid(glm::vec2).hash_code(), GL_FLOAT},
            {typeid(glm::vec3).hash_code(), GL_FLOAT},
            {typeid(glm::vec4).hash_code(), GL_FLOAT},

            {typeid(glm::ivec2).hash_code(), GL_INT},
            {typeid(glm::ivec3).hash_code(), GL_INT},
            {typeid(glm::ivec4).hash_code(), GL_INT},

            {typeid(glm::uvec2).hash_code(), GL_UNSIGNED_INT},
            {typeid(glm::uvec3).hash_code(), GL_UNSIGNED_INT},
            {typeid(glm::uvec4).hash_code(), GL_UNSIGNED_INT},

            {typeid(glm::mat2).hash_code(), GL_FLOAT},
            {typeid(glm::mat3).hash_code(), GL_FLOAT},
            {typeid(glm::mat4).hash_code(), GL_FLOAT},
        };

        if (const auto it = typeMap.find(type->hash_code()); it != typeMap.end()) return it->second;

        return GL_INVALID_ENUM;
    }



    GLenum ConvertOGL::shaderTypeToGL(const ShaderType type)
    {
        if(type == ShaderType::Vertex) return GL_VERTEX_SHADER;
        if(type == ShaderType::Fragment) return GL_FRAGMENT_SHADER;
        if(type == ShaderType::Compute) return GL_COMPUTE_SHADER;
        return GL_INVALID_ENUM;
    }

    GLenum ConvertOGL::pixelFormatToGL(const PixelFormat format)
    {
        switch (format)
        {
            // 8-bit normalized / integer
            case PixelFormat::R8Unorm:        return GL_R8;
            case PixelFormat::R8Snorm:        return GL_R8_SNORM;
            case PixelFormat::R8Uint:         return GL_R8UI;
            case PixelFormat::R8Int:          return GL_R8I;

            // 16-bit normalized / integer / float
            case PixelFormat::R16Unorm:       return GL_R16;
            case PixelFormat::R16Snorm:       return GL_R16_SNORM;
            case PixelFormat::R16Uint:        return GL_R16UI;
            case PixelFormat::R16Int:         return GL_R16I;
            case PixelFormat::R16Float:       return GL_R16F;

            // 8-bit two-channel
            case PixelFormat::R8G8Unorm:      return GL_RG8;
            case PixelFormat::R8G8Snorm:      return GL_RG8_SNORM;
            case PixelFormat::R8G8Uint:       return GL_RG8UI;
            case PixelFormat::R8G8Int:        return GL_RG8I;

            // 16-bit two-channel
            case PixelFormat::R16G16Unorm:    return GL_RG16;
            case PixelFormat::R16G16Snorm:    return GL_RG16_SNORM;
            case PixelFormat::R16G16Uint:     return GL_RG16UI;
            case PixelFormat::R16G16Int:      return GL_RG16I;
            case PixelFormat::R16G16Float:    return GL_RG16F;

            // 32-bit two-channel
            case PixelFormat::R32G32Float:    return GL_RG32F;
            case PixelFormat::R32G32Uint:     return GL_RG32UI;
            case PixelFormat::R32G32Int:      return GL_RG32I;

            // 32-bit single-channel
            case PixelFormat::R32Float:       return GL_R32F;
            case PixelFormat::R32Uint:        return GL_R32UI;
            case PixelFormat::R32Int:         return GL_R32I;

            // 8-bit three-channel
            case PixelFormat::R8G8B8Unorm:    return GL_RGB8;
            case PixelFormat::R8G8B8Snorm:    return GL_RGB8_SNORM;
            case PixelFormat::R8G8B8Uint:     return GL_RGB8UI;
            case PixelFormat::R8G8B8Int:      return GL_RGB8I;
            case PixelFormat::R8G8B8UnormSrgb: return GL_SRGB8;
            case PixelFormat::B8G8R8Unorm:    return GL_RGB8; // Note: BGR stored as RGB
            case PixelFormat::B8G8R8UnormSrgb: return GL_SRGB8;

            // 16-bit three-channel
            case PixelFormat::R16G16B16Unorm: return GL_RGB16;
            case PixelFormat::R16G16B16Snorm: return GL_RGB16_SNORM;
            case PixelFormat::R16G16B16Uint:  return GL_RGB16UI;
            case PixelFormat::R16G16B16Int:   return GL_RGB16I;
            case PixelFormat::R16G16B16Float: return GL_RGB16F;

            // 32-bit three-channel
            case PixelFormat::R32G32B32Float: return GL_RGB32F;
            case PixelFormat::R32G32B32Uint:  return GL_RGB32UI;
            case PixelFormat::R32G32B32Int:   return GL_RGB32I;

            // 8-bit four-channel
            case PixelFormat::R8G8B8A8Unorm:    return GL_RGBA8;
            case PixelFormat::R8G8B8A8Snorm:    return GL_RGBA8_SNORM;
            case PixelFormat::R8G8B8A8Uint:     return GL_RGBA8UI;
            case PixelFormat::R8G8B8A8Int:      return GL_RGBA8I;
            case PixelFormat::R8G8B8A8UnormSrgb: return GL_SRGB8_ALPHA8;
            case PixelFormat::B8G8R8A8Unorm:    return GL_RGBA8; // Note: BGRA stored as RGBA
            case PixelFormat::B8G8R8A8UnormSrgb: return GL_SRGB8_ALPHA8;

            // 16-bit four-channel
            case PixelFormat::R16G16B16A16Unorm: return GL_RGBA16;
            case PixelFormat::R16G16B16A16Snorm: return GL_RGBA16_SNORM;
            case PixelFormat::R16G16B16A16Uint:  return GL_RGBA16UI;
            case PixelFormat::R16G16B16A16Int:   return GL_RGBA16I;
            case PixelFormat::R16G16B16A16Float: return GL_RGBA16F;

            // 32-bit four-channel
            case PixelFormat::R32G32B32A32Float: return GL_RGBA32F;
            case PixelFormat::R32G32B32A32Uint:  return GL_RGBA32UI;
            case PixelFormat::R32G32B32A32Int:   return GL_RGBA32I;

            // Packed / special
            case PixelFormat::R10G10B10A2Unorm: return GL_RGB10_A2;
            case PixelFormat::R11G11B10Ufloat:  return GL_R11F_G11F_B10F;

            // Block-compressed
            case PixelFormat::BC4RUnorm:        return GL_COMPRESSED_RED_RGTC1;
            case PixelFormat::BC5RgUnorm:       return GL_COMPRESSED_RG_RGTC2;
            case PixelFormat::BC7RgbaUnorm:     return GL_COMPRESSED_RGBA_BPTC_UNORM;
            case PixelFormat::BC7RgbaUnormSrgb: return GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM;

            // Depth-stencil
            case PixelFormat::D24UnormS8Uint:   return GL_DEPTH24_STENCIL8;
            case PixelFormat::D32FloatS8Uint:   return GL_DEPTH32F_STENCIL8;

            case PixelFormat::Invalid:
            default:
                return GL_RGBA8; // Safe fallback
        }
    }

    GLenum ConvertOGL::pixelFormatToGLType(const PixelFormat format)
    {
        switch(format)
        {
            case PixelFormat::R8Unorm:
            case PixelFormat::R8G8Unorm:
            case PixelFormat::R8G8B8A8Unorm:
            case PixelFormat::B8G8R8A8Unorm:
            case PixelFormat::R8G8B8A8UnormSrgb:
            case PixelFormat::B8G8R8A8UnormSrgb:
                return GL_UNSIGNED_BYTE;

            // Unsigned normalized 16-bit
            case PixelFormat::R16Unorm:
            case PixelFormat::R16G16Unorm:
            case PixelFormat::R16G16B16A16Unorm:
                return GL_UNSIGNED_SHORT;

            // Signed normalized 8-bit
            case PixelFormat::R8Snorm:
            case PixelFormat::R8G8Snorm:
            case PixelFormat::R8G8B8A8Snorm:
                return GL_BYTE;

            // Signed normalized 16-bit
            case PixelFormat::R16Snorm:
            case PixelFormat::R16G16Snorm:
            case PixelFormat::R16G16B16A16Snorm:
                return GL_SHORT;

            // Half float (16-bit)
            case PixelFormat::R16Float:
            case PixelFormat::R16G16Float:
            case PixelFormat::R16G16B16A16Float:
                return GL_HALF_FLOAT;

            // Float (32-bit)
            case PixelFormat::R32Float:
            case PixelFormat::R32G32Float:
            case PixelFormat::R32G32B32A32Float:
                return GL_FLOAT;

            // Unsigned integer 8-bit
            case PixelFormat::R8Uint:
            case PixelFormat::R8G8Uint:
            case PixelFormat::R8G8B8A8Uint:
                return GL_UNSIGNED_BYTE;

            // Unsigned integer 16-bit
            case PixelFormat::R16Uint:
            case PixelFormat::R16G16Uint:
            case PixelFormat::R16G16B16A16Uint:
                return GL_UNSIGNED_SHORT;

            // Unsigned integer 32-bit
            case PixelFormat::R32Uint:
            case PixelFormat::R32G32Uint:
            case PixelFormat::R32G32B32A32Uint:
                return GL_UNSIGNED_INT;

            // Signed integer 8-bit
            case PixelFormat::R8Int:
            case PixelFormat::R8G8Int:
            case PixelFormat::R8G8B8A8Int:
                return GL_BYTE;

            // Signed integer 16-bit
            case PixelFormat::R16Int:
            case PixelFormat::R16G16Int:
            case PixelFormat::R16G16B16A16Int:
                return GL_SHORT;

            // Signed integer 32-bit
            case PixelFormat::R32Int:
            case PixelFormat::R32G32Int:
            case PixelFormat::R32G32B32A32Int:
                return GL_INT;

            // Special packed formats
            case PixelFormat::R10G10B10A2Unorm:
                return GL_UNSIGNED_INT_2_10_10_10_REV;

            case PixelFormat::R11G11B10Ufloat:
                return GL_UNSIGNED_INT_10F_11F_11F_REV;

            case PixelFormat::D24UnormS8Uint:
                return GL_UNSIGNED_INT_24_8;

            case PixelFormat::D32FloatS8Uint:
                return GL_FLOAT_32_UNSIGNED_INT_24_8_REV;

            case PixelFormat::BC1RgbaUnorm:
            case PixelFormat::BC1RgbaUnormSrgb:
            case PixelFormat::BC2RgbaUnorm:
            case PixelFormat::BC2RgbaUnormSrgb:
            case PixelFormat::BC3RgbaUnorm:
            case PixelFormat::BC3RgbaUnormSrgb:
            case PixelFormat::BC4RUnorm:
            case PixelFormat::BC5RgUnorm:
            case PixelFormat::BC7RgbaUnorm:
            case PixelFormat::BC7RgbaUnormSrgb:
            default:
                return GL_NONE; // Not applicable for compressed formats
        }
    }


    uint32_t ConvertOGL::getComponentCount(const std::type_info* type)
    {
        if (!type) return 1; // Handle null pointer

        // Basic Numeric Types
        if (*type == typeid(float) || *type == typeid(int) || *type == typeid(unsigned int) ||
            *type == typeid(uint32_t) || *type == typeid(double) || *type == typeid(bool)) return 1;

        // GLM Vector Types
        if (*type == typeid(glm::vec2) || *type == typeid(glm::ivec2) || *type == typeid(glm::uvec2)) return 2;
        if (*type == typeid(glm::vec3) || *type == typeid(glm::ivec3) || *type == typeid(glm::uvec3)) return 3;
        if (*type == typeid(glm::vec4) || *type == typeid(glm::ivec4) || *type == typeid(glm::uvec4)) return 4;

        // GLM Matrix Types (components per column)
        if (*type == typeid(glm::mat2)) return 2;
        if (*type == typeid(glm::mat3)) return 3;
        if (*type == typeid(glm::mat4)) return 4;

        // Fallback for unknown types
        return 1;
    }


    KeyCode ConvertOGL::keyCodeFromGLFW(int glfwKey)
    {
        switch (glfwKey) {
            case GLFW_KEY_ENTER:       return KeyCode::Return;
            case GLFW_KEY_ESCAPE:      return KeyCode::Escape;
            case GLFW_KEY_BACKSPACE:   return KeyCode::Backspace;
            case GLFW_KEY_TAB:         return KeyCode::Tab;
            case GLFW_KEY_SPACE:       return KeyCode::Space;
            case GLFW_KEY_APOSTROPHE:  return KeyCode::Apostrophe;
            case GLFW_KEY_COMMA:       return KeyCode::Comma;
            case GLFW_KEY_MINUS:       return KeyCode::Minus;
            case GLFW_KEY_PERIOD:      return KeyCode::Period;
            case GLFW_KEY_SLASH:       return KeyCode::Slash;
            case GLFW_KEY_0:           return KeyCode::Num0;
            case GLFW_KEY_1:           return KeyCode::Num1;
            case GLFW_KEY_2:           return KeyCode::Num2;
            case GLFW_KEY_3:           return KeyCode::Num3;
            case GLFW_KEY_4:           return KeyCode::Num4;
            case GLFW_KEY_5:           return KeyCode::Num5;
            case GLFW_KEY_6:           return KeyCode::Num6;
            case GLFW_KEY_7:           return KeyCode::Num7;
            case GLFW_KEY_8:           return KeyCode::Num8;
            case GLFW_KEY_9:           return KeyCode::Num9;
            case GLFW_KEY_SEMICOLON:   return KeyCode::Semicolon;
            case GLFW_KEY_EQUAL:       return KeyCode::Equals;
            case GLFW_KEY_LEFT_BRACKET:return KeyCode::LeftBracket;
            case GLFW_KEY_BACKSLASH:   return KeyCode::Backslash;
            case GLFW_KEY_RIGHT_BRACKET:return KeyCode::RightBracket;
            case GLFW_KEY_GRAVE_ACCENT:return KeyCode::Grave;
            case GLFW_KEY_A:           return KeyCode::A;
            case GLFW_KEY_B:           return KeyCode::B;
            case GLFW_KEY_C:           return KeyCode::C;
            case GLFW_KEY_D:           return KeyCode::D;
            case GLFW_KEY_E:           return KeyCode::E;
            case GLFW_KEY_F:           return KeyCode::F;
            case GLFW_KEY_G:           return KeyCode::G;
            case GLFW_KEY_H:           return KeyCode::H;
            case GLFW_KEY_I:           return KeyCode::I;
            case GLFW_KEY_J:           return KeyCode::J;
            case GLFW_KEY_K:           return KeyCode::K;
            case GLFW_KEY_L:           return KeyCode::L;
            case GLFW_KEY_M:           return KeyCode::M;
            case GLFW_KEY_N:           return KeyCode::N;
            case GLFW_KEY_O:           return KeyCode::O;
            case GLFW_KEY_P:           return KeyCode::P;
            case GLFW_KEY_Q:           return KeyCode::Q;
            case GLFW_KEY_R:           return KeyCode::R;
            case GLFW_KEY_S:           return KeyCode::S;
            case GLFW_KEY_T:           return KeyCode::T;
            case GLFW_KEY_U:           return KeyCode::U;
            case GLFW_KEY_V:           return KeyCode::V;
            case GLFW_KEY_W:           return KeyCode::W;
            case GLFW_KEY_X:           return KeyCode::X;
            case GLFW_KEY_Y:           return KeyCode::Y;
            case GLFW_KEY_Z:           return KeyCode::Z;
            case GLFW_KEY_DELETE:      return KeyCode::Delete;

            // Function keys
            case GLFW_KEY_CAPS_LOCK:   return KeyCode::CapsLock;
            case GLFW_KEY_F1:          return KeyCode::F1;
            case GLFW_KEY_F2:          return KeyCode::F2;
            case GLFW_KEY_F3:          return KeyCode::F3;
            case GLFW_KEY_F4:          return KeyCode::F4;
            case GLFW_KEY_F5:          return KeyCode::F5;
            case GLFW_KEY_F6:          return KeyCode::F6;
            case GLFW_KEY_F7:          return KeyCode::F7;
            case GLFW_KEY_F8:          return KeyCode::F8;
            case GLFW_KEY_F9:          return KeyCode::F9;
            case GLFW_KEY_F10:         return KeyCode::F10;
            case GLFW_KEY_F11:         return KeyCode::F11;
            case GLFW_KEY_F12:         return KeyCode::F12;
            case GLFW_KEY_F13:         return KeyCode::F13;
            case GLFW_KEY_F14:         return KeyCode::F14;
            case GLFW_KEY_F15:         return KeyCode::F15;
            case GLFW_KEY_F16:         return KeyCode::F16;
            case GLFW_KEY_F17:         return KeyCode::F17;
            case GLFW_KEY_F18:         return KeyCode::F18;
            case GLFW_KEY_F19:         return KeyCode::F19;
            case GLFW_KEY_F20:         return KeyCode::F20;
            case GLFW_KEY_F21:         return KeyCode::F21;
            case GLFW_KEY_F22:         return KeyCode::F22;
            case GLFW_KEY_F23:         return KeyCode::F23;
            case GLFW_KEY_F24:         return KeyCode::F24;
            case GLFW_KEY_F25:         return KeyCode::F25;

            case GLFW_KEY_PRINT_SCREEN:return KeyCode::PrintScreen;
            case GLFW_KEY_SCROLL_LOCK: return KeyCode::ScrollLock;
            case GLFW_KEY_PAUSE:       return KeyCode::Pause;
            case GLFW_KEY_INSERT:      return KeyCode::Insert;
            case GLFW_KEY_HOME:        return KeyCode::Home;
            case GLFW_KEY_PAGE_UP:     return KeyCode::PageUp;
            case GLFW_KEY_END:         return KeyCode::End;
            case GLFW_KEY_PAGE_DOWN:   return KeyCode::PageDown;
            case GLFW_KEY_RIGHT:       return KeyCode::Right;
            case GLFW_KEY_LEFT:        return KeyCode::Left;
            case GLFW_KEY_DOWN:        return KeyCode::Down;
            case GLFW_KEY_UP:          return KeyCode::Up;
            case GLFW_KEY_NUM_LOCK:    return KeyCode::NumLockClear;
            case GLFW_KEY_KP_DIVIDE:   return KeyCode::KpDivide;
            case GLFW_KEY_KP_MULTIPLY: return KeyCode::KpMultiply;
            case GLFW_KEY_KP_SUBTRACT: return KeyCode::KpMinus;
            case GLFW_KEY_KP_ADD:      return KeyCode::KpPlus;
            case GLFW_KEY_KP_ENTER:    return KeyCode::KpEnter;
            case GLFW_KEY_KP_1:        return KeyCode::Kp1;
            case GLFW_KEY_KP_2:        return KeyCode::Kp2;
            case GLFW_KEY_KP_3:        return KeyCode::Kp3;
            case GLFW_KEY_KP_4:        return KeyCode::Kp4;
            case GLFW_KEY_KP_5:        return KeyCode::Kp5;
            case GLFW_KEY_KP_6:        return KeyCode::Kp6;
            case GLFW_KEY_KP_7:        return KeyCode::Kp7;
            case GLFW_KEY_KP_8:        return KeyCode::Kp8;
            case GLFW_KEY_KP_9:        return KeyCode::Kp9;
            case GLFW_KEY_KP_0:        return KeyCode::Kp0;
            case GLFW_KEY_KP_DECIMAL:  return KeyCode::KpPeriod;
            case GLFW_KEY_KP_EQUAL:    return KeyCode::KpEquals;
            case GLFW_KEY_MENU:        return KeyCode::Menu;

            // Modifier keys
            case GLFW_KEY_LEFT_CONTROL: return KeyCode::LCtrl;
            case GLFW_KEY_LEFT_SHIFT:   return KeyCode::LShift;
            case GLFW_KEY_LEFT_ALT:     return KeyCode::LAlt;
            case GLFW_KEY_LEFT_SUPER:   return KeyCode::LGui;
            case GLFW_KEY_RIGHT_CONTROL:return KeyCode::RCtrl;
            case GLFW_KEY_RIGHT_SHIFT:  return KeyCode::RShift;
            case GLFW_KEY_RIGHT_ALT:    return KeyCode::RAlt;
            case GLFW_KEY_RIGHT_SUPER:  return KeyCode::RGui;

            // Note: GLFW doesn't have direct equivalents for many of the specialized SDL keys
            // like media keys, special symbols, or advanced keypad functions
            // These would need to be handled through character input callbacks if needed

            default:
                return KeyCode::Unknown;
        }
    }

    KeyMod ConvertOGL::keyModFromGLFW(const int glfwMods)
    {
        auto mod = KeyMod::None;
        if (glfwMods & GLFW_MOD_SHIFT)     mod = static_cast<KeyMod>(static_cast<uint16_t>(mod) | static_cast<uint16_t>(KeyMod::LShift));
        if (glfwMods & GLFW_MOD_CONTROL)   mod = static_cast<KeyMod>(static_cast<uint16_t>(mod) | static_cast<uint16_t>(KeyMod::LCtrl));
        if (glfwMods & GLFW_MOD_ALT)       mod = static_cast<KeyMod>(static_cast<uint16_t>(mod) | static_cast<uint16_t>(KeyMod::LAlt));
        if (glfwMods & GLFW_MOD_SUPER)     mod = static_cast<KeyMod>(static_cast<uint16_t>(mod) | static_cast<uint16_t>(KeyMod::LGui));
        if (glfwMods & GLFW_MOD_CAPS_LOCK) mod = static_cast<KeyMod>(static_cast<uint16_t>(mod) | static_cast<uint16_t>(KeyMod::Caps));
        if (glfwMods & GLFW_MOD_NUM_LOCK)  mod = static_cast<KeyMod>(static_cast<uint16_t>(mod) | static_cast<uint16_t>(KeyMod::Num));

        // Note: GLFW doesn't distinguish between left/right modifiers in the mods parameter
        // and doesn't have direct equivalents for Mode or Scroll lock modifiers

        return mod;
    }

    MouseButton ConvertOGL::mouseButtonFromGLFW(const int glfwButton)
    {
        switch (glfwButton)
        {
            case GLFW_MOUSE_BUTTON_LEFT:   return MouseButton::Left;
            case GLFW_MOUSE_BUTTON_MIDDLE: return MouseButton::Middle;
            case GLFW_MOUSE_BUTTON_RIGHT:  return MouseButton::Right;
            case GLFW_MOUSE_BUTTON_4:      return MouseButton::Side1;
            case GLFW_MOUSE_BUTTON_5:      return MouseButton::Side2;
            // GLFW supports buttons 6-8 as well if your MouseButton enum has more
            default:                       return MouseButton::None;
        }
    }

    GLenum ConvertOGL::textureWrapToGL(TextureWrap wrap) {
        switch(wrap) {
            case TextureWrap::Repeat:
                return GL_REPEAT;

            case TextureWrap::MirroredRepeat:
                return GL_MIRRORED_REPEAT;

            case TextureWrap::ClampToEdge:
                return GL_CLAMP_TO_EDGE;

            case TextureWrap::ClampToBorder:
                return GL_CLAMP_TO_BORDER;

            default:
                return GL_REPEAT;
        }
    }

    GLenum ConvertOGL::minFilterToGL(const TextureFilter filter, const MipmapFilter mipmapFilter)
    {
        if (mipmapFilter == MipmapFilter::None)
        {
            switch(filter)
            {
                case TextureFilter::Nearest:
                    return GL_NEAREST;
                case TextureFilter::Linear:
                    return GL_LINEAR;
                default:
                    return GL_NONE;
            }
        }

        if (filter == TextureFilter::Nearest)
        {
            switch(mipmapFilter)
            {
                case MipmapFilter::Nearest:
                    return GL_NEAREST_MIPMAP_NEAREST;
                case MipmapFilter::Linear:
                    return GL_NEAREST_MIPMAP_LINEAR;
                default:
                    return GL_NONE;
            }
        }

        // filter == TextureFilter::Linear
        switch(mipmapFilter)
        {
            case MipmapFilter::Nearest:
                return GL_LINEAR_MIPMAP_NEAREST;
            case MipmapFilter::Linear:
                return GL_LINEAR_MIPMAP_LINEAR;
            default:
                return GL_NONE;
        }
    }

    GLenum ConvertOGL::magFilterToGL(const TextureFilter filter)
    {
        switch(filter)
        {
            case TextureFilter::Nearest:
                return GL_NEAREST;
            case TextureFilter::Linear:
                return GL_LINEAR;
            default:
                return GL_NONE;
        }
    }

    GLenum ConvertOGL::pixelLayoutToGL(const PixelLayout layout)
    {
        switch (layout)
        {
            // --- Standard (Float/Normalized) ---
            case PixelLayout::R:              return GL_RED;
            case PixelLayout::RG:             return GL_RG;
            case PixelLayout::RGB:            return GL_RGB;
            case PixelLayout::BGR:            return GL_BGR;
            case PixelLayout::RGBA:           return GL_RGBA;
            case PixelLayout::BGRA:           return GL_BGRA;
            case PixelLayout::Depth:          return GL_DEPTH_COMPONENT;
            case PixelLayout::Stencil:        return GL_STENCIL_INDEX;
            case PixelLayout::DepthStencil:   return GL_DEPTH_STENCIL;

            // --- Integer (Raw Bits) ---
            case PixelLayout::RInt:      return GL_RED_INTEGER;
            case PixelLayout::RGInt:     return GL_RG_INTEGER;
            case PixelLayout::RGBInt:    return GL_RGB_INTEGER;
            case PixelLayout::RGBAInt:   return GL_RGBA_INTEGER;
            case PixelLayout::BGRInt:    return GL_BGR_INTEGER;
            case PixelLayout::BGRAInt:   return GL_BGRA_INTEGER;
        }
        return GL_INVALID_ENUM;
    }


    GLenum ConvertOGL::pixelTypeToGL(const PixelType type)
    {
        switch (type)
        {
            case PixelType::UnsignedByte:       return GL_UNSIGNED_BYTE;
            case PixelType::Byte:               return GL_BYTE;
            case PixelType::UnsignedShort:      return GL_UNSIGNED_SHORT;
            case PixelType::Short:              return GL_SHORT;
            case PixelType::UnsignedInt:        return GL_UNSIGNED_INT;
            case PixelType::Int:                return GL_INT;
            case PixelType::Float:              return GL_FLOAT;
            case PixelType::UnsignedByte332:    return GL_UNSIGNED_BYTE_3_3_2;
            case PixelType::UnsignedShort565:   return GL_UNSIGNED_SHORT_5_6_5;
            case PixelType::UnsignedInt1010102: return GL_UNSIGNED_INT_10_10_10_2;
            default:
                return GL_UNSIGNED_BYTE; // Safe fallback
        }
    }

    GLenum ConvertOGL::textureTypeToGL(const TextureType type)
    {
        switch (type)
        {
            case TextureType::Texture1D:
                return GL_TEXTURE_1D;
            case TextureType::Texture2D:
                return GL_TEXTURE_2D;
            case TextureType::Texture3D:
                return GL_TEXTURE_3D;
            case TextureType::Texture2DArray:
                return GL_TEXTURE_2D_ARRAY;
            case TextureType::TextureCube:
                return GL_TEXTURE_CUBE_MAP;
            case TextureType::TextureCubeArray:
                return GL_TEXTURE_CUBE_MAP_ARRAY;
        }
        return GL_NONE;
    }

    GLenum ConvertOGL::blendFactorToGL(const BlendFactor factor)
    {
        switch (factor)
        {
            case BlendFactor::Zero:         return GL_ZERO;
            case BlendFactor::One:          return GL_ONE;
            case BlendFactor::SrcAlpha:     return GL_SRC_ALPHA;
            case BlendFactor::InvSrcAlpha:  return GL_ONE_MINUS_SRC_ALPHA;
        }

        return GL_ONE;
    }

    GLenum ConvertOGL::blendOpToGL(const BlendOp op)
    {
        switch (op)
        {
            case BlendOp::Add:          return GL_FUNC_ADD;
            case BlendOp::Subtract:     return GL_FUNC_SUBTRACT;
            case BlendOp::RevSubtract:  return GL_FUNC_REVERSE_SUBTRACT;
            case BlendOp::Min:          return GL_MIN;
            case BlendOp::Max:          return GL_MAX;
        }

        return GL_FUNC_ADD;
    }

    GLenum ConvertOGL::indexFormatToGL(const IndexFormat format)
    {
        switch (format)
        {
            case IndexFormat::UInt16:
                return GL_UNSIGNED_SHORT;
            case IndexFormat::UInt32:
                return GL_UNSIGNED_INT;
        }
        return GL_INVALID_ENUM;
    }

    uint32_t ConvertOGL::indexFormatToSize(const IndexFormat format)
    {
        switch (format)
        {
            case IndexFormat::UInt16:
                return 2;
            case IndexFormat::UInt32:
                return 4;
        }

        return 0;
    }

    GLenum ConvertOGL::imageAccessToGL(const ImageAccess access)
    {
        switch (access)
        {
            case ImageAccess::ReadOnly:  return GL_READ_ONLY;
            case ImageAccess::WriteOnly: return GL_WRITE_ONLY;
            case ImageAccess::ReadWrite: return GL_READ_WRITE;
        }

        return GL_WRITE_ONLY;
    }
}
