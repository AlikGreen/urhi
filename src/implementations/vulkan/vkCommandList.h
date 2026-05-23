#pragma once
#include "commandList.h"
#include <vulkan/vulkan.hpp>

#include "commandStream.h"
#include "vkCommandQueue.h"
#include "vkReadbackRequest.h"
#include "vkSubmissionContext.h"
#include "enums/queueType.h"

namespace urhi
{
class VkDevice;
class VkCommandListPool;

class VkCommandList final : public CommandList
{
public:
    explicit VkCommandList(VkDevice* device, VkCommandQueue* queue, VkSubmissionContext* submissionContext)
        : m_device(device), m_submissionContext(submissionContext), m_queue(queue) { }

    [[nodiscard]] VkSubmissionContext* submissionContext() const { return m_submissionContext; }
    [[nodiscard]] VkCommandQueue* queue() const { return m_queue; }
    grl::Rc<CommandStream> commands() const { return m_commands; }
    void submit() { m_inUse = false; }
    bool inUse() const { return m_inUse; }
    void reset(VkSubmissionContext* submissionContext, VkCommandQueue* queue)
    {
        m_commands->clear();
        m_submissionContext = submissionContext;
        m_queue = queue;
    }
protected:
    grl::Rc<ReadbackRequest> createReadback() override { return grl::makeBox<VkReadbackRequest>(); };

private:
    VkDevice* m_device;
    VkSubmissionContext* m_submissionContext;
    VkCommandQueue* m_queue;
};
}

