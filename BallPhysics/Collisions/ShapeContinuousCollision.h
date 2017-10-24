#pragma once

#include "Vector.h"
#include "../ShapelessObject.h"

namespace BallPhysics
{

struct CollisionResult
{
    Vector normal;
    float dt;
    bool occured = false;
};

class Rectangle;
class Circle;
class Line;

CollisionResult collision( const Circle& circle, const Rectangle& rect, const ShapelessObject& circlePos, const ShapelessObject& rectPos, float maxT );
CollisionResult collision( const Circle& circle, const Line& line, const ShapelessObject& circlePos, const ShapelessObject& rectPos, float maxT );
CollisionResult collision( const Circle& circle1, const Circle& circle2, const ShapelessObject& circle1Pos, const ShapelessObject & circle2Pos, float maxT );

} //namespace