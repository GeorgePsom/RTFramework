#include "stdafx.h"






Camera::Camera(XMVECTOR origin, XMVECTOR lookAt, XMVECTOR upVector, float FOV, float aspect, float focal_distance)
{
	this->focal_distance = focal_distance;
	phi = FOV * XM_PI / 180.0f;
	this->FOV = FOV;
	half_height = tan(phi * 0.5f);
	this->aspect = aspect;
	half_width = aspect * half_height;
	XMStoreFloat3(&this->position, origin);
	XMVECTOR viewVector = XMVector3Normalize(origin - lookAt);
	XMStoreFloat3(&this->viewDirection, viewVector);
	right = XMVector3Normalize(XMVector3Cross(viewVector, upVector));
	up = XMVector3Normalize(XMVector3Cross(right, viewVector));
	XMFLOAT3 rightf, upf;
	XMStoreFloat3(&rightf, right);
	XMStoreFloat3(&upf, up);
	XMVECTOR lower_left_corner = origin - half_width * focal_distance * right - half_height * focal_distance * up - focal_distance * viewVector;
	XMStoreFloat3(&this->lower_left_corner, lower_left_corner);
	XMStoreFloat3(&this->horizontal, 2 * half_width * focal_distance * right);
	XMStoreFloat3(&this->vertical, 2 * half_height * focal_distance * up);
	yaw = YAW;
	pitch = PITCH;
}

Camera::~Camera()
{
}

void Camera::MoveCamera(XMVECTOR& moveX, XMVECTOR& moveZ, float dt)
{
	
	XMVECTOR newOrigin = XMLoadFloat3(&this->position) + dt * (moveX + moveZ);
	XMFLOAT3 newOriginF;
	XMStoreFloat3(&newOriginF, newOrigin);
	this->position = newOriginF;
	XMVECTOR lower_left_corner = newOrigin - half_width * focal_distance * right - half_height * focal_distance * up - focal_distance * XMLoadFloat3(&this->viewDirection);
	XMStoreFloat3(&this->lower_left_corner, lower_left_corner);
	XMStoreFloat3(&this->horizontal, 2 * half_width * focal_distance * right);
	XMStoreFloat3(&this->vertical, 2 * half_height * focal_distance * up);

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
	XMFLOAT3 viewVector(
		cos(xR) * cos(yR),
		sin(yR),
		sin(xR) * cos(yR)
	);
	XMStoreFloat3(&viewDirection, XMVector3Normalize(XMLoadFloat3(&viewVector)));
	XMFLOAT3 worldUp(0.0f, 1.0f, 0.0f);
	right = XMVector3Normalize(XMVector3Cross(XMLoadFloat3(&viewDirection),
		XMLoadFloat3(&worldUp) ));

	up = XMVector3Normalize(XMVector3Cross( right, XMLoadFloat3(&viewDirection) ));
	XMFLOAT3 rightf, upf;
	XMStoreFloat3(&rightf, right);
	XMStoreFloat3(&upf, up);
	XMVECTOR lower_left_corner = XMLoadFloat3(&position) - half_width * focal_distance * right - half_height * focal_distance * up - focal_distance * XMLoadFloat3(&viewDirection);
	XMStoreFloat3(&this->lower_left_corner, lower_left_corner);
	XMStoreFloat3(&this->horizontal, 2 * half_width * focal_distance * right);
	XMStoreFloat3(&this->vertical, 2 * half_height * focal_distance * up);

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
	
	
	XMVECTOR lower_left_corner = XMLoadFloat3(&position) - half_width * focal_distance * right - half_height * focal_distance * up - focal_distance * XMLoadFloat3(&viewDirection);
	XMStoreFloat3(&this->lower_left_corner, lower_left_corner);
	XMStoreFloat3(&horizontal, 2 * half_width * focal_distance * right);
	XMStoreFloat3(&vertical, 2 * half_height * focal_distance * up);
}



Ray Camera::GetRayDirection(float u, float v)
{
	XMFLOAT3 direction;
	XMStoreFloat3(&direction,
		XMVector3Normalize(XMLoadFloat3(&lower_left_corner) + u * XMLoadFloat3(&horizontal) + v * XMLoadFloat3(&vertical) -
			XMLoadFloat3(&position)));
	XMFLOAT3 origin = position;
	Ray r(origin, direction, 0.0f);
	return r;


	

}
