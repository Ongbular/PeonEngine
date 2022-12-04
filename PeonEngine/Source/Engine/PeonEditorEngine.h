#pragma once

#include "PeonEngine.h"

class SceneRenderer;
class GameWorld;
class DX11Renderer;

class PeonEditorEngine : public PeonEngine
{
	friend std::unique_ptr<PeonEditorEngine> std::make_unique<PeonEditorEngine>();
	
protected:
	void setMainWindowInstance(shared_ptr<WindowInstance> mainWindow) override;

	void startUp() override;

	bool loop(bool bShouldStop) override;

	void cleanUp() override;

private:
	bool handleInput();

	void preTick();

	void tickFrame(float deltaSeconds);

	void postTick();

	shared_ptr<GameWorld> mWorld;
	shared_ptr<DX11Renderer> mRenderer;

public:
	~PeonEditorEngine() override;

private:
	PeonEditorEngine();
};

