#pragma once

#define WIN32_LEAN_AND_MEAN

#define NOMINMAX

#include <windows.h>
#include <iostream>
#include <iomanip>
#include <chrono>
#include <fstream>
#include <tchar.h>
#include <memory>
#include <string>
#include <sstream>
#include <vector>
#include <array>
#include <list>
#include <set>
#include <map>
#include <unordered_set>
#include <unordered_map>
#include <fstream>

#include <wrl.h>
#include <d3d12.h>
#include "DX12/d3dx12.h"
#include "d3d11_4.h"
#include <dxgi1_6.h>
#include <D3Dcompiler.h>
#include <DirectXMath.h>

#ifdef _DEBUG
#include <dxgidebug.h>
#endif

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")

// using namespace DirectX::PackedVector;

using namespace DirectX;
using namespace Microsoft::WRL;
using namespace std;
using namespace chrono;

#include "CoreTypes.h"
#include "CoreAssert.h"
#include "Logger/Logger.h"
#include "Delegates/UniCastDelegate.h"
#include "Delegates/MultiCastDelegate.h"
#include "Utility/EnumHelper.h"
#include "Engine/PeonEngine.h"
#include "Utility/PeonStringHelper.h"

using namespace joo;
