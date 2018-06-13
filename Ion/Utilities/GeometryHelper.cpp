#include "GeometryHelper.h"


NIX_USING_NAMESPACE


ION_NAMESPACE_BEGIN


Vector GeometryHelper::CalculateAvarageVector(Vector* _vectorArray, ionU32 _count)
{
    Vector vResult = VectorHelper::GetZero();

    for (ionU32 i = 0; i < _count; ++i)
    {
        vResult += _vectorArray[i];
    }

    ionFloat scale = 1.0f / _count;

    vResult *= scale;

    return vResult;
}

Vector GeometryHelper::CalculateSurfaceNormalTriangle(Vector* _vectorArray)
{
    const Vector v1 = _vectorArray[1] - _vectorArray[0];
    const Vector v2 = _vectorArray[2] - _vectorArray[0];
    Vector vSurfaceNormal = v1.Cross(v2);
    vSurfaceNormal.Normalize3();
    return vSurfaceNormal;
}

Vector GeometryHelper::CalculateSurfaceNormalQuad(Vector* _vectorArray)
{
    const Vector vA1 = _vectorArray[1] - _vectorArray[0];
    const Vector vA2 = _vectorArray[2] - _vectorArray[0];
    const Vector vB1 = _vectorArray[3] - _vectorArray[2];
    const Vector vB2 = _vectorArray[0] - _vectorArray[2];

    Vector vSurfaceNormals[2] = { vA1.Cross(vA2), vB1.Cross(vB2) };

    Vector vSurfaceNormal = CalculateAvarageVector(vSurfaceNormals, 2);

    vSurfaceNormal.Normalize3();
    return vSurfaceNormal;
}

ION_NAMESPACE_END