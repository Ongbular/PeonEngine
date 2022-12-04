#pragma once

#include "DX12DescriptorHandle.h"

#define DEFAULT_SWAP_CHAIN_BUFFER_COUNT (3)

class DX12Display : NonCopyable
{
public:
	DX12Display(HWND hWnd, uint32 width, uint32 height);
	~DX12Display() = default;

	void OnWindowResized(uint32 width, uint32 height);

	void Present() const;

	ComPtr<IDXGISwapChain4> GetSwapChain() const { return mSwapChain; }

	DX12DescriptorHandle GetRtvDescriptorHandle() const { return mRtvDescriptorHandle; }

	ComPtr<ID3D12Resource> GetCurrentBackBuffer() const;

	static constexpr uint32 GetBackBufferCount() { return DEFAULT_SWAP_CHAIN_BUFFER_COUNT; }

	ComPtr<ID3D12Resource> GetDepthStencilBuffer() const { return mDepthStencilBuffer; }

	DX12DescriptorHandle GetDsvDescriptorHandle() const { return mDsvDescriptorHandle; }

private:
	void createSwapChain(HWND hWnd, uint32 width, uint32 height);

	void resizeSwapChainBuffers(uint32 width, uint32 height);

	void createRenderTargetViews();

	void createDepthStencilBuffer(uint32 width, uint32 height);

	void createDepthStencilView();

	ComPtr<IDXGISwapChain4> mSwapChain{ nullptr };

	DX12DescriptorHandle mRtvDescriptorHandle{};

	ComPtr<ID3D12Resource> mDepthStencilBuffer{ nullptr };

	DX12DescriptorHandle mDsvDescriptorHandle{};
};
