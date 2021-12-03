#pragma once


struct Surface
{
	XMFLOAT3 position;
	XMFLOAT3 normal;
	XMFLOAT2 tex;
};

class Material
{
public :

	enum class Type { DIFFUSE, SPECULAR};
	Material(Type t, XMFLOAT3 c) :
		type(t), color(c)
	{};
	Material() : type(Type::DIFFUSE), color(XMFLOAT3(1.0f, 0.0f, 0.0f)){}
	~Material() {};
	XMFLOAT3 color;
	Type type;

	XMFLOAT3 Shade(Surface& surf, Ray& ray) const
	{
		XMFLOAT3 pointLightf(0.5f, -0.5f, 0.0f);
		XMVECTOR pointLight = XMLoadFloat3(&pointLightf);
		XMFLOAT3 shadedColor;
		XMStoreFloat3(&shadedColor,
			XMLoadFloat3(&color) / XM_PI * max(0.0f, XMVectorGetX(XMVector3Dot(XMLoadFloat3(&surf.normal), pointLight))));
		return shadedColor;
	}
};
