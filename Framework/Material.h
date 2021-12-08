#pragma once


class Material
{
public :

	enum class Type { DIFFUSE, SPECULAR, DIELECTRIC };
	Material(Type t, XMFLOAT3& c) :
		type(t), color(c)
	{};
	Material() : type(Type::DIFFUSE), color(XMFLOAT3(1.0f, 1.0f, 1.0f)){}
	Material(Type t, XMFLOAT3& c, float i, XMFLOAT3& a) : type(t), color(c), IOR(i), I_IOR(1.0f / i), extinction(a) {};
	~Material() {};
	XMFLOAT3 color;
	float IOR;
	float I_IOR;
	XMFLOAT3 extinction;
	Type type;

	XMFLOAT3 LambertShade(Surface& surf) const
	{
		XMFLOAT3 pointLightf(0.5f, -0.5f, 0.0f);
		XMVECTOR pointLight = XMLoadFloat3(&pointLightf);
		XMFLOAT3 shadedColor;
		XMStoreFloat3(&shadedColor,
			XMLoadFloat3(&color) / XM_PI );
		return shadedColor;
	}
};
