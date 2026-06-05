#include "GlReadbackRequest.h"

#include "clogr.h"
#include "validation.h"
#include "glDevice.h"

namespace urhi
{
    GlReadbackRequest::~GlReadbackRequest()
    {
        glUnmapNamedBuffer(m_buffer);
        glDeleteBuffers(1, &m_buffer);
        glDeleteSync(m_fence);
    }

    bool GlReadbackRequest::isReady() const
    {
        if(m_buffer == 0) return false;

        const GLenum result = glClientWaitSync(m_fence, GL_SYNC_FLUSH_COMMANDS_BIT, 0);
        URHI_VALIDATE(result != GL_WAIT_FAILED, "Failed to check fence");

        return result == GL_ALREADY_SIGNALED || result == GL_CONDITION_SATISFIED;
    }

    void GlReadbackRequest::wait() const
    {
        URHI_VALIDATE(m_buffer != 0, "Readback request not submitted before being waited on - readback request must be submitted before being waited on");

        const GLenum result = glClientWaitSync(m_fence, GL_SYNC_FLUSH_COMMANDS_BIT, GL_TIMEOUT_IGNORED);
        URHI_VALIDATE(result != GL_WAIT_FAILED && result != GL_TIMEOUT_EXPIRED, "Failed to wait on fence");
    }

    const void* GlReadbackRequest::data() const
    {
        URHI_VALIDATE(m_buffer != 0, "Readback request not submitted before getting the data - readback request must be submitted before the .data() is run");
        if(!m_mapped)
            m_mapped = glMapNamedBuffer(m_buffer, GL_READ_ONLY);

        return m_mapped;
    }

    size_t GlReadbackRequest::size() const
    {
        return m_size;
    }
}
