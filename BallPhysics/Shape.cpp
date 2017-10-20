#include "Shape.h"
#include "Circle.h"
#include "Rectangle.h"
#include "Line.h"
#include "Collisions/ShapeContinuousCollision.h"
#include <assert.h>

namespace BallPhysics
{

template<>
ShapeType Shape<Circle>::getType() const
{
    return ShapeType::Circle;
}

template<>
ShapeType Shape<Rectangle>::getType() const
{
    return ShapeType::Rectangle;
}

template<>
ShapeType Shape<Line>::getType() const
{
    return ShapeType::Line;
}

//-----------------------------

template<>
CollisionResult Shape<Circle>::collisionResult(const ShapelessObject& thisPos, const Rectangle& arg, const ShapelessObject& argPos, float maxT) const
{
    return collision(shape, arg, thisPos, argPos, maxT);
}

template<>
CollisionResult Shape<Rectangle>::collisionResult(const ShapelessObject& thisPos, const Rectangle& arg, const ShapelessObject& argPos, float maxT) const
{
    assert(false);
    return CollisionResult();
}

template<>
CollisionResult Shape<Line>::collisionResult(const ShapelessObject& thisPos, const Rectangle& arg, const ShapelessObject& argPos, float maxT) const
{
    assert(false);
    return CollisionResult();
}

template<>
CollisionResult Shape<Circle>::collisionResult(const ShapelessObject& thisPos, const Circle& arg, const ShapelessObject& argPos, float maxT) const
{
    return collision(shape, arg, thisPos, argPos, maxT);
}

template<>
CollisionResult Shape<Rectangle>::collisionResult(const ShapelessObject& thisPos, const Circle& arg, const ShapelessObject& argPos, float maxT) const
{
    return collision(arg, shape, argPos, thisPos, maxT);
}

template<>
CollisionResult Shape<Line>::collisionResult(const ShapelessObject& thisPos, const Circle& arg, const ShapelessObject& argPos, float maxT) const
{
    return collision(arg, shape, argPos, thisPos, maxT);
}

template<>
CollisionResult Shape<Circle>::collisionResult(const ShapelessObject& thisPos, const Line& arg, const ShapelessObject& argPos, float maxT) const
{
    return collision(shape, arg, thisPos, argPos, maxT);
}

template<>
CollisionResult Shape<Rectangle>::collisionResult(const ShapelessObject& thisPos, const Line& arg, const ShapelessObject& argPos, float maxT) const
{
    assert(false);
    return CollisionResult();
}

template<>
CollisionResult Shape<Line>::collisionResult(const ShapelessObject& thisPos, const Line& arg, const ShapelessObject& argPos, float maxT) const
{
    assert(false);
    return CollisionResult();
}

} //namespace BallPhysics