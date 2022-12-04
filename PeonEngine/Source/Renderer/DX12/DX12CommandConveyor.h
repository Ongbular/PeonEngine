#pragma once

class DX12DescriptorHandle;

class DX12CommandConveyor
{
public:
	DX12CommandConveyor();

	void PrePopulateCommands(const ComPtr<ID3D12Resource>& backBuffer, DX12DescriptorHandle handle);

	void ExecuteCommand(const ComPtr<ID3D12Resource>& backBuffer);

	ComPtr<ID3D12CommandAllocator> GetCommandAllocator() const { return mCommandAllocator; }
	ComPtr<ID3D12GraphicsCommandList> GetCommandList() const { return mCommandList; }

private:
	void postPopulateCommands(const ComPtr<ID3D12Resource>& backBuffer);

	ComPtr<ID3D12CommandAllocator> mCommandAllocator;
	ComPtr<ID3D12GraphicsCommandList> mCommandList;
};
