#include "VkReadbackRequest.h"

#include "clogr.h"
#include "vkDevice.h"

namespace urhi
{
    VkReadbackRequest::~VkReadbackRequest()
    {
        vmaDestroyBuffer(m_device->getAllocator(), m_buffer, m_bufferAllocation);
    }

    bool VkReadbackRequest::isReady() const
    {
        if(m_timeline == VK_NULL_HANDLE) return false;

        return m_device->getHandle().getSemaphoreCounterValue(m_timeline) >= m_waitValue;
    }

    void VkReadbackRequest::wait() const
    {
        clogr::ensure(m_timeline != VK_NULL_HANDLE, "Tried to wait on readback before submitting command list");

        const vk::SemaphoreWaitInfo waitInfo { {}, {m_timeline}, {m_waitValue} };
        auto res = m_device->getHandle().waitSemaphores(waitInfo, UINT64_MAX);
    }

    const void* VkReadbackRequest::data() const
    {
        return m_mapped;
    }

    size_t VkReadbackRequest::size() const
    {
        return m_size;
    }
}
