#include "ravengine/engine.h"
#include "ravengine/windows.h"

#include <spdlog/spdlog.h>

namespace ravengine {

void Engine::run()
{
    spdlog::info("RavenEngine starting...");
    if (!show_basic_window()) {
        spdlog::error("Failed to show the main window.");
    }
    spdlog::info("RavenEngine shutting down.");
}

} // namespace ravengine
