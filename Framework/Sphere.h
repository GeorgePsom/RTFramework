#pragma once

using namespace DirectX;

class Sphere
{
public:
	Sphere(const XMFLOAT3& center, float radius);
	~Sphere();
	bool Intersect(Ray& ray);

	XMFLOAT3 center;
	float r2;

};

