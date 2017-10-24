#include "EffectText.h"
#include "TextMesh.h"
#include "MappedResource.h"

using namespace DirectX;

namespace BallGraphics
{

EffectText::EffectText() noexcept:
Effect( L"TextVS.cso", L"TextPS.cso" )
{
}

EffectText::~EffectText()
{
}

std::unique_ptr<TextMesh> EffectText::createMesh( Texture& texture, Font & font ) noexcept
{
    return std::unique_ptr<TextMesh>( new TextMesh( *d3d_, *this, texture, font ) );
}

void EffectText::initialize( const D3D& d3d ) noexcept
{
    __super::initialize( d3d );

    {
        D3D11_BUFFER_DESC matrixBufferDesc;
        matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
        matrixBufferDesc.ByteWidth = sizeof( MatrixBufferType );
        matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        matrixBufferDesc.MiscFlags = 0;
        matrixBufferDesc.StructureByteStride = 0;

        matrixBuffer_ = createBuffer( matrixBufferDesc );
    }

    {
        D3D11_BUFFER_DESC bufferDesc;
        bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
        bufferDesc.ByteWidth = sizeof( ColorBufferType );
        bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        bufferDesc.MiscFlags = 0;
        bufferDesc.StructureByteStride = 0;

        colorBuffer_ = createBuffer( bufferDesc );
    }

    D3D11_SAMPLER_DESC samplerDesc;

    samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.MipLODBias = 0.0f;
    samplerDesc.MaxAnisotropy = 1;
    samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
    samplerDesc.BorderColor[0] = 0;
    samplerDesc.BorderColor[1] = 0;
    samplerDesc.BorderColor[2] = 0;
    samplerDesc.BorderColor[3] = 0;
    samplerDesc.MinLOD = 0;
    samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

    samplerState_ = createSamplerState( samplerDesc );
}

void EffectText::setParameters( const MatrixBufferType& params, ID3D11ShaderResourceView* texture, const DirectX::XMVECTOR& color )
{
    {
        MappedResource<MatrixBufferType> mapped( d3d_->getDeviceContext(), matrixBuffer_ );
        mapped->world = XMMatrixTranspose( params.world );
        mapped->view = XMMatrixTranspose( params.view );
        mapped->projection = XMMatrixTranspose( params.projection );
    }

    {
        MappedResource<ColorBufferType> mapped( d3d_->getDeviceContext(), colorBuffer_ );
        mapped->color = color;
    }

    {
        unsigned bufferIndex = 0;
        ID3D11Buffer *const mb = matrixBuffer_;
        d3d_->getDeviceContext()->VSSetConstantBuffers( bufferIndex, 1, &mb );
    }

    {
        unsigned bufferIndex = 0;
        ID3D11Buffer *const buff = colorBuffer_;
        d3d_->getDeviceContext()->PSSetConstantBuffers( bufferIndex, 1, &buff );
    }

    d3d_->getDeviceContext()->PSSetShaderResources( 0, 1, &texture );
}

void EffectText::render( UINT indexCount, const MatrixBufferType& params, ID3D11ShaderResourceView* texture, const DirectX::XMVECTOR& color )
{
    setParameters( params, texture, color );
    finalizeRender_( indexCount );
}

std::vector<D3D11_INPUT_ELEMENT_DESC> EffectText::inputLayout_() const
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
            "TEXCOORD",
            0,
            DXGI_FORMAT_R32G32_FLOAT,
            0,
            D3D11_APPEND_ALIGNED_ELEMENT,
            D3D11_INPUT_PER_VERTEX_DATA,
            0
        }
    };
}

} //namespace