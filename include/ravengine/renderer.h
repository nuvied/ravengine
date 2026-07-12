#pragma once
#include <bgfx/bgfx.h>
#include <bgfx/platform.h>
#include <cstdint>
namespace ravengine {
    class Renderer {
    public:
        bool init(void* nativeWindowHandle, void* nativeDisplayHandle, uint32_t width, uint32_t height);
        void resize(uint32_t width, uint32_t height);
        void shutdown();
        void render();

namespace ravengine {

class Renderer {
public:
    bool init(void* nativeWindowHandle, void* nativeDisplayHandle, uint32_t width, uint32_t height);
    void resize(uint32_t width, uint32_t height);
    void shutdown();
    void render();

private:
    uint32_t     m_width      = 0;
    uint32_t     m_height     = 0;
    uint32_t     m_clearColor = 0x181c24ff;
    bgfx::ViewId m_mainView   = 0;
};

} // namespace ravengine