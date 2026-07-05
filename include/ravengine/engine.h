#pragma once

namespace ravengine {

class Engine
{
public:
    Engine() = default;
    virtual ~Engine() = default;

    void run();
};

} // namespace ravengine
