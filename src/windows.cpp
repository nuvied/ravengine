#include "ravengine/windows.h"
#include "renderer.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_syswm.h>
#include <bgfx/bgfx.h>
#include <bx/platform.h>
#include <cstdint>
#include <spdlog/spdlog.h>

namespace ravengine {

bool show_basic_window(const char* title, int width, int height)
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        spdlog::error("SDL_Init failed: {}", SDL_GetError());
        return false;
    }

    const Uint32 window_flags =
        SDL_WINDOW_SHOWN |
        SDL_WINDOW_RESIZABLE |
        SDL_WINDOW_ALLOW_HIGHDPI
#if BX_PLATFORM_OSX
        | SDL_WINDOW_METAL
#endif
        ;

    SDL_Window* window = SDL_CreateWindow(
        title,
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        width,
        height,
        window_flags);

    if (window == nullptr) {
        spdlog::error("SDL_CreateWindow failed: {}", SDL_GetError());
        SDL_Quit();
        return false;
    }

    SDL_SysWMinfo wm_info{};
    SDL_VERSION(&wm_info.version);
    if (SDL_GetWindowWMInfo(window, &wm_info) != SDL_TRUE) {
        spdlog::error("SDL_GetWindowWMInfo failed: {}", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return false;
    }

    bgfx::PlatformData platform_data{};
#if BX_PLATFORM_WINDOWS
    platform_data.nwh = wm_info.info.win.window;
#elif BX_PLATFORM_OSX
    platform_data.nwh = wm_info.info.cocoa.window;
#elif BX_PLATFORM_LINUX || BX_PLATFORM_BSD
    platform_data.ndt = wm_info.info.x11.display;
    platform_data.nwh = reinterpret_cast<void*>(static_cast<uintptr_t>(wm_info.info.x11.window));
#else
#error "Unsupported platform for SDL window handle extraction"
#endif

    if (platform_data.nwh == nullptr) {
        spdlog::error("Platform window handle is null; cannot initialize bgfx.");
        SDL_DestroyWindow(window);
        SDL_Quit();
        return false;
    }

    bgfx::Init init{};
    init.type = bgfx::RendererType::Count;
    init.platformData = platform_data;
    init.resolution.width = static_cast<uint32_t>(width);
    init.resolution.height = static_cast<uint32_t>(height);
    init.resolution.reset = BGFX_RESET_VSYNC;

    if (!bgfx::init(init)) {
        spdlog::error("bgfx::init failed.");
        SDL_DestroyWindow(window);
        SDL_Quit();
        return false;
    }

    bgfx::setViewClear(0, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x181C24FF, 1.0f, 0);
    bgfx::setViewRect(0, 0, 0, bgfx::BackbufferRatio::Equal);

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
            if (event.type == SDL_WINDOWEVENT &&
                (event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED || event.window.event == SDL_WINDOWEVENT_RESIZED)) {
                const int new_width = event.window.data1;
                const int new_height = event.window.data2;
                if (new_width > 0 && new_height > 0) {
                    bgfx::reset(static_cast<uint32_t>(new_width), static_cast<uint32_t>(new_height), BGFX_RESET_VSYNC);
                }
            }
        }

        bgfx::touch(0);
        bgfx::frame();
    }

    bgfx::shutdown();
    SDL_DestroyWindow(window);
    SDL_Quit();

    return true;
}

} // namespace ravengine
