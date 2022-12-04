#pragma once

DECLARE_UNI_CAST_DELEGATE(JOnWindowResized, int32 /* width */, int32 /* height */)

#define WINDOW_MIN_WIDTH (100)
#define WINDOW_MIN_HEIGHT (100)

class WindowInstance
{
public:
	WindowInstance(HINSTANCE hInstance, const std::string& name, const std::string& title, int32 width, int32 height);
	~WindowInstance() = default;

	void ShowWindow() const;

	void OnResized(int32 width, int32 height);

	void SetMinimized(const bool cond) { mbIsMinimized = cond; }

	bool IsMinimized() const { return mbIsMinimized; }

	JOnWindowResized OnWindowResized;

private:
	HWND mHandle;
	std::string mName;
	std::string mTitle;
	int32 mWidth;
	int32 mHeight;
	bool mbIsMinimized;

public:
	HWND GetHandle() const { return mHandle; }
	std::string GetName() const { return mName; }
	std::string GetTitle() const { return mTitle; }
	int32 GetWidth() const { return mWidth; }
	int32 GetHeight() const { return mHeight; }
};

