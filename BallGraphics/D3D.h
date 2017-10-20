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
    D3D();
    ~D3D();

    void initialize(
        int screen_width,
        int screen_height,
        bool vsync,
        HWND hwnd,
        bool full_screen);
    void begin_scene();
    void end_scene();
    void shutdown();

    void turn_on_alpha_blending();

    ID3D11Device* const get_device() const;
    ID3D11DeviceContext* get_device_context() const;
	void enableZBuffer(bool) noexcept;

private:
    ID3D11RenderTargetView* get_render_target_view() const;
    ID3D11DepthStencilView* get_depth_stencil_view() const;

    static BallUtils::ComPtr<IDXGIFactory> create_dxgi_factory();
    static BallUtils::ComPtr<IDXGIAdapter> enum_adapters(UINT index, IDXGIFactory* factory);
    static BallUtils::ComPtr<IDXGIOutput> enum_outputs(UINT index, IDXGIAdapter* adapter);
    static std::vector<DXGI_MODE_DESC> get_display_mode_list(IDXGIOutput* adapter_output, DXGI_FORMAT EnumFormat, UINT Flags);
    static DXGI_ADAPTER_DESC get_adapter_description(IDXGIAdapter* adapter);

	void createDevice(IDXGIAdapter* adapter) noexcept;
	DXGI_SAMPLE_DESC getMultisamplingSettings(DXGI_FORMAT) noexcept;
	void createSwapChain(IDXGIFactory* factory, DXGI_SWAP_CHAIN_DESC& desc) noexcept;
    BallUtils::ComPtr<ID3D11Texture2D> get_swap_chain_buffer(UINT index);
    void create_render_target_view(ID3D11Texture2D* swap_chain_back_buffer);
    void create_depth_stencil_buffer(const D3D11_TEXTURE2D_DESC& desc);
	BallUtils::ComPtr<ID3D11DepthStencilState> createDepthStencilState_(const D3D11_DEPTH_STENCIL_DESC& desc) noexcept;
    void create_depth_stencil_view(const D3D11_DEPTH_STENCIL_VIEW_DESC& desc);
    BallUtils::ComPtr<ID3D11BlendState> create_blend_state(const D3D11_BLEND_DESC & desc);
    void add_rasterizer_state(D3D11_FILL_MODE FillMode, D3D11_CULL_MODE CullMode, BOOL FrontCounterClockwise,
                              INT DepthBias, FLOAT DepthBiasClamp, FLOAT SlopeScaledDepthBias, BOOL DepthClipEnable, BOOL ScissorEnable,
                              BOOL MultisampleEnable, BOOL AntialiasedLineEnable);
    void set_rasterizer_state(size_t index);
    void add_viewport(FLOAT TopLeftX, FLOAT TopLeftY, FLOAT Width, FLOAT Height, FLOAT MinDepth, FLOAT MaxDepth);
    void set_viewports();

    bool m_vsync_enabled{false};
    SIZE_T m_video_memory{0};
    std::vector<D3D11_VIEWPORT> m_viewports;

    BallUtils::ComPtr<IDXGISwapChain> swapChain_;
    BallUtils::ComPtr<ID3D11Device> device_;
    BallUtils::ComPtr<ID3D11DeviceContext> deviceContext_;
    BallUtils::ComPtr<ID3D11RenderTargetView> m_render_target_view;
    BallUtils::ComPtr<ID3D11DepthStencilState> depthStencilStateZOn_;
	BallUtils::ComPtr<ID3D11DepthStencilState> depthStencilStateZOff_;
    BallUtils::ComPtr<ID3D11DepthStencilView> m_depth_stencil_view;
    BallUtils::ComPtr<ID3D11Texture2D> m_depth_stencil_buffer;
    std::vector<BallUtils::ComPtr<ID3D11RasterizerState>> m_raster_states;

    BallUtils::ComPtr<ID3D11BlendState> m_alpha_enable_blending_state = nullptr;
    BallUtils::ComPtr<ID3D11BlendState> m_alpha_disable_blending_state = nullptr;
};

}//Balls
