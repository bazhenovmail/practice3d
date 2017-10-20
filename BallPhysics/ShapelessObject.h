#pragma once
#include "Vector.h"

namespace BallPhysics
{

struct ShapelessObject
{
    Vector position;
    Vector velocity;
    float mass;
};

} //namespace BallPhysics