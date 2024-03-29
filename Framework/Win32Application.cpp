#include "stdafx.h"
#include "Win32Application.h"

HWND Win32Application::m_hwnd = nullptr;

int Win32Application::Run(DXSample* pSample, HINSTANCE hInstance, int nCmdShow)
{
	// Parse command line params
	int argc;
	LPWSTR* argv = CommandLineToArgvW(GetCommandLineW(), &argc);
	pSample->ParseCommandLineArgs(argv, argc);
	LocalFree(argv);

	// Initialize the window class
	WNDCLASSEX windowClass = { 0 };
	windowClass.cbSize = sizeof(WNDCLASSEX);
	windowClass.style = CS_HREDRAW | CS_VREDRAW;
	windowClass.lpfnWndProc = WindowProc;
	windowClass.hInstance = hInstance;
	windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	windowClass.lpszClassName = L"DXSampleClass";
	RegisterClassEx(&windowClass);

	RECT windowRect = { 0, 0, static_cast<LONG>(pSample->GetWidth()), static_cast<LONG>(pSample->GetHeight()) };
	AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, FALSE);

	// Create the window and store a handle to it
	m_hwnd = CreateWindow(
		windowClass.lpszClassName,
		pSample->GetTitle().c_str(),
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		windowRect.right - windowRect.left,
		windowRect.bottom - windowRect.top,
		nullptr,
		nullptr,
		hInstance,
		pSample
	);

	// Initialize the sample. OnInit is defined in each child-implementation of DXSample.
	pSample->OnInit();

	ShowWindow(m_hwnd, nCmdShow);

	// Main sample loop.
	MSG msg = {};
	while (msg.message != WM_QUIT)
	{
		// Process any messages in the queue
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	pSample->OnDestroy();

	// return this part of the WM_QUIT message to windows.
	return static_cast<char>(msg.wParam);

}

void Win32Application::CalculateFrameStats(DXSample* pSample)
{
    static int frameCnt = 0;
    static double prevTime = 0.0f;
    double totalTime = m_timer.GetTotalSeconds();

    frameCnt++;

    // Compute averages over one second period.
    if ((totalTime - prevTime) >= 1.0f)
    {
        float diff = static_cast<float>(totalTime - prevTime);
        float fps = static_cast<float>(frameCnt) / diff; // Normalize to an exact second.

        frameCnt = 0;
        prevTime = totalTime;
        

        std::wstringstream windowText;
        windowText << std::setprecision(2) << std::fixed
            << L"    fps: " << fps;
		std::wstring windowTextWS = pSample->GetTitle() + L": " + windowText.str().c_str();
		SetWindowText(Win32Application::GetHwnd(), windowTextWS.c_str());
    }
}

LRESULT CALLBACK Win32Application::WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	DXSample* pSample = reinterpret_cast<DXSample*> (GetWindowLongPtr(hWnd, GWLP_USERDATA));

	switch (message)
	{
		case WM_CREATE:
		{
			// Save the DXSample* passsed in to CreateWindow
			LPCREATESTRUCT pCreateStruct = reinterpret_cast<LPCREATESTRUCT>(lParam);
			SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR> (pCreateStruct->lpCreateParams));
		}
		return 0;
		
		case WM_KEYDOWN:
			if (pSample)
				pSample->OnKeyDown(static_cast<UINT8>(wParam));
			return 0;
		
		case WM_KEYUP:
			if (pSample)
				pSample->OnKeyUp(static_cast<UINT8>(wParam));
			return 0;

		
		case WM_PAINT:
			if (pSample)
			{
				
				pSample->OnUpdate();
				pSample->OnRender();
			}
			return 0;

		case WM_LBUTTONDOWN:
			if (pSample)
			{
				pSample->OnMouseDown();

			}
			return 0;
		case WM_LBUTTONUP:
			if (pSample)
			{
				pSample->OnMouseUp();

			}
			return 0;

		case WM_MOUSEMOVE:
			if (pSample && static_cast<UINT8>(wParam) == MK_LBUTTON)
			{
				UINT x = LOWORD(lParam);
				UINT y = HIWORD(lParam);
				pSample->OnMouseMove(x, y);
			}
			return 0;

		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;
		
	}



	// Handle any messages the switch statement did not
	return DefWindowProc(hWnd, message, wParam, lParam);
}