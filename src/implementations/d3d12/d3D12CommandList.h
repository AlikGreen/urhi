#pragma once
#include "commandList.h"
#include "enums/queueType.h"

namespace urhi
{
class D3D12CommandList final : public CommandList
{
public:
    explicit D3D12CommandList(QueueType queueType);

    QueueType queueType() const { return m_queueType; }
    bool inUse() const { return m_inUse; }
    void submit() { m_inUse = false; }

    grl::Rc<CommandStream> commands() { return m_commands; }
protected:
    grl::Rc<ReadbackRequest> createReadback() override;
private:
    QueueType m_queueType;
};
}
