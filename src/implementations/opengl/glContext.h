#pragma once

#include "context.h"
#include <filesystem>
#include <glad/gl.h>

#include "logger.h"

namespace urhi
{
class GlSwapchain;

class GlContext final : public Context
{
public:
    explicit GlContext(const ContextDesc& desc);

    grl::Rc<Device> createDevice(const DeviceDesc& desc) override;
    grl::Rc<Window> createWindow(const WindowDesc& desc) override;
    grl::Rc<Swapchain> createSwapchain(const SwapchainDesc& desc) override;

    clogr::Logger& logger() override;

    std::filesystem::path cachePath() { return m_cachePath; }
private:
    grl::Rc<clogr::Logger> m_logger;
    std::filesystem::path m_cachePath;
};

}
