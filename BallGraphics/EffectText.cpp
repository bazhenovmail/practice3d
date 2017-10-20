#include "EffectText.h"
#include "TextMesh.h"
#include "MappedResource.h"

using namespace DirectX;

namespace BallGraphics
{

EffectText::EffectText() noexcept:
    Effect(L"TextVS.cso", L"TextPS.cso"){}

EffectText::~EffectText(){}

std::unique_ptr<TextMesh> EffectText::createMesh(Texture& texture, Font & font) noexcept
{
	return std::unique_ptr<TextMesh>(new TextMesh(*d3d_, *this, texture, font));
}

void EffectText::initialize(const D3D& d3d) noexcept
{
    __super::initialize(d3d);

    {
        D3D11_BUFFER_DESC matrixBufferDesc;
        matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
        matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
        matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        matrixBufferDesc.MiscFlags = 0;
        matrixBufferDesc.StructureByteStride = 0;

        matrixBuffer_ = create_buffer(matrixBufferDesc);
    }

    {
        D3D11_BUFFER_DESC bufferDesc;
        bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
        bufferDesc.ByteWidth = sizeof(ColorBufferType);
        bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        bufferDesc.MiscFlags = 0;
        bufferDesc.StructureByteStride = 0;

        colorBuffer_ = create_buffer(bufferDesc);
    }

    D3D11_SAMPLER_DESC sampler_desc;

    sampler_desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    sampler_desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    sampler_desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    sampler_desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    sampler_desc.MipLODBias = 0.0f;
    sampler_desc.MaxAnisotropy = 1;
    sampler_desc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
    sampler_desc.BorderColor[0] = 0;
    sampler_desc.BorderColor[1] = 0;
    sampler_desc.BorderColor[2] = 0;
    sampler_desc.BorderColor[3] = 0;
    sampler_desc.MinLOD = 0;
    sampler_desc.MaxLOD = D3D11_FLOAT32_MAX;

    m_sampler_state = create_sampler_state(sampler_desc);
}

void EffectText::setParameters(const MatrixBufferType& params, ID3D11ShaderResourceView* texture, const DirectX::XMVECTOR& color)
{
	{
		MappedResource<MatrixBufferType> mapped(d3d_->get_device_context(), matrixBuffer_);
		mapped->world = XMMatrixTranspose(params.world);
		mapped->view = XMMatrixTranspose(params.view);
		mapped->projection = XMMatrixTranspose(params.projection);
	}

	{
		MappedResource<ColorBufferType> mapped(d3d_->get_device_context(), colorBuffer_);
		mapped->color = color;
	}

    {
        unsigned buffer_index = 0;
        ID3D11Buffer *const mb = matrixBuffer_;
        d3d_->get_device_context()->VSSetConstantBuffers(buffer_index, 1, &mb);
    }

    {
        unsigned buffer_index = 0;
        ID3D11Buffer *const buff = colorBuffer_;
        d3d_->get_device_context()->PSSetConstantBuffers(buffer_index, 1, &buff);
    }

    d3d_->get_device_context()->PSSetShaderResources(0, 1, &texture);
}

void EffectText::render(UINT index_count, const MatrixBufferType& params, ID3D11ShaderResourceView* texture, const DirectX::XMVECTOR& color)
{
    setParameters(params, texture, color);
    finalize_render(index_count);
}

std::vector<D3D11_INPUT_ELEMENT_DESC> EffectText::input_layout() const{
    return std::vector<D3D11_INPUT_ELEMENT_DESC>{
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

}