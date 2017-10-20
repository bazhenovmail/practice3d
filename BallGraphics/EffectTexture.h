#pragma once
#include "Effect.h"
#include <DirectXMath.h>

namespace BallGraphics
{

class TextureMesh;
class Texture;

class EffectTexture : public Effect
{
public:

	struct MatrixBufferType
	{
		DirectX::XMMATRIX world;
		DirectX::XMMATRIX view;
		DirectX::XMMATRIX projection;
	};

	struct LightBufferType
	{
		DirectX::XMFLOAT4 ambientColor;
		DirectX::XMFLOAT4 diffuseColor;
		DirectX::XMFLOAT3 lightDirection;
		float padding;
	};

	std::unique_ptr<TextureMesh> createMesh(Texture& texture) noexcept;

    EffectTexture() noexcept;
    ~EffectTexture();

    void initialize(const D3D& d3d) noexcept override;
    void setParameters(const MatrixBufferType& params, const LightBufferType& lightParams, ID3D11ShaderResourceView* texture);
    void render(UINT index_count, const MatrixBufferType& params, const LightBufferType& lightParams, ID3D11ShaderResourceView* texture);
protected:
    std::vector<D3D11_INPUT_ELEMENT_DESC> input_layout() const override;
private:
	BallUtils::ComPtr<ID3D11Buffer> matrixBuffer_{ nullptr };
	BallUtils::ComPtr<ID3D11Buffer> lightBuffer_{ nullptr };
    BallUtils::ComPtr<ID3D11SamplerState> m_sampler_state = nullptr;
};

}//BallGraphics