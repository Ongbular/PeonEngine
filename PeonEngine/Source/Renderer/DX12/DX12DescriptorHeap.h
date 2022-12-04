#pragma once

#include "DX12DescriptorHandle.h"

class DX12DescriptorHeap
{
public:
	DX12DescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE type, D3D12_DESCRIPTOR_HEAP_FLAGS flag, uint32 maxCount);
	~DX12DescriptorHeap() = default;

	bool HasAvailableSpace(const uint32 descriptorNum) const { return descriptorNum <= mNumFreeDescriptors; }

	DX12DescriptorHandle Allocate(uint32 count = 1);
	DX12DescriptorHandle operator[](uint32 index) const;

	uint32 GetOffsetOfHandle(const DX12DescriptorHandle& handle) const;

	ComPtr<ID3D12DescriptorHeap> GetHeap() const { return mHeap; }

private:
	ComPtr<ID3D12DescriptorHeap> mHeap;
	D3D12_DESCRIPTOR_HEAP_DESC mDescription{};
	uint32 mDescriptorSize;
	uint32 mNumFreeDescriptors;

	DX12DescriptorHandle mFirstHandle;
	DX12DescriptorHandle mNextFreeHandle;
};