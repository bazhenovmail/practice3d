#pragma once
#include <World.h>
#include <Graphics.h>

namespace BallsGame
{

class Brick
{
public:
    BallPhysics::Id physicsId{ 0 };
    BallGraphics::Id graphicsIdForeground{ 0 };
	BallGraphics::Id graphicsIdBackground{ 0 };
	unsigned durability{ 0 };
};

}//namespace BallsGame