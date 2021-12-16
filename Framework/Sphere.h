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
	
	
	Material mat;
	XMVECTOR center;
	float r2;

	

	
	
};

