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
	XMVECTOR originV = XMLoadFloat3(&surf.position) + R * EPSILON;
	XMStoreFloat3(&this->origin, originV);
	XMStoreFloat3(&this->direction, R);
	// to avoid self-intersection due to floating precision error
	t = 0.0f;
	tMin = EPSILON;
	this->depth++;
}

void Ray::RefractRay(Surface& surf, float cosThetaI, float IOR, float k, bool front)
{
	XMVECTOR refractDir = XMVector3Normalize(IOR * XMLoadFloat3(&direction) + XMLoadFloat3(&surf.normal) * (IOR * cosThetaI - sqrt(k)));
	XMStoreFloat3(&direction, refractDir);
	/*XMVECTOR newOrigin = XMLoadFloat3(&surf.position);
	XMStoreFloat3(&origin, newOrigin);*/
	/*XMVECTOR N = front ? XMLoadFloat3(&surf.normal) : -XMLoadFloat3(&surf.normal);
	XMVECTOR r_out_per = IOR * (XMLoadFloat3(&direction) + cosThetaI * N);
	XMVECTOR r_out_parallel = -sqrtf(fabs(1.0 - XMVectorGetX(XMVector3LengthSq(r_out_per)))) * N;
	XMStoreFloat3(&direction, r_out_per + r_out_parallel);
	XMVECTOR newOrigin = XMLoadFloat3(&surf.position);
	XMStoreFloat3(&origin, newOrigin);*/
	
	tMin = EPSILON;
	depth++;
}
