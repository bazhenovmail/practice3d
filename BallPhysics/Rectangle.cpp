#include "Rectangle.h"

namespace BallPhysics
{

Rectangle::Rectangle()
{
}

Rectangle::Rectangle(float w, float h, bool holl):
    width{w}, height(h), hollow(holl)
{
}

}