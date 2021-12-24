#include "stdafx.h"

Plane::Plane(XMVECTOR& U, XMVECTOR& V, XMVECTOR& Pos, bool reverseNormal, Material& m, bool light) : position(Pos), Intersectable(m, light)
{
    isLight = light;
    mat = m;
    u = U;
    v = V;
    normal = XMVector3Normalize(XMVector3Cross(u, v));
    reverse = reverseNormal;
    //normal = reverse ? -normal : normal;
    u_dot_u = XMVectorGetX(XMVector3Dot(u, u));
    u_dot_v = XMVectorGetX(XMVector3Dot(u, v));
    v_dot_v = XMVectorGetX(XMVector3Dot(v, v));
    det = u_dot_u * v_dot_v - u_dot_v * u_dot_v;
}

Plane::~Plane()
{
}

bool Plane::Intersect(Ray& ray) 
{
    
    float t;
    float NdotD = XMVectorGetX(XMVector3Dot(ray.direction, normal));
    /*if (NdotD > 0.0f)
        return false;*/

   /* if ( ( !reverse && (NdotD) < -1e-6 )  || (reverse && (NdotD > 1e-6)))*/
    if ((NdotD) < -1e-6)
    {
        XMVECTOR posOrigin = position - ray.origin;
        t = XMVectorGetX(XMVector3Dot(posOrigin, normal)) / NdotD;
        XMVECTOR intPoint = ray.origin + t * ray.direction;
        XMVECTOR rhs = intPoint - position;

        float u_dot_rhs = XMVectorGetX(XMVector3Dot(u, rhs));
        float v_dot_rhs = XMVectorGetX(XMVector3Dot(v,rhs));
        float w1 = (v_dot_v * u_dot_rhs - u_dot_v * v_dot_rhs) / det;
        float w2 = (-u_dot_v * u_dot_rhs + u_dot_u * v_dot_rhs) / det;
        if (w1 >= 0 && w1 <= 1 && w2 >= 0 && w2 <= 1 && t > Ray::EPSILON)
        {
            ray.t = t;
            return true;
        }
    }

    return false;
    //if (fabs(NdotD) > 1e-6) {
    //    XMVECTOR centerOrigin = center - ray.origin;
    //    t = XMVectorGetX(XMVector3Dot(centerOrigin, normal)) / NdotD;
    //    XMVECTOR p = ray.origin + t * ray.direction;
    //    // Calculate tangent binormal
    //    XMVECTOR tang = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
    //    XMVECTOR binormal = XMVector3Normalize(XMVector3Cross(normal, tang));
    //    XMVECTOR tangent = XMVector3Normalize(XMVector3Cross(binormal,normal));
    //    XMVECTOR pCenter = center - p;
    //    float x = XMVectorGetX(XMVector3Dot(pCenter, tangent));
    //    float z = XMVectorGetX(XMVector3Dot(pCenter, binormal));
    //    if (t >= 0 && fabs(x) <= xSize/2.0f && fabs(z) <= zSize/2.0f)
    //    {
    //        ray.t = t;
    //        return true;
    //    }
    //}

    //return false;
}

void Plane::GetSurfaceData(Surface& surf, Ray& ray) const
{
    surf.position = ray.origin + ray.t * ray.direction;
    surf.normal = normal;
    /*XMVECTOR tang = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
    XMVECTOR binormal = XMVector3Normalize(XMVector3Cross(normal, tang));
    XMVECTOR tangent = XMVector3Normalize(XMVector3Cross(binormal, normal));*/
    XMVECTOR pCenter = position - surf.position;
    /*float x = XMVectorGetX(XMVector3Dot(pCenter, tangent));
    float z = XMVectorGetX(XMVector3Dot(pCenter, binormal));*/
   
    surf.tex = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
  
}

void Plane::CalculateAABB(XMFLOAT3& pMin, XMFLOAT3& pMax) const
{
    //Do nothing
}

XMVECTOR Plane::SamplePoint(float x1, float x2)
{
    return position + x1 * u + x2 * v;
}
