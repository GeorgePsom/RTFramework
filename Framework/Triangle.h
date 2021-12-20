#pragma once

using namespace DirectX;

struct Vertex
{
	XMVECTOR position;
	XMVECTOR normal;
};

class Triangle : public Intersectable
{
public:

	Triangle(const Vertex& p0, const Vertex& p1, const Vertex& p2, Material& mat);
	~Triangle();
	Triangle();
	bool Intersect(Ray& ray) override;
	void GetSurfaceData(Surface& surf, Ray& ray) const override;
	void CalculateAABB(XMFLOAT3& pMin, XMFLOAT3& pMax) const override;

	Material mat;
	Vertex v0, v1, v2;

	

		
};
