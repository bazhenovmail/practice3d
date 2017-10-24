#pragma once
#include <memory>
#include "Vector.h"
#include "Collisions/ShapeContinuousCollision.h"
#include "Intersections/BoundingVolumes.h"

namespace BallPhysics
{

struct ShapelessObject;
class Circle;
class Rectangle;
class Line;

enum class ShapeType
{
    Circle,
    Line,
    Rectangle
};

class IShape;
using ShapePtr = std::unique_ptr<IShape>;
class IShape
{
public:
    virtual ~IShape() {};
    virtual ShapeType getType() const = 0;
    virtual std::pair<Rectangle, Vector> boundingVolume(const ShapelessObject& thisPos, float maxT) const = 0;
    virtual CollisionResult collisionResult(const ShapelessObject& thisPos, const ShapePtr& arg, const ShapelessObject& argPos, float maxT) const = 0;
    virtual CollisionResult collisionResult(const ShapelessObject& thisPos, const Circle&, const ShapelessObject& argPos, float maxT) const = 0;
    virtual CollisionResult collisionResult(const ShapelessObject& thisPos, const Rectangle&, const ShapelessObject& argPos, float maxT) const = 0;
    virtual CollisionResult collisionResult(const ShapelessObject& thisPos, const Line&, const ShapelessObject& argPos, float maxT) const = 0;
};

template<typename T>
class Shape: public IShape
{
public:
    Shape(const T& shape);
    virtual ShapeType getType() const override;
    virtual std::pair<Rectangle, Vector> boundingVolume(const ShapelessObject& thisPos, float maxT) const override;
    virtual CollisionResult collisionResult(const ShapelessObject& thisPos, const ShapePtr&, const ShapelessObject& argPos, float maxT) const override;
    virtual CollisionResult collisionResult(const ShapelessObject& thisPos, const Circle&, const ShapelessObject& argPos, float maxT) const override;
    virtual CollisionResult collisionResult(const ShapelessObject& thisPos, const Rectangle&, const ShapelessObject& argPos, float maxT) const override;
    virtual CollisionResult collisionResult(const ShapelessObject& thisPos, const Line&, const ShapelessObject& argPos, float maxT) const override;
    T shape;
private:

};

template<typename T>
Shape<T>::Shape(const T& sh):
    shape(sh)
{
}

template<typename T>
std::pair<Rectangle, Vector> Shape<T>::boundingVolume(const ShapelessObject& thisPos, float maxT) const
{
    return BallPhysics::boundingVolume(shape, thisPos, maxT);
}

template<typename T>
CollisionResult Shape<T>::collisionResult(const ShapelessObject& thisPos, const ShapePtr& arg, const ShapelessObject& argPos, float maxT) const
{
    return arg->collisionResult(argPos, shape, thisPos, maxT);
}

} //namespace
