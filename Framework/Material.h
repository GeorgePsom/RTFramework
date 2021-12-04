#pragma once


class Material
{
public :

	enum class Type { LAMBERTIAN, SPECULAR, DIELECTRICS_TRANSPARENT, DIELECTRICS_SOLID };
	Material(Type t, XMFLOAT3 c) :
		type(t), color(c)
	{};
	Material() : type(Type::LAMBERTIAN), color(XMFLOAT3(1.0f, 0.0f, 0.0f)){}
	~Material() {};
	XMFLOAT3 color;
	Type type;

	XMFLOAT3 LambertShade(Surface& surf) const
	{
		XMFLOAT3 pointLightf(0.5f, -0.5f, 0.0f);
		XMVECTOR pointLight = XMLoadFloat3(&pointLightf);
		XMFLOAT3 shadedColor;
		XMStoreFloat3(&shadedColor,
			XMLoadFloat3(&color) / XM_PI /** max(0.0f, XMVectorGetX(XMVector3Dot(XMLoadFloat3(&surf.normal), pointLight)))*/);
		return shadedColor;
	}
};
