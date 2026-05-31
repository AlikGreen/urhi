#pragma once
#include "commandList.h"

#include "commandStream.h"
#include "glReadbackRequest.h"

namespace urhi
{
class GlDevice;
class GlCommandListPool;

class GlCommandList final : public CommandList
{
public:
    explicit GlCommandList(GlDevice* device)
        : m_device(device) { }

    grl::Rc<CommandStream> commands() const { return m_commands; }
    void submit() { m_inUse = false; }
    bool inUse() const { return m_inUse; }
    void reset() const { m_commands->clear(); }
protected:
    grl::Rc<ReadbackRequest> createReadback() override { return grl::makeBox<GlReadbackRequest>(); };
private:
    GlDevice* m_device;
};
}

