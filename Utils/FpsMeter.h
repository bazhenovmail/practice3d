#pragma once
#include "Timer.h"
#include <array>

class FpsMeter
{
public:
    FpsMeter();
    void frame();
    bool canGet() const;
    float getFps() const;
private:

    const static Timer::Clock::duration refreshPeriod_;
    constexpr static unsigned countLength_ = 10;

    Timer timer_;
    std::array<float, countLength_> frames_;
    size_t counted_{ 0u };
    size_t nextFrame_{ 0u };
    mutable Timer::Clock::time_point lastChanged_{};
    mutable float fps_;
};

