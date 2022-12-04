#include "CorePCH.h"
#include "WindowInstance.h"
#include "WindowApplication.h"
#include "Media/Win64/resource.h"

WindowInstance::WindowInstance(const HINSTANCE hInstance, const std::string& name, const std::string& title, const int32 width, const int32 height)
{
	const int32 clampedWidth = std::max(width, WINDOW_MIN_WIDTH);
	const int32 clampedHeight = std::max(height, WINDOW_MIN_HEIGHT);
	
	WNDCLASSEX windowClass = { 0 };
	windowClass.cbSize = sizeof(WNDCLASSEX);
	windowClass.style = CS_HREDRAW | CS_VREDRAW;
	windowClass.lpfnWndProc = WindowApplication::WndProc;
	windowClass.hInstance = hInstance;
	windowClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
	windowClass.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDC_LARGE));
	windowClass.hIconSm = LoadIcon(hInstance, MAKEINTRESOURCE(IDC_SMALL));
	windowClass.lpszClassName = name.c_str();

	RegisterClassEx(&windowClass);

	RECT windowRect = { 0, 0, static_cast<LONG>(clampedWidth), static_cast<LONG>(clampedHeight) };
	AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, FALSE);

	const HWND hWnd = CreateWindow(
		windowClass.lpszClassName,
		title.c_str(),
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		windowRect.right - windowRect.left,
		windowRect.bottom - windowRect.top,
		nullptr, // We have no parent window.
		nullptr, // We aren't using menus.
		hInstance,
		nullptr);

	const HRESULT hr = hWnd != nullptr ? S_OK : E_ABORT;
	verifyResult(hr);

	mHandle = hWnd;
	mName = name;
	mTitle = title;
	mWidth = clampedWidth;
	mHeight = clampedHeight;
}

void WindowInstance::ShowWindow() const
{
	::ShowWindow(mHandle, SW_SHOWDEFAULT);
}

void WindowInstance::OnResized(const int32 width, const int32 height)
{
	mWidth = width;
	mHeight = height;

	OnWindowResized.ExecuteIfBound(mWidth, mHeight);
}
