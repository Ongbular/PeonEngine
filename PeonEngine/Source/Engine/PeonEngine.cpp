#include "CorePCH.h"
#include "PeonEngine.h"
#include "TimerManager/TimerManager.h"
#include "PeonEditorEngine.h"

void PeonEngine::InitiateEngine(const shared_ptr<WindowInstance> mainWindow)
{	
	check(sInstance.get());
	sInstance->setMainWindowInstance(mainWindow);
	sInstance->startUp();
}

bool PeonEngine::Loop(const bool bShouldStop)
{
	check(sInstance.get());

	return sInstance->loop(bShouldStop);
}

void PeonEngine::Cleanup()
{
	if (sInstance == nullptr)
	{
		Logger::Warn("Attempt to shut down the engine even though it has not been initialized.");
		return;
	}

	sInstance->cleanUp();
}

PeonEngine& PeonEngine::Get()
{
	if (sInstance == nullptr)
	{
#ifdef EDITOR_ENGINE
		sInstance = make_unique<PeonEditorEngine>();
#endif
	}

	return *sInstance;
}

PeonEngine::PeonEngine()
	: mTimerManager(make_unique<TimerManager>())
{
}

PeonEngine::~PeonEngine()
{
}
