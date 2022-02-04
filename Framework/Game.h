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
	struct Path
	{
		enum Type {CameraType, LightType, SurfaceType};
		enum DirectionType{CameraDir, LightDir};
		XMFLOAT3 beta;
		XMFLOAT3 pos;
		XMFLOAT3 normal;
		XMFLOAT3 dir;
		int objIntex;
		int lightIndex;
		const Intersectable* object = nullptr;
		float pdfFwd = 0.0f, pdfPrev = 0.0f;
		Type type;
		bool delta = false;
		DirectionType dirType;
		Path() {};
		Path(Type t, XMFLOAT3 b, XMFLOAT3 p, XMFLOAT3 n, XMFLOAT3 d) :
			type(t), beta(b), pos(p), normal(n), dir(d){}
		Path(Type t, XMFLOAT3 b, XMFLOAT3 p) :
			type(t), beta(b), pos(p) {}

		float ConvertDensity(float pdf, const Path& next)
		{
			/*if(next.object != nullptr)
				if (next.object->isLight)
				return pdf;*/
			XMVECTOR  w = XMLoadFloat3(&next.pos) - XMLoadFloat3(&pos);
			float lengthSq = XMVectorGetX(XMVector3LengthSq(w));
			if (lengthSq <= 0.0f)
				return 0.0f;
			float invDist2 = 1 / lengthSq;
			if (next.type == Path::Type::SurfaceType)
				pdf *= max(0.0f, XMVectorGetX(XMVector3Dot(XMLoadFloat3(&next.normal), w * sqrt(invDist2))));
			return pdf * invDist2;

				
		}
		static Path CreateCameraPath(const Ray ray, const XMFLOAT3 beta)
		{
			XMFLOAT3 p, d;
			XMStoreFloat3(&p, ray.origin);
			XMStoreFloat3(&d, ray.direction);
			return Path(Type::CameraType, beta, p, XMFLOAT3(0.0f, 0.0f, 1.0f), d);
		}
		static Path CreateLightPath(Ray& ray, Surface surf, const XMFLOAT3 beta, float pdf, Path prev)
		{
			XMFLOAT3 p, n, d, b;
			XMStoreFloat3(&p, surf.position);
			XMStoreFloat3(&n, surf.normal);
			XMStoreFloat3(&d, ray.direction);
			
			Path path(Type::LightType, beta, p, n, d);
			path.pdfFwd = prev.ConvertDensity(pdf, path);
			return path;
		}

		static Path CreateLightPath(Ray& ray, Surface surf, const XMFLOAT3 beta, float pdf)
		{
			XMFLOAT3 p, n, d, b;
			XMStoreFloat3(&p, surf.position);
			XMStoreFloat3(&n, surf.normal);
			XMStoreFloat3(&d, ray.direction);

			Path path(Type::LightType, beta, p, n, d);
			path.pdfFwd = pdf;
			return path;
		}
		static Path CreateSurfacePath(Ray& ray, Surface surf, XMFLOAT3 beta, Path& prev, float pdf)
		{
			XMFLOAT3 p, n, d, b;
			XMStoreFloat3(&p, surf.position);
			XMStoreFloat3(&n, surf.normal);
			XMStoreFloat3(&d, ray.direction);
			Path path(Type::SurfaceType, beta, p, n, d);
			//path.pdfFwd = pdf;
			path.pdfFwd = prev.ConvertDensity(pdf, path);

			return path;
		}
		float Pdf(Path* prev, Path& next)
		{
			if (type == LightType)
				return PdfLight(next);
			XMVECTOR wp;
			XMVECTOR wn = XMVector3Normalize(XMLoadFloat3(&next.pos) - XMLoadFloat3(&pos));
			if (prev)
				wp = XMVector3Normalize(XMLoadFloat3(&prev->pos) - XMLoadFloat3(&pos));
			float pdf;
			if (type == CameraType)
				pdf = 1.0f;
			else
				if (object->mat.type == Material::Type::DIFFUSE)
					pdf = XMVectorGetX(XMVector3Dot(wp, wn)) >= 0.0f ? max(0.0f, XMVectorGetX(XMVector3Dot(wn, XMLoadFloat3(&normal)))) * XM_1DIVPI : 0.0f;
				else if (object->mat.type == Material::Type::SPECULAR)
					pdf =1.0f;
					//pdf = XMVectorGetX(XMVector3Dot(wp, wn)) >= 0.0f ? max(0.0f, XMVectorGetX(XMVector3Dot(wn, XMLoadFloat3(&normal)))) * XM_1DIVPI : 0.0f;
				else if (object->mat.type == Material::Type::DIELECTRIC)
					pdf = 1.0f;
					//pdf = XMVectorGetX(XMVector3Dot(wp, wn)) >= 0.0f ? max(0.0f, XMVectorGetX(XMVector3Dot(wn, XMLoadFloat3(&normal)))) * XM_1DIVPI : 0.0f;

			return ConvertDensity(pdf, next);
		}

		float PdfLight(Path& next)
		{
			XMVECTOR w = XMLoadFloat3(&next.pos) - XMLoadFloat3(&pos);
			float invDistSq = 1 / XMVectorGetX(XMVector3LengthSq(w));
			w *= sqrt(invDistSq);
			float pdf = max(0.0f, XMVectorGetX(XMVector3Dot(XMLoadFloat3(&normal), w))) * XM_1DIVPI;
			pdf *= invDistSq;
			if (next.type == SurfaceType)
				pdf *= max(0.0f, XMVectorGetX(XMVector3Dot(XMLoadFloat3(&next.normal), w)));
			return pdf;
			
		}
	};

	template <typename Type>
	class ScopedAssignment {
	public:
		
		ScopedAssignment(Type* target = nullptr, Type value = Type())
			: target(target) {
			if (target) {
				backup = *target;
				*target = value;
			}
		}
		~ScopedAssignment() {
			if (target) *target = backup;
		}
		ScopedAssignment(const ScopedAssignment&) = delete;
		ScopedAssignment& operator=(const ScopedAssignment&) = delete;
		ScopedAssignment& operator=(ScopedAssignment&& other) {
			if (target) *target = backup;
			target = other.target;
			backup = other.backup;
			other.target = nullptr;
			return *this;
		}

	private:
		Type* target, backup;
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
	std::vector<std::shared_ptr<Plane>> m_lights;

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
	bool m_BPT;


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
	bool AnyHit(Ray& ray, const Intersectable* avoidObj, bool ignoreLights);
	bool AnyHit(Ray& ray, int avoidLight, bool ignoreLights);
	XMVECTOR GetTexture(XMVECTOR& tex);
	void CalculateFrameStats(float energy);
	void PopulateCommandList();
	void WaitForPreviousFrame();
	int CreateCameraPaths(Ray& ray, std::vector<Path>& cameraPaths, UINT& randSeed);
	int CreateLightPaths(Ray& ray, std::vector<Path>& lightPaths, UINT& randSeed);
	
	int PathTrace(Ray& ray, XMFLOAT3 beta, float pdf, int depth, Path::DirectionType type, std::vector<Path>& paths, UINT& randSeed);
	XMVECTOR ConnectPaths(std::vector<Path>& cameraPaths, std::vector<Path>& lightPaths, int nCameraPaths, int nLightPaths, UINT randSeed, int& nPaths);
	float MIS(std::vector<Path>& cameraPaths, std::vector<Path>& lightPaths, int nCameraPaths, int nLightPaths, Path* newLightPath);

};



