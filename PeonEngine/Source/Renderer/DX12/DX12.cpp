#include "CorePCH.h"
#include "DX12.h"
#include "DX12DescriptorHeap.h"
#include "DX12Fence.h"

void DX12::Initialize()
{
	if (sbInitialized)
	{
		Logger::Warn("DirectX12 already initialized.");

		return;
	}

	Logger::Info("Initialization of Graphics interfaces has been started...");

	HRESULT hr = E_FAIL;

#if defined(_DEBUG)
	// Enable the debug layer(requires the Graphics Tools "optional feature").
	// NOTE: Enabling the debug layer after device creation will invalidate the active device.
	ComPtr<ID3D12Debug> debugController;

	hr = D3D12GetDebugInterface(IID_PPV_ARGS(&debugController));
	checkf(SUCCEEDED(hr), "Failed to query debug controller interface.");
	debugController->EnableDebugLayer();
#endif

	hr = createFactory();
	verifyResult(hr);

	hr = updateHardwareAdapter();
	verifyResult(hr);

	hr = createDevice(sHardwareAdapter);
	verifyResult(hr);

	hr = createCommandQueue();
	verifyResult(hr);

	sFence = make_shared<DX12Fence>();

	sRtvDescriptorSize = sDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	sDsvDescriptorSize = sDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
	sCbvSrvUavDescriptorSize = sDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	sSamplerSize = sDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER);

	createDescriptorHeaps();

	sbInitialized = true;
}

void DX12::FlushCommandQueue()
{
	sFence->Signal(sCommandQueue);
	sFence->WaitForFenceValue();

	sbIsCommandQueueFlushed = true;
}

uint32 DX12::GetDescriptorSize(const D3D12_DESCRIPTOR_HEAP_TYPE type)
{
	switch (type)
	{
	case D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV:
		return sCbvSrvUavDescriptorSize;
	case D3D12_DESCRIPTOR_HEAP_TYPE_RTV:
		return sRtvDescriptorSize;
	case D3D12_DESCRIPTOR_HEAP_TYPE_DSV:
		return sDsvDescriptorSize;
	case D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER:
		return sSamplerSize;
	default:
		check(false);
	}

	return 0;
}

HRESULT DX12::DisableFullScreen(const HWND hWnd)
{
	return sFactory->MakeWindowAssociation(hWnd, DXGI_MWA_NO_ALT_ENTER);
}

bool DX12::CheckTearingSupport()
{
	check(sFactory.Get());

	static bool bAlreadyChecked = false;
	if (!bAlreadyChecked)
	{
		// Rather than create the DXGI 1.5 factory interface directly, we create the
		// DXGI 1.4 interface and query for the 1.5 interface. This is to enable the 
		// graphics debugging tools which will not support the 1.5 factory interface 
		// until a future update.
		if (FAILED(sFactory->CheckFeatureSupport(DXGI_FEATURE_PRESENT_ALLOW_TEARING, &sbAllowTearing, sizeof(sbAllowTearing))))
		{
			sbAllowTearing = false;
		}
	}

	return sbAllowTearing;
}

void DX12::ShutDown()
{
	if (!sbInitialized)
	{
		Logger::Warn("Attempt to ShutDown DirectX12 even if it is not initialized.");
		return;
	}

	if (!sbIsCommandQueueFlushed)
	{
		FlushCommandQueue();
	}

	sRtvDescriptorHeap.reset();
	sDsvDescriptorHeap.reset();

	sFence.reset();

	sCommandQueue.Reset();
	sDevice.Reset();

	sOutputs.clear();
	sHardwareAdapter.Reset();
	sFactory.Reset();
}

HRESULT DX12::createFactory()
{
#ifdef _DEBUG
	return CreateDXGIFactory2(DXGI_CREATE_FACTORY_DEBUG, IID_PPV_ARGS(&sFactory));
#elif
	return CreateDXGIFactory2(0, IID_PPV_ARGS(&sFactory));
#endif
}

HRESULT DX12::updateHardwareAdapter(const bool bHighPerformanceRequired)
{
	const DXGI_GPU_PREFERENCE preference = bHighPerformanceRequired ? DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE : DXGI_GPU_PREFERENCE_UNSPECIFIED;
	ComPtr<IDXGIAdapter> tempAdapter;

	HRESULT hr = E_FAIL;
	uint32 i = 0;

	while (SUCCEEDED(sFactory->EnumAdapterByGpuPreference(i, preference, IID_PPV_ARGS(&tempAdapter))))
	{
		DXGI_ADAPTER_DESC adapterDesc;
		tempAdapter->GetDesc(&adapterDesc);

		wstring tempStr = adapterDesc.Description;
		string adapterName(tempStr.cbegin(), tempStr.cend());

		hr = D3D12CreateDevice(tempAdapter.Get(), D3D_FEATURE_LEVEL_12_1, _uuidof(ID3D12Device), nullptr);
		if (hr == S_OK)
		{
			Logger::Info("Hardware Adapter #{}: {} has collected...", i + 1, adapterName);

			if (i == 0)
			{
				hr = tempAdapter.As(&sHardwareAdapter);
				check(SUCCEEDED(hr));
			}

			hr = updateOutputs(tempAdapter);
			if (i == 0 && FAILED(hr))
			{
				return E_FAIL;
			}
		}

		check(i < MAX_HARDWARE_ADAPTER_NUM);
		i++;
	}

	return hr;
}

HRESULT DX12::updateOutputs(const ComPtr<IDXGIAdapter>& adapter)
{
	checkf(adapter.Get(), "Invalid adapter.");

	ComPtr<IDXGIOutput> tempOutput{ nullptr };
	uint32 i = 0;

	while (adapter->EnumOutputs(i, &tempOutput) != DXGI_ERROR_NOT_FOUND)
	{
		DXGI_OUTPUT_DESC desc;
		tempOutput->GetDesc(&desc);

		wstring tempStr = desc.DeviceName;
		string outputName(tempStr.cbegin(), tempStr.cend());

		Logger::Info("\tOutput Device #{}: {} has collected...", i + 1, outputName);

		if (sHardwareAdapter == adapter)
		{
			sOutputs.emplace_back(std::move(tempOutput));
		}

		check(i < MAX_OUTPUT_DEVICE_NUM);
		i++;
	}

	const HRESULT hr = i > 0 ? S_OK : E_FAIL;
	return hr;
}

HRESULT DX12::createDevice(const ComPtr<IDXGIAdapter4>& adapter)
{
	HRESULT hr = D3D12CreateDevice(sHardwareAdapter.Get(), D3D_FEATURE_LEVEL_12_1, IID_PPV_ARGS(&sDevice));
	verifyResult(hr);

#ifdef _DEBUG
	ComPtr<ID3D12InfoQueue> pInfoQueue;
	if (SUCCEEDED(sDevice.As(&pInfoQueue)))
	{
		pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, TRUE);
		pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, TRUE);
		pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, TRUE);

		// Suppress whole categories of messages
		//D3D12_MESSAGE_CATEGORY Categories[] = {};

		// Suppress messages based on their severity level
		D3D12_MESSAGE_SEVERITY Severities[] =
		{
			D3D12_MESSAGE_SEVERITY_INFO
		};

		// Suppress individual messages by their ID
		D3D12_MESSAGE_ID DenyIds[] = {
			D3D12_MESSAGE_ID_CLEARRENDERTARGETVIEW_MISMATCHINGCLEARVALUE,   // I'm really not sure how to avoid this message.
			D3D12_MESSAGE_ID_MAP_INVALID_NULLRANGE,                         // This warning occurs when using capture frame while graphics debugging.
			D3D12_MESSAGE_ID_UNMAP_INVALID_NULLRANGE,                       // This warning occurs when using capture frame while graphics debugging.
		};

		D3D12_INFO_QUEUE_FILTER NewFilter = {};
		//NewFilter.DenyList.NumCategories = _countof(Categories);
		//NewFilter.DenyList.pCategoryList = Categories;
		NewFilter.DenyList.NumSeverities = _countof(Severities);
		NewFilter.DenyList.pSeverityList = Severities;
		NewFilter.DenyList.NumIDs = _countof(DenyIds);
		NewFilter.DenyList.pIDList = DenyIds;

		hr = pInfoQueue->PushStorageFilter(&NewFilter);
		verifyResult(hr);
	}
#endif

	return S_OK;
}

HRESULT DX12::createCommandQueue()
{
	D3D12_COMMAND_QUEUE_DESC queueDesc;
	queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	queueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
	queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	queueDesc.NodeMask = 0; // GPU를 한개 사용하기 때문에 0

	return sDevice->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&sCommandQueue));
}


void DX12::createDescriptorHeaps()
{
	sRtvDescriptorHeap = make_shared<DX12DescriptorHeap>(D3D12_DESCRIPTOR_HEAP_TYPE_RTV, D3D12_DESCRIPTOR_HEAP_FLAG_NONE, 3);
	sDsvDescriptorHeap = make_shared<DX12DescriptorHeap>(D3D12_DESCRIPTOR_HEAP_TYPE_DSV, D3D12_DESCRIPTOR_HEAP_FLAG_NONE, 1);
}

