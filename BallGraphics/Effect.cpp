#include "Effect.h"
#include <fileutil.h>

using namespace BallUtils;

namespace BallGraphics
{

Effect::Effect( const wchar_t* vertexShaderFileName, const wchar_t* pixelShaderFileName ) noexcept:
vsFileName_( vertexShaderFileName ), psFileName_( pixelShaderFileName )
{
}

Effect::~Effect()
{
}

void Effect::initialize( const D3D& d3d ) noexcept
{
    d3d_ = &d3d;
    std::vector<char> psData, vsData;
    bool ok = readFile( psFileName_, psData );
    assert( ok == true );
    ok = readFile( vsFileName_, vsData );
    assert( ok == true );

    ID3D11VertexShader* tmpVertShader;
    HRESULT hresult = d3d_->getDevice()->CreateVertexShader(
        vsData.data(),
        vsData.size(),
        nullptr,
        &tmpVertShader
    );
    assert( hresult == S_OK );
    vertexShader_ = tmpVertShader;

    ID3D11PixelShader* tmpPixShader;
    hresult = d3d_->getDevice()->CreatePixelShader(
        psData.data(),
        psData.size(),
        nullptr,
        &tmpPixShader
    );
    assert( hresult == S_OK );
    pixelShader_ = tmpPixShader;

    createInputLayout_( vsData );
}

ComPtr<ID3D11Buffer> Effect::createBuffer( const D3D11_BUFFER_DESC & desc )
{
    ID3D11Buffer* pointer;
    HRESULT result = d3d_->getDevice()->CreateBuffer( &desc, NULL, &pointer );
    assert( result == S_OK );
    assert( pointer != nullptr );
    return ComPtr<ID3D11Buffer>( pointer );
}

ComPtr<ID3D11SamplerState> Effect::createSamplerState( const D3D11_SAMPLER_DESC & desc )
{
    ID3D11SamplerState* pointer;
    HRESULT result = d3d_->getDevice()->CreateSamplerState( &desc, &pointer );
    assert( result == S_OK );
    assert( pointer != nullptr );
    return ComPtr<ID3D11SamplerState>( pointer );
}

void Effect::finalizeRender_( UINT index_count )
{
    d3d_->getDeviceContext()->IASetInputLayout( (ID3D11InputLayout*) layout_ );

    d3d_->getDeviceContext()->VSSetShader( (ID3D11VertexShader*) vertexShader_, NULL, 0 );
    d3d_->getDeviceContext()->PSSetShader( (ID3D11PixelShader*) pixelShader_, NULL, 0 );

    d3d_->getDeviceContext()->DrawIndexed( index_count, 0, 0 );
}

void Effect::shutdown()
{
}

void Effect::createInputLayout_( const std::vector<char>& vs_data )
{
    auto polygonLayout = inputLayout_();
    ID3D11InputLayout* tmpLayout;
    HRESULT result = d3d_->getDevice()->CreateInputLayout( polygonLayout.data(), polygonLayout.size(), vs_data.data(),
                                                           vs_data.size(), &tmpLayout );
    assert( result == S_OK );
    layout_ = tmpLayout;
}

} //namespace