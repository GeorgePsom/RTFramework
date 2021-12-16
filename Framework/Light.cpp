#include "stdafx.h"
#include "stdafx.h"

Light::Light(XMVECTOR& pos, XMVECTOR& col, float strength, Type t) : type(t), color(col), intensity(strength), position(pos)
{
	
}

Light::Light(XMVECTOR& pos, XMVECTOR& dir, XMVECTOR& col, float strength, Type t, float cut, float outer) :
	type(t), color(col), intensity(strength), position(pos), direction(dir), cutOff(cut), outerCutOff(outer)
{
}

Light::~Light()
{
}

