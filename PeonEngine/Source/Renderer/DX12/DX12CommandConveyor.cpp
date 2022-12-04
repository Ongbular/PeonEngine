#include "CorePCH.h"
#include "DX12CommandConveyor.h"
#include "DX12DescriptorHandle.h"
#include "DX12.h"

DX12CommandConveyor::DX12CommandConveyor()
{
	const auto device = DX12::GetDevice();
	check(device.Get());

	HRESULT hr = device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&mCommandAllocator));
	verifyResult(hr);

	hr = device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, mCommandAllocator.Get(), nullptr,
		IID_PPV_ARGS(&mCommandList));
	verifyResult(hr);

	mCommandList->Close();
}

void DX12CommandConveyor::PrePopulateCommands(const ComPtr<ID3D12Resource>& backBuffer, const DX12DescriptorHandle handle)
{
	// Command list allocators can only be reset when the associated 
	// command lists have finished execution on the GPU; apps should use 
	// fences to determine GPU execution progress.
	mCommandAllocator->Reset();

	// However, when ExecuteCommandList() is called on a particular command 
	// list, that command list can then be reset at any time and must be before 
	// re-recording.
	mCommandList->Reset(mCommandAllocator.Get(), nullptr);

	const auto barrierToRenderTarget = CD3DX12_RESOURCE_BARRIER::Transition(backBuffer.Get(), D3D12_RESOURCE_STATE_PRESENT,
		D3D12_RESOURCE_STATE_RENDER_TARGET);

	// Indicate that the back buffer will be used as a render target.
	mCommandList->ResourceBarrier(1, &barrierToRenderTarget);

	// TODO: clearColor를 바꿔야 할 수도 있음.
	const float clearColor[] = { 0.0f, 0.2f, 0.4f, 1.0f };
	mCommandList->ClearRenderTargetView(handle.GetCpuHandle(), clearColor, 0, nullptr);
}

void DX12CommandConveyor::ExecuteCommand(const ComPtr<ID3D12Resource>& backBuffer)
{
	postPopulateCommands(backBuffer);

	// Execute the command list.
	ID3D12CommandList* ppCommandLists[] = { mCommandList.Get() };

	DX12::GetCommandQueue()->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);
}

void DX12CommandConveyor::postPopulateCommands(const ComPtr<ID3D12Resource>& backBuffer)
{
	const auto barrierToPresent = CD3DX12_RESOURCE_BARRIER::Transition(backBuffer.Get(),
		D3D12_RESOURCE_STATE_RENDER_TARGET,
		D3D12_RESOURCE_STATE_PRESENT);

	// Indicate that the back buffer will now be used to present.
	mCommandList->ResourceBarrier(1, &barrierToPresent);
	mCommandList->Close();
}
