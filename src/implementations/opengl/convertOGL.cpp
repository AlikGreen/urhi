#include "convertOGL.h"
#include <glm/glm.hpp>

#include "enums/shaderType.h"
#include "GLFW/glfw3.h"

namespace NRHI
{
    GLenum ConvertOGL::typeinfoToGL(const std::type_info *type)
    {
        if (!type) return GL_INVALID_ENUM;

        static const std::unordered_map<size_t, GLenum> typeMap =
        {
            // --- Floating point types ---
            {typeid(float).hash_code(), GL_FLOAT},
            {typeid(double).hash_code(), GL_DOUBLE},

            // --- Signed integer types ---
            {typeid(int8_t).hash_code(), GL_BYTE},
            {typeid(signed char).hash_code(), GL_BYTE},
            {typeid(int16_t).hash_code(), GL_SHORT},
            {typeid(short).hash_code(), GL_SHORT},
            {typeid(int32_t).hash_code(), GL_INT},
            {typeid(int).hash_code(), GL_INT},

            // --- Unsigned integer types ---
            {typeid(uint8_t).hash_code(), GL_UNSIGNED_BYTE},
            {typeid(unsigned char).hash_code(), GL_UNSIGNED_BYTE},
            {typeid(uint16_t).hash_code(), GL_UNSIGNED_SHORT},
            {typeid(unsigned short).hash_code(), GL_UNSIGNED_SHORT},
            {typeid(uint32_t).hash_code(), GL_UNSIGNED_INT},
            {typeid(unsigned int).hash_code(), GL_UNSIGNED_INT},

            // --- GLM Types ---
            // GLM vector types (vec2, vec3, vec4) are composed of floats.
            // Their OpenGL type is GL_FLOAT.
            {typeid(glm::vec2).hash_code(), GL_FLOAT},
            {typeid(glm::vec3).hash_code(), GL_FLOAT},
            {typeid(glm::vec4).hash_code(), GL_FLOAT},

            // GLM integer vector types (ivec2, ivec3, ivec4) are composed of ints.
            // Their OpenGL type is GL_INT.
            {typeid(glm::ivec2).hash_code(), GL_INT},
            {typeid(glm::ivec3).hash_code(), GL_INT},
            {typeid(glm::ivec4).hash_code(), GL_INT},

            // GLM unsigned integer vector types (uvec2, uvec3, uvec4) are composed of unsigned ints.
            // Their OpenGL type is GL_UNSIGNED_INT.
            {typeid(glm::uvec2).hash_code(), GL_UNSIGNED_INT},
            {typeid(glm::uvec3).hash_code(), GL_UNSIGNED_INT},
            {typeid(glm::uvec4).hash_code(), GL_UNSIGNED_INT},

            // GLM matrix types (mat2, mat3, mat4) are composed of floats.
            // Their OpenGL type is GL_FLOAT, as each column is a vector of floats.
            {typeid(glm::mat2).hash_code(), GL_FLOAT},
            {typeid(glm::mat3).hash_code(), GL_FLOAT},
            {typeid(glm::mat4).hash_code(), GL_FLOAT},
        };

        const auto it = typeMap.find(type->hash_code());
        if (it != typeMap.end()) return it->second;

        return GL_INVALID_ENUM;
    }

    GLenum ConvertOGL::shaderTypeToGL(const ShaderType type)
    {
        if(type == ShaderType::Vertex) return GL_VERTEX_SHADER;
        if(type == ShaderType::Fragment) return GL_FRAGMENT_SHADER;
        if(type == ShaderType::Compute) return GL_COMPUTE_SHADER;
        return GL_INVALID_ENUM;
    }

    GLenum ConvertOGL::textureFormatToGL(TextureFormat format)
    {
        switch(format)
        {
            case TextureFormat::R8Unorm:
            case TextureFormat::R8Snorm:
            case TextureFormat::R8Uint:
            case TextureFormat::R8Int:
            case TextureFormat::R16Unorm:
            case TextureFormat::R16Snorm:
            case TextureFormat::R16Float:
            case TextureFormat::R16Uint:
            case TextureFormat::R16Int:
            case TextureFormat::R32Float:
            case TextureFormat::R32Uint:
            case TextureFormat::R32Int:
            return GL_RED;

            // Two channel
            case TextureFormat::R8G8Unorm:
            case TextureFormat::R8G8Snorm:
            case TextureFormat::R8G8Uint:
            case TextureFormat::R8G8Int:
            case TextureFormat::R16G16Unorm:
            case TextureFormat::R16G16Snorm:
            case TextureFormat::R16G16Float:
            case TextureFormat::R16G16Uint:
            case TextureFormat::R16G16Int:
            case TextureFormat::R32G32Float:
            case TextureFormat::R32G32Uint:
            case TextureFormat::R32G32Int:
            case TextureFormat::BC4RUnorm:
            case TextureFormat::BC5RgUnorm:
                return GL_RG;

            // Three channel
            case TextureFormat::R11G11B10Ufloat:
                return GL_RGB;

            // Four channel RGBA
            case TextureFormat::R8G8B8A8Unorm:
            case TextureFormat::R8G8B8A8Snorm:
            case TextureFormat::R8G8B8A8Uint:
            case TextureFormat::R8G8B8A8Int:
            case TextureFormat::R16G16B16A16Unorm:
            case TextureFormat::R16G16B16A16Snorm:
            case TextureFormat::R16G16B16A16Float:
            case TextureFormat::R16G16B16A16Uint:
            case TextureFormat::R16G16B16A16Int:
            case TextureFormat::R32G32B32A32Float:
            case TextureFormat::R32G32B32A32Uint:
            case TextureFormat::R32G32B32A32Int:
            case TextureFormat::R10G10B10A2Unorm:
            case TextureFormat::R8G8B8A8UnormSrgb:
            case TextureFormat::BC1RgbaUnorm:
            case TextureFormat::BC1RgbaUnormSrgb:
            case TextureFormat::BC2RgbaUnorm:
            case TextureFormat::BC2RgbaUnormSrgb:
            case TextureFormat::BC3RgbaUnorm:
            case TextureFormat::BC3RgbaUnormSrgb:
            case TextureFormat::BC7RgbaUnorm:
            case TextureFormat::BC7RgbaUnormSrgb:
                return GL_RGBA;

            // Four channel BGRA
            case TextureFormat::B8G8R8A8Unorm:
            case TextureFormat::B8G8R8A8UnormSrgb:
                return GL_BGRA;

            // Depth-stencil formats
            case TextureFormat::D24UnormS8Uint:
            case TextureFormat::D32FloatS8Uint:
                return GL_DEPTH_STENCIL;

            default:
                return GL_RGBA;
        }
    }

    GLenum ConvertOGL::textureFormatToGLType(const TextureFormat format)
    {
        switch(format)
        {
            case TextureFormat::R8Unorm:
            case TextureFormat::R8G8Unorm:
            case TextureFormat::R8G8B8A8Unorm:
            case TextureFormat::B8G8R8A8Unorm:
            case TextureFormat::R8G8B8A8UnormSrgb:
            case TextureFormat::B8G8R8A8UnormSrgb:
                return GL_UNSIGNED_BYTE;

            // Unsigned normalized 16-bit
            case TextureFormat::R16Unorm:
            case TextureFormat::R16G16Unorm:
            case TextureFormat::R16G16B16A16Unorm:
                return GL_UNSIGNED_SHORT;

            // Signed normalized 8-bit
            case TextureFormat::R8Snorm:
            case TextureFormat::R8G8Snorm:
            case TextureFormat::R8G8B8A8Snorm:
                return GL_BYTE;

            // Signed normalized 16-bit
            case TextureFormat::R16Snorm:
            case TextureFormat::R16G16Snorm:
            case TextureFormat::R16G16B16A16Snorm:
                return GL_SHORT;

            // Half float (16-bit)
            case TextureFormat::R16Float:
            case TextureFormat::R16G16Float:
            case TextureFormat::R16G16B16A16Float:
                return GL_HALF_FLOAT;

            // Float (32-bit)
            case TextureFormat::R32Float:
            case TextureFormat::R32G32Float:
            case TextureFormat::R32G32B32A32Float:
                return GL_FLOAT;

            // Unsigned integer 8-bit
            case TextureFormat::R8Uint:
            case TextureFormat::R8G8Uint:
            case TextureFormat::R8G8B8A8Uint:
                return GL_UNSIGNED_BYTE;

            // Unsigned integer 16-bit
            case TextureFormat::R16Uint:
            case TextureFormat::R16G16Uint:
            case TextureFormat::R16G16B16A16Uint:
                return GL_UNSIGNED_SHORT;

            // Unsigned integer 32-bit
            case TextureFormat::R32Uint:
            case TextureFormat::R32G32Uint:
            case TextureFormat::R32G32B32A32Uint:
                return GL_UNSIGNED_INT;

            // Signed integer 8-bit
            case TextureFormat::R8Int:
            case TextureFormat::R8G8Int:
            case TextureFormat::R8G8B8A8Int:
                return GL_BYTE;

            // Signed integer 16-bit
            case TextureFormat::R16Int:
            case TextureFormat::R16G16Int:
            case TextureFormat::R16G16B16A16Int:
                return GL_SHORT;

            // Signed integer 32-bit
            case TextureFormat::R32Int:
            case TextureFormat::R32G32Int:
            case TextureFormat::R32G32B32A32Int:
                return GL_INT;

            // Special packed formats
            case TextureFormat::R10G10B10A2Unorm:
                return GL_UNSIGNED_INT_2_10_10_10_REV;

            case TextureFormat::R11G11B10Ufloat:
                return GL_UNSIGNED_INT_10F_11F_11F_REV;

            case TextureFormat::D24UnormS8Uint:
                return GL_UNSIGNED_INT_24_8;

            case TextureFormat::D32FloatS8Uint:
                return GL_FLOAT_32_UNSIGNED_INT_24_8_REV;

            // Compressed formats - these don't have a "type" in the traditional sense
            // They're handled differently in OpenGL (glCompressedTexImage2D)
            case TextureFormat::BC1RgbaUnorm:
            case TextureFormat::BC1RgbaUnormSrgb:
            case TextureFormat::BC2RgbaUnorm:
            case TextureFormat::BC2RgbaUnormSrgb:
            case TextureFormat::BC3RgbaUnorm:
            case TextureFormat::BC3RgbaUnormSrgb:
            case TextureFormat::BC4RUnorm:
            case TextureFormat::BC5RgUnorm:
            case TextureFormat::BC7RgbaUnorm:
            case TextureFormat::BC7RgbaUnormSrgb:
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

    GLenum ConvertOGL::textureFilterCombineToGL(const TextureFilter filter, const MipmapFilter mipmapFilter)
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
                    return GL_LINEAR;
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
                    return GL_NEAREST_MIPMAP_LINEAR;
            }
        }

        switch(mipmapFilter)
        {
            case MipmapFilter::Nearest:
                return GL_LINEAR_MIPMAP_NEAREST;
            case MipmapFilter::Linear:
                return GL_LINEAR_MIPMAP_LINEAR;
            default:
                return GL_LINEAR_MIPMAP_LINEAR;
        }
    }

}
