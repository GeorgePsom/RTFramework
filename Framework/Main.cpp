#include "stdafx.h"
#include "Game.h"

_Use_decl_annotations_
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow)
{
	Game sample(1280, 720, L"HELLO");
	return Win32Application::Run(&sample, hInstance, nCmdShow);

}