#pragma once
#include "buffer.h"
#include "glDevice.h"
#include "descriptions/bufferDesc.h"

namespace urhi
{
class GlBuffer final : public Buffer
{
public:
    GlBuffer(GlDevice* device, const BufferDesc& desc);

    [[nodiscard]] uint64_t size() const override { return m_size; }
    GLuint handle() const { return m_handle; }

    BufferUsage usage() { return m_usage; }

    bool pendingComputeWrite = false;
private:
    GLuint m_handle{};

    uint64_t m_size;
    BufferUsage m_usage;
};
}
