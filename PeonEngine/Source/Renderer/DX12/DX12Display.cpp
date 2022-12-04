#include "CorePCH.h"
#include "DX12Display.h"
#include "DX12.h"
#include "DX12DescriptorHeap.h"

DX12Display::DX12Display(const HWND hWnd, const uint32 width, const uint32 height)
{
	createSwapChain(hWnd, width, height);
	createDepthStencilBuffer(width, height);
}

void DX12Display::OnWindowResized(const uint32 width, const uint32 height)
{
	check(mSwapChain.Get());

	mDepthStencilBuffer.Reset();

	resizeSwapChainBuffers(width, height);
	createDepthStencilBuffer(width, height);
}

void DX12Display::Present() const
{
	check(mSwapChain.Get());
	const UINT syncInterval = DX12::IsVSyncEnabled() ? 1 : 0;
	const UINT presentFlags = DX12::CheckTearingSupport() && !DX12::IsVSyncEnabled() ? DXGI_PRESENT_ALLOW_TEARING : 0;

	mSwapChain->Present(syncInterval, presentFlags);
}

ComPtr<ID3D12Resource> DX12Display::GetCurrentBackBuffer() const
{
	check(mSwapChain.Get());
	const uint32 currentBackBufferIndex = mSwapChain->GetCurrentBackBufferIndex();

	ComPtr<ID3D12Resource> ret;
	const HRESULT hr = mSwapChain->GetBuffer(currentBackBufferIndex, IID_PPV_ARGS(&ret));
	check(SUCCEEDED(hr));

	return ret;
}

void DX12Display::createSwapChain(const HWND hWnd, const uint32 width, const uint32 height)
{
	check(mSwapChain.Get() == nullptr);
	check(DX12::GetFactory().Get() != nullptr);
	check(DX12::GetCommandQueue().Get() != nullptr);
	
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
	swapChainDesc.Width = width;
	swapChainDesc.Height = height;
	swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.Stereo = FALSE;
	swapChainDesc.SampleDesc = { 1, 0 };
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.BufferCount = DEFAULT_SWAP_CHAIN_BUFFER_COUNT;
	swapChainDesc.Scaling = DXGI_SCALING_STRETCH;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
	// It is recommended to always allow tearing if tearing support is available.
	swapChainDesc.Flags = DX12::CheckTearingSupport() ? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING : 0;

	ComPtr<IDXGISwapChain1> swapChain;

	HRESULT hr = DX12::GetFactory()->CreateSwapChainForHwnd(DX12::GetCommandQueue().Get(), hWnd, &swapChainDesc, nullptr, nullptr, &swapChain);
	verifyResult(hr);

	hr = swapChain.As(&mSwapChain);
	verifyResult(hr);

	createRenderTargetViews();

	Logger::Info("SwapChain created successfully.");
}

void DX12Display::resizeSwapChainBuffers(const uint32 width, const uint32 height)
{
	check(DX12::IsCommandQueueFlushed());

	DXGI_SWAP_CHAIN_DESC1 desc;
	HRESULT hr = mSwapChain->GetDesc1(&desc);
	verifyResult(hr);
	
	hr = mSwapChain->ResizeBuffers(desc.BufferCount, width, height, desc.Format, desc.Flags);
	verifyResult(hr);

	createRenderTargetViews();
}

void DX12Display::createRenderTargetViews()
{
	if (mRtvDescriptorHandle.GetCpuHandle().ptr == D3D12_GPU_VIRTUAL_ADDRESS_UNKNOWN)
	{
		check(DX12::GetRtvDescriptorHeap()->HasAvailableSpace(DEFAULT_SWAP_CHAIN_BUFFER_COUNT));
		mRtvDescriptorHandle = DX12::GetRtvDescriptorHeap()->Allocate(DEFAULT_SWAP_CHAIN_BUFFER_COUNT);
	}
	
	auto handle = mRtvDescriptorHandle;

	for (int i = 0; i < DEFAULT_SWAP_CHAIN_BUFFER_COUNT; ++i)
	{
		ComPtr<ID3D12Resource> backBuffer;
		
		const HRESULT hr = mSwapChain->GetBuffer(i, IID_PPV_ARGS(&backBuffer));
		check(SUCCEEDED(hr));

		DX12::GetDevice()->CreateRenderTargetView(backBuffer.Get(), nullptr, handle.GetCpuHandle());
		handle.Offset();
	}
}

void DX12Display::createDepthStencilBuffer(const uint32 width, const uint32 height)
{
	D3D12_RESOURCE_DESC depthStencilDesc{};
	depthStencilDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	depthStencilDesc.Alignment = 0;
	depthStencilDesc.Width = width;
	depthStencilDesc.Height = height;
	depthStencilDesc.DepthOrArraySize = 1;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilDesc.SampleDesc.Count = 1;
	depthStencilDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	depthStencilDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

	D3D12_CLEAR_VALUE clearValue{};
	clearValue.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	clearValue.DepthStencil = { 1.f, 0 };

	const CD3DX12_HEAP_PROPERTIES heapProperties(D3D12_HEAP_TYPE_DEFAULT);

	const HRESULT hr = DX12::GetDevice()->CreateCommittedResource(
		&heapProperties, D3D12_HEAP_FLAG_NONE, &depthStencilDesc, D3D12_RESOURCE_STATE_DEPTH_WRITE, &clearValue,
		IID_PPV_ARGS(&mDepthStencilBuffer));
	check(SUCCEEDED(hr));

	createDepthStencilView();

	Logger::Info("Depth Stencil Buffer created successfully.");
}

void DX12Display::createDepthStencilView()
{
	check(mDepthStencilBuffer.Get());

	if (mDsvDescriptorHandle.GetCpuHandle().ptr == D3D12_GPU_VIRTUAL_ADDRESS_UNKNOWN)
	{
		check(DX12::GetDsvDescriptorHeap()->HasAvailableSpace(1));
		mDsvDescriptorHandle = DX12::GetDsvDescriptorHeap()->Allocate();
	}
	
	// Create descriptor to mip level 0 of entire resource using the format of the resource.
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc;
	dsvDesc.Flags = D3D12_DSV_FLAG_NONE;
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	dsvDesc.Texture2D.MipSlice = 0;
	
	// A null pDesc is used to initialize a default descriptor, if possible.
	// This behavior is identical to the D3D11 null descriptor behavior, where defaults are filled in.
	// This behavior inherits the resource format and dimension (if not typeless) and DSVs target the first mip and all array slices.
	// Not all resources support null descriptor initialization.
	DX12::GetDevice()->CreateDepthStencilView(mDepthStencilBuffer.Get(), &dsvDesc, mDsvDescriptorHandle.GetCpuHandle());

	// auto barrierToWritable = CD3DX12_RESOURCE_BARRIER::Transition(mBuffer.Get(),
	//                                                                    D3D12_RESOURCE_STATE_COMMON,
	//                                                                    D3D12_RESOURCE_STATE_DEPTH_WRITE);
	//
	// mCommandList->ResourceBarrier(1, &depthStencilTransition);
}
