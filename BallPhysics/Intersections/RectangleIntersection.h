#pragma once
#include "../Rectangle.h"
#include "Vector.h"
namespace BallPhysics
{

bool rectangleIntersection(const Rectangle& rect1, const Vector& rect1Pos, const Rectangle& rect2, const Vector& rect2Pos);

}//namespace BallPhysics

