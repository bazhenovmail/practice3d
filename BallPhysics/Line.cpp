#include "Line.h"

namespace BallPhysics
{

Line::Line()
{
}

Line::Line( const std::vector<Vector>& vertices ) :
    vertices_( vertices )
{
}

} //namespace