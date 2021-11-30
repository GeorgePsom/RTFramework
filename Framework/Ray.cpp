#include "stdafx.h"

Ray::Ray(const XMFLOAT3& o, const XMFLOAT3& d, float t)
{
	origin = o;
	direction = d;
	this->t = t;
}

Ray::~Ray()
{
}
