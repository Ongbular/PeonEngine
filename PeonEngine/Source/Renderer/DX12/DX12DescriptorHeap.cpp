#include "CorePCH.h"
#include "DX12DescriptorHeap.h"
#include "DX12.h"

DX12DescriptorHeap::DX12DescriptorHeap(const D3D12_DESCRIPTOR_HEAP_TYPE type, const D3D12_DESCRIPTOR_HEAP_FLAGS flag, const uint32 maxCount)
{
	mDescription.Type = type;
	mDescription.NumDescriptors = maxCount;
	mDescription.Flags = flag;
	mDescription.NodeMask = 0;

	DX12::GetDevice()->CreateDescriptorHeap(&mDescription, IID_PPV_ARGS(&mHeap));

	mNumFreeDescriptors = mDescription.NumDescriptors;
	mDescriptorSize = DX12::GetDescriptorSize(type);

	if (flag == D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE)
	{
		mFirstHandle = DX12DescriptorHandle(type, mHeap->GetCPUDescriptorHandleForHeapStart(), mHeap->GetGPUDescriptorHandleForHeapStart());
	}
	else
	{
		mFirstHandle = DX12DescriptorHandle(type, mHeap->GetCPUDescriptorHandleForHeapStart());
	}

	mNextFreeHandle = mFirstHandle;
}

DX12DescriptorHandle DX12DescriptorHeap::Allocate(const uint32 count)
{
	check(HasAvailableSpace(count));

	const DX12DescriptorHandle ret = mNextFreeHandle;
	mNextFreeHandle.Offset(count * mDescriptorSize);
	mNumFreeDescriptors -= count;
	return ret;
}

DX12DescriptorHandle DX12DescriptorHeap::operator[](uint32 index) const
{
	DX12DescriptorHandle ret;

	if (index >= mDescription.NumDescriptors - (mNumFreeDescriptors + 1))
	{
		ret = mFirstHandle;
		ret.Offset(index * mDescriptorSize);
	}

	return ret;
}

uint32 DX12DescriptorHeap::GetOffsetOfHandle(const DX12DescriptorHandle& handle) const
{
	return static_cast<uint32>(handle.mCpuHandle.ptr - mFirstHandle.mCpuHandle.ptr) / mDescriptorSize;
}