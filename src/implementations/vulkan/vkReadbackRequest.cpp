#include "VkReadbackRequest.h"

#include "clogr.h"
#include "validation.h"
#include "vkDevice.h"

namespace urhi
{
    VkReadbackRequest::~VkReadbackRequest()
    {
        vmaDestroyBuffer(m_device->allocator(), m_buffer, m_bufferAllocation);
    }

    bool VkReadbackRequest::isReady() const
    {
        if(m_timeline == VK_NULL_HANDLE) return false;

        return m_device->handle().getSemaphoreCounterValue(m_timeline) >= m_waitValue;
    }

    void VkReadbackRequest::wait() const
    {
        URHI_VALIDATE(m_timeline != VK_NULL_HANDLE, "Readback request not submitted before being waited on - readback request must be submitted before being waited on");

        const vk::SemaphoreWaitInfo waitInfo { {}, {m_timeline}, {m_waitValue} };
        const auto res = m_device->handle().waitSemaphores(waitInfo, UINT64_MAX);
        URHI_VALIDATE(res == vk::Result::eSuccess, "Failed to wait on semaphore - vk::Device::waitSemaphores returned {}", vk::to_string(res));
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
