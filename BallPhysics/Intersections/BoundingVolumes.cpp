#include "BoundingVolumes.h"
#include <algorithm>

namespace BallPhysics
{

std::pair<Rectangle, Vector> boundingVolume( const Rectangle& rect, const ShapelessObject& obj, float maxT )
{
    Vector rectMovement{ obj.velocity * maxT };
    return{ { std::abs( rectMovement.x ) + rect.width, std::abs( rectMovement.y ) + rect.height, false },
    { obj.position + ( rectMovement * 0.5 ) } };
}

std::pair<Rectangle, Vector> boundingVolume( const Circle& circle, const ShapelessObject& obj, float maxT )
{
    Vector circleMovement{ obj.velocity * maxT };
    return{ { std::abs( circleMovement.x ) + 2 * circle.radius, std::abs( circleMovement.y ) + 2 * circle.radius, false },
    { obj.position + ( circleMovement * 0.5 ) } };
}

std::pair<Rectangle, Vector> boundingVolume( const Line& line, const ShapelessObject& obj, float maxT )
{
    std::vector<Vector> allVertices = line.vertices_;
    for ( size_t i = 0; i < allVertices.size(); i++ )
    {
        allVertices[i] += obj.position;
    }
    auto minmaxX = std::minmax_element( allVertices.begin(), allVertices.end(), []( const Vector& v1, const Vector& v2 )
    {
        return v1.x < v2.x;
    } );
    auto minmaxY = std::minmax_element( allVertices.begin(), allVertices.end(), []( const Vector& v1, const Vector& v2 )
    {
        return v1.y < v2.y;
    } );

    Vector lineMovement{ obj.velocity * maxT };
    return{ { std::abs( lineMovement.x ) + minmaxX.second->x - minmaxX.first->x, std::abs( lineMovement.y ) + minmaxY.second->y - minmaxY.first->y, false },
    { Vector{ minmaxX.first->x + minmaxX.second->x, minmaxY.first->y + minmaxY.second->y } / 2 + ( lineMovement / 2 ) } };
}

} //namespace