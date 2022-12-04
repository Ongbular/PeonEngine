#include "CorePCH.h"
#include "DX11.h"

void DX11::Initialize()
{
	if (sbInitialized)
	{
		Logger::Warn("DirectX11 already initialized.");
		return;
	}

	createFactory();
	updateHardwareAdapter();
	updateOutputs(sHardwareAdapter);

	UINT createDeviceFlags = 0;
#ifdef _DEBUG
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	D3D_FEATURE_LEVEL featureLevels[] =
	{
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
	};

	constexpr UINT numFeatureLevels = ARRAYSIZE(featureLevels);

	ComPtr<ID3D11Device> tempDevice;
	ComPtr<ID3D11DeviceContext> tempDeviceContext;
	D3D_FEATURE_LEVEL resultFeatureLevel;

	HRESULT hr = D3D11CreateDevice(sHardwareAdapter.Get(), D3D_DRIVER_TYPE_UNKNOWN, nullptr, createDeviceFlags, featureLevels,
		numFeatureLevels, D3D11_SDK_VERSION, &tempDevice, &resultFeatureLevel, &tempDeviceContext);
	verifyResult(hr);
	check(resultFeatureLevel == D3D_FEATURE_LEVEL_11_1);

	hr = tempDevice.As(&sDevice);
	verifyResult(hr);
	Logger::Info("DirectX11 Device created successfully.");

	hr = tempDeviceContext.As(&sImmediateContext);
	verifyResult(hr);
	Logger::Info("DirectX11 Device Context created successfully.");

	sbInitialized = true;
}

void DX11::ShutDown()
{
	sImmediateContext.Reset();
	sDevice.Reset();

	sOutputs.clear();
	sHardwareAdapter.Reset();
	sFactory.Reset();

	sbInitialized = false;
}

bool DX11::CheckTearingSupport()
{
	if (sFactory.Get() == nullptr)
	{
		Logger::Warn("IDXGIFactory6 is not initialized.");
		return false;
	}

	BOOL bAllowTearing = FALSE;
	const HRESULT hr = sFactory->CheckFeatureSupport(DXGI_FEATURE_PRESENT_ALLOW_TEARING, &bAllowTearing, sizeof(bAllowTearing));

	return SUCCEEDED(hr) && bAllowTearing;
}

void DX11::createFactory()
{
#ifdef _DEBUG
	verifyResult(CreateDXGIFactory2(DXGI_CREATE_FACTORY_DEBUG, IID_PPV_ARGS(&sFactory)));
#elif
	verifyResult(CreateDXGIFactory2(0, IID_PPV_ARGS(&gFactory)));
#endif

	Logger::Info("IDXGIFactory6 created successfully.");
}

void DX11::updateHardwareAdapter(bool bHighPerformanceRequired)
{
	const DXGI_GPU_PREFERENCE preference = bHighPerformanceRequired
		? DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE
		: DXGI_GPU_PREFERENCE_UNSPECIFIED;
	ComPtr<IDXGIAdapter> tempAdapter;

	uint32 i = 0;

	while (SUCCEEDED(sFactory->EnumAdapterByGpuPreference(i, preference, IID_PPV_ARGS(&tempAdapter))))
	{
		DXGI_ADAPTER_DESC adapterDesc;
		tempAdapter->GetDesc(&adapterDesc);

		wstring tempStr = adapterDesc.Description;
		string adapterName = PeonStringHelper::WStringToString(tempStr);

		Logger::Info("Hardware Adapter #{}: {} has collected...", i + 1, adapterName);

		if (i == 0)
		{
			const HRESULT hr = tempAdapter.As(&sHardwareAdapter);
			check(SUCCEEDED(hr));
		}

		updateOutputs(tempAdapter);

		check(i < MAX_HARDWARE_ADAPTER_NUM);
		i++;
	}
}

void DX11::updateOutputs(const ComPtr<IDXGIAdapter>& adapter)
{
	checkf(adapter.Get(), "Invalid adapter.");

	ComPtr<IDXGIOutput> tempOutput{ nullptr };
	uint32 i = 0;

	while (adapter->EnumOutputs(i, tempOutput.ReleaseAndGetAddressOf()) != DXGI_ERROR_NOT_FOUND)
	{
		DXGI_OUTPUT_DESC desc;
		tempOutput->GetDesc(&desc);

		wstring tempStr = desc.DeviceName;
		string outputName = PeonStringHelper::WStringToString(tempStr);

		Logger::Info("\t\tOutput Device #{}: {} has collected...", i + 1, outputName);

		if (sHardwareAdapter == adapter)
		{
			sOutputs.emplace_back(std::move(tempOutput));
		}

		check(i < MAX_OUTPUT_DEVICE_NUM);
		i++;
	}
}
