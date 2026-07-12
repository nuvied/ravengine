#include "ravengine/engine.h"
#include "ravengine/window.h"
#include "ravengine/renderer.h"

#include <spdlog/spdlog.h>

namespace ravengine {

Engine::Engine()  = default;
Engine::~Engine() = default;

void Engine::run()
{
    spdlog::info("RavenEngine starting...");

    m_window = std::make_unique<Window>();
    if (!m_window->is_valid()) {
        spdlog::error("Failed to create the main window.");
        return;
    }

    m_renderer = std::make_unique<Renderer>();
    if (!m_renderer->init(
            m_window->native_window_handle(),
            m_window->native_display_handle(),
            static_cast<uint32_t>(m_window->width()),
            static_cast<uint32_t>(m_window->height()))) {
        spdlog::error("Failed to initialize renderer.");
        return;
    }

    bool running = true;
    while (running) {
        const EventResult ev = m_window->poll_events();
        if (ev.quit) {
            running = false;
        }
        if (ev.resized) {
            m_renderer->resize(
                static_cast<uint32_t>(ev.new_width),
                static_cast<uint32_t>(ev.new_height));
        }
        m_renderer->render();
    }

    m_renderer->shutdown();
    spdlog::info("RavenEngine shutting down.");
}

} // namespace ravengine

