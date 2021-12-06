#pragma once

class Light
{
public:
	enum class Type {POINT, DIRECTIONAL, SPOT};

	Light(XMFLOAT3& pos, XMFLOAT3& col, float strength, Type type);
	~Light();
	inline float Light::Attenuate(XMFLOAT3& pos)
	{
		XMVECTOR posV = XMLoadFloat3(&pos);
		XMVECTOR lPosV = XMLoadFloat3(&position);
		float length = XMVectorGetX(XMVector3Length(posV - lPosV));
		float length2 = length * length;
		float attenuation = 1.0f / (max(0.0001f,  length2)); // not to oversaturate objects that are too close to the light source
		return attenuation;

	}

	inline float CosineAttenuation(XMFLOAT3& pos, XMFLOAT3& normal)
	{
		XMVECTOR N = XMLoadFloat3(&normal);
		XMVECTOR P = XMLoadFloat3(&pos);

		return max(0, XMVectorGetX(XMVector3Dot(N,
			XMVector3Normalize(XMLoadFloat3(&position) - P))));
	}

	Type type;
	float intensity;
	XMFLOAT3 position;
	XMFLOAT3 direction;
	XMFLOAT3 color;

};
