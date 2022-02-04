#pragma once
#include "Material.h"



class Intersectable
{
public:
	Intersectable(Material& material, bool light, int lIndex = -1) : mat(material), isLight(light), lightIndex(lIndex)  {};
	Intersectable() {};
	virtual bool Intersect(Ray& ray)   = 0;
	virtual void GetSurfaceData(Surface& surf, Ray& ray) const = 0;
	virtual void CalculateAABB(XMFLOAT3& pMin, XMFLOAT3& pMax) const = 0;

	Material mat;
	bool isLight;
	int lightIndex = -1;
};

