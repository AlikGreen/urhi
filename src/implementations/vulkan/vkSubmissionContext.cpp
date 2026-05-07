#include "vkSubmissionContext.h"


#include "clogr.h"
#include "vkCommandList.h"

namespace urhi
{
    VkSubmissionContext::VkSubmissionContext(VkDevice* device, uint32_t queueFamily)
    {
        m_device = device;

        m_stagingAllocator.init(m_device);
        m_commandAllocator.init(m_device, queueFamily);
        m_descriptorAllocator.init(m_device);
    }

    VkSubmissionContext::~VkSubmissionContext()
    {
        m_commandAllocator.destroy();
        m_stagingAllocator.destroy();
        m_descriptorAllocator.destroy();
    }

    void VkSubmissionContext::reset()
    {
        m_commandAllocator.reset();
        m_stagingAllocator.reset();
        m_descriptorAllocator.reset();
    }
}
