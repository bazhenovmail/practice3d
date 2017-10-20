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
    Effect(const wchar_t* vs_file_name, const wchar_t* ps_file_name) noexcept;
    virtual ~Effect();

    virtual void initialize(const D3D& d3d) noexcept;

    BallUtils::ComPtr<ID3D11Buffer> create_buffer(const D3D11_BUFFER_DESC& desc);
    BallUtils::ComPtr<ID3D11SamplerState> create_sampler_state(const D3D11_SAMPLER_DESC& desc);
    void shutdown();

protected:
    void create_input_layout(const std::vector<char>& vs_data);
    void finalize_render(UINT index_count);

    virtual std::vector<D3D11_INPUT_ELEMENT_DESC> input_layout() const = 0;

    BallUtils::ComPtr<ID3D11VertexShader> m_vertex_shader;
    BallUtils::ComPtr<ID3D11PixelShader> m_pixel_shader;
    BallUtils::ComPtr<ID3D11InputLayout> m_layout;
    std::wstring m_ps_file_name;
    std::wstring m_vs_file_name;

	const D3D* d3d_{ nullptr };
};

}//BallGraphics