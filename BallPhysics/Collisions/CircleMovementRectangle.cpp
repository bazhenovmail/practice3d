#include "CircleMovementRectangle.h"

namespace BallPhysics
{

CircleMovementRectangle::CircleMovementRectangle(const Rectangle& circle, const ShapelessObject& obj, float dt)
{
    Vector movement = obj.velocity * dt;
}


}//namespace BallPhysics