#include "CorePCH.h"
#include "WindowApplication.h"
#include "WindowInstance.h"
#include "ConfigParser/ConfigParser.h"

HRESULT WindowApplication::Run(HINSTANCE hInstance, int nCmdShow)
{
	const HRESULT hr = initializeApplication(hInstance, nCmdShow);
	verifyResult(hr);
	
	do
	{
		MSG msg = {};
		bool done = false;
		while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);

			if (msg.message == WM_QUIT)
				done = true;
		}

		if (done)
			break;
	} while (gEngine->Loop(sbShouldStop)); // Returns false to quit loop

	terminateApplication();

	return 0;
}

HRESULT WindowApplication::initializeApplication(HINSTANCE hInstance, int nCmdShow)
{
	sInstanceHandle = hInstance;

	HRESULT hr = Logger::InitializeLogger();
	verifyResult(hr);

	Logger::PrintEngineLogo();

	hr = ConfigParser::ParseConfigurations();
	verifyResult(hr);

	const string name = ConfigParser::GetValue<string>(eConfigType::Editor, CONFIG_SECTION_MAIN_EDITOR, CONFIG_PROP_MAIN_EDITOR_NAME);
	const string title = ConfigParser::GetValue<string>(eConfigType::Editor, CONFIG_SECTION_MAIN_EDITOR, CONFIG_PROP_MAIN_EDITOR_TITLE);
	const int32 width = ConfigParser::GetValue<int32>(eConfigType::Editor, CONFIG_SECTION_MAIN_EDITOR, CONFIG_PROP_MAIN_EDITOR_WIDTH);
	const int32 height = ConfigParser::GetValue<int32>(eConfigType::Editor, CONFIG_SECTION_MAIN_EDITOR, CONFIG_PROP_MAIN_EDITOR_HEIGHT);

	const shared_ptr<WindowInstance> mainWindow = make_shared<WindowInstance>(hInstance, name, title, width, height);
	check(mainWindow.get());

	gEngine->InitiateEngine(mainWindow);

	return S_OK;
}

void WindowApplication::terminateApplication()
{
	gEngine->Cleanup();
}

LRESULT WindowApplication::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	// ������ٸ� ��� �巡���ϸ� �⺻������ ������� �̷���. �ٸ� ���� ������ ���ư��� ����
	// �̴ϸ���� �ϸ� ������ ����� ��. �ƽø������ �������.
	// ������ ����� �ϴ� ��Ȳ : �̴ϸ�����, ���̵�� �巡��
	// ������ ���� ���� �� �ٽ� ����ؾ� �ϴ� ��Ȳ : �̴ϸ������ ���¿��� ��Ƽ����Ʈ �Ǿ�����, ���̵�� �巡���� ������ ��
	// �������� �ؾ� �ϴ� ��Ȳ : �ƽø�����, ���̵�� �巡��

	switch (message)
	{
	case WM_SIZE:
	{
		const auto mainWindow = gEngine->GetMainWindow();
		check(mainWindow.get());
	
		const int32 newWidth = LOWORD(lParam);
		const int32 newHeight = LOWORD(lParam);
	
		if (wParam == SIZE_MINIMIZED)
		{
			sbShouldStop = true;
			mainWindow->SetMinimized(true);
		}
		else if (wParam == SIZE_MAXIMIZED)
		{
			mainWindow->OnResized(newWidth, newHeight);
		}
		else if (wParam == SIZE_RESTORED)
		{
			if (mainWindow->IsMinimized())
			{
				sbShouldStop = false;
				mainWindow->SetMinimized(false);
			}
	
			mainWindow->OnResized(newWidth, newHeight);
		}
	}
	break;
	case WM_ENTERSIZEMOVE:
		sbShouldStop = true;
		break;
	case WM_EXITSIZEMOVE:
		sbShouldStop = false;
		break;
		// Check if the window is being destroyed or closed.
	case WM_DESTROY:
		// Intentional fall through...
	case WM_CLOSE:
		PostQuitMessage(0);
		break;
		// All other messages pass to the message handler in the system class.
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return 0;
}
