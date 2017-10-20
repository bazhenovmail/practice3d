#define NOMINMAX
#include <algorithm>
#include "D3D.h"
#include <Windows.h>
#include <d3d11.h>
#include <vector>
#include "ComPtr.h"
#include <assert.h>
//#include "Globals.h"

using namespace BallUtils;

namespace BallGraphics
{

D3D::D3D(){}


D3D::~D3D(){}

void D3D::initialize(int screen_width,
                     int screen_height,
                     bool vsync,
                     HWND hwnd,
                     bool full_screen)
{
    //float field_of_view, screen_aspect;
    //g_log << "width: " << std::to_wstring(screen_width) <<
    //    ", height: " << std::to_wstring(screen_height) << "\n";

    m_vsync_enabled = vsync;

    ComPtr<IDXGIFactory> factory = create_dxgi_factory();
    ComPtr<IDXGIAdapter> adapter = enum_adapters(0, factory);
    ComPtr<IDXGIOutput> adapter_output = enum_outputs(0, adapter);

    std::vector<DXGI_MODE_DESC> display_modes = 
        get_display_mode_list(adapter_output, DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED);

    // Now go through all the display modes and find the one that matches the screen width and height.
    // When a match is found store the numerator and denominator of the refresh rate for that monitor.
    UINT numerator{0}, denominator{0};
    for(const DXGI_MODE_DESC& display_mode : display_modes){
        if(display_mode.Width == (unsigned int)screen_width){
            if(display_mode.Height == (unsigned int)screen_height){
                numerator = display_mode.RefreshRate.Numerator;
                denominator = display_mode.RefreshRate.Denominator;
            }
        }
    }
    assert(numerator > 0);
    assert(denominator > 0);

    DXGI_ADAPTER_DESC adapter_description = get_adapter_description(adapter);

    m_video_memory = adapter_description.DedicatedVideoMemory;

	createDevice(adapter);
	DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
	DXGI_SAMPLE_DESC sampleDesc = getMultisamplingSettings(dxgiFormat);


	DXGI_SWAP_CHAIN_DESC swap_chain_desc;
	ZeroMemory(&swap_chain_desc, sizeof(swap_chain_desc));
	swap_chain_desc.BufferCount = 1;
	swap_chain_desc.BufferDesc.Width = screen_width;
	swap_chain_desc.BufferDesc.Height = screen_height;
	swap_chain_desc.BufferDesc.Format = dxgiFormat;

	// Set the refresh rate of the back buffer.
	if (m_vsync_enabled) {
		swap_chain_desc.BufferDesc.RefreshRate.Numerator = numerator;
		swap_chain_desc.BufferDesc.RefreshRate.Denominator = denominator;
	}
	else {
		swap_chain_desc.BufferDesc.RefreshRate.Numerator = 0;
		swap_chain_desc.BufferDesc.RefreshRate.Denominator = 1;
	}

	swap_chain_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swap_chain_desc.OutputWindow = hwnd;

	swap_chain_desc.Windowed = !full_screen;

	swap_chain_desc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swap_chain_desc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	swap_chain_desc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	// Don't set the advanced flags.
	swap_chain_desc.Flags = 0;

	swap_chain_desc.SampleDesc = sampleDesc;

	createSwapChain(factory, swap_chain_desc);

    create_render_target_view(get_swap_chain_buffer(0));

    D3D11_TEXTURE2D_DESC depth_buffer_desc;
    ZeroMemory(&depth_buffer_desc, sizeof(depth_buffer_desc));
    depth_buffer_desc.Width = screen_width;
    depth_buffer_desc.Height = screen_height;
    depth_buffer_desc.MipLevels = 1;
    depth_buffer_desc.ArraySize = 1;
    depth_buffer_desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depth_buffer_desc.SampleDesc = sampleDesc;
    depth_buffer_desc.Usage = D3D11_USAGE_DEFAULT;
    depth_buffer_desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    depth_buffer_desc.CPUAccessFlags = 0;
    depth_buffer_desc.MiscFlags = 0;

    create_depth_stencil_buffer(depth_buffer_desc);

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

		depthStencilStateZOn_ = createDepthStencilState_(desc);
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

		depthStencilStateZOff_ = createDepthStencilState_(desc);
	}

    D3D11_DEPTH_STENCIL_VIEW_DESC depth_stencil_view_desc;
    ZeroMemory(&depth_stencil_view_desc, sizeof(depth_stencil_view_desc));
    depth_stencil_view_desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depth_stencil_view_desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
    depth_stencil_view_desc.Texture2D.MipSlice = 0;

    create_depth_stencil_view(depth_stencil_view_desc);

    {
        ID3D11RenderTargetView* tmp_ptr = m_render_target_view;
		deviceContext_->OMSetRenderTargets(1, &tmp_ptr, m_depth_stencil_view);
    }

    add_rasterizer_state(D3D11_FILL_SOLID, D3D11_CULL_BACK, false, 0, 0.0f, 0.0f, true, false, false, false);

    set_rasterizer_state(0);

    add_viewport(0.0f, 0.0f, float(screen_width), float(screen_height), 0.0f, 1.0f);
    set_viewports();


    D3D11_BLEND_DESC blend_state_description;
    ZeroMemory(&blend_state_description, sizeof(D3D11_BLEND_DESC));
    // Create an alpha enabled blend state description.
    blend_state_description.RenderTarget[0].BlendEnable = TRUE;
    blend_state_description.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
    blend_state_description.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
    blend_state_description.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
    //blend_state_description.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
    //blend_state_description.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;

    blend_state_description.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_SRC_ALPHA;
    blend_state_description.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_DEST_ALPHA;

    blend_state_description.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
    blend_state_description.RenderTarget[0].RenderTargetWriteMask = 0x0f;
    m_alpha_enable_blending_state = create_blend_state(blend_state_description);

    blend_state_description.RenderTarget[0].BlendEnable = FALSE;
    m_alpha_disable_blending_state = create_blend_state(blend_state_description);
}

void D3D::shutdown()
{
    if(swapChain_)
	{
		swapChain_->SetFullscreenState(false, nullptr);
    }
}

ID3D11Device * const D3D::get_device() const
{
    assert(device_ != nullptr);
    return device_;
}

ID3D11DeviceContext * D3D::get_device_context() const
{
    assert(deviceContext_ != nullptr);
    return deviceContext_;
}

ID3D11RenderTargetView* D3D::get_render_target_view() const
{
    assert(m_render_target_view != nullptr);
    return m_render_target_view;
}

ID3D11DepthStencilView* D3D::get_depth_stencil_view() const
{
    assert(m_depth_stencil_view != nullptr);
    return m_depth_stencil_view;
}

void D3D::begin_scene()
{
    float color[4];

    // Setup the color to clear the buffer to.
    color[0] = 0.8f;
    color[1] = 0.8f;
    color[2] = 0.8f;
    color[3] = 1.0f;

    // Clear the back buffer.
    get_device_context()->ClearRenderTargetView(get_render_target_view(), color);

    // Clear the depth buffer.
    get_device_context()->ClearDepthStencilView(get_depth_stencil_view(), D3D11_CLEAR_DEPTH, 1.0f, 0);
}

void D3D::end_scene()
{
    if(m_vsync_enabled)
	{
        // Lock to screen refresh rate.
		swapChain_->Present(1, 0);
    }
    else
	{
        // Present as fast as possible.
		swapChain_->Present(0, 0);
    }
}

ComPtr<IDXGIFactory> D3D::create_dxgi_factory()
{
    IDXGIFactory* tmp_f;
    HRESULT result = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&tmp_f);
    assert(result == S_OK);
    return ComPtr<IDXGIFactory>(tmp_f);
}

ComPtr<IDXGIAdapter> D3D::enum_adapters(UINT index, IDXGIFactory * factory)
{
    IDXGIAdapter* tmp_adapter;
    HRESULT result = factory->EnumAdapters(index, &tmp_adapter);
    assert(result == S_OK);
    return ComPtr<IDXGIAdapter>(tmp_adapter);
}

ComPtr<IDXGIOutput> D3D::enum_outputs(UINT index, IDXGIAdapter * adapter)
{
    IDXGIOutput* tmp_ad_output;
    HRESULT result = adapter->EnumOutputs(index, &tmp_ad_output);
    assert(result == S_OK);
    return ComPtr<IDXGIOutput>(tmp_ad_output);
}

std::vector<DXGI_MODE_DESC> D3D::get_display_mode_list(IDXGIOutput * adapter_output, DXGI_FORMAT EnumFormat, UINT Flags)
{
    UINT mode_number{0};
    HRESULT result = adapter_output->GetDisplayModeList(EnumFormat, Flags, &mode_number, nullptr);
    assert(result == S_OK);
    std::vector<DXGI_MODE_DESC> display_mode_list(mode_number);
    result = adapter_output->GetDisplayModeList(EnumFormat, Flags, &mode_number, display_mode_list.data());
    assert(result == S_OK);
    return display_mode_list;
}

DXGI_ADAPTER_DESC D3D::get_adapter_description(IDXGIAdapter * adapter)
{
    DXGI_ADAPTER_DESC desc;
    HRESULT result = adapter->GetDesc(&desc);
    assert(result == S_OK);
    return desc;
}

void D3D::createDevice(IDXGIAdapter * adapter) noexcept
{
	D3D_FEATURE_LEVEL feature_level = D3D_FEATURE_LEVEL_11_0;

	ID3D11Device* tmp_device;
	ID3D11DeviceContext* tmp_device_context;

	UINT flags{ 0u };
#ifdef _DEBUG
	flags |= D3D11_CREATE_DEVICE_FLAG::D3D11_CREATE_DEVICE_DEBUG;
#endif

	HRESULT result = D3D11CreateDevice(adapter, adapter == nullptr ? D3D_DRIVER_TYPE_HARDWARE : D3D_DRIVER_TYPE_UNKNOWN, nullptr,
									   flags,
									   &feature_level, 1, D3D11_SDK_VERSION, &tmp_device, nullptr, &tmp_device_context);

	assert(result == S_OK);
	device_ = tmp_device;
	deviceContext_ = tmp_device_context;
	assert(deviceContext_ != nullptr);
}

DXGI_SAMPLE_DESC D3D::getMultisamplingSettings(DXGI_FORMAT format) noexcept
{
	DXGI_SAMPLE_DESC result;

	UINT qualityLevels;
	HRESULT hResult = device_->CheckMultisampleQualityLevels(format, 4, &qualityLevels);
	assert(hResult == S_OK);
	result.Count = 4;
	result.Quality = std::min(qualityLevels - 1, 3u);

	return result;
}

void D3D::createSwapChain(IDXGIFactory* factory, DXGI_SWAP_CHAIN_DESC & desc) noexcept
{
	IDXGISwapChain* tmpSwapChain;
	HRESULT result = factory->CreateSwapChain(device_, &desc, &tmpSwapChain);
	assert(result == S_OK);
	swapChain_ = tmpSwapChain;
}

ComPtr<ID3D11Texture2D> D3D::get_swap_chain_buffer(UINT index)
{
    //g_log << "get_swap_chain_buffer\n";
    ID3D11Texture2D* tmp_ptr;
    HRESULT result = swapChain_->GetBuffer(index, __uuidof(ID3D11Texture2D), (LPVOID*)&tmp_ptr);
    assert(result == S_OK);
    return ComPtr<ID3D11Texture2D>(tmp_ptr);
}

void D3D::create_render_target_view(ID3D11Texture2D * swap_chain_back_buffer)
{
    //g_log << "create_render_target_view begin\n";
    ID3D11RenderTargetView* tmp_rtv;
    HRESULT result = device_->CreateRenderTargetView(swap_chain_back_buffer, nullptr, &tmp_rtv);
    assert(result == S_OK);
    m_render_target_view = tmp_rtv;
    //g_log << "create_render_target_view end\n";
}

void D3D::create_depth_stencil_buffer(const D3D11_TEXTURE2D_DESC & desc)
{
    ID3D11Texture2D* tmp;
    HRESULT result = device_->CreateTexture2D(&desc, nullptr, &tmp);
    assert(result == S_OK);
    m_depth_stencil_buffer = tmp;
}

ComPtr<ID3D11DepthStencilState> D3D::createDepthStencilState_(const D3D11_DEPTH_STENCIL_DESC & desc) noexcept
{
    ID3D11DepthStencilState* tmpResult;
    HRESULT hResult = device_->CreateDepthStencilState(&desc, &tmpResult);
    assert(hResult == S_OK);
	return tmpResult;
}

void D3D::create_depth_stencil_view(const D3D11_DEPTH_STENCIL_VIEW_DESC & desc)
{
    ID3D11DepthStencilView* tmp_ptr;
    HRESULT result = device_->CreateDepthStencilView(m_depth_stencil_buffer, &desc, &tmp_ptr);
    assert(result == S_OK);
    m_depth_stencil_view = tmp_ptr;
}

ComPtr<ID3D11BlendState> D3D::create_blend_state(const D3D11_BLEND_DESC & desc)
{
    ID3D11BlendState* tmp_ptr;
    HRESULT result = device_->CreateBlendState(&desc, &tmp_ptr);
    assert(result == S_OK);
    return ComPtr<ID3D11BlendState>(tmp_ptr);
}

void D3D::add_rasterizer_state(D3D11_FILL_MODE FillMode, D3D11_CULL_MODE CullMode, BOOL FrontCounterClockwise,
                               INT DepthBias, FLOAT DepthBiasClamp, FLOAT SlopeScaledDepthBias, BOOL DepthClipEnable, BOOL ScissorEnable,
                               BOOL MultisampleEnable, BOOL AntialiasedLineEnable)
{
    D3D11_RASTERIZER_DESC raster_desc;
    // Setup the raster description which will determine how and what polygons will be drawn.
    raster_desc.AntialiasedLineEnable = AntialiasedLineEnable;
    raster_desc.CullMode = CullMode;
    raster_desc.DepthBias = DepthBias;
    raster_desc.DepthBiasClamp = DepthBiasClamp;
    raster_desc.DepthClipEnable = DepthClipEnable;
    raster_desc.FillMode = FillMode;
    raster_desc.FrontCounterClockwise = FrontCounterClockwise;
    raster_desc.MultisampleEnable = MultisampleEnable;
    raster_desc.ScissorEnable = ScissorEnable;
    raster_desc.SlopeScaledDepthBias = SlopeScaledDepthBias;
    ID3D11RasterizerState* tmp_ptr;
    HRESULT result = device_->CreateRasterizerState(&raster_desc, &tmp_ptr);
    assert(result == S_OK);
    m_raster_states.emplace_back(tmp_ptr);
}

void D3D::set_rasterizer_state(size_t index)
{
    assert(index < m_raster_states.size());
    assert(m_raster_states[index] != nullptr);
	deviceContext_->RSSetState(m_raster_states[index]);
}

void D3D::add_viewport(FLOAT TopLeftX, FLOAT TopLeftY, FLOAT Width, FLOAT Height, FLOAT MinDepth, FLOAT MaxDepth)
{
    m_viewports.resize(m_viewports.size() + 1);
    m_viewports.back().TopLeftX = TopLeftX;
    m_viewports.back().TopLeftY = TopLeftY;
    m_viewports.back().Width = Width;
    m_viewports.back().Height = Height;
    m_viewports.back().MinDepth = MinDepth;
    m_viewports.back().MaxDepth = MaxDepth;
}

void D3D::set_viewports()
{
	deviceContext_->RSSetViewports(m_viewports.size(), m_viewports.data());
}

void D3D::enableZBuffer(bool enable) noexcept
{
	deviceContext_->OMSetDepthStencilState(enable ? depthStencilStateZOn_ : depthStencilStateZOff_, 1);
}

void D3D::turn_on_alpha_blending()
{
    float blendFactor[4];

    // Setup the blend factor.
    blendFactor[0] = 0.0f;
    blendFactor[1] = 0.0f;
    blendFactor[2] = 0.0f;
    blendFactor[3] = 0.0f;

    // Turn on the alpha blending.
	deviceContext_->OMSetBlendState(m_alpha_enable_blending_state, blendFactor, 0xffffffff);

    return;
}

}