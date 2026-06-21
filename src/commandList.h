#pragma once


#include <bit>

#include "buffer.h"
#include "readbackRequest.h"

#include "computePass.h"
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
    ComputePass& beginComputePass();

    void updateTexture(const TextureUploadDesc& desc);
    void generateMipmaps(const grl::Rc<Texture>& texture);
    void blitTexture(const BlitTextureDesc& desc);

    grl::Rc<ReadbackRequest> readback(const grl::Rc<Texture>& texture) { return readback(TextureReadbackDesc{texture}); }
    grl::Rc<ReadbackRequest> readback(const TextureReadbackDesc& desc);

    grl::Rc<ReadbackRequest> readback(const grl::Rc<Buffer>& buffer) { return readback(BufferReadbackDesc{buffer}); }
    grl::Rc<ReadbackRequest> readback(const BufferReadbackDesc& desc);

    void updateBuffer(const grl::Rc<Buffer>& buffer, const void* data, uint32_t size, uint32_t offset = 0);
    template<typename T>
    void updateBuffer(const grl::Rc<Buffer>& buffer, T& data, const uint32_t offset = 0) { updateBuffer(buffer, &data, sizeof(T), offset); }
    template<typename T>
    void updateBuffer(const grl::Rc<Buffer>& buffer, std::vector<T> data, const uint32_t offset = 0) { updateBuffer(buffer, data.data(), sizeof(T)*data.size(), offset); }
    template<typename T>
    void updateBuffer(const grl::Rc<Buffer>& buffer, T* data, const uint32_t offset = 0) { updateBuffer(buffer, data, sizeof(T), offset); }

    template<typename T>
    requires(
        sizeof(T) == sizeof(uint32_t) &&
        std::is_trivially_copyable_v<T>
    )
    void fillBuffer(const grl::Rc<Buffer>& buffer, T value, const uint32_t offset = 0, uint32_t size = ~0u)
    {
        const auto bits = std::bit_cast<uint32_t>(value);
        size = size != ~0u ? size : buffer->size();
        m_commands->emplace(CmdFillBuffer{ buffer, offset, size, bits });
    }
    void copyBuffer(const grl::Rc<Buffer>& src, const grl::Rc<Buffer>& dst, uint32_t size = ~0u, uint32_t srcOffset = 0, uint32_t dstOffset = 0);
protected:
    virtual grl::Rc<ReadbackRequest> createReadback() = 0;

    bool m_inUse = false;
    grl::Box<RenderPass> m_renderPass;
    grl::Box<ComputePass> m_computePass;
    grl::Rc<CommandStream> m_commands;
};
}
