#pragma once

struct SDL_Window;

namespace ravengine {

struct EventResult {
    bool quit       = false;
    bool resized    = false;
    int  new_width  = 0;
    int  new_height = 0;
};

class Window {
public:
    Window(const char* title = "Ravengine", int width = 960, int height = 540);
    ~Window();

    Window(const Window&)            = delete;
    Window& operator=(const Window&) = delete;

    bool is_valid() const;

    EventResult poll_events();

    void* native_window_handle()  const;
    void* native_display_handle() const;

    int width()  const;
    int height() const;

private:
    SDL_Window* m_window                = nullptr;
    void*       m_native_window_handle  = nullptr;
    void*       m_native_display_handle = nullptr;
    int         m_width                 = 0;
    int         m_height                = 0;
};

} // namespace ravengine
