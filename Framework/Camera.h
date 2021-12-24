#pragma once


using namespace DirectX;
class Ray;

class Camera
{

	
public:
	Camera(XMVECTOR origin, XMVECTOR viewVector, XMVECTOR upVector, float FOV, float aspect, float focal_distance);
	~Camera();
	void MoveCamera(XMVECTOR& moveX, XMVECTOR& moveZ, float dt);
	void RotateCamera(float x, float y);
	void ModifyFOV(float delta, INT increase);
	Ray GetRayDirection(float u, float v);
	
	enum class MOVEMENT
	{
		FORWARD,
		BACKWARD,
		LEFT,
		RIGHT

	};
public:
	XMVECTOR position;
	XMVECTOR viewDirection;
	float focal_length;
	float aspect;
	XMVECTOR lower_left_corner;
	XMVECTOR horizontal, vertical;
	float FOV;
	float width, height;
	XMVECTOR right;
	XMVECTOR up;



private:
	float lens_radius;
	float phi;
	float half_height;
	float half_width;
	
	float focal_distance;
	float yaw;
	float pitch;
	const float SENSITIVITY = 0.1f;
	const float YAW = -90.0f;
	const float PITCH = 0.0f;

	

};
