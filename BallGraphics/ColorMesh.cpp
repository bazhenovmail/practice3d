#include "ColorMesh.h"

namespace BallGraphics
{

ColorMesh::~ColorMesh(){}

void ColorMesh::render(const GetWorldFunc& worldFunc, const Camera* camera) noexcept
{
	assert(worldFunc);
	assert(camera);
	render_({ worldFunc(), camera->getViewMatrix(), camera->getProjectionMatrix() });
}

ColorMesh::ColorMesh(ColorMesh && arg) :
    Mesh(std::forward<Mesh>(arg)), m_effect_color{arg.m_effect_color}
{
	vertices_ = std::move(arg.vertices_);
    arg.vertices_.clear();
}

ColorMesh & ColorMesh::operator=(ColorMesh && arg)
{
    Mesh::operator=(std::forward<Mesh>(arg));
    std::swap(vertices_, arg.vertices_);
    return *this;
}

void ColorMesh::initialize_buffers_() noexcept
{
    std::vector<VertexType> vertices;// (m_vertices.size());
    std::vector<unsigned long> indices;// (m_vertices.size());
    D3D11_BUFFER_DESC vertex_buffer_desc, index_buffer_desc;
    D3D11_SUBRESOURCE_DATA vertex_data, index_data;
    HRESULT result;

    // Load the vertex array and index array with data.
    unsigned long i = 0;
    for(const ModelType& mt : vertices_)
	{
        vertices.emplace_back(mt);
        indices.emplace_back(i);
        i++;
    }

    // Set up the description of the static vertex buffer.
    vertex_buffer_desc.Usage = D3D11_USAGE_DEFAULT;
    vertex_buffer_desc.ByteWidth = UINT(sizeof(VertexType) * vertices.size());
    vertex_buffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vertex_buffer_desc.CPUAccessFlags = 0;
    vertex_buffer_desc.MiscFlags = 0;
    vertex_buffer_desc.StructureByteStride = 0;

    // Give the subresource structure a pointer to the vertex data.
    vertex_data.pSysMem = vertices.data();
    vertex_data.SysMemPitch = 0;
    vertex_data.SysMemSlicePitch = 0;

    // Now create the vertex buffer.
    ID3D11Buffer* temp_vert_buf;
    result = d3d_.get_device()->CreateBuffer(&vertex_buffer_desc, &vertex_data, &temp_vert_buf);
    assert(result == S_OK);
    m_vertex_buffer = temp_vert_buf;

    // Set up the description of the static index buffer.
    index_buffer_desc.Usage = D3D11_USAGE_DEFAULT;
    index_buffer_desc.ByteWidth = UINT(sizeof(unsigned long) * indices.size());
    index_buffer_desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    index_buffer_desc.CPUAccessFlags = 0;
    index_buffer_desc.MiscFlags = 0;
    index_buffer_desc.StructureByteStride = 0;

    // Give the subresource structure a pointer to the index data.
    index_data.pSysMem = indices.data();
    index_data.SysMemPitch = 0;
    index_data.SysMemSlicePitch = 0;

    // Create the index buffer.
    ID3D11Buffer* temp_ind_buf;
    result = d3d_.get_device()->CreateBuffer(&index_buffer_desc, &index_data, &temp_ind_buf);
    assert(result == S_OK);
    m_index_buffer = temp_ind_buf;
}

UINT ColorMesh::get_index_count()
{
    return vertices_.size();
}

void ColorMesh::load(float width, float height, float depth, float ir, float ig, float ib, float ia)
{
    //FIXME: remove depth from here!
	vertices_.emplace_back(-width / 2, -height / 2, depth, ir, ig, ib, ia);
	vertices_.emplace_back(-width / 2, +height / 2, depth, ir, ig, ib, ia);
	vertices_.emplace_back(+width / 2, -height / 2, depth, ir, ig, ib, ia);
    //horizontal axe directed up -1 to 1, vertical axe directed right -1 to 1, depth directed in depth 0 to 1
	vertices_.emplace_back(+width / 2, +height / 2, depth, ir, ig, ib, ia);
	topology_ = D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
	initialize_buffers_();
}

void ColorMesh::load(const Vector & cPoint, const std::vector<Vector>& points, float depth, float ir, float ig, float ib, float ia)
{
	for (size_t i = 0; i < points.size(); i++)
	{
		size_t j = (i + 1) % points.size();
		vertices_.emplace_back(cPoint.x, cPoint.y, depth, ir, ig, ib, ia);
		vertices_.emplace_back(points[i].x, points[i].y, depth, ir, ig, ib, ia);
		vertices_.emplace_back(points[j].x, points[j].y, depth, ir, ig, ib, ia);
	}	
	topology_ = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	initialize_buffers_();
}

void ColorMesh::render_(const EffectColor::MatrixBufferType& params)
{
    unsigned int stride;
    unsigned int offset;

    // Set vertex buffer stride and offset.
    stride = sizeof(VertexType);
    offset = 0;

    // Set the vertex buffer to active in the input assembler so it can be rendered.
    assert(m_vertex_buffer != nullptr);
    ID3D11Buffer* tmp_vert_buf = m_vertex_buffer;
	d3d_.get_device_context()->IASetVertexBuffers(0, 1, &tmp_vert_buf, &stride, &offset);

    // Set the index buffer to active in the input assembler so it can be rendered.
    assert(m_index_buffer != nullptr);

	d3d_.get_device_context()->IASetIndexBuffer((ID3D11Buffer*)m_index_buffer, DXGI_FORMAT_R32_UINT, 0);

    // Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
	d3d_.get_device_context()->IASetPrimitiveTopology(topology_);


    m_effect_color.render(get_index_count(), params);
}

ColorMesh::ModelType::ModelType(float ix, float iy, float iz, float ir, float ig, float ib, float ia)
    : x{ix}, y{iy}, z{iz}, r{ir}, g{ig}, b{ib}, a{ia}
{
}

ColorMesh::VertexType::VertexType(const ModelType &arg)
    : position{DirectX::XMFLOAT3(arg.x, arg.y, arg.z)}, color{DirectX::XMFLOAT4(arg.r, arg.g, arg.b, arg.a)}
{
}

}//namespace BallGraphics