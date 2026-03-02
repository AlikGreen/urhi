#include "vkComputePass.h"

#include "clogr.h"
#include "vkPipeline.h"

namespace urhi
{

    VkComputePass::VkComputePass(std::vector<Command> &commands)
        : m_commands(commands)
    {
        m_commands.push_back(CmdBeginComputePass{});
    }

    void VkComputePass::setPipeline(const grl::Rc<Pipeline> &pipeline)
    {
        m_commands.push_back(CmdSetPipeline{pipeline, vk::PipelineBindPoint::eCompute});
    }

    void VkComputePass::setUniformBuffer(const std::string &name, const grl::Rc<Buffer> &buffer)
    {
        m_commands.push_back(CmdSetUniformBuffer{ name, buffer });
    }

    void VkComputePass::setStorageBuffer(const std::string &name, const grl::Rc<Buffer> &buffer)
    {
        m_commands.push_back(CmdSetStorageBuffer{ name, buffer });
    }

    void VkComputePass::setTexture(const std::string &name, const grl::Rc<TextureView> &texture)
    {
        m_commands.push_back(CmdSetTexture{ name, texture });
    }

    void VkComputePass::setSampler(const std::string &name, const grl::Rc<Sampler> &sampler)
    {
        m_commands.push_back(CmdSetSampler{ name, sampler });
    }

    void VkComputePass::setImage(const std::string &name, const grl::Rc<TextureView> &texture, const ResourceAccess access)
    {
        m_commands.push_back(CmdSetImage{ name, texture, access });
    }

    void VkComputePass::pushConstants(void *data, const size_t size)
    {
        std::vector<uint8_t> vec{};
        vec.resize(size);
        std::memcpy(vec.data(), data, size);

        m_commands.push_back(CmdPushConstants{ vec });
    }

    void VkComputePass::dispatch(const uint32_t groupsX, const uint32_t groupsY, const uint32_t groupsZ)
    {
        m_commands.push_back(CmdDispatchCompute{ groupsX, groupsY, groupsZ });
    }

    void VkComputePass::end()
    {

    }
}
