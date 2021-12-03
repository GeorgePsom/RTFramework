#include "stdafx.h"

Ray::Ray( XMFLOAT3& o,  XMFLOAT3& d, float t)
{
	origin = o;
	direction = d;
	this->t = t;
}

Ray::Ray()
{
}

Ray::~Ray()
{
}
