#pragma once
#include <cstdint>
#include <grl/mem.h>

#include "buffer.h"
#include "commandStream.h"
#include "pipeline.h"
#include "sampler.h"
#include "textureView.h"
#include "enums/resourceAccess.h"

namespace urhi
{
class ComputePass final
{
public:
    ComputePass(const ComputePass&)            = delete;
    ComputePass& operator=(const ComputePass&) = delete;
    ComputePass(ComputePass&&)                 = delete;
    ComputePass& operator=(ComputePass&&)      = delete;

    void setPipeline(const grl::Rc<Pipeline>& pipeline);

    void setBuffer(const HashedName &name, const grl::Rc<Buffer>& buffer) const
    {
        m_commands->emplace(CmdSetBuffer{name.hash, buffer});
    }

    void setTexture(const HashedName &name, const grl::Rc<TextureView>& texture) const
    {
        m_commands->emplace(CmdSetTexture{name.hash, texture});
    }

    void setSampler(const HashedName &name, const grl::Rc<Sampler>& sampler) const
    {
        m_commands->emplace(CmdSetSampler{name.hash, sampler});
    }

    void setBuffer(std::string_view name, const grl::Rc<Buffer>& buffer);
    void setImage(std::string_view name, const grl::Rc<TextureView>& texture);
    void setTexture(std::string_view name, const grl::Rc<TextureView>& texture);
    void setSampler(std::string_view name, const grl::Rc<Sampler>& sampler);

    template<typename T>
    void pushConstants(T& data) { pushConstants(&data, sizeof(T)); }
    void pushConstants(const void* data, uint32_t size);

    void dispatch(uint32_t groupsX, uint32_t groupsY, uint32_t groupsZ);
    void dispatchIndirect(const grl::Rc<Buffer>& buffer, uint32_t offset = 0);

    void end();
private:
    friend class CommandList;
    explicit ComputePass(const grl::Rc<CommandStream>& commands);

    grl::Rc<CommandStream> m_commands;
};
}
