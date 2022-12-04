#pragma once

class DX11
{
public:
	static void Initialize();

	static void ShutDown();

	static bool CheckTearingSupport();

	static void EnableVSync(const bool bEnabled) { sbVSyncEnabled = bEnabled; }
	static bool IsVSyncEnabled() { return sbVSyncEnabled; }
	static bool IsInitialized() { return sbInitialized; }
	static ComPtr<ID3D11Device5> GetDevice() { return sDevice; }
	static ComPtr<ID3D11DeviceContext4> GetImmediateContext() { return sImmediateContext; }
	static ComPtr<IDXGIFactory6> GetFactory() { return sFactory; }
	static ComPtr<IDXGIAdapter4> GetHardwareAdapter() { return sHardwareAdapter; }
	static vector<ComPtr<IDXGIOutput>> GetOutputs() { return sOutputs; }

private:
	static void createFactory();
	static void updateHardwareAdapter(bool bHighPerformanceRequired = true);
	static void updateOutputs(const ComPtr<IDXGIAdapter>& adapter);

	enum
	{
		MAX_HARDWARE_ADAPTER_NUM = 8,
		MAX_OUTPUT_DEVICE_NUM = 4,
	};

	inline static bool sbInitialized = false;
	inline static bool sbVSyncEnabled = false;

	inline static ComPtr<ID3D11Device5> sDevice{ nullptr };
	inline static ComPtr<ID3D11DeviceContext4> sImmediateContext{ nullptr };

	inline static ComPtr<IDXGIFactory6> sFactory{ nullptr };
	inline static ComPtr<IDXGIAdapter4> sHardwareAdapter{ nullptr };
	inline static vector<ComPtr<IDXGIOutput>> sOutputs{ nullptr };
};