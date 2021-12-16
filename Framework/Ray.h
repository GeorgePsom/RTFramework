#pragma once

using namespace DirectX;

class Ray
{
public:
	
	Ray(XMVECTOR& o,  XMVECTOR& d, float t, int depth = 0, float tMax = 1000000.0, float tMin = EPSILON);
	Ray();
	~Ray();

	void ReflectRay(Surface& surf);
	void RefractRay(Surface& surf, float cosThetaI, float IOR, float k, bool front);

public:
    static float EPSILON;
	XMVECTOR origin;
	XMVECTOR direction;
	float t;
	float tMax;
	float tMin;
	int depth;
};

