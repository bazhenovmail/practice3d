#include "EffectColor.h"
#include "ColorMesh.h"
#include "MappedResource.h"

using namespace DirectX;

namespace BallGraphics
{

EffectColor::EffectColor() noexcept:
Effect( L"ColorVS.cso", L"ColorPS.cso" )
{
}

EffectColor::~EffectColor()
{
}

void EffectColor::initialize( const D3D& d3d ) noexcept
{
    __super::initialize( d3d );

    D3D11_BUFFER_DESC matrixBufferDesc;
    matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    matrixBufferDesc.ByteWidth = sizeof( MatrixBufferType );
    matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    matrixBufferDesc.MiscFlags = 0;
    matrixBufferDesc.StructureByteStride = 0;

    matrixBuffer_ = createBuffer( matrixBufferDesc );
}

std::unique_ptr<ColorMesh> EffectColor::createMesh() noexcept
{
    return std::unique_ptr<ColorMesh>( new ColorMesh( *d3d_, *this ) );
}

void EffectColor::setParameters( const MatrixBufferType& params )
{
    {
        MappedResource<MatrixBufferType> mapped( d3d_->getDeviceContext(), matrixBuffer_ );
        mapped->world = XMMatrixTranspose( params.world );
        mapped->view = XMMatrixTranspose( params.view );
        mapped->projection = XMMatrixTranspose( params.projection );
    }

    unsigned bufferIndex = 0;
    ID3D11Buffer *const mb = matrixBuffer_;
    d3d_->getDeviceContext()->VSSetConstantBuffers( bufferIndex, 1, &mb );
}

void EffectColor::render( UINT index_count, const MatrixBufferType& params )
{
    setParameters( params );
    finalizeRender_( index_count );
}

std::vector<D3D11_INPUT_ELEMENT_DESC> EffectColor::inputLayout_() const
{
    return std::vector<D3D11_INPUT_ELEMENT_DESC>
    {
        {
            "POSITION",
            0,
            DXGI_FORMAT_R32G32B32_FLOAT,
            0,
            0,
            D3D11_INPUT_PER_VERTEX_DATA,
            0
        },
        {
            "COLOR",
            0,
            DXGI_FORMAT_R32G32B32A32_FLOAT,
            0,
            D3D11_APPEND_ALIGNED_ELEMENT,
            D3D11_INPUT_PER_VERTEX_DATA,
            0
        }
    };
}

} //namespace
