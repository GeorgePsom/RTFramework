#pragma once

using namespace DirectX;

class Sphere : public Intersectable
{
public:
	Sphere(const XMVECTOR& center, float radius, Material& mat);
	~Sphere();
	Sphere();
	bool Intersect(Ray& ray)  override;
	void GetSurfaceData(Surface& surf, Ray& ray) const override;
	void CalculateAABB(XMFLOAT3& pMin, XMFLOAT3& pMax) const override;
	
	
	Material mat;
	XMVECTOR center;
	float r2;

	

	
	
};

