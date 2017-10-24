#include "EffectTexture.h"
#include "TextureMesh.h"
#include "MappedResource.h"
#include <array>

using namespace DirectX;

namespace BallGraphics
{

std::unique_ptr<TextureMesh> EffectTexture::createMesh( Texture& texture ) noexcept
{
    return std::unique_ptr<TextureMesh>( new TextureMesh( *d3d_, *this, texture ) );
}

EffectTexture::EffectTexture() noexcept:
Effect( L"TextureVS.cso", L"TexturePS.cso" )
{
}

EffectTexture::~EffectTexture()
{
}

void EffectTexture::initialize( const D3D& d3d ) noexcept
{
    __super::initialize( d3d );

    {
        D3D11_BUFFER_DESC bufferDesc;
        bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
        bufferDesc.ByteWidth = sizeof( MatrixBufferType );
        bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        bufferDesc.MiscFlags = 0;
        bufferDesc.StructureByteStride = 0;

        matrixBuffer_ = createBuffer( bufferDesc );
    }

    {
        D3D11_BUFFER_DESC bufferDesc;
        bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
        bufferDesc.ByteWidth = sizeof( LightBufferType );
        bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        bufferDesc.MiscFlags = 0;
        bufferDesc.StructureByteStride = 0;

        lightBuffer_ = createBuffer( bufferDesc );
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

void EffectTexture::setParameters( const MatrixBufferType & params, const LightBufferType& lightParams, ID3D11ShaderResourceView * texture )
{
    {
        MappedResource<MatrixBufferType> mapped( d3d_->getDeviceContext(), matrixBuffer_ );
        mapped->world = XMMatrixTranspose( params.world );
        mapped->view = XMMatrixTranspose( params.view );
        mapped->projection = XMMatrixTranspose( params.projection );
    }

    {
        MappedResource<LightBufferType> mapped( d3d_->getDeviceContext(), lightBuffer_ );
        *mapped = lightParams;
    }

    {
        std::array<ID3D11Buffer *const, 1> buffers{ matrixBuffer_ };
        d3d_->getDeviceContext()->VSSetConstantBuffers( 0, buffers.size(), buffers.data() );
    }

    {
        std::array<ID3D11Buffer *const, 1> buffers{ lightBuffer_ };
        d3d_->getDeviceContext()->PSSetConstantBuffers( 0, buffers.size(), buffers.data() );
    }

    d3d_->getDeviceContext()->PSSetShaderResources( 0, 1, &texture );
}

void EffectTexture::render( UINT index_count, const MatrixBufferType& params, const LightBufferType& lightParams, ID3D11ShaderResourceView* texture )
{
    setParameters( params, lightParams, texture );
    finalizeRender_( index_count );
}

std::vector<D3D11_INPUT_ELEMENT_DESC> EffectTexture::inputLayout_() const
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
        },
        {
            "NORMAL",
            0,
            DXGI_FORMAT_R32G32B32_FLOAT,
            0,
            D3D11_APPEND_ALIGNED_ELEMENT,
            D3D11_INPUT_PER_VERTEX_DATA,
            0
        }
    };
}

} //namespace