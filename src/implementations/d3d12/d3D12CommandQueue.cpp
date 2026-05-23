#include "d3D12CommandQueue.h"

#include "d3D12CommandList.h"
#include "d3D12CommandListEmitter.h"
#include "d3D12CommandListTracker.h"
#include "d3D12Context.h"

namespace urhi
{
    D3D12CommandQueue::D3D12CommandQueue(D3D12Device* device, const D3D12_COMMAND_LIST_TYPE type)
        : m_device(device), m_type(type)
    {
        D3D12_COMMAND_QUEUE_DESC desc{};
        desc.Type = type;
        desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;

        auto hr = device->device()->CreateCommandQueue(&desc, IID_PPV_ARGS(&m_queue));
        URHI_VALIDATE(SUCCEEDED(hr), "Failed to create command queue of type {}", static_cast<int>(type));

        hr = device->device()->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_fence));
        URHI_VALIDATE(SUCCEEDED(hr), "Failed to create fence");

        for(auto& ctx : m_submissionContexts)
        {
            ctx.init(device, type);
        }
    }

    D3D12SubmissionContext& D3D12CommandQueue::acquireSubmissionContext()
    {
        auto& ctx = m_submissionContexts[m_nextSubmissionIndex];
        m_nextSubmissionIndex = (m_nextSubmissionIndex + 1) % m_submissionContexts.size();

        const auto hr = m_queue->Wait(m_fence.Get(), ctx.lastSubmitValue());
        URHI_VALIDATE(SUCCEEDED(hr), "Failed to wait on fence");

        ctx.reset();

        return ctx;
    }

    void D3D12CommandQueue::submit(const grl::Rc<D3D12CommandList> &cmdList)
    {
        auto& submissionCtx = acquireSubmissionContext();
        auto cmdBuffer = submissionCtx.commandAllocator().acquireCommandList();

        // const auto commandStream = cmdList->commands();
        // D3D12CommandListTracker tracker;
        // for (auto& cmd : commandStream->commands())
        //     std::visit([&](auto& c) { tracker.record(c); }, cmd);
        //
        // D3D12CommandListEmitter emitter{submitValue};
        // for (auto& cmd : commandStream->commands())
        //     std::visit([&](auto& c) { emitter.emit(c); }, cmd);
        //
        // emitter.endRecording();
        //
        //
        // if(m_type == D3D12_COMMAND_LIST_TYPE_DIRECT)
        // {
        //     waitSwapchainFence = m_device->context()->swapchain()->consumeReadySemaphore();
        // }
        //
        // std::scoped_lock lock(m_mutex);
        //
        // const uint64_t signalValue = ++m_currentTimelineValue;
        //
        // // context cannot be reset until the GPU reaches this timeline value
        // context->completionValue(signalValue);
        //
        // std::vector<vk::SemaphoreSubmitInfo> waitSemaphoreInfos;
        // std::vector<vk::SemaphoreSubmitInfo> signalSemaphoreInfos;
        //
        // if (waitSwapchainSemaphore)
        // {
        //     vk::SemaphoreSubmitInfo waitInfo{};
        //     waitInfo.semaphore = waitSwapchainSemaphore;
        //     waitInfo.value = 0; // binary semaphore
        //     waitInfo.stageMask = vk::PipelineStageFlagBits2::eAllCommands;
        //     waitSemaphoreInfos.push_back(waitInfo);
        // }
        //
        // vk::SemaphoreSubmitInfo timelineSignalInfo{};
        // timelineSignalInfo.semaphore = m_timelineSemaphore;
        // timelineSignalInfo.value = signalValue;
        // timelineSignalInfo.stageMask = vk::PipelineStageFlagBits2::eAllCommands;
        // signalSemaphoreInfos.push_back(timelineSignalInfo);
        //
        // vk::CommandBufferSubmitInfo cmdInfo{};
        // cmdInfo.commandBuffer = cmd;



        // m_queue->ExecuteCommandLists(1, {submitInfo});

        m_nextSubmissionIndex = (m_nextSubmissionIndex + 1) % kMaxInFlight;
        m_submitValue++;
    }
}
