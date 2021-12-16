#include "stdafx.h"

Triangle::Triangle(const Vertex& P0, const Vertex& P1, const Vertex& P2, Material& m) :
	v0(P0), v1(P1), v2(P2), Intersectable(m)
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
	/*XMVECTOR v0v1 = v1.position - v0.position;
	XMVECTOR v0v2 = v2.position - v0.position;
	XMVECTOR N = XMVector3Cross(v0v1, v0v2);
	float denom = XMVectorGetX(XMVector3Dot(N, N));

	float NdotRayDir = XMVectorGetX(XMVector3Dot(N, ray.direction));
	if (fabs(NdotRayDir) < Ray::EPSILON)
		return false;

	float d = XMVectorGetX(XMVector3Dot(N, v0.position));
	ray.t = (XMVectorGetX(XMVector3Dot(N, ray.origin)) + d) / NdotRayDir;

	if (ray.t < 0)
		return false;

	XMVECTOR P = ray.origin + ray.t * ray.direction;


	XMVECTOR edge0 = v1.position - v0.position;
	XMVECTOR vp0 = P - v0.position;
	XMVECTOR C = XMVector3Cross(edge0, vp0);

	if (XMVectorGetX(XMVector3Dot(N, C)) < 0)
		return false;

	XMVECTOR edge1 = v2.position - v1.position;
	XMVECTOR vp1 = P - v1.position;
	C = XMVector3Cross(edge1, vp1);
	float u = XMVectorGetX(XMVector3Dot(N, C));
	if ( u < 0)
		return false;

	XMVECTOR edge2 = v0.position - v2.position;
	XMVECTOR vp2 = P - v2.position;
	C = XMVector3Cross(edge2, vp2);
	float v = XMVectorGetX(XMVector3Dot(N, C));
	if (u < 0)
		return false;

	u /= denom;
	v /= denom;
	ray.bary = XMVectorSet(u, v, 0.0f, 0.0f);
	return true;*/

	




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
