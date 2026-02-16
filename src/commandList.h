#pragma once

#include <grl/grl.h>

#include "buffer.h"
#include "renderPass.h"

#include "texture.h"
#include "textureView.h"
#include "descriptions/renderPassDesc.h"
#include "descriptions/textureReadDesc.h"
#include "descriptions/textureUploadDesc.h"

namespace urhi
{
class CommandList
{
public:
    CommandList() = default;
    virtual ~CommandList() = default;

    CommandList(const CommandList&) = delete;
    CommandList& operator= (const CommandList&) = delete;

    virtual void begin() = 0;

    virtual grl::Rc<RenderPass> beginRenderPass(const RenderPassDesc& desc) = 0;

    virtual void updateTexture(const grl::Rc<Texture>& texture, const TextureUploadDesc& desc) = 0;
    virtual void generateMipmaps(const grl::Rc<Texture>& texture) = 0;

    virtual void reserveBuffer(const grl::Rc<Buffer>& buffer, size_t size) = 0;

    // virtual void dispatch(const glm::ivec3& numGroups) = 0; put in compute pass

    template<typename T>
    void readTexture(const grl::Rc<TextureView>& texture, const TextureReadDesc& desc, std::vector<T>& dest) { readTextureImpl(texture, desc, dest.size() * sizeof(T), dest.data()); }
    template<typename T>
    void updateBuffer(const grl::Rc<Buffer>& buffer, T& data) { updateBufferImpl(buffer, &data, sizeof(T)); }
    template<typename T>
    void updateBuffer(const grl::Rc<Buffer>& buffer, std::vector<T> data) { updateBufferImpl(buffer, data.data(), sizeof(T)*data.size()); }
    template<typename T>
    void updateBuffer(const grl::Rc<Buffer>& buffer, T* data) { updateBufferImpl(buffer, data, sizeof(T)); }
protected:
    virtual void updateBufferImpl(const grl::Rc<Buffer>& buffer, void* data, uint32_t size) = 0;
    virtual void readTextureImpl(const grl::Rc<TextureView>& texture, const TextureReadDesc& desc, size_t destSize, void* dest) = 0;
};
}
