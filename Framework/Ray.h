#pragma once

using namespace DirectX;

class Ray
{
public:
	
	Ray( XMFLOAT3& o,  XMFLOAT3& d, float t, int depth = 0);
	Ray();
	~Ray();

	void ReflectRay(Surface& surf);

public:
    static float EPSILON;
	XMFLOAT3 origin;
	XMFLOAT3 direction;
	float t;
	int depth;
};

