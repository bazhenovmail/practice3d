#include "TextureMesh.h"
#include <algorithm>
#include "Camera.h"

namespace BallGraphics
{

TextureMesh::~TextureMesh()
{
}

void TextureMesh::render( const GetWorldFunc& worldFunc, const Camera* camera ) noexcept
{
    assert( worldFunc );
    assert( camera );
    render_( { worldFunc(), camera->getViewMatrix(), camera->getProjectionMatrix() }, { { 0.1f, 0.1f, 0.1f, 1.0f },{ 0.5f, 0.5f, 0.5f, 1.0f },{ 0.f, 0.f, 1.f }, 0.f } );
}

TextureMesh::TextureMesh( TextureMesh && arg ) :
    Mesh( std::forward<Mesh>( arg ) ), effectTexture_{ arg.effectTexture_ }, texture_{ arg.texture_ }
{
    vertices_ = std::move( arg.vertices_ );
    arg.vertices_.clear();
}

TextureMesh & TextureMesh::operator=( TextureMesh && arg )
{
    Mesh::operator=( std::forward<Mesh>( arg ) );
    std::swap( vertices_, arg.vertices_ );
    return *this;
}

void TextureMesh::initializeBuffers_() noexcept
{
    std::vector<VertexType> vertices;
    std::vector<unsigned long> indices;
    D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
    D3D11_SUBRESOURCE_DATA vertexData, indexData;
    HRESULT result;

    // Load the vertex array and index array with data.
    unsigned long i = 0;
    for ( const ModelType& mt : vertices_ )
    {
        vertices.emplace_back( mt );
        indices.emplace_back( i );
        i++;
    }

    // Set up the description of the static vertex buffer.
    vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    vertexBufferDesc.ByteWidth = UINT( sizeof( VertexType ) * vertices.size() );
    vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vertexBufferDesc.CPUAccessFlags = 0;
    vertexBufferDesc.MiscFlags = 0;
    vertexBufferDesc.StructureByteStride = 0;

    // Give the subresource structure a pointer to the vertex data.
    vertexData.pSysMem = vertices.data();
    vertexData.SysMemPitch = 0;
    vertexData.SysMemSlicePitch = 0;

    // Now create the vertex buffer.
    ID3D11Buffer* tempVertBuf;
    result = d3d_.getDevice()->CreateBuffer( &vertexBufferDesc, &vertexData, &tempVertBuf );
    assert( result == S_OK );
    vertexBuffer_ = tempVertBuf;

    // Set up the description of the static index buffer.
    indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    indexBufferDesc.ByteWidth = UINT( sizeof( unsigned long ) * indices.size() );
    indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    indexBufferDesc.CPUAccessFlags = 0;
    indexBufferDesc.MiscFlags = 0;
    indexBufferDesc.StructureByteStride = 0;

    // Give the subresource structure a pointer to the index data.
    indexData.pSysMem = indices.data();
    indexData.SysMemPitch = 0;
    indexData.SysMemSlicePitch = 0;

    // Create the index buffer.
    ID3D11Buffer* tempIndBuf;
    result = d3d_.getDevice()->CreateBuffer( &indexBufferDesc, &indexData, &tempIndBuf );
    assert( result == S_OK );
    indexBuffer_ = tempIndBuf;
}

UINT TextureMesh::getIndexCount()
{
    return vertices_.size();
}

//horizontal axe directed up -1 to 1, vertical axe directed right -1 to 1, depth directed in depth 0 to 1

bool TextureMesh::load( const ObjFile & objFile, float mul ) noexcept
{
    return load( objFile, [mul]( float x, float y, float z )
    {
        return Position( x * mul, y * mul, z * mul );
    } );
}

bool TextureMesh::load( const ObjFile& objFile, const std::function<Position( float, float, float )>& func ) noexcept
{
    auto xIt = std::minmax_element( objFile.vertices.begin(), objFile.vertices.end(),
                                    []( const ObjFile::ObjVertexType& arg1, const ObjFile::ObjVertexType& arg2 )
    {
        return arg1.x < arg2.x;
    } );
    auto yIt = std::minmax_element( objFile.vertices.begin(), objFile.vertices.end(),
                                    []( const ObjFile::ObjVertexType& arg1, const ObjFile::ObjVertexType& arg2 )
    {
        return arg1.y < arg2.y;
    } );
    auto zIt = std::minmax_element( objFile.vertices.begin(), objFile.vertices.end(),
                                    []( const ObjFile::ObjVertexType& arg1, const ObjFile::ObjVertexType& arg2 )
    {
        return arg1.z < arg2.z;
    } );
    assert( xIt.first != xIt.second && yIt.first != yIt.second && zIt.first != zIt.second );

    xSize = xIt.second->x - xIt.first->x;
    ySize = yIt.second->y - yIt.first->y;
    zSize = zIt.second->z - zIt.first->z;

    auto getPos = [&func, &objFile]( int ind )
    {
        return func( objFile.vertices[ind - 1].x, objFile.vertices[ind - 1].y, objFile.vertices[ind - 1].z );
    };

    for ( const auto& face : objFile.faces )
    {
        vertices_.emplace_back( getPos( face.vIndex1 ),
                                objFile.texCoords[face.tIndex1 - 1].x, objFile.texCoords[face.tIndex1 - 1].y,
                                objFile.normals[face.nIndex1 - 1].x, objFile.normals[face.nIndex1 - 1].y, objFile.normals[face.nIndex1 - 1].z );
        vertices_.emplace_back( getPos( face.vIndex2 ),
                                objFile.texCoords[face.tIndex2 - 1].x, objFile.texCoords[face.tIndex2 - 1].y,
                                objFile.normals[face.nIndex2 - 1].x, objFile.normals[face.nIndex2 - 1].y, objFile.normals[face.nIndex2 - 1].z );
        vertices_.emplace_back( getPos( face.vIndex3 ),
                                objFile.texCoords[face.tIndex3 - 1].x, objFile.texCoords[face.tIndex3 - 1].y,
                                objFile.normals[face.nIndex3 - 1].x, objFile.normals[face.nIndex3 - 1].y, objFile.normals[face.nIndex3 - 1].z );
    }

    topology_ = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
    initializeBuffers_();
    return true;
}

void TextureMesh::render_( const EffectTexture::MatrixBufferType& params, const EffectTexture::LightBufferType& lightParams ) noexcept
{
    unsigned int stride;
    unsigned int offset;

    // Set vertex buffer stride and offset.
    stride = sizeof( VertexType );
    offset = 0;

    // Set the vertex buffer to active in the input assembler so it can be rendered.
    assert( vertexBuffer_ != nullptr );
    ID3D11Buffer* tmpVertBuf = vertexBuffer_;
    d3d_.getDeviceContext()->IASetVertexBuffers( 0, 1, &tmpVertBuf, &stride, &offset );

    // Set the index buffer to active in the input assembler so it can be rendered.
    assert( indexBuffer_ != nullptr );

    d3d_.getDeviceContext()->IASetIndexBuffer( (ID3D11Buffer*) indexBuffer_, DXGI_FORMAT_R32_UINT, 0 );

    // Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
    d3d_.getDeviceContext()->IASetPrimitiveTopology( topology_ );

    effectTexture_.render( getIndexCount(), params, lightParams, texture_.GetTexture() );
}

TextureMesh::ModelType::ModelType( Position position, float itu, float itv, float inx, float iny, float inz )
    : position{ position }, tu{ itu }, tv{ itv }, nx{ inx }, ny{ iny }, nz{ inz }
{
}

TextureMesh::VertexType::VertexType( const ModelType &arg )
    : position{ DirectX::XMFLOAT3( arg.position.x, arg.position.y, arg.position.z ) }, texture{ arg.tu, arg.tv }, normal{ arg.nx, arg.ny, arg.nz }
{
}

} //namespace