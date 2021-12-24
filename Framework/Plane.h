#pragma once

using namespace DirectX;

class Plane : public Intersectable
{

public:
	Plane(XMVECTOR& u, XMVECTOR& v, XMVECTOR& position, bool reverseNormal, Material& mat, bool light);
	~Plane();
	bool Intersect(Ray& ray)  override;
	void GetSurfaceData(Surface& surf, Ray& ray) const override;
	void CalculateAABB(XMFLOAT3& pMin, XMFLOAT3& pMax) const override;
	XMVECTOR SamplePoint(float x1, float x2);
	inline float GetArea() { return XMVectorGetX(XMVector3Length(u))* XMVectorGetX(XMVector3Length(v)); }

	bool isLight;
	XMVECTOR normal;
	bool reverse;
	XMVECTOR position;
	XMVECTOR u, v;
	float u_dot_u, u_dot_v, v_dot_v, det;
	/*XMVECTOR center;
	float xSize, zSize;*/
	Material mat;
	


};