#include "clogr.h"
#include "context.h"
#include "window.h"

#include "imgui/imGuiController.h"

int main()
{
    using namespace urhi;
    using namespace grl;

    const auto context = Context::create({
        .api = BackendAPI::OpenGL,
        .cachePath = "./cache/shaders",
        .debug = true,
    });

    const auto window = context->createWindow({
        .title = "ImGui Example",
        .width = 1280,
        .height = 720,
    });

    const auto device = context->createDevice({
        .window = window
    });

    auto swapchain = context->createSwapchain({
        .window      = window,
        .device      = device,
        .presentMode = PresentMode::VSync,
    });

    ImGui::CreateContext();

    ImGuiController imGui({
        .device = device,
        .window = window,
    });

    bool  running    = true;
    bool  showDemo   = true;
    float clearColor[3] = { 0.39f, 0.58f, 0.93f };

    window->show();

    auto start = std::chrono::high_resolution_clock::now();
    float dt = 0.0f;

    while (running)
    {
        auto events = window->pollEvents();
        for (auto& event : events)
        {
            imGui.processEvent(event);

            if (event.type == Event::Type::Quit)
                running = false;
            if (event.type == Event::Type::WindowResize)
                swapchain->resize(window->width(), window->height());
        }

        imGui.newFrame();


        ImGui::Begin("Controls");
        ImGui::Checkbox("Show Demo Window", &showDemo);
        ImGui::ColorEdit3("Clear Color", clearColor);
        ImGui::Text("%.1f FPS", 1.0f/dt);
        ImGui::End();

        if (showDemo)
            ImGui::ShowDemoWindow(&showDemo);

        imGui.endFrame();

        auto cmd        = device->acquireCommandList(QueueType::Graphics);
        const auto backBuffer = swapchain->acquireNextImage();

        cmd->begin();

        cmd->blitTexture({
            .src    = imGui.getFramebufferTexture(),
            .dst    = backBuffer->texture(),
            .filter = TextureFilter::Linear,
        });

        device->submit(cmd);
        swapchain->present();

        auto end = std::chrono::high_resolution_clock::now();
        dt = std::chrono::duration<float>(end - start).count();
        start = end;
    }

    ImGui::DestroyContext();
}