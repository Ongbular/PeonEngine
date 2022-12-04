#pragma once

#if defined(_WIN32) || defined(_WIN64)
#include <Windows.h>
#include "Platform/Win64/WindowApplication.h"

#define INITIATE_APPLICATION \
	int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE /* hPrevInstance */, _In_ LPWSTR /* lpCmdLine */, _In_ int nCmdShow) \
	{ \
		return WindowApplication::Run(hInstance, nCmdShow); \
	}
#endif
