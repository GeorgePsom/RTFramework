#pragma once

#include "DXSample.h"

using namespace DirectX;

using Microsoft::WRL::ComPtr;

class Camera;

class Game : public DXSample
{
public:

	Game(UINT width, UINT height, std::wstring name);

	virtual void OnInit();
	virtual void OnUpdate();
	virtual void OnRender();
	virtual void OnDestroy();
	virtual void OnKeyDown(UINT8 key);
	virtual void OnKeyUp(UINT8 key);
	virtual void OnMouseMove(UINT x, UINT y);
	virtual void OnMouseUp();
	virtual void OnMouseDown();


private:
	static const UINT FrameCount = 2;
	UINT TextureWidth;
	UINT TextureHeight;
	static const UINT TexturePixelSize = 4;

	struct Vertex
	{
		XMFLOAT3 position;
		XMFLOAT2 uv;
	};

	// Pipeline objects.
	CD3DX12_VIEWPORT m_viewport;
	CD3DX12_RECT m_scissorRect;
	ComPtr<IDXGISwapChain3> m_swapChain;
	ComPtr<ID3D12Device> m_device;
	ComPtr<ID3D12Resource> m_renderTargets[FrameCount];
	ComPtr<ID3D12CommandAllocator> m_commandAllocator;
	ComPtr<ID3D12CommandQueue> m_commandQueue;
	ComPtr<ID3D12RootSignature> m_rootSignature;
	ComPtr<ID3D12DescriptorHeap> m_rtvHeap;
	ComPtr<ID3D12Resource> m_textureUploadHeap;
	ComPtr<ID3D12DescriptorHeap> m_srvHeap;
	ComPtr<ID3D12PipelineState> m_pipelineState;
	ComPtr<ID3D12GraphicsCommandList> m_commandList;
	UINT m_rtvDescriptorSize;

	// App resources
	ComPtr<ID3D12Resource> m_vertexBuffer;
	D3D12_VERTEX_BUFFER_VIEW m_vertexBufferView;
	ComPtr<ID3D12Resource> m_texture;

	Camera* m_camera;
	aiMesh* m_pMesh;
	XMVECTOR m_cameraMovementX, m_cameraMovementZ;
	UINT m_Xprev, m_Yprev;
	float m_AngleX, m_AngleY;
	bool m_firstClick;
	INT m_fovState;
	float m_barrelPower;
	float m_barrelState;
	float m_parallel;
	float m_enableBarrel;
	float m_texturing;
	int m_samples;
	int m_depth;
	StepTimer m_timer;
	float m_prevTime;
	//std::vector<Light> m_lights;
	std::vector<std::shared_ptr<Intersectable>> m_geometry;
	std::vector<std::shared_ptr<Plane>> m_lights;;

	BVH* m_BVH;
	std::vector<UINT8> m_rtOutput;
	std::vector<XMVECTOR> m_prevOutput;
	UINT m_Frames;
	float m_energy;
	bool m_NEE;
	bool m_RR;
	bool m_BRDF;
	bool m_blueNoise;
	bool m_MIS;
	bool m_sampleLights;


	// Synchronization Objects
	UINT m_frameIndex;
	HANDLE m_fenceEvent;
	ComPtr<ID3D12Fence> m_fence;
	UINT64 m_fenceValue;
	XMVECTOR m_prevFrame;
	bool cameraMoved;

	void LoadPipeline();
	void LoadAssets();
	void GenerateTextureData();
	bool Trace(Ray& ray, const Intersectable*& object);
	XMVECTOR ClosestHitShade(Ray& ray, UINT randSeed, bool lastSpecular);
	bool AnyHit(Ray& ray);
	XMVECTOR GetTexture(XMVECTOR& tex);
	void CalculateFrameStats(float energy);
	void PopulateCommandList();
	void WaitForPreviousFrame();


};



