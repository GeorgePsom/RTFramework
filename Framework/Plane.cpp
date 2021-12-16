#include "stdafx.h"

Plane::Plane(XMVECTOR& c, XMVECTOR& n, float x, float z, Material& m) : center(c), xSize(x), zSize(z), mat(m)
{
    normal = XMVector3Normalize(n);
}

Plane::~Plane()
{
}

bool Plane::Intersect(Ray& ray) 
{
    
    float t;
    float NdotD = XMVectorGetX(XMVector3Dot(ray.direction, normal));
    if (fabs(NdotD) > 1e-6) {
        XMVECTOR centerOrigin = center - ray.origin;
        t = XMVectorGetX(XMVector3Dot(centerOrigin, normal)) / NdotD;
        XMVECTOR p = ray.origin + t * ray.direction;
        // Calculate tangent binormal
        XMVECTOR tang = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
        XMVECTOR binormal = XMVector3Normalize(XMVector3Cross(normal, tang));
        XMVECTOR tangent = XMVector3Normalize(XMVector3Cross(binormal,normal));
        XMVECTOR pCenter = center - p;
        float x = XMVectorGetX(XMVector3Dot(pCenter, tangent));
        float z = XMVectorGetX(XMVector3Dot(pCenter, binormal));
        if (t >= 0 && fabs(x) <= xSize/2.0f && fabs(z) <= zSize/2.0f)
        {
            ray.t = t;
            return true;
        }
    }

    return false;
}

void Plane::GetSurfaceData(Surface& surf, Ray& ray) const
{
    surf.position = ray.origin + ray.t * ray.direction;
    surf.normal = normal;
    XMVECTOR tang = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
    XMVECTOR binormal = XMVector3Normalize(XMVector3Cross(normal, tang));
    XMVECTOR tangent = XMVector3Normalize(XMVector3Cross(binormal, normal));
    XMVECTOR pCenter = center - surf.position;
    float x = XMVectorGetX(XMVector3Dot(pCenter, tangent));
    float z = XMVectorGetX(XMVector3Dot(pCenter, binormal));
   
    surf.tex = XMVectorSet((x / xSize) * 0.5f + 0.5f, (z / zSize)* 0.5f + 0.5f, 0.0f, 0.0f);
  
}
