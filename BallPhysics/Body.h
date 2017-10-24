#pragma once
#include "Shape.h"
#include "ShapelessObject.h"
#include <functional>

namespace BallPhysics
{

class Body
{
public:
    Body( const ShapelessObject&, bool userControlled );

    using VelocityChanger = std::function<void( ShapelessObject& )>;

    Vector getPosition() const noexcept;
    Vector getVelocity() const noexcept;
    CollisionResult collisionResult( const Body&, float maxT ) const;
    std::pair<Rectangle, Vector> boundingVolume( float maxT ) const;

    void affectVelocity( const VelocityChanger& changer );
    void locomote( float dt );
    void processCollision( const CollisionResult& result, Body& arg );

    void setLine( const Line& );
    void setCircle( const Circle& );
    void setRectangle( const Rectangle& );

private:
    ShapelessObject obj_;
    ShapePtr shape_;
    bool userControlled_{ false };
};

} //namespace