#include "Application.h"

#include <Input.h>
#include <Graphics.h>
#include <World.h>
#include <Rectangle.h>
#include <Circle.h>
#include <Line.h>
#include <limits>
#include <Vector.h>

namespace
{
BallsGame::Application application;
}

using namespace DirectX;
using namespace BallPhysics;
using namespace BallUtils;

Balls::IApplication& Balls::getApplication()
{
    return application;
}

namespace BallsGame
{

Balls::Params Application::getParams() const
{
    return Balls::Params();
}

void Application::initialize( BallPhysics::World& world, UI& ui, BallInput::Input& input ) noexcept
{
    world_ = &world;
    ui_ = &ui;
    input_ = &input;

    mainMenu_.initialize( ui, input, [this]()
    {
        game_.enter();
    }, [this]()
    {
        termination_ = true;
    } );
    game_.initialize( ui, input, world,
                      [this]()
    {
        dt_ = 0.f;
    },
                      [this]()
    {
        dt_ = defaultDt_;
    },
        [this]()
    {
        mainMenu_.enter();
    } );

    mainMenu_.enter();
}

Balls::FrameResult Application::frame()
{

#ifdef STEPBYSTEP
    if ( input_->getAndReleaseDownKey( VK_SPACE ) )
    {
        return{ !termination_, true };
    }
    return{ !termination_, false };
#else
    return{ !termination_, dt_ };
#endif
}

} //namespace