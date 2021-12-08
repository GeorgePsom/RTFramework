#pragma once

class Light
{
public:
	enum class Type {POINT, DIRECTIONAL, SPOT};

	Light(XMFLOAT3& pos, XMFLOAT3& col, float strength, Type type);
	Light(XMFLOAT3& pos, XMFLOAT3& direction,  XMFLOAT3& col, float strength, Type type, float cutOff,float  outerCutOff);
	~Light();
	inline float Light::Attenuate(XMFLOAT3& pos)
	{
		XMVECTOR posV = XMLoadFloat3(&pos);
		XMVECTOR lPosV = XMLoadFloat3(&position);
		float length = XMVectorGetX(XMVector3Length(posV - lPosV));
		float length2 = length * length;
		float attenuation = 1.0f / (max(0.001f,  length2)); // not to oversaturate objects that are too close to the light source
		return attenuation;

	}

	inline float CosineAttenuation(XMFLOAT3& pos, XMFLOAT3& normal)
	{
		XMVECTOR N = XMLoadFloat3(&normal);
		XMVECTOR P = XMLoadFloat3(&pos);

		return max(0, XMVectorGetX(XMVector3Dot(N,
			XMVector3Normalize(XMLoadFloat3(&position) - P))));
	}

	inline float SpotAttenuation(XMFLOAT3& pos)
	{
		XMVECTOR lightDir = XMVector3Normalize(XMLoadFloat3(&position) - XMLoadFloat3(&pos));
		float theta = XMVectorGetX(XMVector3Dot(lightDir, -XMLoadFloat3(&direction)));
		float diff = max(0.0001f, cutOff - outerCutOff);
		return  std::clamp(((theta - outerCutOff) / diff), 0.0f, 1.0f);
	}

	Type type;
	float intensity;
	XMFLOAT3 position;
	XMFLOAT3 direction;
	XMFLOAT3 color;
	float cutOff; // in radians
	float outerCutOff;

};
