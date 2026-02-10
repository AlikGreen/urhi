#pragma once

#include <string>
#include <grl/grl.h>

#include "pipeline.h"
#include "buffer.h"

#include "texture.h"
#include "textureView.h"
#include "sampler.h"
#include "descriptions/renderPassDesc.h"
#include "enums/imageAccess.h"
#include "enums/indexFormat.h"
#include "glm/glm.hpp"

#include "descriptions/scissorRect.h"
#include "descriptions/textureReadDesc.h"
#include "descriptions/textureUploadDesc.h"

namespace urhi
{
class CommandList
{
public:
    virtual ~CommandList() = default;

    virtual void begin() = 0;

    virtual void setUniformBuffer(const std::string& name, const grl::Rc<Buffer>& buffer) = 0;
    virtual void setStorageBuffer(const std::string& name, const grl::Rc<Buffer>& buffer) = 0;
    virtual void setTexture(const std::string& name, const grl::Rc<TextureView>& texture) = 0;
    virtual void setSampler(const std::string& name, const grl::Rc<Sampler>& sampler) = 0;
    virtual void setImage(const std::string& name, const grl::Rc<TextureView>& texture, ImageAccess access) = 0;

    virtual void setPipeline(const grl::Rc<Pipeline>& pipeline) = 0;

    virtual void beginRenderPass(const RenderPassDesc& desc) = 0;
    virtual void endRenderPass() = 0;

    virtual void setVertexBuffer(uint32_t index, const grl::Rc<Buffer>& vertexBuffer) = 0;
    virtual void setIndexBuffer(const grl::Rc<Buffer>& indexBuffer, IndexFormat indexFormat) = 0;

    virtual void setScissor(ScissorRect rect) = 0;
    void setScissor(const int x, const int y, const int width, const int height) { setScissor(ScissorRect{ x, y, width, height }); }

    virtual void updateTexture(const grl::Rc<Texture>& texture, const TextureUploadDesc& desc) = 0;
    virtual void generateMipmaps(const grl::Rc<Texture>& texture) = 0;

    virtual void reserveBuffer(const grl::Rc<Buffer>& buffer, size_t size) = 0;

    virtual void dispatch(const glm::ivec3& numGroups) = 0;

    virtual void resourceBarrier(const grl::Rc<Texture>& texture, ImageAccess nextAccess) = 0;
    virtual void resourceBarrier(const grl::Rc<Buffer>& buffer) = 0;


    template<typename T>
    void readTexture(const grl::Rc<TextureView>& texture, const TextureReadDesc& desc, std::vector<T>& dest) { readTextureImpl(texture, desc, dest.size() * sizeof(T), dest.data()); }
    template<typename T>
    void updateBuffer(const grl::Rc<Buffer>& buffer, T& data) { updateBufferImpl(buffer, &data, sizeof(T)); }
    template<typename T>
    void updateBuffer(const grl::Rc<Buffer>& buffer, std::vector<T> data) { updateBufferImpl(buffer, data.data(), sizeof(T)*data.size()); }
    template<typename T>
    void updateBuffer(const grl::Rc<Buffer>& buffer, T* data) { updateBufferImpl(buffer, data, sizeof(T)); }

    void draw(const uint32_t vertexCount, const uint32_t instanceCount = 1, const uint32_t firstVertex = 0, const uint32_t firstInstance = 0) { drawImpl(vertexCount, instanceCount, firstVertex, firstInstance); };
    void drawIndexed(const uint32_t indexCount, const uint32_t instanceCount = 1, const uint32_t firstIndex = 0, const int vertexOffset = 0, const uint32_t firstInstance = 0) { drawIndexedImpl(indexCount, instanceCount, firstIndex, vertexOffset, firstInstance); };
protected:
    virtual void drawImpl(uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance) = 0;
    virtual void drawIndexedImpl(uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex, int vertexOffset, uint32_t firstInstance) = 0;
    virtual void updateBufferImpl(const grl::Rc<Buffer>& buffer, void* data, uint32_t size) = 0;
    virtual void readTextureImpl(const grl::Rc<TextureView>& texture, const TextureReadDesc& desc, size_t destSize, void* dest) = 0;
};
}
