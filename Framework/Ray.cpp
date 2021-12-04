#include "stdafx.h"


float Ray::EPSILON = 1.f;
Ray::Ray( XMFLOAT3& o,  XMFLOAT3& d, float t, int depth)
{
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
	XMStoreFloat3(&this->direction, R);
	this->origin = surf.position;
	this->t = EPSILON; // to avoid self-intersection due to floating precision error
	this->depth++;
}
