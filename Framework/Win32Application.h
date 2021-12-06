#pragma once

#include "DXSample.h"

class DXSample;

class Win32Application
{
public:
	static int Run(DXSample* pSample, HINSTANCE hInstance, int nCmdShow);
	static HWND GetHwnd() { return m_hwnd; }

	void CalculateFrameStats(DXSample* pSample);

protected:
	static LRESULT CALLBACK WindowProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam);

private:
	StepTimer m_timer;
	static HWND m_hwnd;
};
