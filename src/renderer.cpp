#include "renderer.h"
#include <spdlog/spdlog.h>

bool renderer::init(void* nativeWindowHandle, void* nativeDisplayHandle, uint32_t width, uint32_t height)
{
    m_width  = width;
    m_height = height;

    bgfx::PlatformData pd{};
    pd.nwh = nativeWindowHandle;
    pd.ndt = nativeDisplayHandle;
    bgfx::setPlatformData(pd);

    // Calling renderFrame() before init() opts into single-threaded mode.
    // This is required on macOS where Cocoa must be driven from the main thread.
    bgfx::renderFrame();

    bgfx::Init init;
    init.type                  = bgfx::RendererType::Count; // auto-select backend
    init.resolution.width      = m_width;
    init.resolution.height     = m_height;
    init.resolution.reset      = BGFX_RESET_VSYNC;
    init.platformData         = pd;
    if(!bgfx::init(init)) {
        spdlog::error("Failed to initialize bgfx.");
        return false;
    }

    bgfx::setViewClear(m_mainView,
        BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH,
        m_clearColor,
        1.0f, 0);
    bgfx::setViewRect(m_mainView, 0, 0, m_width, m_height);
    return true;
}

void renderer::resize(uint32_t width, uint32_t height)
{
    m_width = width;
    m_height = height;
    bgfx::reset(m_width, m_height, BGFX_RESET_VSYNC);
    bgfx::setViewRect(m_mainView, 0, 0, m_width, m_height);
}

void renderer::shutdown()
{
    bgfx::shutdown();
}

void renderer::render()
{
    bgfx::touch(m_mainView);
    bgfx::frame();
}

