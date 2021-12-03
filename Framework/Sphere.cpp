#include "stdafx.h"

Sphere::Sphere(const XMFLOAT3& c, float radius, Material& material) :
	center(c), r2(sqrt(radius)), Intersectable(material)
{}

Sphere::Sphere() 
	
{}

Sphere::~Sphere(){}

bool Sphere::Intersect(Ray& ray) const
{
	
	float t0, t1;
	XMVECTOR L = XMLoadFloat3(&center) - XMLoadFloat3(&ray.origin);
	float tca = XMVectorGetX( XMVector3Dot(L, XMLoadFloat3(&ray.direction)));
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
	XMStoreFloat3(&surface.position,
		XMLoadFloat3(&ray.origin) + ray.t * XMLoadFloat3(&ray.direction)
	);
	XMStoreFloat3(&surface.normal,
		XMVector3Normalize(
			XMLoadFloat3(&surface.position) - XMLoadFloat3(&center)));
	surface.tex.x = (1 + atan2(surface.normal.z, surface.normal.x) / XM_PI) * 0.5f;
	surface.tex.y = acosf(surface.normal.y) / XM_PI;
}



