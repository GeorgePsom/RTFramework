#pragma once


using namespace DirectX;

class Camera
{
public:
	Camera(const XMFLOAT3& position, const XMFLOAT3& view, float FOV, float width, float height);
	~Camera();
	

public:
	XMFLOAT3 position;
	XMFLOAT3 viewDirection;
	float focal_length;
	XMFLOAT3 P0, P1, P2;
	float FOV;
	float width, height;
};
