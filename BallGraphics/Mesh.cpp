#include "Mesh.h"

namespace BallGraphics
{

Mesh::Mesh(Mesh&& arg) noexcept: d3d_{ arg.d3d_ }
{
	m_vertex_buffer = std::move(arg.m_vertex_buffer);
	m_index_buffer = std::move(arg.m_index_buffer);
	arg.m_vertex_buffer = nullptr;
	arg.m_index_buffer = nullptr;
}

Mesh& Mesh::operator=(Mesh && arg) noexcept
{
	std::swap(m_vertex_buffer, arg.m_vertex_buffer);
	std::swap(m_index_buffer, arg.m_index_buffer);
	return *this;
}

}//namespace BallGraphics
