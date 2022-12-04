#pragma once

class WindowInstance;
class Scene;

class DX11Display;

class DX11Renderer
{
public:
	DX11Renderer();
	~DX11Renderer();

	void Initialize(const shared_ptr<WindowInstance>& targetWindow);

	void Cleanup();

	void UpdateSceneProxy();

	void PreRenderScene();

	void RenderScene();

	void PostRenderScene();

private:
	shared_ptr<Scene> mScene{ nullptr };
	shared_ptr<DX11Display> mDisplay{ nullptr };
};
