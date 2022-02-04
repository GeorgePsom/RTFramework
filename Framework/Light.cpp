#include "stdafx.h"
#include "stdafx.h"

Light::Light(XMVECTOR& pos, XMVECTOR& col, float strength, Type t, int objectId) : type(t), color(col), intensity(strength), position(pos), objectIndex(objectId)
{
	
}

Light::Light(XMVECTOR& pos, XMVECTOR& dir, XMVECTOR& col, float strength, Type t, float cut, float outer) :
	type(t), color(col), intensity(strength), position(pos), direction(dir), cutOff(cut), outerCutOff(outer)
{
}

Light::~Light()
{
}

