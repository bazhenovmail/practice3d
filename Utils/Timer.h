#pragma once
#include <chrono>

class Timer
{
public:
    using Clock = std::chrono::high_resolution_clock;
    void start();
    void update();
    void reset( Clock::duration );
    float getInSeconds() const;
private:
    static_assert( sizeof( Clock::duration ) == 8, "Deficient clock" );

    Clock::duration totalTime_{ Clock::duration() };
    Clock::time_point lastMeasured_{ Clock::time_point() };
    bool started_{ false };
};

