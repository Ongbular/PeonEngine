#pragma once

class DX12Fence;
class DX12DescriptorHeap;

class DX12
{
public:
	static void Initialize();

	static void FlushCommandQueue();

	static uint32 GetDescriptorSize(D3D12_DESCRIPTOR_HEAP_TYPE type);

	static HRESULT DisableFullScreen(HWND hWnd);

	static constexpr DXGI_FORMAT GetDefaultOutputBufferFormat() { return DXGI_FORMAT_R8G8B8A8_UNORM; }

	static bool CheckTearingSupport();

	static void ShutDown();

	inline static ComPtr<ID3D12Device2> GetDevice() { return sDevice; }
	inline static ComPtr<ID3D12CommandQueue> GetCommandQueue() { return sCommandQueue; }
	inline static shared_ptr<DX12Fence> GetFence() { return sFence; }
	inline static ComPtr<IDXGIFactory6> GetFactory() { return sFactory; }
	inline static ComPtr<IDXGIAdapter4> GetAdapter() { return sHardwareAdapter; }
	inline static vector<ComPtr<IDXGIOutput>> GetOutputs() { return sOutputs; }
	inline static bool IsInitialized() { return sbInitialized; }
	inline static void EnableVSync(const bool bEnable) { sbVSyncEnabled = bEnable; }
	inline static bool IsVSyncEnabled() { return sbVSyncEnabled; }
	inline static bool IsCommandQueueFlushed() { return sbIsCommandQueueFlushed; }

	inline static shared_ptr<DX12DescriptorHeap> GetRtvDescriptorHeap() { return sRtvDescriptorHeap; }
	inline static shared_ptr<DX12DescriptorHeap> GetDsvDescriptorHeap() { return sDsvDescriptorHeap; }

private:
	static HRESULT createFactory();

	static HRESULT updateHardwareAdapter(bool bHighPerformanceRequired = true);

	static HRESULT updateOutputs(const ComPtr<IDXGIAdapter>& adapter);

	static HRESULT createDevice(const ComPtr<IDXGIAdapter4>& adapter);

	static HRESULT createCommandQueue();

	static void createDescriptorHeaps();

private:
	enum
	{
		MAX_HARDWARE_ADAPTER_NUM = 8,
		MAX_OUTPUT_DEVICE_NUM = 4,
	};

	inline static bool sbInitialized = false;
	inline static bool sbAllowTearing = false;
	inline static bool sbVSyncEnabled = false;
	inline static bool sbIsCommandQueueFlushed = false;

	inline static ComPtr<ID3D12Device2> sDevice{ nullptr };
	inline static ComPtr<ID3D12CommandQueue> sCommandQueue{ nullptr };
	inline static shared_ptr<DX12Fence> sFence{ nullptr };

	inline static ComPtr<IDXGIFactory6> sFactory{ nullptr };
	inline static ComPtr<IDXGIAdapter4> sHardwareAdapter{ nullptr };
	inline static vector<ComPtr<IDXGIOutput>> sOutputs{ nullptr };

	inline static uint32 sRtvDescriptorSize = 0;
	inline static uint32 sDsvDescriptorSize = 0;
	inline static uint32 sCbvSrvUavDescriptorSize = 0;
	inline static uint32 sSamplerSize = 0;

	inline static shared_ptr<DX12DescriptorHeap> sRtvDescriptorHeap{ nullptr };
	inline static shared_ptr<DX12DescriptorHeap> sDsvDescriptorHeap{ nullptr };
};
