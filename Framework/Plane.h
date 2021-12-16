#pragma once

using namespace DirectX;

class Plane : public Intersectable
{

public:
	Plane(XMVECTOR& center, XMVECTOR& normal, float xSize, float ySize, Material& mat);
	~Plane();
	bool Intersect(Ray& ray)  override;
	void GetSurfaceData(Surface& surf, Ray& ray) const override;

	XMVECTOR normal;
	XMVECTOR center;
	float xSize, zSize;
	Material mat;
	


};