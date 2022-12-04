#pragma once

class WindowInstance;
class TimerManager;
class WindowInstance;

#define EDITOR_ENGINE

class PeonEngine
{
	// friend std::unique_ptr<PeonEngine> std::make_unique<PeonEngine>();

public:
	static void InitiateEngine(shared_ptr<WindowInstance> mainWindow);
	
	static bool Loop(bool bShouldStop);

	static void Cleanup();

	static PeonEngine& Get();

	shared_ptr<WindowInstance> GetMainWindow() const { return mMainWindow; }

protected:
	virtual void setMainWindowInstance(shared_ptr<WindowInstance> mainWindow) = 0;

	virtual void startUp() = 0;

	virtual bool loop(bool bShouldStop) = 0;

	virtual void cleanUp() = 0;
	
	inline static unique_ptr<PeonEngine> sInstance{ nullptr };

	shared_ptr<WindowInstance> mMainWindow;

	unique_ptr<TimerManager> mTimerManager;
	
	PeonEngine();

public:
	virtual ~PeonEngine();
};

#define gEngine (&PeonEngine::Get())