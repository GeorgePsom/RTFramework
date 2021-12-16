#include "stdafx.h"


float Ray::EPSILON = 0.1f;
Ray::Ray( XMVECTOR& o,  XMVECTOR& d, float t, int depth, float tmax, float tmin)
{
	tMin = tmin;
	tMax = tmax;
	origin = o;
	direction = d;
	this->t = t;
	this->depth = depth;
	bary = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
}

Ray::Ray()
{
	bary = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);

}

Ray::~Ray()
{
}

void Ray::ReflectRay(Surface& surf)
{
	XMVECTOR D =this->direction;
	XMVECTOR N = surf.normal;
	XMVECTOR R = XMVector3Normalize(D - 2 * XMVectorGetX(XMVector3Dot(D, N)) * N);
	XMVECTOR originV = surf.position + R * EPSILON;
	this->origin =  originV;
	this->direction =  R;
	// to avoid self-intersection due to floating precision error
	t = 0.0f;
	tMin = EPSILON;
	this->depth++;
}

void Ray::RefractRay(Surface& surf, float cosThetaI, float IOR, float k, bool front)
{
	XMVECTOR refractDir = XMVector3Normalize(IOR * direction + surf.normal * (IOR * cosThetaI - sqrt(k)));
	direction =  refractDir;
	
	
	tMin = EPSILON;
	depth++;
}
