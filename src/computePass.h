#pragma once
#include <cstdint>
#include <grl/mem.h>

#include "buffer.h"
#include "pipeline.h"
#include "sampler.h"
#include "textureView.h"
#include "enums/resourceAccess.h"

namespace urhi
{
class ComputePass
{
public:
    ComputePass() = default;
    virtual ~ComputePass() = default;

    ComputePass(const ComputePass&) = delete;
    ComputePass& operator= (const ComputePass&) = delete;

    virtual void setPipeline(const grl::Rc<Pipeline>& pipeline) = 0;

    virtual void setUniformBuffer(const std::string& name, const grl::Rc<Buffer>& buffer) = 0;
    virtual void setStorageBuffer(const std::string& name, const grl::Rc<Buffer>& buffer) = 0;

    virtual void setTexture(const std::string& name, const grl::Rc<TextureView>& texture) = 0;
    virtual void setSampler(const std::string& name, const grl::Rc<Sampler>& sampler) = 0;
    virtual void setImage(const std::string& name, const grl::Rc<TextureView>& texture, ResourceAccess access) = 0;

    template<typename T>
    void pushConstants(T& data) { pushConstants(&data, sizeof(T)); }
    virtual void pushConstants(void* data, size_t size) = 0;

    virtual void dispatch(uint32_t groupsX, uint32_t groupsY, uint32_t groupsZ) = 0;
    virtual void end() = 0;
};
}
