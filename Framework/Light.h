#pragma once

class Light
{
public:
	enum class Type {POINT, DIRECTIONAL, SPOT};

	Light(XMVECTOR& pos, XMVECTOR& col, float strength, Type type, int object);
	Light(XMVECTOR& pos, XMVECTOR& direction,  XMVECTOR& col, float strength, Type type, float cutOff,float  outerCutOff);
	~Light();
	inline float Light::Attenuate(XMVECTOR& pos)
	{
		
		float length = XMVectorGetX(XMVector3Length(pos - position));
		float length2 = length * length;
		float attenuation = 1.0f / (max(0.001f,  length2)); // not to oversaturate objects that are too close to the light source
		return attenuation;

	}

	inline float CosineAttenuation(XMVECTOR& pos, XMVECTOR& normal)
	{
		

		return max(0, XMVectorGetX(XMVector3Dot(normal,
			XMVector3Normalize(position - pos))));
	}

	inline float SpotAttenuation(XMVECTOR& pos)
	{
		XMVECTOR lightDir = XMVector3Normalize(position -pos);
		float theta = XMVectorGetX(XMVector3Dot(lightDir, -direction));
		float diff = max(0.0001f, cutOff - outerCutOff);
		return  std::clamp(((theta - outerCutOff) / diff), 0.0f, 1.0f);
	}

	Type type;
	float intensity;
	XMVECTOR position;
	XMVECTOR direction;
	int objectIndex;
	XMVECTOR color;
	float cutOff; // in radians
	float outerCutOff;

};
