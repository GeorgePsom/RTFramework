#pragma once


class Material
{
public :

	enum class Type { DIFFUSE, SPECULAR, DIELECTRIC };
	Material(Type t, XMVECTOR& c) :
		type(t), color(c)
	{};
	Material() : type(Type::DIFFUSE), color(XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f)){}
	Material(Type t, XMVECTOR& c, float i, XMVECTOR& a) : type(t), color(c), IOR(i), I_IOR(1.0f / i), extinction(a) {};
	~Material() {};
	XMVECTOR color;
	float IOR;
	float I_IOR;
	XMVECTOR extinction;
	Type type;

	/*XMFLOAT3 LambertShade(Surface& surf) const
	{
		XMFLOAT3 pointLightf(0.5f, -0.5f, 0.0f);
		XMVECTOR pointLight = XMLoadFloat3(&pointLightf);
		XMFLOAT3 shadedColor;
		XMStoreFloat3(&shadedColor,
			XMLoadFloat3(&color) / XM_PI );
		return shadedColor;
	}*/
};
