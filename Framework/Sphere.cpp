#include "stdafx.h"

Sphere::Sphere(const XMFLOAT3& c, float radius) :
	center(c), r2(sqrt(radius))
{}

Sphere::~Sphere(){}

bool Sphere::Intersect(Ray& ray)
{
	XMVECTOR origin = XMLoadFloat3(&ray.origin);
	XMVECTOR dir = XMLoadFloat3(&ray.direction);
	XMVECTOR c = XMLoadFloat3(&center);

	XMVECTOR C = c - origin;
	float t = XMVectorGetX(XMVector3Dot(C, dir));
	XMVECTOR Q = C - t * dir;
	float p2 = XMVectorGetX(XMVector3Dot(Q, Q));
	if (p2 > r2) return false;
	t -= sqrt(r2 - p2);
	if ((t < ray.t) && (t > 0))
	{
		ray.t = t;
		return true;
	}



}