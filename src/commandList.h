#pragma once


#include "buffer.h"
#include "computePass.h"
#include "readbackRequest.h"
#include "renderPass.h"

#include "texture.h"
#include "textureView.h"
#include "descriptions/renderPassDesc.h"
#include "descriptions/textureReadbackDesc.h"
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
    virtual grl::Rc<ComputePass> beginComputePass() = 0;

    virtual void updateTexture(const TextureUploadDesc& desc) = 0;
    virtual void generateMipmaps(const grl::Rc<Texture>& texture) = 0;

    grl::Rc<ReadbackRequest> readback(const grl::Rc<Texture>& texture) { return readback(TextureReadbackDesc{texture}); }
    virtual grl::Rc<ReadbackRequest> readback(const TextureReadbackDesc& desc) = 0;
    // TODO add reading back buffers

    template<typename T>
    void updateBuffer(const grl::Rc<Buffer>& buffer, T& data) { updateBufferImpl(buffer, &data, sizeof(T)); }
    template<typename T>
    void updateBuffer(const grl::Rc<Buffer>& buffer, std::vector<T> data) { updateBufferImpl(buffer, data.data(), sizeof(T)*data.size()); }
    template<typename T>
    void updateBuffer(const grl::Rc<Buffer>& buffer, T* data) { updateBufferImpl(buffer, data, sizeof(T)); }
protected:
    virtual void updateBufferImpl(const grl::Rc<Buffer>& buffer, void* data, uint32_t size) = 0;
};
}
