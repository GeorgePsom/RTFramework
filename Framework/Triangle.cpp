#include "stdafx.h"

Triangle::Triangle(const Vertex& P0, const Vertex& P1, const Vertex& P2, Material& m, bool light) :
	v0(P0), v1(P1), v2(P2), Intersectable(m, light)
{
}

Triangle::~Triangle()
{
}

Triangle::Triangle()
{
}

bool Triangle::Intersect(Ray& ray)
{
	XMVECTOR v0v1 = v1.position - v0.position;
	XMVECTOR v0v2 = v2.position - v0.position;
	XMVECTOR pvec = XMVector3Cross(ray.direction, v0v2);
	float det = XMVectorGetX(XMVector3Dot(v0v1, pvec));

	if (fabs(det) < 1e-10) return false;

	float I_det = 1 / det;
	XMVECTOR tvec = ray.origin - v0.position;
	float u = XMVectorGetX(XMVector3Dot(pvec, tvec)) * I_det;
	if (u < 0 || u > 1) return false;
	XMVectorSetX(ray.bary, u);

	XMVECTOR qvec = XMVector3Cross(tvec, v0v1);
	float v = XMVectorGetX(XMVector3Dot(ray.direction, qvec)) * I_det;
	if (v < 0 || u + v > 1) return false;
	
	ray.bary = XMVectorSet(u, v, 0.0f, 0.0f);
	ray.t = XMVectorGetX(XMVector3Dot(v0v2, qvec)) * I_det;
	if (ray.t < 0)
		return false;
	return true;
	
}

void Triangle::GetSurfaceData(Surface& surf, Ray& ray) const
{
	surf.position = ray.origin + ray.t * ray.direction;
	float u = XMVectorGetX(ray.bary);
	float v = XMVectorGetY(ray.bary);
	surf.normal = XMVector4Normalize(u * v0.normal + v * v1.normal + (1 - u - v) * v2.normal);
	surf.position = u * v0.position + v * v1.position + (1 - u - v) * v2.position;
	surf.tex = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);

}

void Triangle::CalculateAABB(XMFLOAT3& pMin, XMFLOAT3& pMax) const
{
	XMFLOAT3 v0f, v1f, v2f;
	XMStoreFloat3(&v0f, v0.position);
	XMStoreFloat3(&v1f, v1.position);
	XMStoreFloat3(&v2f, v2.position);

	pMin = {
		min(min(v0f.x, v1f.x), v2f.x),
		min(min(v0f.y, v1f.y), v2f.y),
		min(min(v0f.z, v1f.z), v2f.z)
	};

	pMax = {
		max(max(v0f.x, v1f.x), v2f.x),
		max(max(v0f.y, v1f.y), v2f.y),
		max(max(v0f.z, v1f.z), v2f.z)
	};


}
