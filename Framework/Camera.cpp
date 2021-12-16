#include "stdafx.h"






Camera::Camera(XMVECTOR origin, XMVECTOR lookAt, XMVECTOR upVector, float FOV, float aspect, float focal_distance)
{
	this->focal_distance = focal_distance;
	phi = FOV * XM_PI / 180.0f;
	this->FOV = FOV;
	half_height = tan(phi * 0.5f);
	this->aspect = aspect;
	half_width = aspect * half_height;
	this->position =  origin;
	XMVECTOR viewVector = XMVector3Normalize(origin - lookAt);
	this->viewDirection = viewVector;
	right = XMVector3Normalize(XMVector3Cross(viewVector, upVector));
	up = XMVector3Normalize(XMVector3Cross(right, viewVector));
	
	XMVECTOR lower_left_corner = origin - half_width * focal_distance * right - half_height * focal_distance * up - focal_distance * viewVector;
	this->lower_left_corner, lower_left_corner;
	this->horizontal = 2 * half_width * focal_distance * right;
	this->vertical = 2 * half_height * focal_distance * up;
	yaw = YAW;
	pitch = PITCH;
}

Camera::~Camera()
{
}

void Camera::MoveCamera(XMVECTOR& moveX, XMVECTOR& moveZ, float dt)
{
	
	position = this->position + dt * (moveX + moveZ);
	
	XMVECTOR lower_left_corner = position - half_width * focal_distance * right - half_height * focal_distance * up - focal_distance * this->viewDirection;
	this->lower_left_corner = lower_left_corner;
	this->horizontal = 2 * half_width * focal_distance * right;
	this->vertical = 2 * half_height * focal_distance * up;

}

void Camera::RotateCamera(float x, float y)
{
	x *= SENSITIVITY;
	y *= SENSITIVITY;

	yaw += x;
	pitch += y;
	if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;
	float xR = XMConvertToRadians(yaw);
	float yR = XMConvertToRadians(pitch);
	viewDirection = XMVectorSet(cos(xR) * cos(yR),
		sin(yR),
		sin(xR) * cos(yR), 0.0f);
	
	
	XMVECTOR worldUp = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	right = XMVector3Normalize(XMVector3Cross(viewDirection,
		worldUp));

	up = XMVector3Normalize(XMVector3Cross( right, viewDirection));
	
	XMVECTOR lower_left_corner = position - half_width * focal_distance * right - half_height * focal_distance * up - focal_distance * viewDirection;
	this->lower_left_corner = lower_left_corner;
	this->horizontal = 2 * half_width * focal_distance * right;
	this->vertical = 2 * half_height * focal_distance * up;

}

void Camera::ModifyFOV(float delta, INT increase)
{
	FOV = FOV + (float)increase * delta * 20.0f;
	FOV = FOV >= 179.9 ? 179.9 : FOV;
	FOV = FOV <= 0.1 ? 0.1 : FOV;
	phi = FOV * XM_PI / 180.0f;
	this->FOV = FOV;
	half_height = tan(phi * 0.5f);
	half_width = aspect * half_height;
	
	
	XMVECTOR lower_left_corner = position - half_width * focal_distance * right - half_height * focal_distance * up - focal_distance * viewDirection;
	this->lower_left_corner =  lower_left_corner;
	horizontal =  2 * half_width * focal_distance * right;
	vertical = 2 * half_height * focal_distance * up;
}



Ray Camera::GetRayDirection(float u, float v)
{
	XMVECTOR direction =
		XMVector3Normalize(lower_left_corner + u * horizontal + v * vertical - 	position);
	
	Ray r(position, direction, 0.0f);
	return r;


	

}
