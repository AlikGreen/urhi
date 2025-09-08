#pragma once
#include <cstdint>

namespace Neon::RHI
{
class Texture
{
public:
    virtual ~Texture() = default;

    virtual uint32_t getWidth() = 0;
    virtual uint32_t getHeight() = 0;
    virtual uint32_t getDepth() = 0;
};
}
