#pragma once
#include <vector>

#include "vertexAttribute.h"
#include "vertexBufferDescription.h"

namespace NRHI
{
class VertexInputState
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

    std::vector<VertexAttribute> getVertexAttributes()
    {
        return vertexAttributes;
    }

    std::vector<VertexBufferDescription> getVertexBuffers()
    {
        return vertexBuffers;
    }

    uint32_t getStride() const
    {
        return stride;
    }
private:
    uint32_t stride{};
    std::vector<VertexAttribute> vertexAttributes;
    std::vector<VertexBufferDescription> vertexBuffers;
};
}
