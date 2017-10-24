#pragma once

#include <d3d11.h>
#include <directxmath.h>
#include <vector>
#include <memory>
#include <ComPtr.h>
#include "D3D.h"
#include "Mesh.h"

namespace BallGraphics
{

class Effect
{
public:
    Effect( const wchar_t* vertexShaderFileName, const wchar_t* pixelShaderFileName ) noexcept;
    virtual ~Effect();

    virtual void initialize( const D3D& d3d ) noexcept;

    BallUtils::ComPtr<ID3D11Buffer> createBuffer( const D3D11_BUFFER_DESC& desc );
    BallUtils::ComPtr<ID3D11SamplerState> createSamplerState( const D3D11_SAMPLER_DESC& desc );
    void shutdown();

protected:
    void createInputLayout_( const std::vector<char>& vs_data );
    void finalizeRender_( UINT index_count );

    virtual std::vector<D3D11_INPUT_ELEMENT_DESC> inputLayout_() const = 0;

    BallUtils::ComPtr<ID3D11VertexShader> vertexShader_;
    BallUtils::ComPtr<ID3D11PixelShader> pixelShader_;
    BallUtils::ComPtr<ID3D11InputLayout> layout_;
    std::wstring psFileName_;
    std::wstring vsFileName_;

    const D3D* d3d_{ nullptr };
};

} //namespace