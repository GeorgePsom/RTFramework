#pragma once

using namespace DirectX;

class Plane : public Intersectable
{

public:
	Plane(XMFLOAT3& center, XMFLOAT3& normal, float xSize, float ySize, Material& mat);
	~Plane();
	bool Intersect(Ray& ray)  override;
	void GetSurfaceData(Surface& surf, Ray& ray) const override;

	XMFLOAT3 normal;
	XMFLOAT3 center;
	float xSize, zSize;
	Material mat;
	


};