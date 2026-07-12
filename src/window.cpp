#include "ravengine/window.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_syswm.h>
#include <bx/platform.h>
#include <spdlog/spdlog.h>

namespace ravengine {

Window::Window(const char* title, int width, int height)
    : m_width(width), m_height(height)
{
    if (SDL_InitSubSystem(SDL_INIT_VIDEO) != 0) {
        spdlog::error("SDL_Init failed: {}", SDL_GetError());
        return;
    }

    const Uint32 window_flags =
        SDL_WINDOW_SHOWN |
        SDL_WINDOW_RESIZABLE |
        SDL_WINDOW_ALLOW_HIGHDPI
#if BX_PLATFORM_OSX
        | SDL_WINDOW_METAL
#endif
        ;

    m_window = SDL_CreateWindow(
        title,
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        width,
        height,
        window_flags);

    if (m_window == nullptr) {
        spdlog::error("SDL_CreateWindow failed: {}", SDL_GetError());
        SDL_QuitSubSystem(SDL_INIT_VIDEO);
        return;
    }

    SDL_SysWMinfo wm_info{};
    SDL_VERSION(&wm_info.version);
    if (SDL_GetWindowWMInfo(m_window, &wm_info) != SDL_TRUE) {
        spdlog::error("SDL_GetWindowWMInfo failed: {}", SDL_GetError());
        SDL_DestroyWindow(m_window);
        m_window = nullptr;
        SDL_QuitSubSystem(SDL_INIT_VIDEO);
        return;
    }

#if BX_PLATFORM_WINDOWS
    m_native_window_handle = wm_info.info.win.window;
#elif BX_PLATFORM_OSX
    m_native_window_handle = wm_info.info.cocoa.window;
#elif BX_PLATFORM_LINUX || BX_PLATFORM_BSD
    m_native_display_handle = wm_info.info.x11.display;
    m_native_window_handle  = reinterpret_cast<void*>(static_cast<uintptr_t>(wm_info.info.x11.window));
#else
#error "Unsupported platform for SDL window handle extraction"
#endif

    if (m_native_window_handle == nullptr) {
        spdlog::error("Platform window handle is null.");
        SDL_DestroyWindow(m_window);
        m_window = nullptr;
        SDL_QuitSubSystem(SDL_INIT_VIDEO);
    }
}

Window::~Window()
{
    if (m_window != nullptr) {
        SDL_DestroyWindow(m_window);
        SDL_QuitSubSystem(SDL_INIT_VIDEO);
    }
}

bool Window::is_valid() const
{
    if (m_window == nullptr || m_native_window_handle == nullptr)
        return false;
#if BX_PLATFORM_LINUX || BX_PLATFORM_BSD
    if (m_native_display_handle == nullptr)
        return false;
#endif
    return true;
}

EventResult Window::poll_events()
{
    EventResult result;
    SDL_Event event;
    while (SDL_PollEvent(&event) != 0) {
        if (event.type == SDL_QUIT) {
            result.quit = true;
            break;
        }
        if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE) {
            result.quit = true;
            break;
        }
        if (event.type == SDL_WINDOWEVENT &&
            event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
            const int new_width  = event.window.data1;
            const int new_height = event.window.data2;
            if (new_width > 0 && new_height > 0) {
                result.resized    = true;
                result.new_width  = new_width;
                result.new_height = new_height;
                m_width           = new_width;
                m_height          = new_height;
            }
        }
    }
    return result;
}

void* Window::native_window_handle()  const { return m_native_window_handle; }
void* Window::native_display_handle() const { return m_native_display_handle; }
int   Window::width()  const                { return m_width; }
int   Window::height() const                { return m_height; }

} // namespace ravengine
