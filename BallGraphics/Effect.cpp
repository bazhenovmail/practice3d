#include "Effect.h"
#include <fileutil.h>

using namespace BallUtils;

namespace BallGraphics
{

Effect::Effect(const wchar_t* vs_file_name, const wchar_t* ps_file_name) noexcept:
	m_vs_file_name(vs_file_name), m_ps_file_name(ps_file_name){}

Effect::~Effect(){}

void Effect::initialize(const D3D& d3d) noexcept
{
	d3d_ = &d3d;
    std::vector<char> ps_data, vs_data;
    bool ok = readFile(m_ps_file_name, ps_data);
    assert(ok == true);
    ok = readFile(m_vs_file_name, vs_data);
    assert(ok == true);

    ID3D11VertexShader* tmp_vert_sh;
    HRESULT hresult = d3d_->getDevice()->CreateVertexShader(
        vs_data.data(),
        vs_data.size(),
        nullptr,
        &tmp_vert_sh
        );
    assert(hresult == S_OK);
    m_vertex_shader = tmp_vert_sh;

    ID3D11PixelShader* tmp_pix_sh;
    hresult = d3d_->getDevice()->CreatePixelShader(
        ps_data.data(),
        ps_data.size(),
        nullptr,
        &tmp_pix_sh
        );
    assert(hresult == S_OK);
    m_pixel_shader = tmp_pix_sh;

    create_input_layout(vs_data);
}

ComPtr<ID3D11Buffer> Effect::create_buffer(const D3D11_BUFFER_DESC & desc){
    ID3D11Buffer* tmp_ptr;
    HRESULT result = d3d_->getDevice()->CreateBuffer(&desc, NULL, &tmp_ptr);
    assert(result == S_OK);
    assert(tmp_ptr != nullptr);
    return ComPtr<ID3D11Buffer>(tmp_ptr);
}

ComPtr<ID3D11SamplerState> Effect::create_sampler_state(const D3D11_SAMPLER_DESC & desc){
    ID3D11SamplerState* tmp_ptr;
    HRESULT result = d3d_->getDevice()->CreateSamplerState(&desc, &tmp_ptr);
    assert(result == S_OK);
    assert(tmp_ptr != nullptr);
    return ComPtr<ID3D11SamplerState>(tmp_ptr);
}

void Effect::finalize_render(UINT index_count){
    d3d_->getDeviceContext()->IASetInputLayout((ID3D11InputLayout*)m_layout);

    d3d_->getDeviceContext()->VSSetShader((ID3D11VertexShader*)m_vertex_shader, NULL, 0);
    d3d_->getDeviceContext()->PSSetShader((ID3D11PixelShader*)m_pixel_shader, NULL, 0);

    d3d_->getDeviceContext()->DrawIndexed(index_count, 0, 0);
}

void Effect::shutdown(){}

void Effect::create_input_layout(const std::vector<char>& vs_data){
    auto polygonLayout = input_layout();
    ID3D11InputLayout* tmp_layout;
    HRESULT result = d3d_->getDevice()->CreateInputLayout(polygonLayout.data(), polygonLayout.size(), vs_data.data(),
                                               vs_data.size(), &tmp_layout);
    assert(result == S_OK);
    m_layout = tmp_layout;
}

}//BallGraphics