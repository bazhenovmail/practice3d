#pragma once

#include "../Rectangle.h"
#include "../ShapelessObject.h"

namespace BallPhysics
{

class CircleMovementRectangle
{
public:
    CircleMovementRectangle(const Rectangle&, const ShapelessObject&, float dt);
};

}//namespace BallPhysics