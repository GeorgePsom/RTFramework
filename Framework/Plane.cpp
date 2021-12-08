#include "stdafx.h"

Plane::Plane(XMFLOAT3& c, XMFLOAT3& n, float x, float z, Material& m) : center(c), xSize(x), zSize(z), mat(m)
{
    XMStoreFloat3(&normal, XMVector3Normalize(XMLoadFloat3(&n)));
}

Plane::~Plane()
{
}

bool Plane::Intersect(Ray& ray) 
{
    
    float t;
    float NdotD = XMVectorGetX(XMVector3Dot(XMLoadFloat3(&ray.direction), XMLoadFloat3(&normal)));
    if (fabs(NdotD) > 1e-6) {
        XMVECTOR centerOrigin = XMLoadFloat3(&center) - XMLoadFloat3(&ray.origin);
        t = XMVectorGetX(XMVector3Dot(centerOrigin, XMLoadFloat3(&normal))) / NdotD;
        XMVECTOR p = XMLoadFloat3(&ray.origin) + t * XMLoadFloat3(&ray.direction);
        // Calculate tangent binormal
        XMFLOAT3 tang(0.0f, 0.0f, 1.0f);
        XMVECTOR binormal = XMVector3Normalize(XMVector3Cross(XMLoadFloat3(&normal), XMLoadFloat3(&tang)));
        XMVECTOR tangent = XMVector3Normalize(XMVector3Cross(binormal, XMLoadFloat3(&normal)));
        XMVECTOR pCenter = XMLoadFloat3(&center) - p;
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
    XMStoreFloat3(&surf.position,
        XMLoadFloat3(&ray.origin) + ray.t * XMLoadFloat3(&ray.direction)
    );
    XMStoreFloat3(&surf.normal, XMLoadFloat3(&normal));
    XMFLOAT3 tang(0.0f, 0.0f, 1.0f);
    XMVECTOR binormal = XMVector3Normalize(XMVector3Cross(XMLoadFloat3(&normal), XMLoadFloat3(&tang)));
    XMVECTOR tangent = XMVector3Normalize(XMVector3Cross(binormal, XMLoadFloat3(&normal)));
    XMVECTOR pCenter = XMLoadFloat3(&center) - XMLoadFloat3(&surf.position);
    float x = XMVectorGetX(XMVector3Dot(pCenter, tangent));
    float z = XMVectorGetX(XMVector3Dot(pCenter, binormal));
   
    surf.tex = XMFLOAT2((x / xSize) * 0.5f + 0.5f, (z / zSize)* 0.5f + 0.5f);
  
}
