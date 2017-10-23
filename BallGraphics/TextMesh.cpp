#include "TextMesh.h"

namespace BallGraphics
{

TextMesh::TextMesh( const D3D& d3d, EffectText& et, Texture& texture, const Font& font )
    : Mesh( d3d ), effectText_{ et }, texture_{ texture }, font_{ font }
{
}

TextMesh::~TextMesh()
{
}

void TextMesh::render( const GetWorldFunc& worldFunc, const Camera* camera ) noexcept
{
    assert( worldFunc );
    assert( camera );
    render_( { worldFunc(), camera->getViewMatrix(), camera->getProjectionMatrix() } );
}

TextMesh::TextMesh( TextMesh && arg ) :
    Mesh( std::forward<Mesh>( arg ) ), effectText_{ arg.effectText_ }, texture_{ arg.texture_ }, font_{ arg.font_ }
{
    vertices_ = std::move( arg.vertices_ );
    arg.vertices_.clear();
}

TextMesh & TextMesh::operator=( TextMesh && arg )
{
    Mesh::operator=( std::forward<Mesh>( arg ) );
    std::swap( vertices_, arg.vertices_ );
    return *this;
}

void TextMesh::initializeBuffers_() noexcept
{
    std::vector<VertexType> vertices;// (m_vertices.size());
    std::vector<unsigned long> indices;// (m_vertices.size());
    D3D11_BUFFER_DESC vertex_buffer_desc, index_buffer_desc;
    D3D11_SUBRESOURCE_DATA vertex_data, index_data;
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
    vertex_buffer_desc.Usage = D3D11_USAGE_DEFAULT;
    vertex_buffer_desc.ByteWidth = UINT( sizeof( VertexType ) * vertices.size() );
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
    result = d3d_.getDevice()->CreateBuffer( &vertex_buffer_desc, &vertex_data, &temp_vert_buf );
    assert( result == S_OK );
    vertexBuffer_ = temp_vert_buf;

    // Set up the description of the static index buffer.
    index_buffer_desc.Usage = D3D11_USAGE_DEFAULT;
    index_buffer_desc.ByteWidth = UINT( sizeof( unsigned long ) * indices.size() );
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
    result = d3d_.getDevice()->CreateBuffer( &index_buffer_desc, &index_data, &temp_ind_buf );
    assert( result == S_OK );
    indexBuffer_ = temp_ind_buf;
}

UINT TextMesh::getIndexCount()
{
    return vertices_.size();
}

void TextMesh::load( const std::string& text, const DirectX::XMVECTOR& color, float depth ) noexcept
{
    initialized_ = true;
    color_ = color;
    depth_ = depth;

    updateText( text );
}

void TextMesh::updateText( const std::string & text ) noexcept
{
    assert( initialized_ );

    float x{ 0.0f };
    vertices_.clear();
    for ( auto sym : text )
    {
        auto result = font_.getData( sym );
        if ( result.first )
        {
            vertices_.emplace_back( x, 0, depth_, result.second.left, 1. );
            vertices_.emplace_back( x, 16, depth_, result.second.left, 0. );
            vertices_.emplace_back( x + result.second.size, 0, depth_, ( result.second.right ), 1. );
            vertices_.emplace_back( x + result.second.size, 16, depth_, ( result.second.right ), 0. );
            x += result.second.size;
        }
        else
        {
            vertices_.emplace_back( x, 0, depth_, 1., 1. );
            vertices_.emplace_back( x, 16, depth_, 1., 0. );
            vertices_.emplace_back( x + result.second.size, 0, depth_, 1., 1. );
            vertices_.emplace_back( x + result.second.size, 16, depth_, 1., 0. );
            x += 2.;
        }
        x += 1.;
    }
    initializeBuffers_();
}

void TextMesh::render_( const EffectText::MatrixBufferType& params )
{
    unsigned int stride;
    unsigned int offset;

    // Set vertex buffer stride and offset.
    stride = sizeof( VertexType );
    offset = 0;

    // Set the vertex buffer to active in the input assembler so it can be rendered.
    assert( vertexBuffer_ != nullptr );
    ID3D11Buffer* tmp_vert_buf = vertexBuffer_;
    d3d_.getDeviceContext()->IASetVertexBuffers( 0, 1, &tmp_vert_buf, &stride, &offset );

    // Set the index buffer to active in the input assembler so it can be rendered.
    assert( indexBuffer_ != nullptr );

    d3d_.getDeviceContext()->IASetIndexBuffer( (ID3D11Buffer*) indexBuffer_, DXGI_FORMAT_R32_UINT, 0 );

    // Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
    d3d_.getDeviceContext()->IASetPrimitiveTopology( D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP );


    effectText_.render( getIndexCount(), params, texture_.GetTexture(), color_ );
}

TextMesh::ModelType::ModelType( float ix, float iy, float iz, float itu, float itv )
    : x{ ix }, y{ iy }, z{ iz }, tu{ itu }, tv{ itv }
{
}

TextMesh::VertexType::VertexType( const ModelType &arg )
    : position{ DirectX::XMFLOAT3( arg.x, arg.y, arg.z ) }, texture{ arg.tu, arg.tv }
{
}

} //namespace