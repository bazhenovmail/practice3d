#include "Body.h"
#include <assert.h>
#include "Line.h"
#include "Circle.h"
#include "Rectangle.h"

namespace BallPhysics
{

Body::Body( const ShapelessObject& obj, bool userControlled ) :
    obj_{ obj }, userControlled_{ userControlled }
{
}

Vector Body::getPosition() const noexcept
{
    return obj_.position;
}

Vector Body::getVelocity() const noexcept
{
    return obj_.velocity;
}

CollisionResult Body::collisionResult( const Body& arg, float maxT ) const
{
    assert( shape_ );
    assert( arg.shape_ );
    assert( this != &arg );
    return shape_->collisionResult( obj_, arg.shape_, arg.obj_, maxT );
}

std::pair<Rectangle, Vector> Body::boundingVolume( float maxT ) const
{
    return shape_->boundingVolume( obj_, maxT );
}

void Body::affectVelocity( const VelocityChanger& changer )
{
    changer( obj_ );
}

void Body::locomote( float dt )
{
    //DEBUG
    auto newPos = obj_.position + obj_.velocity * dt;
    if ( std::abs( newPos.x ) > 955 || std::abs( newPos.y ) > 535 )
    {
        obj_.position = newPos;
        //__debugbreak();
    }
    else
        obj_.position = obj_.position + obj_.velocity * dt;
}

void Body::processCollision( const CollisionResult & result, Body & arg )
{
    assert( obj_.mass != std::numeric_limits<float>::infinity() || arg.obj_.mass != std::numeric_limits<float>::infinity() );
    assert( !userControlled_ || !arg.userControlled_ );

    Vector relativeVelocity;
    if ( userControlled_ )
        relativeVelocity = -arg.obj_.velocity;
    else if ( arg.userControlled_ )
        relativeVelocity = obj_.velocity;
    else
        relativeVelocity = obj_.velocity - arg.obj_.velocity;

    Vector normal = result.normal;
    float normalLen = normal.lengthSquared();
    if ( normalLen != 0.f )
    {
        normal = normal / sqrt( normalLen );
    }

    float relVelocityAlongNormal = dotProduct( relativeVelocity, normal );


    auto vel1 = obj_.velocity;
    auto vel2 = arg.obj_.velocity;


    if ( arg.obj_.mass == std::numeric_limits<float>::infinity() )
    {
        obj_.velocity = ( obj_.velocity - 2 * relVelocityAlongNormal * normal );
    }
    else if ( obj_.mass == std::numeric_limits<float>::infinity() )
    {
        arg.obj_.velocity = ( arg.obj_.velocity + 2 * relVelocityAlongNormal * normal );
    }
    else
    {
        float massSum = obj_.mass + arg.obj_.mass;
        float p = 2 * relVelocityAlongNormal / massSum;
        obj_.velocity = ( obj_.velocity - p * arg.obj_.mass * normal );
        arg.obj_.velocity = ( arg.obj_.velocity + p * obj_.mass * normal );
    }

    auto newVel1 = obj_.velocity;
    auto newVel2 = arg.obj_.velocity;


    auto len1 = vel1.lengthSquared();
    auto len2 = vel2.lengthSquared();
    auto nlen1 = newVel1.lengthSquared();
    auto nlen2 = newVel2.lengthSquared();

    if ( nlen1 != 0.f && nlen2 != 0.f )
    {
        auto diff1 = vel1.lengthSquared() / newVel1.lengthSquared();
        auto diff2 = vel2.lengthSquared() / newVel2.lengthSquared();
        assert( diff1 == NAN || diff1 == -NAN || abs( diff1 - 1.f ) <= 0.02f );
        assert( diff2 == NAN || diff2 == -NAN || abs( diff2 - 1.f ) <= 0.02f );
    }


}

void Body::setLine( const Line& arg )
{
    shape_ = ShapePtr( new Shape<Line>( arg ) );
}

void Body::setCircle( const Circle& arg )
{
    shape_ = ShapePtr( new Shape<Circle>( arg ) );
}

void Body::setRectangle( const Rectangle& arg )
{
    shape_ = ShapePtr( new Shape<Rectangle>( arg ) );
}

} //namespace