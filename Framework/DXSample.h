#pragma once

#include "DXSampleHelper.h"
#include "Win32Application.h"

class DXSample
{
public:

	DXSample(UINT width, UINT height, std::wstring name);
	virtual ~DXSample();

	virtual void OnInit() = 0;
	virtual void OnUpdate() = 0;
	virtual void OnRender() = 0;
	virtual void OnDestroy() = 0;

	virtual void OnKeyDown(UINT8) {}
	virtual void OnKeyUp(UINT8) {}
	virtual void OnMouseMove(UINT x, UINT y){}
	virtual void OnMouseUp() {}
	virtual void OnMouseDown() {}

	UINT GetWidth() const {return m_width;}
	UINT GetHeight() const{ return m_height; }
	const std::wstring GetTitle() const { return m_title; }

	void ParseCommandLineArgs(_In_reads_(argc) WCHAR* argv[], int argc);

protected:
	std::wstring GetAssetFullPath(LPCWSTR assetName);
	
	void GetHardwareAdapter(
		_In_ IDXGIFactory1* pFactory,
		_Outptr_result_maybenull_ IDXGIAdapter1** ppAdapter,
		bool requestHighPerformance = false);

	void SetCustomWindowText(LPCWSTR text);

	// Viewport Dimensions
	UINT m_width;
	UINT m_height;
	float m_aspectRatio;

	// Adapter Info
	bool m_useWarpDevice;
private:
	//Root assets path
	std::wstring m_assetsPath;

	// Window Title
	std::wstring m_title;
};
