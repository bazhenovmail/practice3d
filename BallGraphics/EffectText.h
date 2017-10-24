#pragma once
#include "Effect.h"
#include <DirectXMath.h>

namespace BallGraphics
{

class Font;
class Texture;
class TextMesh;

class EffectText : public Effect
{
    struct ColorBufferType
    {
        DirectX::XMVECTOR color;
    };

public:
    struct MatrixBufferType
    {
        DirectX::XMMATRIX world;
        DirectX::XMMATRIX view;
        DirectX::XMMATRIX projection;
    };

    EffectText() noexcept;
    ~EffectText();

    std::unique_ptr<TextMesh> createMesh( Texture& texture, Font& font ) noexcept;

    void initialize( const D3D& d3d ) noexcept override;
    void setParameters( const MatrixBufferType& params, ID3D11ShaderResourceView* texture, const DirectX::XMVECTOR& color );
    void render( UINT indexCount, const MatrixBufferType& params, ID3D11ShaderResourceView* texture, const DirectX::XMVECTOR& color );
protected:
    std::vector<D3D11_INPUT_ELEMENT_DESC> inputLayout_() const override;
private:
    BallUtils::ComPtr<ID3D11Buffer> matrixBuffer_{ nullptr };
    BallUtils::ComPtr<ID3D11Buffer> colorBuffer_{ nullptr };
    BallUtils::ComPtr<ID3D11SamplerState> samplerState_{ nullptr };
};

} //namespace