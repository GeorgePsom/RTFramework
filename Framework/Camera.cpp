#include "stdafx.h"
//#include "Ray.h"



//Camera::Camera(const XMFLOAT3& position, const XMFLOAT3& viewVector, float FOV, float width, float height)
//{
//	this->position = position;
//	this->viewDirection = viewVector;
//	this->FOV = FOV * XM_PI / 180.0f;
//	this->width = 2;
//	this->height = 2;
//	focal_length = this->width * tan(0.5 * (double)FOV);
//	double f = 2 * atan(36.0 / (2.0 * 50.0));
//	double fdegrees = f * 180.0 / XM_PI;
//	XMVECTOR C = XMLoadFloat3(&this->position) + focal_length * XMLoadFloat3(&this->viewDirection);
//	XMStoreFloat3(&P0, C + XMLoadFloat3(new XMFLOAT3(-1.0f, -1.0f, 0.0f)));
//	XMStoreFloat3(&P1, C + XMLoadFloat3(new XMFLOAT3(1.0f, -1.0f, 0.0f)));
//	XMStoreFloat3(&P2, C + XMLoadFloat3(new XMFLOAT3(-1.0f, 1.0f, 0.0f)));
//
//
//		
//}

Camera::Camera(XMVECTOR origin, XMVECTOR lookAt, XMVECTOR upVector, float FOV, float aspect, float aperture, float focal_distance)
{
	this->focal_distance = focal_distance;
	lens_radius = aperture * 0.5f;
	phi = FOV * XM_PI / 180.0f;
	half_height = tan(phi * 0.5f);
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
