#define NOMINMAX
#include <algorithm>
#include "D3D.h"
#include <Windows.h>
#include <d3d11.h>
#include <vector>
#include "ComPtr.h"
#include <assert.h>

using namespace BallUtils;

namespace
{

const float cColor[4]{ 0.8f, 0.8f, 0.8f, 1.0f };
const float cBlendFactor[4]{ 0.0f, 0.0f, 0.0f, 0.0f };

ComPtr<IDXGIFactory> sCreateDxgiFactory()
{
    void* pointer;
    HRESULT result = CreateDXGIFactory( __uuidof( IDXGIFactory ), &pointer );
    assert( result == S_OK );
    return ComPtr<IDXGIFactory>( static_cast<IDXGIFactory*>( pointer ) );
}

ComPtr<IDXGIAdapter> sEnumAdapters( UINT index, IDXGIFactory * factory )
{
    IDXGIAdapter* pointer;
    HRESULT result = factory->EnumAdapters( index, &pointer );
    assert( result == S_OK );
    return ComPtr<IDXGIAdapter>( pointer );
}

ComPtr<IDXGIOutput> sEnumOutputs( UINT index, IDXGIAdapter * adapter )
{
    IDXGIOutput* pointer;
    HRESULT result = adapter->EnumOutputs( index, &pointer );
    assert( result == S_OK );
    return ComPtr<IDXGIOutput>( pointer );
}

std::vector<DXGI_MODE_DESC> sGetDisplayModeList( IDXGIOutput * adapter_output, DXGI_FORMAT EnumFormat, UINT Flags )
{
    UINT mode_number{ 0 };
    HRESULT result = adapter_output->GetDisplayModeList( EnumFormat, Flags, &mode_number, nullptr );
    assert( result == S_OK );
    std::vector<DXGI_MODE_DESC> display_mode_list( mode_number );
    result = adapter_output->GetDisplayModeList( EnumFormat, Flags, &mode_number, display_mode_list.data() );
    assert( result == S_OK );
    return display_mode_list;
}

DXGI_ADAPTER_DESC sGetAdapterDescription( IDXGIAdapter * adapter )
{
    DXGI_ADAPTER_DESC desc;
    HRESULT result = adapter->GetDesc( &desc );
    assert( result == S_OK );
    return desc;
}

} //namespace

namespace BallGraphics
{

void D3D::initialize(
    int screenWidth,
    int screenHeight,
    bool vsync,
    HWND hwnd,
    bool fullScreen)
{
    vsyncEnabled_ = vsync;

    ComPtr<IDXGIFactory> factory = sCreateDxgiFactory();
    ComPtr<IDXGIAdapter> adapter = sEnumAdapters( 0, factory );
    ComPtr<IDXGIOutput> adapter_output = sEnumOutputs( 0, adapter );

    std::vector<DXGI_MODE_DESC> displayModes =
        sGetDisplayModeList( adapter_output, DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED );

    // Now go through all the display modes and find the one that matches the screen width and height.
    // When a match is found store the numerator and denominator of the refresh rate for that monitor.
    UINT numerator{ 0 }, denominator{ 0 };
    for ( const DXGI_MODE_DESC& display_mode : displayModes )
    {
        if ( display_mode.Width == (unsigned int) screenWidth )
        {
            if ( display_mode.Height == (unsigned int) screenHeight )
            {
                numerator = display_mode.RefreshRate.Numerator;
                denominator = display_mode.RefreshRate.Denominator;
            }
        }
    }
    assert( numerator > 0 );
    assert( denominator > 0 );

    DXGI_ADAPTER_DESC adapterDescription = sGetAdapterDescription( adapter );

    videoMemory_ = adapterDescription.DedicatedVideoMemory;

    createDevice_( adapter );
    DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
    DXGI_SAMPLE_DESC sampleDesc = getMultisamplingSettings_( dxgiFormat );


    {
        DXGI_SWAP_CHAIN_DESC swapChainDesc;
        ZeroMemory( &swapChainDesc, sizeof( swapChainDesc ) );
        swapChainDesc.BufferCount = 1;
        swapChainDesc.BufferDesc.Width = screenWidth;
        swapChainDesc.BufferDesc.Height = screenHeight;
        swapChainDesc.BufferDesc.Format = dxgiFormat;

        // Set the refresh rate of the back buffer.
        if ( vsyncEnabled_ )
        {
            swapChainDesc.BufferDesc.RefreshRate.Numerator = numerator;
            swapChainDesc.BufferDesc.RefreshRate.Denominator = denominator;
        }
        else
        {
            swapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
            swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
        }

        swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        swapChainDesc.OutputWindow = hwnd;

        swapChainDesc.Windowed = !fullScreen;

        swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
        swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

        swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

        // Don't set the advanced flags.
        swapChainDesc.Flags = 0;

        swapChainDesc.SampleDesc = sampleDesc;

        createSwapChain_( factory, swapChainDesc );
    }

    createRenderTargetView_( getSwapChainBuffer_( 0 ) );

    {
        D3D11_TEXTURE2D_DESC depthBufferDesc;
        ZeroMemory( &depthBufferDesc, sizeof( depthBufferDesc ) );
        depthBufferDesc.Width = screenWidth;
        depthBufferDesc.Height = screenHeight;
        depthBufferDesc.MipLevels = 1;
        depthBufferDesc.ArraySize = 1;
        depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
        depthBufferDesc.SampleDesc = sampleDesc;
        depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
        depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
        depthBufferDesc.CPUAccessFlags = 0;
        depthBufferDesc.MiscFlags = 0;

        createDepthStencilBuffer_( depthBufferDesc );
    }

    {
        D3D11_DEPTH_STENCIL_DESC desc;

        desc.DepthEnable = true;
        desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
        desc.DepthFunc = D3D11_COMPARISON_LESS;

        desc.StencilEnable = true;
        desc.StencilReadMask = 0xFF;
        desc.StencilWriteMask = 0xFF;

        desc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
        desc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
        desc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
        desc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

        desc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
        desc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
        desc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
        desc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

        depthStencilStateZOn_ = createDepthStencilState_( desc );
    }

    {
        D3D11_DEPTH_STENCIL_DESC desc;

        desc.DepthEnable = false;
        desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
        desc.DepthFunc = D3D11_COMPARISON_LESS;

        desc.StencilEnable = true;
        desc.StencilReadMask = 0xFF;
        desc.StencilWriteMask = 0xFF;

        desc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
        desc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
        desc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
        desc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

        desc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
        desc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
        desc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
        desc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

        depthStencilStateZOff_ = createDepthStencilState_( desc );
    }

    {
        D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
        ZeroMemory( &depthStencilViewDesc, sizeof( depthStencilViewDesc ) );
        depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
        depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
        depthStencilViewDesc.Texture2D.MipSlice = 0;

        createDepthStencilView_( depthStencilViewDesc );
    }

    {
        ID3D11RenderTargetView* pointer = renderTargetView_;
        deviceContext_->OMSetRenderTargets( 1, &pointer, depthStencilView_ );
    }

    addRasterizerState_( D3D11_FILL_SOLID, D3D11_CULL_BACK, false, 0, 0.0f, 0.0f, true, false, false, false );

    setRasterizerState_( 0 );

    addViewport_( 0.0f, 0.0f, float( screenWidth ), float( screenHeight ), 0.0f, 1.0f );
    setViewports_();

    {
        D3D11_BLEND_DESC blendStateDescription;
        ZeroMemory( &blendStateDescription, sizeof( D3D11_BLEND_DESC ) );
        // Create an alpha enabled blend state description.
        blendStateDescription.RenderTarget[0].BlendEnable = TRUE;
        blendStateDescription.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
        blendStateDescription.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
        blendStateDescription.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;

        blendStateDescription.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_SRC_ALPHA;
        blendStateDescription.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_DEST_ALPHA;

        blendStateDescription.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
        blendStateDescription.RenderTarget[0].RenderTargetWriteMask = 0x0f;
        alphaEnableBlendingState_ = createBlendState_( blendStateDescription );

        blendStateDescription.RenderTarget[0].BlendEnable = FALSE;
        alphaDisableBlendingState_ = createBlendState_( blendStateDescription );
    }
}

void D3D::shutdown()
{
    if ( swapChain_ )
    {
        swapChain_->SetFullscreenState( false, nullptr );
    }
}

ID3D11Device * const D3D::getDevice() const
{
    assert( device_ != nullptr );
    return device_;
}

ID3D11DeviceContext * D3D::getDeviceContext() const
{
    assert( deviceContext_ != nullptr );
    return deviceContext_;
}

ID3D11RenderTargetView* D3D::getRenderTargetView_() const
{
    assert( renderTargetView_ != nullptr );
    return renderTargetView_;
}

ID3D11DepthStencilView* D3D::getDepthStencilView_() const
{
    assert( depthStencilView_ != nullptr );
    return depthStencilView_;
}

void D3D::beginScene()
{
    // Clear the back buffer.
    getDeviceContext()->ClearRenderTargetView( getRenderTargetView_(), cColor );

    // Clear the depth buffer.
    getDeviceContext()->ClearDepthStencilView( getDepthStencilView_(), D3D11_CLEAR_DEPTH, 1.0f, 0 );
}

void D3D::endScene()
{
    if ( vsyncEnabled_ )
    {
        // Lock to screen refresh rate.
        swapChain_->Present( 1, 0 );
    }
    else
    {
        // Present as fast as possible.
        swapChain_->Present( 0, 0 );
    }
}

void D3D::createDevice_( IDXGIAdapter * adapter ) noexcept
{
    D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;

    ID3D11Device* tmpDevice;
    ID3D11DeviceContext* tmpDeviceContext;

    UINT flags{ 0u };
#ifdef _DEBUG
    flags |= D3D11_CREATE_DEVICE_FLAG::D3D11_CREATE_DEVICE_DEBUG;
#endif

    HRESULT result = D3D11CreateDevice(
        adapter,
        adapter == nullptr ? D3D_DRIVER_TYPE_HARDWARE : D3D_DRIVER_TYPE_UNKNOWN,
        nullptr,
        flags,
        &featureLevel,
        1,
        D3D11_SDK_VERSION,
        &tmpDevice,
        nullptr,
        &tmpDeviceContext );

    assert( result == S_OK );
    device_ = tmpDevice;
    deviceContext_ = tmpDeviceContext;
    assert( deviceContext_ != nullptr );
}

DXGI_SAMPLE_DESC D3D::getMultisamplingSettings_( DXGI_FORMAT format ) noexcept
{
    DXGI_SAMPLE_DESC result;

    UINT qualityLevels;
    HRESULT hResult = device_->CheckMultisampleQualityLevels( format, 4, &qualityLevels );
    assert( hResult == S_OK );
    result.Count = 4;
    result.Quality = std::min( qualityLevels - 1, 3u );

    return result;
}

void D3D::createSwapChain_( IDXGIFactory* factory, DXGI_SWAP_CHAIN_DESC & desc ) noexcept
{
    IDXGISwapChain* pointer;
    HRESULT result = factory->CreateSwapChain( device_, &desc, &pointer );
    assert( result == S_OK );
    swapChain_ = pointer;
}

ComPtr<ID3D11Texture2D> D3D::getSwapChainBuffer_( UINT index )
{
    void* pointer;
    HRESULT result = swapChain_->GetBuffer( index, __uuidof( ID3D11Texture2D ), &pointer );
    assert( result == S_OK );
    return ComPtr<ID3D11Texture2D>( static_cast<ID3D11Texture2D*>( pointer ) );
}

void D3D::createRenderTargetView_( ID3D11Texture2D * swap_chain_back_buffer )
{
    ID3D11RenderTargetView* pointer;
    HRESULT result = device_->CreateRenderTargetView( swap_chain_back_buffer, nullptr, &pointer );
    assert( result == S_OK );
    renderTargetView_ = pointer;
}

void D3D::createDepthStencilBuffer_( const D3D11_TEXTURE2D_DESC & desc )
{
    ID3D11Texture2D* pointer;
    HRESULT result = device_->CreateTexture2D( &desc, nullptr, &pointer );
    assert( result == S_OK );
    depthStencilBuffer_ = pointer;
}

ComPtr<ID3D11DepthStencilState> D3D::createDepthStencilState_( const D3D11_DEPTH_STENCIL_DESC & desc ) noexcept
{
    ID3D11DepthStencilState* pointer;
    HRESULT hResult = device_->CreateDepthStencilState( &desc, &pointer );
    assert( hResult == S_OK );
    return pointer;
}

void D3D::createDepthStencilView_( const D3D11_DEPTH_STENCIL_VIEW_DESC & desc )
{
    ID3D11DepthStencilView* pointer;
    HRESULT result = device_->CreateDepthStencilView( depthStencilBuffer_, &desc, &pointer );
    assert( result == S_OK );
    depthStencilView_ = pointer;
}

ComPtr<ID3D11BlendState> D3D::createBlendState_( const D3D11_BLEND_DESC & desc )
{
    ID3D11BlendState* pointer;
    HRESULT result = device_->CreateBlendState( &desc, &pointer );
    assert( result == S_OK );
    return ComPtr<ID3D11BlendState>( pointer );
}

void D3D::addRasterizerState_(
    D3D11_FILL_MODE FillMode,
    D3D11_CULL_MODE CullMode,
    BOOL FrontCounterClockwise,
    INT DepthBias, 
    FLOAT DepthBiasClamp,
    FLOAT SlopeScaledDepthBias,
    BOOL DepthClipEnable, 
    BOOL ScissorEnable,
    BOOL MultisampleEnable,
    BOOL AntialiasedLineEnable )
{
    D3D11_RASTERIZER_DESC rasterDesc;
    // Setup the raster description which will determine how and what polygons will be drawn.
    rasterDesc.AntialiasedLineEnable = AntialiasedLineEnable;
    rasterDesc.CullMode = CullMode;
    rasterDesc.DepthBias = DepthBias;
    rasterDesc.DepthBiasClamp = DepthBiasClamp;
    rasterDesc.DepthClipEnable = DepthClipEnable;
    rasterDesc.FillMode = FillMode;
    rasterDesc.FrontCounterClockwise = FrontCounterClockwise;
    rasterDesc.MultisampleEnable = MultisampleEnable;
    rasterDesc.ScissorEnable = ScissorEnable;
    rasterDesc.SlopeScaledDepthBias = SlopeScaledDepthBias;
    ID3D11RasterizerState* pointer;
    HRESULT result = device_->CreateRasterizerState( &rasterDesc, &pointer );
    assert( result == S_OK );
    rasterStates_.emplace_back( pointer );
}

void D3D::setRasterizerState_( size_t index )
{
    assert( index < rasterStates_.size() );
    assert( rasterStates_[index] != nullptr );
    deviceContext_->RSSetState( rasterStates_[index] );
}

void D3D::addViewport_(
    FLOAT TopLeftX,
    FLOAT TopLeftY,
    FLOAT Width, 
    FLOAT Height,
    FLOAT MinDepth,
    FLOAT MaxDepth )
{
    viewports_.resize( viewports_.size() + 1 );
    viewports_.back().TopLeftX = TopLeftX;
    viewports_.back().TopLeftY = TopLeftY;
    viewports_.back().Width = Width;
    viewports_.back().Height = Height;
    viewports_.back().MinDepth = MinDepth;
    viewports_.back().MaxDepth = MaxDepth;
}

void D3D::setViewports_()
{
    deviceContext_->RSSetViewports( viewports_.size(), viewports_.data() );
}

void D3D::enableZBuffer( bool enable ) noexcept
{
    deviceContext_->OMSetDepthStencilState( enable ? depthStencilStateZOn_ : depthStencilStateZOff_, 1 );
}

void D3D::turnOnAlphaBlending()
{
    deviceContext_->OMSetBlendState( alphaEnableBlendingState_, cBlendFactor, 0xffffffff );
}

} //namespace