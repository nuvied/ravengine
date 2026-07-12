#pragma once
#include <memory>

namespace ravengine {

class Window;
class Renderer;

class Engine
{
public:
    Engine();
    ~Engine();

    void run();

private:
    std::unique_ptr<Window>   m_window;
    std::unique_ptr<Renderer> m_renderer;
};

} // namespace ravengine
