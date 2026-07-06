#include "ravengine/windows.h"
#include "renderer.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_syswm.h>
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

    void* native_window_handle = nullptr;
    void* native_display_handle = nullptr;
#if BX_PLATFORM_WINDOWS
    native_window_handle = wm_info.info.win.window;
#elif BX_PLATFORM_OSX
    native_window_handle = wm_info.info.cocoa.window;
#elif BX_PLATFORM_LINUX || BX_PLATFORM_BSD
    native_display_handle = wm_info.info.x11.display;
    native_window_handle = reinterpret_cast<void*>(static_cast<uintptr_t>(wm_info.info.x11.window));
#else
#error "Unsupported platform for SDL window handle extraction"
#endif

    if (native_window_handle == nullptr) {
        spdlog::error("Platform window handle is null; cannot initialize bgfx.");
        SDL_DestroyWindow(window);
        SDL_Quit();
        return false;
    }

    renderer renderer;
    // if (!renderer.init(native_window_handle,
    //                          native_display_handle,
    //                          static_cast<uint32_t>(width),
    //                          static_cast<uint32_t>(height))) {
    //     SDL_DestroyWindow(window);
    //     SDL_Quit();
    //     return false;
    // }

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
                // if (new_width > 0 && new_height > 0) {
                //     renderer.resize(static_cast<uint32_t>(new_width), static_cast<uint32_t>(new_height));
                // }
            }
        }

       // renderer.render();
    }

    // renderer.shutdown();
    SDL_DestroyWindow(window);
    SDL_Quit();

    return true;
}

} // namespace ravengine
