#include "stdafx.h"



Camera::Camera(const XMFLOAT3& position, const XMFLOAT3& viewVector, float FOV, float width, float height)
{
	this->position = position;
	this->viewDirection = viewVector;
	this->FOV = FOV * XM_PI / 180.0f;
	this->width = width;
	this->height = height;
	focal_length = 0.5f * width * 0.5f * tan(0.5f * this->FOV);
	XMVECTOR C = XMLoadFloat3(&this->position) + focal_length * XMLoadFloat3(&this->viewDirection);
	XMStoreFloat3(&P0, C + XMLoadFloat3(new XMFLOAT3(-1.0f, -1.0f, 0.0f)));
	XMStoreFloat3(&P1, C + XMLoadFloat3(new XMFLOAT3(1.0f, -1.0f, 0.0f)));
	XMStoreFloat3(&P2, C + XMLoadFloat3(new XMFLOAT3(-1.0f, 1.0f, 0.0f)));


		
}

Camera::~Camera()
{
}
