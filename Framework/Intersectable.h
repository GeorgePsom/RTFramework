#pragma once
#include "Material.h"



class Intersectable
{
public:
	Intersectable(Material& material) : mat(material) {};
	Intersectable() {};
	virtual bool Intersect(Ray& ray)   = 0;
	/*virtual XMFLOAT3 Shade(Surface& surf)  const = 0;*/
	virtual void GetSurfaceData(Surface& surf, Ray& ray) const = 0;

	Material mat;
};

