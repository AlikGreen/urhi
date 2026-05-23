#pragma once


#include "buffer.h"
#include "computePass.h"
#include "readbackRequest.h"
#include "renderPass.h"

#include "texture.h"
#include "descriptions/blitTextureDesc.h"
#include "descriptions/bufferReadbackDesc.h"
#include "descriptions/renderPassDesc.h"
#include "descriptions/textureReadbackDesc.h"
#include "descriptions/textureUploadDesc.h"

namespace urhi
{
class CommandStream;

class CommandList
{
public:
    CommandList();
    virtual ~CommandList() = default;

    CommandList(const CommandList&) = delete;
    CommandList& operator= (const CommandList&) = delete;

    void begin();

    RenderPass& beginRenderPass(const RenderPassDesc& desc);
    // virtual grl::Rc<ComputePass> beginComputePass() = 0;

    void updateTexture(const TextureUploadDesc& desc);
    void generateMipmaps(const grl::Rc<Texture>& texture);
    void blitTexture(const BlitTextureDesc& desc);

    grl::Rc<ReadbackRequest> readback(const grl::Rc<Texture>& texture) { return readback(TextureReadbackDesc{texture}); }
    grl::Rc<ReadbackRequest> readback(const TextureReadbackDesc& desc);

    grl::Rc<ReadbackRequest> readback(const grl::Rc<Buffer>& buffer) { return readback(BufferReadbackDesc{buffer}); }
    grl::Rc<ReadbackRequest> readback(const BufferReadbackDesc& desc);

    void updateBuffer(const grl::Rc<Buffer>& buffer, const void* data, uint32_t size);
    template<typename T>
    void updateBuffer(const grl::Rc<Buffer>& buffer, T& data) { updateBuffer(buffer, &data, sizeof(T)); }
    template<typename T>
    void updateBuffer(const grl::Rc<Buffer>& buffer, std::vector<T> data) { updateBuffer(buffer, data.data(), sizeof(T)*data.size()); }
    template<typename T>
    void updateBuffer(const grl::Rc<Buffer>& buffer, T* data) { updateBuffer(buffer, data, sizeof(T)); }
protected:
    virtual grl::Rc<ReadbackRequest> createReadback() = 0;

    bool m_inUse = false;
    RenderPass m_renderPass;
    grl::Rc<CommandStream> m_commands;
};
}
