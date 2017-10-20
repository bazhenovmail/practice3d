#pragma once

namespace BallPhysics
{

class Rectangle final
{
public:
    Rectangle();
    Rectangle(float width, float height, bool hollow);

    float width{0.f};
    float height{0.f};
    bool hollow{false};
};

}