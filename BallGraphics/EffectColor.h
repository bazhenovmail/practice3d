#pragma once

#include "Effect.h"
#include <DirectXMath.h>

namespace BallGraphics
{

class ColorMesh;

class EffectColor: public Effect
{
public:
    EffectColor() noexcept;
    ~EffectColor();

	struct MatrixBufferType
	{
		DirectX::XMMATRIX world;
		DirectX::XMMATRIX view;
		DirectX::XMMATRIX projection;
	};

	std::unique_ptr<ColorMesh> createMesh() noexcept;

    void initialize(const D3D& d3d) noexcept override;
    void setParameters(const MatrixBufferType& params);
    void render(UINT index_count, const MatrixBufferType& params);
protected:
    std::vector<D3D11_INPUT_ELEMENT_DESC> input_layout() const override;
private:
	BallUtils::ComPtr<ID3D11Buffer> matrixBuffer_{ nullptr };
};

}//BallGraphics