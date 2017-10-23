#include "TextureMesh.h"
#include <algorithm>
#include "Camera.h"

namespace BallGraphics
{

TextureMesh::~TextureMesh(){}

void TextureMesh::render(const GetWorldFunc& worldFunc, const Camera* camera) noexcept
{
	assert(worldFunc);
	assert(camera);
	render_({ worldFunc(), camera->getViewMatrix(), camera->getProjectionMatrix() }, { {0.1f, 0.1f, 0.1f, 1.0f}, { 0.5f, 0.5f, 0.5f, 1.0f }, {0.f, 0.f, 1.f}, 0.f });
}

TextureMesh::TextureMesh(TextureMesh && arg):
    Mesh(std::forward<Mesh>(arg)), m_effect_texture{arg.m_effect_texture}, m_texture{arg.m_texture}
{
	vertices_ = std::move(arg.vertices_);
    arg.vertices_.clear();
}

TextureMesh & TextureMesh::operator=(TextureMesh && arg)
{
    Mesh::operator=(std::forward<Mesh>(arg));
    std::swap(vertices_, arg.vertices_);
    return *this;
}

void TextureMesh::initialize_buffers_() noexcept
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
    result = d3d_.getDevice()->CreateBuffer(&vertex_buffer_desc, &vertex_data, &temp_vert_buf);
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
    result = d3d_.getDevice()->CreateBuffer(&index_buffer_desc, &index_data, &temp_ind_buf);
    assert(result == S_OK);
    m_index_buffer = temp_ind_buf;
}

UINT TextureMesh::get_index_count()
{
    return vertices_.size();
}

//horizontal axe directed up -1 to 1, vertical axe directed right -1 to 1, depth directed in depth 0 to 1

bool TextureMesh::load(const ObjFile & objFile, float mul) noexcept
{
	return load(objFile, [mul](float x, float y, float z)
	{
		return Position(x * mul, y * mul, z * mul);
	});
}

bool TextureMesh::load(const ObjFile& objFile, const std::function<Position(float, float, float)>& func) noexcept
{
	auto xIt = std::minmax_element(objFile.vertices.begin(), objFile.vertices.end(),
								   [](const ObjFile::ObjVertexType& arg1, const ObjFile::ObjVertexType& arg2)
	{
		return arg1.x < arg2.x;
	});
	auto yIt = std::minmax_element(objFile.vertices.begin(), objFile.vertices.end(),
								   [](const ObjFile::ObjVertexType& arg1, const ObjFile::ObjVertexType& arg2)
	{
		return arg1.y < arg2.y;
	});
	auto zIt = std::minmax_element(objFile.vertices.begin(), objFile.vertices.end(),
								   [](const ObjFile::ObjVertexType& arg1, const ObjFile::ObjVertexType& arg2)
	{
		return arg1.z < arg2.z;
	});
	assert(xIt.first != xIt.second && yIt.first != yIt.second && zIt.first != zIt.second);

	xSize = xIt.second->x - xIt.first->x;
	ySize = yIt.second->y - yIt.first->y;
	zSize = zIt.second->z - zIt.first->z;

	auto getPos = [&func, &objFile](int ind)
	{
		return func(objFile.vertices[ind - 1].x, objFile.vertices[ind - 1].y, objFile.vertices[ind - 1].z);
	};

	for (const auto& face : objFile.faces)
	{
		vertices_.emplace_back(getPos(face.vIndex1),
							   objFile.texCoords[face.tIndex1 - 1].x, objFile.texCoords[face.tIndex1 - 1].y,
							   objFile.normals[face.nIndex1 - 1].x, objFile.normals[face.nIndex1 - 1].y, objFile.normals[face.nIndex1 - 1].z);
		vertices_.emplace_back(getPos(face.vIndex2),
							   objFile.texCoords[face.tIndex2 - 1].x, objFile.texCoords[face.tIndex2 - 1].y,
							   objFile.normals[face.nIndex2 - 1].x, objFile.normals[face.nIndex2 - 1].y, objFile.normals[face.nIndex2 - 1].z);
		vertices_.emplace_back(getPos(face.vIndex3),
							   objFile.texCoords[face.tIndex3 - 1].x, objFile.texCoords[face.tIndex3 - 1].y,
							   objFile.normals[face.nIndex3 - 1].x, objFile.normals[face.nIndex3 - 1].y, objFile.normals[face.nIndex3 - 1].z);
	}

	topology_ = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	initialize_buffers_();
	return true;
}

void TextureMesh::render_(const EffectTexture::MatrixBufferType& params, const EffectTexture::LightBufferType& lightParams) noexcept
{
    unsigned int stride;
    unsigned int offset;

    // Set vertex buffer stride and offset.
    stride = sizeof(VertexType);
    offset = 0;

    // Set the vertex buffer to active in the input assembler so it can be rendered.
    assert(m_vertex_buffer != nullptr);
    ID3D11Buffer* tmp_vert_buf = m_vertex_buffer;
    d3d_.getDeviceContext()->IASetVertexBuffers(0, 1, &tmp_vert_buf, &stride, &offset);

    // Set the index buffer to active in the input assembler so it can be rendered.
    assert(m_index_buffer != nullptr);

    d3d_.getDeviceContext()->IASetIndexBuffer((ID3D11Buffer*)m_index_buffer, DXGI_FORMAT_R32_UINT, 0);

    // Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
    d3d_.getDeviceContext()->IASetPrimitiveTopology(topology_);

    m_effect_texture.render(get_index_count(), params, lightParams, m_texture.GetTexture());
}

TextureMesh::ModelType::ModelType(Position position, float itu, float itv, float inx, float iny, float inz)
	: position{ position }, tu{ itu }, tv{ itv }, nx{ inx }, ny{ iny }, nz{ inz } {}

TextureMesh::VertexType::VertexType(const ModelType &arg)
	: position{ DirectX::XMFLOAT3(arg.position.x, arg.position.y, arg.position.z) }, texture{ arg.tu, arg.tv }, normal{ arg.nx, arg.ny, arg.nz } {}

}//namespace BallGraphics