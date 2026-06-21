#include "computePass.h"

#include "commandStream.h"

namespace urhi
{
    void ComputePass::setPipeline(const grl::Rc<Pipeline> &pipeline)
    {
        m_commands->emplace(CmdSetComputePipeline{pipeline});
    }

    void ComputePass::setBuffer(const std::string_view name, const grl::Rc<Buffer> &buffer)
    {
        m_commands->emplace(CmdSetBuffer{NameRegistry::getHash(name), buffer});
    }

    void ComputePass::setImage(std::string_view name, const grl::Rc<TextureView> &texture)
    {
        m_commands->emplace(CmdSetImage{NameRegistry::getHash(name), texture});
    }

    void ComputePass::setTexture(const std::string_view name, const grl::Rc<TextureView> &texture)
    {
        m_commands->emplace(CmdSetTexture{NameRegistry::getHash(name), texture});
    }

    void ComputePass::setSampler(const std::string_view name, const grl::Rc<Sampler> &sampler)
    {
        m_commands->emplace(CmdSetSampler{NameRegistry::getHash(name), sampler});
    }

    void ComputePass::pushConstants(const void *data, const uint32_t size)
    {
        const uint32_t offset = m_commands->copyData(data, size);
        m_commands->emplace(CmdPushConstants{ offset, size });
    }

    void ComputePass::dispatch(const uint32_t groupsX, const uint32_t groupsY, const uint32_t groupsZ)
    {
        m_commands->emplace(CmdDispatchCompute{groupsX, groupsY, groupsZ});
    }

    void ComputePass::dispatchIndirect(const grl::Rc<Buffer>& buffer, uint32_t offset)
    {
        m_commands->emplace(CmdDispatchComputeIndirect{buffer, offset});
    }

    void ComputePass::end()
    {
        m_commands->emplace(CmdEndComputePass{});
    }

    ComputePass::ComputePass(const grl::Rc<CommandStream>& commands)
        : m_commands(commands) {  }
}
