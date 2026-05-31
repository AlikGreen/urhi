#include "glBuffer.h"

#include "glConvert.h"

namespace urhi
{
    GlBuffer::GlBuffer(GlDevice *device, const BufferDesc &desc)
        : m_size(desc.size), m_usage(desc.usage)
    {
        glCreateBuffers(1, &m_handle);
        glNamedBufferData(m_handle, m_size, nullptr, GlConvert::bufferUsage(desc.usage));
    }
}
