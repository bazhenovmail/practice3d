#pragma once
#include "Vector.h"
#include <vector>

namespace BallPhysics
{

class Line final
{
public:
    Line();
	Line(const std::vector<Vector>& vertices);

    std::vector<Vector> vertices_;
};

}