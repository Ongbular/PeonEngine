#include "CorePCH.h"
#include "PeonEditorEngine.h"
#include "Platform/Win64/WindowInstance.h"
#include "TimerManager/TimerManager.h"
#include "Renderer/DX11/DX11Renderer.h"
#include "Renderer/DX11/DX11.h"
#include "Framework/GameWorld.h"

void PeonEditorEngine::setMainWindowInstance(const shared_ptr<WindowInstance> mainWindow)
{
	check(mainWindow.get());
	mMainWindow = mainWindow;
}

void PeonEditorEngine::startUp()
{
	// TODO: load level
	mMainWindow->ShowWindow();

	mWorld = make_shared<GameWorld>();
	mRenderer = make_shared<DX11Renderer>();
	mRenderer->Initialize(mMainWindow);
}

bool PeonEditorEngine::loop(const bool bShouldStop)
{
	if (handleInput() == false)
	{
		return false;
	}

	const float deltaSeconds = mTimerManager->UpdateDeltaSeconds(bShouldStop);

	if (std::isgreater(deltaSeconds, 0.f))
	{
		preTick();
		tickFrame(deltaSeconds);
		postTick();
	}

	return true;
}

void PeonEditorEngine::cleanUp()
{
	mRenderer->Cleanup();
}

bool PeonEditorEngine::handleInput()
{
	return true;
}

void PeonEditorEngine::preTick()
{
	mRenderer->PreRenderScene();
}

void PeonEditorEngine::tickFrame(const float deltaSeconds)
{
		// TODO: update game logic using deltaSeconds.

	mRenderer->RenderScene();
}

void PeonEditorEngine::postTick()
{
	mRenderer->PostRenderScene();
}

PeonEditorEngine::PeonEditorEngine()
{
}

PeonEditorEngine::~PeonEditorEngine()
{
}
