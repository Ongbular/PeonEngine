#include "CorePCH.h"
#include "DX11Display.h"
#include "DX11.h"
#include "Platform/Win64/WindowInstance.h"

DX11Display::DX11Display()
{
}

void DX11Display::Initialize(const shared_ptr<WindowInstance>& targetWindow)
{
	createSwapChain(targetWindow->GetHandle(), targetWindow->GetWidth(), targetWindow->GetHeight());
	createDepthStencilBuffer();
	createDepthStencilState();

	DX11::GetImmediateContext()->OMSetRenderTargets(1, mRenderTargetView.GetAddressOf(), mDepthStencilView.Get());

	updateViewPort();

	targetWindow->OnWindowResized.BindShared(shared_from_this(), &DX11Display::ResizeDisplay);
}

void DX11Display::ResizeDisplay(const int32 width, const int32 height)
{
	DX11::GetImmediateContext()->Flush();

	mRenderTargetView.Reset();
	mDepthStencilBuffer.Reset();
	mDepthStencilView.Reset();

	resizeSwapChainBuffers(width, height);
	createDepthStencilBuffer();

	DX11::GetImmediateContext()->OMSetRenderTargets(1, mRenderTargetView.GetAddressOf(), mDepthStencilView.Get());

	updateViewPort();
}

void DX11Display::SetClearColor(const float r, const float g, const float b, const float a)
{
	mClearColor[0] = r;
	mClearColor[1] = g;
	mClearColor[2] = b;
	mClearColor[3] = a;
}

void DX11Display::ClearBackBuffer() const
{
	DX11::GetImmediateContext()->ClearRenderTargetView(mRenderTargetView.Get(), mClearColor);
}

void DX11Display::ClearDepthBuffer(const float depth, const uint32 stencil) const
{
	DX11::GetImmediateContext()->ClearDepthStencilView(mDepthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
	                                                   depth, stencil);
}

void DX11Display::Present() const
{
	check(mSwapChain.Get());

	const UINT syncInterval = DX11::IsVSyncEnabled() ? 1 : 0;
	const UINT presentFlags = DX11::CheckTearingSupport() && !DX11::IsVSyncEnabled() ? DXGI_PRESENT_ALLOW_TEARING : 0;

	mSwapChain->Present(syncInterval, presentFlags);
}

void DX11Display::createSwapChain(const HWND hWnd, const uint32 width, const uint32 height)
{
	check(mSwapChain.Get() == nullptr);
	check(DX11::GetFactory().Get() != nullptr);
	check(DX11::GetDevice().Get() != nullptr);

	mSwapChainDesc.Width = width;
	mSwapChainDesc.Height = height;
	mSwapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	mSwapChainDesc.Stereo = FALSE;
	mSwapChainDesc.SampleDesc = { 1, 0 };
	mSwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	mSwapChainDesc.BufferCount = DEFAULT_SWAP_CHAIN_BUFFER_COUNT;
	mSwapChainDesc.Scaling = DXGI_SCALING_STRETCH;
	mSwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	mSwapChainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
	// It is recommended to always allow tearing if tearing support is available.
	mSwapChainDesc.Flags = DX11::CheckTearingSupport() ? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING : 0;

	ComPtr<IDXGISwapChain1> swapChain;

	HRESULT hr = DX11::GetFactory()->CreateSwapChainForHwnd(DX11::GetDevice().Get(), hWnd, &mSwapChainDesc, nullptr,
	                                                        nullptr, &swapChain);
	verifyResult(hr);

	hr = swapChain.As(&mSwapChain);
	verifyResult(hr);

	createRenderTargetViews();

	Logger::Info("SwapChain created successfully.");
}

void DX11Display::resizeSwapChainBuffers(const uint32 width, const uint32 height)
{
	HRESULT hr = mSwapChain->ResizeBuffers(mSwapChainDesc.BufferCount, width, height, mSwapChainDesc.Format,
	                                       mSwapChainDesc.Flags);
	verifyResult(hr);

	hr = mSwapChain->GetDesc1(&mSwapChainDesc);
	verifyResult(hr);

	createRenderTargetViews();
}

void DX11Display::createRenderTargetViews()
{
	check(mSwapChain.Get());

	ComPtr<ID3D11Texture2D> backBuffer;
	HRESULT hr = mSwapChain->GetBuffer(0, IID_PPV_ARGS(&backBuffer));
	verifyResult(hr);

	hr = DX11::GetDevice()->CreateRenderTargetView(backBuffer.Get(), nullptr, &mRenderTargetView);
	verifyResult(hr);
}

void DX11Display::createDepthStencilBuffer()
{
	D3D11_TEXTURE2D_DESC descDepth{};
	descDepth.Width = mSwapChainDesc.Width;
	descDepth.Height = mSwapChainDesc.Height;
	descDepth.MipLevels = 1;
	descDepth.ArraySize = 1;
	descDepth.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	descDepth.SampleDesc.Count = 1;
	descDepth.SampleDesc.Quality = 0;
	descDepth.Usage = D3D11_USAGE_DEFAULT;
	descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	descDepth.CPUAccessFlags = 0;
	descDepth.MiscFlags = 0;

	const HRESULT hr = DX11::GetDevice()->CreateTexture2D(&descDepth, nullptr, &mDepthStencilBuffer);
	verifyResult(hr);

	createDepthStencilView();
}

void DX11Display::createDepthStencilState()
{
	D3D11_DEPTH_STENCIL_DESC desc{};

	// Set up the description of the stencil state.
	desc.DepthEnable = true;
	desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	desc.DepthFunc = D3D11_COMPARISON_LESS;

	desc.StencilEnable = true;
	desc.StencilReadMask = 0xFF;
	desc.StencilWriteMask = 0xFF;

	// Stencil operations if pixel is front-facing.
	desc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	desc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	desc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	desc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// Stencil operations if pixel is back-facing.
	desc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	desc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	desc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	desc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	const HRESULT hr = DX11::GetDevice()->CreateDepthStencilState(&desc, &mDepthStencilState);
	verifyResult(hr);

	DX11::GetImmediateContext()->OMSetDepthStencilState(mDepthStencilState.Get(), 1);
}

void DX11Display::createDepthStencilView()
{
	check(mDepthStencilBuffer.Get());

	const HRESULT hr = DX11::GetDevice()->
		CreateDepthStencilView(mDepthStencilBuffer.Get(), nullptr, &mDepthStencilView);
	verifyResult(hr);
}

void DX11Display::updateViewPort()
{
	mViewport.TopLeftX = 0.f;
	mViewport.TopLeftY = 0.f;
	mViewport.Width = static_cast<float>(mSwapChainDesc.Width);
	mViewport.Height = static_cast<float>(mSwapChainDesc.Height);
	mViewport.MinDepth = 0.f;
	mViewport.MaxDepth = 1.f;

	DX11::GetImmediateContext()->RSSetViewports(1, &mViewport);
}
