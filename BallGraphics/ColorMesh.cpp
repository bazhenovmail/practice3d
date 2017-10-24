#include "ColorMesh.h"

namespace BallGraphics
{

ColorMesh::~ColorMesh()
{
}

void ColorMesh::render( const GetWorldFunc& worldFunc, const Camera* camera ) noexcept
{
    assert( worldFunc );
    assert( camera );
    render_( { worldFunc(), camera->getViewMatrix(), camera->getProjectionMatrix() } );
}

ColorMesh::ColorMesh( ColorMesh && arg ) :
    Mesh( std::forward<Mesh>( arg ) ), effectColor_{ arg.effectColor_ }
{
    vertices_ = std::move( arg.vertices_ );
    arg.vertices_.clear();
}

ColorMesh & ColorMesh::operator=( ColorMesh && arg )
{
    Mesh::operator=( std::forward<Mesh>( arg ) );
    std::swap( vertices_, arg.vertices_ );
    return *this;
}

void ColorMesh::initializeBuffers_() noexcept
{
    std::vector<VertexType> vertices;// (m_vertices.size());
    std::vector<unsigned long> indices;// (m_vertices.size());
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

UINT ColorMesh::getIndexCount()
{
    return vertices_.size();
}

void ColorMesh::load( float width, float height, float depth, float ir, float ig, float ib, float ia )
{
    //FIXME: remove depth from here!
    vertices_.emplace_back( -width / 2, -height / 2, depth, ir, ig, ib, ia );
    vertices_.emplace_back( -width / 2, +height / 2, depth, ir, ig, ib, ia );
    vertices_.emplace_back( +width / 2, -height / 2, depth, ir, ig, ib, ia );
    //horizontal axe directed up -1 to 1, vertical axe directed right -1 to 1, depth directed in depth 0 to 1
    vertices_.emplace_back( +width / 2, +height / 2, depth, ir, ig, ib, ia );
    topology_ = D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
    initializeBuffers_();
}

void ColorMesh::load( const Vector & cPoint, const std::vector<Vector>& points, float depth, float ir, float ig, float ib, float ia )
{
    for ( size_t i = 0; i < points.size(); i++ )
    {
        size_t j = ( i + 1 ) % points.size();
        vertices_.emplace_back( cPoint.x, cPoint.y, depth, ir, ig, ib, ia );
        vertices_.emplace_back( points[i].x, points[i].y, depth, ir, ig, ib, ia );
        vertices_.emplace_back( points[j].x, points[j].y, depth, ir, ig, ib, ia );
    }
    topology_ = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
    initializeBuffers_();
}

void ColorMesh::render_( const EffectColor::MatrixBufferType& params )
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


    effectColor_.render( getIndexCount(), params );
}

ColorMesh::ModelType::ModelType( float ix, float iy, float iz, float ir, float ig, float ib, float ia )
    : x{ ix }, y{ iy }, z{ iz }, r{ ir }, g{ ig }, b{ ib }, a{ ia }
{
}

ColorMesh::VertexType::VertexType( const ModelType &arg )
    : position{ DirectX::XMFLOAT3( arg.x, arg.y, arg.z ) }, color{ DirectX::XMFLOAT4( arg.r, arg.g, arg.b, arg.a ) }
{
}

} //namespace