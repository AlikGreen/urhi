#pragma once
#include "frameBuffer.h"
#include "graphicsPipeline.h"
#include "buffer.h"
#include "sampler.h"
#include "texture.h"
#include "enums/indexFormat.h"
#include "enums/shaderType.h"
#include "glm/glm.hpp"
#include "util/memory.h"

namespace NRHI
{
class CommandList
{
public:
    virtual ~CommandList() = default;

    virtual void begin() = 0;

    virtual void setUniformBuffer(const std::string& name, Buffer* buffer) = 0;
    virtual void setTexture(      const std::string& name, uint32_t offset, Texture* texture) = 0;
    virtual void setSampler(      const std::string& name, uint32_t offset, Sampler* sampler) = 0;
    void setTexture(              const std::string& name, Texture* texture) { setTexture(name, 0, texture); };
    void setSampler(              const std::string& name, Sampler* sampler) { setSampler(name, 0, sampler); };


    virtual void setPipeline(GraphicsPipeline* pipeline) = 0;
    virtual void setFrameBuffer(FrameBuffer* frameBuffer) = 0;

    virtual void setVertexBuffer(uint32_t index, Buffer* vertexBuffer) = 0;
    virtual void setIndexBuffer(Buffer* indexBuffer, IndexFormat indexFormat) = 0;

    virtual void clearColorTarget(uint32_t target, glm::vec4 color) = 0;
    virtual void clearDepthStencil(float value) = 0;

    virtual void updateTexture(Texture* texture, const void* data) = 0;

    virtual void reserveBuffer(Buffer* buffer, size_t size) = 0;

    template<typename T>
    void updateBuffer(Buffer* buffer, T& data)
    {
        updateBufferImpl(buffer, &data, sizeof(T));
    }

    template<typename T>
    void updateBuffer(Buffer* buffer, std::vector<T> data)
    {
        updateBufferImpl(buffer, data.data(), sizeof(T)*data.size());
    }

    template<typename T>
    void updateBuffer(Buffer* buffer, T* data)
    {
        updateBufferImpl(buffer, data, sizeof(T));
    }

    void draw(const uint32_t vertexCount, const uint32_t instanceCount = 1, const uint32_t firstVertex = 0, const uint32_t firstInstance = 0)
    {
        drawImpl(vertexCount, instanceCount, firstVertex, firstInstance);
    };

    void drawIndexed(const uint32_t indexCount, const uint32_t instanceCount = 1, const uint32_t firstIndex = 0, const int vertexOffset = 0, const uint32_t firstInstance = 0)
    {
        drawIndexedImpl(indexCount, instanceCount, firstIndex, vertexOffset, firstInstance);
    };
protected:
    virtual void drawImpl(uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance) = 0;
    virtual void drawIndexedImpl(uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex, int vertexOffset, uint32_t firstInstance) = 0;
    virtual void updateBufferImpl(Buffer* buffer, void* data, uint32_t size) = 0;
};
}
