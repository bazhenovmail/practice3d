#include "RectangleIntersection.h"

namespace BallPhysics
{

bool rectangleIntersection( const Rectangle& rect1, const Vector& rect1Pos, const Rectangle& rect2, const Vector& rect2Pos )
{
    float y11 = rect1Pos.y - rect1.height / 2;
    float y12 = rect1Pos.y + rect1.height / 2;
    float y21 = rect2Pos.y - rect2.height / 2;
    float y22 = rect2Pos.y + rect2.height / 2;
    bool intersect = y11 >= y21 && y11 < y22 || y21 >= y11 && y21 < y12;
    if ( intersect )
    {
        float x11 = rect1Pos.x - rect1.width / 2;
        float x12 = rect1Pos.x + rect1.width / 2;
        float x21 = rect2Pos.x - rect2.width / 2;
        float x22 = rect2Pos.x + rect2.width / 2;
        intersect &= x11 >= x21 && x11 < x22 || x21 >= x11 && x21 < x12;
    }
    return intersect;
}

} //namespace
