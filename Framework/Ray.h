#pragma once

using namespace DirectX;

class Ray
{
public:
	
	Ray( XMFLOAT3& o,  XMFLOAT3& d, float t);
	Ray();
	~Ray();

public:
	XMFLOAT3 origin;
	XMFLOAT3 direction;
	float t;
};

