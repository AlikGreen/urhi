#include "vkComputePass.h"

#include "clogr.h"
#include "vkPipeline.h"

namespace urhi
{
    VkComputePass::VkComputePass(VkDevice *device, const vk::CommandBuffer cmd)
        : VkPassBase(cmd), m_device(device), m_cmd(cmd)
    {
    }

    void VkComputePass::setPipeline(const grl::Rc<Pipeline> &pipeline)
    {
        setPipelineImpl(pipeline, vk::PipelineBindPoint::eCompute);
    }

    void VkComputePass::setUniformBuffer(const std::string &name, const grl::Rc<Buffer> &buffer)
    {
        setUniformBufferImpl(name, buffer);
    }

    void VkComputePass::setStorageBuffer(const std::string &name, const grl::Rc<Buffer> &buffer)
    {
        setStorageBufferImpl(name, buffer);
    }

    void VkComputePass::setTexture(const std::string &name, const grl::Rc<TextureView> &texture)
    {
        setTextureImpl(name, texture);
    }

    void VkComputePass::setSampler(const std::string &name, const grl::Rc<Sampler> &sampler)
    {
        setSamplerImpl(name, sampler);
    }

    void VkComputePass::setImage(const std::string &name, const grl::Rc<TextureView> &texture, const ResourceAccess access)
    {
        setImageImpl(name, texture, access);
    }

    void VkComputePass::pushConstants(void *data, const size_t size)
    {
        pushConstantsImpl(data, size);
    }

    void VkComputePass::dispatch(const uint32_t groupsX, const uint32_t groupsY, const uint32_t groupsZ)
    {
        pushDescriptorsImpl(vk::PipelineBindPoint::eCompute);
        m_cmd.dispatchBase(0, 0, 0, groupsX, groupsY, groupsZ);
    }

    void VkComputePass::end()
    {

    }
}
