#include "CorePCH.h"
#include "DX12Fence.h"
#include "DX12.h"

DX12Fence::DX12Fence()
	: mFenceValue(0)
{
	const auto device = DX12::GetDevice();
	check(device.Get());

	const HRESULT hr = device->CreateFence(mFenceValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&mFence));
	verifyResult(hr);

	mFenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
	check(mFenceEvent);
}

DX12Fence::~DX12Fence()
{
	CloseHandle(mFenceEvent);
}

void DX12Fence::Signal(const ComPtr<ID3D12CommandQueue>& commandQueue)
{
	const HRESULT hr = commandQueue->Signal(mFence.Get(), ++mFenceValue);
	check(SUCCEEDED(hr));
}

void DX12Fence::WaitForFenceValue(const std::chrono::milliseconds duration) const
{
	// Wait until the previous frame is finished.
	if (mFence->GetCompletedValue() < mFenceValue)
	{
		mFence->SetEventOnCompletion(mFenceValue, mFenceEvent);
		WaitForSingleObject(mFenceEvent, static_cast<DWORD>(duration.count()));
	}
}
