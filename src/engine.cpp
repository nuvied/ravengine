#include "ravengine/engine.h"

#include <spdlog/spdlog.h>

namespace ravengine {

void Engine::run()
{
    spdlog::info("RavenEngine starting...");
    spdlog::info("RavenEngine shutting down.");
}

} // namespace ravengine
