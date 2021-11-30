#pragma once

using namespace DirectX;

class Ray
{
public:
	
	Ray(const XMFLOAT3& o, const XMFLOAT3& d, float t);
	~Ray();

public:
	XMFLOAT3 origin;
	XMFLOAT3 direction;
	float t;
};

