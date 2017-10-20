#include "Timer.h"

void Timer::start()
{
    if(!started_)
    {
        lastMeasured_ = Clock::now();
        started_ = true;
    }
}

void Timer::update()
{
    if(started_)
    {
        totalTime_ += Clock::now() - lastMeasured_;
    }
}

void Timer::reset(Clock::duration dur)
{
    totalTime_ = dur;
    lastMeasured_ = Clock::now();
}

float Timer::getInSeconds() const
{
    return float(totalTime_.count()) / Clock::period::den;
}
