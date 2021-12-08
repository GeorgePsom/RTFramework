#pragma once

using namespace DirectX;

class Sphere : public Intersectable
{
public:
	Sphere(const XMFLOAT3& center, float radius, Material& mat);
	~Sphere();
	Sphere();
	bool Intersect(Ray& ray) const override;
	void GetSurfaceData(Surface& surf, Ray& ray) const override;
	
	
	Material mat;
	XMFLOAT3 center;
	float r2;

	

	
	
};

