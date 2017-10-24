#pragma once
#include "Vector.h"
#include "../ShapelessObject.h"
#include "../Rectangle.h"
#include "../Circle.h"
#include "../Line.h"
#include <utility>

namespace BallPhysics
{

std::pair<Rectangle, Vector> boundingVolume( const Rectangle&, const ShapelessObject&, float maxT );
std::pair<Rectangle, Vector> boundingVolume( const Circle&, const ShapelessObject&, float maxT );
std::pair<Rectangle, Vector> boundingVolume( const Line&, const ShapelessObject&, float maxT );

} //namespace