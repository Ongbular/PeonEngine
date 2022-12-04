#pragma once

#include "DX12Defines.h"

class DX12DescriptorHandle
{
	friend class DX12DescriptorHeap;

public:
	DX12DescriptorHandle() = default;
	DX12DescriptorHandle(D3D12_DESCRIPTOR_HEAP_TYPE type, D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle);
	DX12DescriptorHandle(D3D12_DESCRIPTOR_HEAP_TYPE type, D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle, D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle);

	DX12DescriptorHandle operator+(int32 offset) const;
	DX12DescriptorHandle& operator+=(int32 offset);
	DX12DescriptorHandle& operator++();
	DX12DescriptorHandle operator++(int32);
	void Offset(int32 offset = 1);

	D3D12_DESCRIPTOR_HEAP_TYPE GetType() const { return mType; }
	D3D12_CPU_DESCRIPTOR_HANDLE GetCpuHandle() const { return mCpuHandle; }
	D3D12_GPU_DESCRIPTOR_HANDLE GetGpuHandle() const { return mGpuHandle; }

private:
	D3D12_DESCRIPTOR_HEAP_TYPE mType = D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES;
	D3D12_CPU_DESCRIPTOR_HANDLE mCpuHandle{ D3D12_GPU_VIRTUAL_ADDRESS_UNKNOWN };
	D3D12_GPU_DESCRIPTOR_HANDLE mGpuHandle{ D3D12_GPU_VIRTUAL_ADDRESS_UNKNOWN };
};