#pragma once

#include <d3d11.h>
#include <vector>
#include <ComPtr.h>

namespace BallGraphics
{

class Texture
{
private:
    struct TargaHeader
    {
        unsigned char data1[12];
        unsigned short width;
        unsigned short height;
        unsigned char bpp;
        unsigned char data2;
    };

public:
    Texture();
    Texture( const Texture& ) = delete;
    Texture& operator = ( const Texture& ) = delete;
    Texture( Texture&& ) = default;
    Texture& operator = ( Texture&& ) = default;
    ~Texture();

    static BallUtils::ComPtr<ID3D11Texture2D> createTexture2d( ID3D11Device* device, const D3D11_TEXTURE2D_DESC& desc );
    static BallUtils::ComPtr<ID3D11ShaderResourceView> createShaderResourceView(
        ID3D11Device* device, ID3D11Resource* resource, const D3D11_SHADER_RESOURCE_VIEW_DESC& desc );

    bool Initialize( ID3D11Device*, ID3D11DeviceContext*, const char* );
    void Shutdown();

    ID3D11ShaderResourceView* GetTexture();

private:
    void LoadTarga( const char*, int&, int& );

private:
    std::vector<unsigned char> targaData_;
    BallUtils::ComPtr<ID3D11Texture2D> texture_{ nullptr };
    BallUtils::ComPtr<ID3D11ShaderResourceView> textureView_{ nullptr };

};

} //namespace