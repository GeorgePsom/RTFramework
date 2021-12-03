#pragma once


using namespace DirectX;
class Ray;

class Camera
{
public:
	Camera(XMVECTOR origin, XMVECTOR viewVector, XMVECTOR upVector, float FOV, float aspect, float aperture, float focal_distance);
	~Camera();
	Ray GetRayDirection(float u, float v);
	

public:
	XMFLOAT3 position;
	XMFLOAT3 viewDirection;
	float focal_length;
	XMFLOAT3 lower_left_corner;
	XMFLOAT3 horizontal, vertical;
	float FOV;
	float width, height;
};
