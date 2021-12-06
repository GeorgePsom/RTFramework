#include "stdafx.h"


float Ray::EPSILON = 0.1f;
Ray::Ray( XMFLOAT3& o,  XMFLOAT3& d, float t, int depth, float tmax, float tmin)
{
	tMin = tmin;
	tMax = tmax;
	origin = o;
	direction = d;
	this->t = t;
	this->depth = depth;
}

Ray::Ray()
{
}

Ray::~Ray()
{
}

void Ray::ReflectRay(Surface& surf)
{
	XMVECTOR D = XMLoadFloat3(&this->direction);
	XMVECTOR N = XMLoadFloat3(&surf.normal);
	XMVECTOR R = XMVector3Normalize(D - 2 * XMVectorGetX(XMVector3Dot(D, N)) * N);
	/*R = XMVector3Reflect(-D, N);*/
	XMStoreFloat3(&this->origin, XMLoadFloat3(&surf.position));
	XMStoreFloat3(&this->direction, R);
	this->t = EPSILON; // to avoid self-intersection due to floating precision error
	tMin = EPSILON;
	this->depth++;
}

void Ray::RefractRay(Surface& surf, float cosThetaI, float IOR, float k)
{
	XMVECTOR refractDir = XMVector3Normalize(IOR * XMLoadFloat3(&direction) + XMLoadFloat3(&surf.normal) * (IOR * cosThetaI - sqrt(k)));
	XMStoreFloat3(&direction, refractDir);
	XMVECTOR newOrigin = XMLoadFloat3(&surf.position);
	XMStoreFloat3(&origin, newOrigin);
	t = EPSILON;
	tMin = EPSILON;
	depth++;
}
