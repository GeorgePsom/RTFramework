#pragma once

using namespace DirectX;

class Ray
{
public:
	
	Ray( XMFLOAT3& o,  XMFLOAT3& d, float t, int depth = 0, float tMax = 1000000.0, float tMin = EPSILON);
	Ray();
	~Ray();

	void ReflectRay(Surface& surf);
	void RefractRay(Surface& surf, float cosThetaI, float IOR, float k);

public:
    static float EPSILON;
	XMFLOAT3 origin;
	XMFLOAT3 direction;
	float t;
	float tMax;
	float tMin;
	int depth;
};

