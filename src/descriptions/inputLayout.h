#pragma once
#include <vector>

#include "vertexAttribute.h"
#include "vertexBufferDescription.h"

namespace Neon::RHI
{
class InputLayout
{
public:
    template<typename  T>
    void addVertexBuffer(uint32_t slot)
    {
        stride = sizeof(T);
        vertexBuffers.emplace_back(slot, sizeof(T));
    }

    template<typename  T>
    void addVertexAttribute(uint32_t slot, const uint32_t location)
    {
        vertexAttributes.emplace_back(location, slot, sizeof(T), typeid(T));
    }

    [[nodiscard]] const std::vector<VertexAttribute>& getVertexAttributes() const
    {
        return vertexAttributes;
    }

    [[nodiscard]] const std::vector<VertexBufferDescription>& getVertexBuffers() const
    {
        return vertexBuffers;
    }

    [[nodiscard]] uint32_t getStride() const
    {
        return stride;
    }
private:
    uint32_t stride{};
    std::vector<VertexAttribute> vertexAttributes;
    std::vector<VertexBufferDescription> vertexBuffers;
};
}
