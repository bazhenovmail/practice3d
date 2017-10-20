#include "FpsMeter.h"

const Timer::Clock::duration FpsMeter::refreshPeriod_{Timer::Clock::period::den / 10};

FpsMeter::FpsMeter()
{
    timer_.start();
}

void FpsMeter::frame()
{
    timer_.update();
    frames_[nextFrame_] = timer_.getInSeconds();
    timer_.reset(Timer::Clock::duration());
    nextFrame_++;
    nextFrame_ %= countLength_;
    if(counted_ < countLength_)
        counted_++;
}

bool FpsMeter::canGet() const
{
    return counted_ == countLength_;
}

float FpsMeter::getFps() const
{
    if(Timer::Clock::time_point() == lastChanged_ || lastChanged_ + refreshPeriod_ <  Timer::Clock::now())
    {
        lastChanged_ = Timer::Clock::now();
        float sum{0.f};
        for(unsigned i = 0; i < countLength_; i++)
        {
            sum += frames_[i];
        }
        fps_ = 1 / (sum / countLength_);
    }

    return fps_;
}
