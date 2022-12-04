#pragma once

class DX12Fence : NonCopyable
{
public:
	DX12Fence();
	~DX12Fence();

	void Signal(const ComPtr<ID3D12CommandQueue>& commandQueue);

	void WaitForFenceValue(std::chrono::milliseconds duration = std::chrono::milliseconds::max()) const;

	uint64 GetFenceValue() const { return mFenceValue; }

private:
	ComPtr<ID3D12Fence> mFence;
	uint64 mFenceValue;
	HANDLE mFenceEvent;
};
