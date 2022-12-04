#pragma once

class WindowApplication
{
public:
	static HRESULT Run(HINSTANCE hInstance, int nCmdShow);

	static HINSTANCE GetInstanceHandle() { return sInstanceHandle; }

private:
	static HRESULT initializeApplication(HINSTANCE hInstance, int nCmdShow);

	static void terminateApplication();
	
	inline static bool sbShouldStop = false;

	inline static HINSTANCE sInstanceHandle = nullptr;

public:
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

};

