#pragma once

#include "Params.h"
#include <utility>

namespace BallPhysics
{
class World;
}

namespace BallInput
{
class Input;
}

class UI;

namespace Balls
{

struct FrameResult
{
    FrameResult( bool fin, float t ) : running{ fin }, dt{ t }
    {
    }
    bool running;
    float dt;
};

class IApplication
{
public:
    virtual ~IApplication()
    {
    };

    virtual Params getParams() const = 0;
    virtual void initialize( BallPhysics::World&, UI&, BallInput::Input& ) noexcept = 0;
    virtual FrameResult frame() = 0;
};

extern IApplication& getApplication();

} //namespace
