#pragma once
#include "Material.h"



class Intersectable
{
public:
	Intersectable(Material& material, bool light) : mat(material), isLight(light)  {};
	Intersectable() {};
	virtual bool Intersect(Ray& ray)   = 0;
	virtual void GetSurfaceData(Surface& surf, Ray& ray) const = 0;
	virtual void CalculateAABB(XMFLOAT3& pMin, XMFLOAT3& pMax) const = 0;

	Material mat;
	bool isLight;
};

