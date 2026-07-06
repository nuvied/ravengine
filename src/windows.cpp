#include "ravengine/windows.h"
#include "renderer.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_syswm.h>
#include <spdlog/spdlog.h>

namespace ravengine {

bool show_basic_window(const char* title, int width, int height)
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        spdlog::error("SDL_Init failed: {}", SDL_GetError());
        return false;
    }

    SDL_Window* window = SDL_CreateWindow(
        title,
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        width,
        height,
        SDL_WINDOW_SHOWN);

    if (window == nullptr) {
        spdlog::error("SDL_CreateWindow failed: {}", SDL_GetError());
        SDL_Quit();
        return false;
    }

    // Retrieve native window/display handles for bgfx.
    SDL_SysWMinfo wmi;
    SDL_VERSION(&wmi.version);
    if (!SDL_GetWindowWMInfo(window, &wmi)) {
        spdlog::error("SDL_GetWindowWMInfo failed: {}", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return false;
    }

    void* nwh = nullptr;
    void* ndt = nullptr;
    switch (wmi.subsystem) {
#if defined(SDL_VIDEO_DRIVER_COCOA)
        case SDL_SYSWM_COCOA:   nwh = wmi.info.cocoa.window;                               break;
#endif
#if defined(SDL_VIDEO_DRIVER_WINDOWS)
        case SDL_SYSWM_WINDOWS: nwh = wmi.info.win.window;                                 break;
#endif
#if defined(SDL_VIDEO_DRIVER_X11)
        case SDL_SYSWM_X11:     nwh = reinterpret_cast<void*>(wmi.info.x11.window);
                                ndt = wmi.info.x11.display;                                break;
#endif
#if defined(SDL_VIDEO_DRIVER_WAYLAND)
        case SDL_SYSWM_WAYLAND: nwh = wmi.info.wl.surface;
                                ndt = wmi.info.wl.display;                                 break;
#endif
        default:
            spdlog::error("Unsupported window subsystem ({}), cannot init bgfx.", static_cast<int>(wmi.subsystem));
            SDL_DestroyWindow(window);
            SDL_Quit();
            return false;
    }

    renderer gfx;
    if(nwh == nullptr) {
        spdlog::error("Failed to retrieve native window handle, cannot init bgfx.");
        SDL_DestroyWindow(window);
        SDL_Quit();
        return false;
    }
    gfx.init(nwh, ndt, static_cast<uint16_t>(width), static_cast<uint16_t>(height));

    bool running = true;
    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event) != 0) {
            if (event.type == SDL_QUIT) {
                running = false;
            }
            if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE) {
                running = false;
            }
        }

        gfx.render();
    }

    gfx.shutdown();
    SDL_DestroyWindow(window);
    SDL_Quit();

    return true;
}

} // namespace ravengine
