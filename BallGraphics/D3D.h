#pragma once

#include <Windows.h>
#include <ComPtr.h>
#include <d3d11.h>
#include <directxmath.h>
#include <vector>

namespace BallGraphics
{

class D3D
{
public:

    void initialize(
        int screen_width,
        int screen_height,
        bool vsync,
        HWND hwnd,
        bool fullScreen );
    void beginScene();
    void endScene();
    void shutdown();

    void turnOnAlphaBlending();

    ID3D11Device* const getDevice() const;
    ID3D11DeviceContext* getDeviceContext() const;
    void enableZBuffer( bool ) noexcept;

private:
    bool vsyncEnabled_{ false };
    SIZE_T videoMemory_{ 0 };
    std::vector<D3D11_VIEWPORT> viewports_;

    BallUtils::ComPtr<IDXGISwapChain> swapChain_;
    BallUtils::ComPtr<ID3D11Device> device_;
    BallUtils::ComPtr<ID3D11DeviceContext> deviceContext_;
    BallUtils::ComPtr<ID3D11RenderTargetView> renderTargetView_;
    BallUtils::ComPtr<ID3D11DepthStencilState> depthStencilStateZOn_;
    BallUtils::ComPtr<ID3D11DepthStencilState> depthStencilStateZOff_;
    BallUtils::ComPtr<ID3D11DepthStencilView> depthStencilView_;
    BallUtils::ComPtr<ID3D11Texture2D> depthStencilBuffer_;
    std::vector<BallUtils::ComPtr<ID3D11RasterizerState>> rasterStates_;

    BallUtils::ComPtr<ID3D11BlendState> alphaEnableBlendingState_{ nullptr };
    BallUtils::ComPtr<ID3D11BlendState> alphaDisableBlendingState_{ nullptr };


    ID3D11RenderTargetView* getRenderTargetView_() const;
    ID3D11DepthStencilView* getDepthStencilView_() const;

    void createDevice_( IDXGIAdapter* adapter ) noexcept;
    DXGI_SAMPLE_DESC getMultisamplingSettings_( DXGI_FORMAT ) noexcept;
    void createSwapChain_( IDXGIFactory* factory, DXGI_SWAP_CHAIN_DESC& desc ) noexcept;
    BallUtils::ComPtr<ID3D11Texture2D> getSwapChainBuffer_( UINT index );
    void createRenderTargetView_( ID3D11Texture2D* swap_chain_back_buffer );
    void createDepthStencilBuffer_( const D3D11_TEXTURE2D_DESC& desc );
    BallUtils::ComPtr<ID3D11DepthStencilState> createDepthStencilState_( const D3D11_DEPTH_STENCIL_DESC& desc ) noexcept;
    void createDepthStencilView_( const D3D11_DEPTH_STENCIL_VIEW_DESC& desc );
    BallUtils::ComPtr<ID3D11BlendState> createBlendState_( const D3D11_BLEND_DESC & desc );
    void addRasterizerState_( 
        D3D11_FILL_MODE FillMode,
        D3D11_CULL_MODE CullMode,
        BOOL FrontCounterClockwise,
        INT DepthBias,
        FLOAT DepthBiasClamp,
        FLOAT SlopeScaledDepthBias,
        BOOL DepthClipEnable,
        BOOL ScissorEnable,
        BOOL MultisampleEnable,
        BOOL AntialiasedLineEnable );
    void setRasterizerState_( size_t index );
    void addViewport_( FLOAT TopLeftX, FLOAT TopLeftY, FLOAT Width, FLOAT Height, FLOAT MinDepth, FLOAT MaxDepth );
    void setViewports_();
};

} //namespace
