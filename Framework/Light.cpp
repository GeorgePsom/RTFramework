#include "stdafx.h"
#include "stdafx.h"

Light::Light(XMFLOAT3& pos, XMFLOAT3& col, float strength, Type t) : type(t), color(col), intensity(strength), position(pos)
{
	
}

Light::~Light()
{
}

