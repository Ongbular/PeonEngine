#pragma once

#define DEFAULT_SWAP_CHAIN_BUFFER_COUNT (3)

class WindowInstance;

class DX11Display : public enable_shared_from_this<DX11Display>
{
public:
	DX11Display();
	~DX11Display() = default;

	void Initialize(const shared_ptr<WindowInstance>& targetWindow);

	void ResizeDisplay(int32 width, int32 height);

	void SetClearColor(float r, float g, float b, float a);

	void ClearBackBuffer() const;

	void ClearDepthBuffer(float depth, uint32 stencil) const;

	void Present() const;

	ComPtr<ID3D11RenderTargetView> GetRenderTargetView() const { return mRenderTargetView; }

	ComPtr<ID3D11DepthStencilView> GetDepthStencilView() const { return mDepthStencilView; }

private:
	void createSwapChain(HWND hWnd, uint32 width, uint32 height);

	void resizeSwapChainBuffers(uint32 width, uint32 height);

	void createRenderTargetViews();

	void createDepthStencilBuffer();

	void createDepthStencilState();

	void createDepthStencilView();

	void updateViewPort();

	ComPtr<IDXGISwapChain4> mSwapChain{ nullptr };

	ComPtr<ID3D11RenderTargetView> mRenderTargetView{ nullptr };

	DXGI_SWAP_CHAIN_DESC1 mSwapChainDesc{};

	float mClearColor[4]{ 0.f, 0.f, 0.f, 0.f };

	ComPtr<ID3D11Texture2D> mDepthStencilBuffer{ nullptr };

	ComPtr<ID3D11DepthStencilState> mDepthStencilState{ nullptr };

	ComPtr<ID3D11DepthStencilView> mDepthStencilView{ nullptr };

	D3D11_VIEWPORT mViewport{};
};