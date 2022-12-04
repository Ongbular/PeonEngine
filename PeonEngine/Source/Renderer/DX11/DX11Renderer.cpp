#include "CorePCH.h"
#include "DX11Renderer.h"
#include "Scene/Scene.h"
#include "DX11Display.h"
#include "DX11.h"

DX11Renderer::DX11Renderer()
	: mScene(make_shared<Scene>())
	, mDisplay(make_shared<DX11Display>())
{
	DX11::Initialize();
}

DX11Renderer::~DX11Renderer()
{
	DX11::ShutDown();
}

void DX11Renderer::Initialize(const shared_ptr<WindowInstance>& targetWindow)
{
	mDisplay->Initialize(targetWindow);
	mDisplay->SetClearColor(0.f, 0.2f, 0.4f, 1.f);
}

void DX11Renderer::Cleanup()
{
}

void DX11Renderer::UpdateSceneProxy()
{
}

void DX11Renderer::PreRenderScene()
{
	mDisplay->ClearBackBuffer();
	mDisplay->ClearDepthBuffer(1.f, 0);
}

void DX11Renderer::RenderScene()
{
	mDisplay->Present();
}

void DX11Renderer::PostRenderScene()
{
}
