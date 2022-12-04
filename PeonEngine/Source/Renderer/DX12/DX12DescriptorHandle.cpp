#include "CorePCH.h"
#include "DX12DescriptorHandle.h"
#include "DX12.h"

DX12DescriptorHandle::DX12DescriptorHandle(const D3D12_DESCRIPTOR_HEAP_TYPE type,
                                           const D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle)
	: mType(type)
	, mCpuHandle(cpuHandle)
{
}

DX12DescriptorHandle::DX12DescriptorHandle(const D3D12_DESCRIPTOR_HEAP_TYPE type,
	const D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle,
	const D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle)
	: mType(type)
	, mCpuHandle(cpuHandle)
	, mGpuHandle(gpuHandle)
{
}

DX12DescriptorHandle DX12DescriptorHandle::operator+(const int32 offset) const
{
	DX12DescriptorHandle ret(mType, mCpuHandle, mGpuHandle);
	ret.Offset(offset);
	return ret;
}

DX12DescriptorHandle& DX12DescriptorHandle::operator+=(const int32 offset)
{
	Offset(offset);
	return *this;
}

DX12DescriptorHandle& DX12DescriptorHandle::operator++()
{
	Offset(1);
	return *this;
}

DX12DescriptorHandle DX12DescriptorHandle::operator++(int32)
{
	Offset(1);
	return {};
}

void DX12DescriptorHandle::Offset(const int32 offset)
{
	if (offset == 0)
	{
		return;
	}

	const SIZE_T descriptorSize = DX12::GetDescriptorSize(mType);

	if (mCpuHandle.ptr != D3D12_GPU_VIRTUAL_ADDRESS_UNKNOWN)
		mCpuHandle.ptr += descriptorSize * offset;
	if (mGpuHandle.ptr != D3D12_GPU_VIRTUAL_ADDRESS_UNKNOWN)
		mGpuHandle.ptr += descriptorSize * offset;
}