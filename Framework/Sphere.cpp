#include "stdafx.h"

Sphere::Sphere(const XMVECTOR& c, float radius, Material& material) :
	center(c), r2(sqrt(radius)), Intersectable(material)
{}

Sphere::Sphere() 
	
{}

Sphere::~Sphere(){}

bool Sphere::Intersect(Ray& ray) 
{
	
	float t0, t1;
	XMVECTOR L = center - ray.origin;
	float tca = XMVectorGetX( XMVector3Dot(L, ray.direction));
	float d2 = XMVectorGetX(XMVector3Dot(L, L)) - tca * tca;
	if (d2 > r2) return false;
	float thc = sqrt(r2 - d2);
	t0 = tca - thc;
	t1 = tca + thc;

	if (t0 > t1) std::swap(t0, t1);
	if (t0 < 0)
	{
		t0 = t1;
		if (t0 < 0) return false;
	}

	ray.t = t0;
	return true;

}



void Sphere::GetSurfaceData(Surface& surface, Ray& ray) const
{
	surface.position = ray.origin + ray.t * ray.direction;
	surface.normal = XMVector3Normalize(surface.position - center);
	surface.tex = XMVectorSet((1 + atan2(XMVectorGetZ(surface.normal), XMVectorGetX(surface.normal)) / XM_PI) * 0.5f, acosf(XMVectorGetY(surface.normal)) / XM_PI, 0.0f, 0.0f);
}

void Sphere::CalculateAABB(XMFLOAT3& pMin, XMFLOAT3& pMax) const
{
	
}



