#include "Mesh.h"

namespace BallGraphics
{

Mesh::Mesh( const D3D& d3d ) noexcept:
d3d_{ d3d }
{
};

Mesh::Mesh( Mesh&& arg ) noexcept:
d3d_{ arg.d3d_ }
{
    vertexBuffer_ = std::move( arg.vertexBuffer_ );
    indexBuffer_ = std::move( arg.indexBuffer_ );
    arg.vertexBuffer_ = nullptr;
    arg.indexBuffer_ = nullptr;
}

Mesh& Mesh::operator=( Mesh && arg ) noexcept
{
    std::swap( vertexBuffer_, arg.vertexBuffer_ );
    std::swap( indexBuffer_, arg.indexBuffer_ );
    return *this;
}

} //namespace
