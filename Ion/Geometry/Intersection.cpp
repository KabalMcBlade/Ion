// Copyright (c) 2025-2025 Michele Condo'
// File: C:\Projects\Ion\Ion\Geometry\Intersection.cpp
// Licensed under the GPL-3.0 License. See LICENSE file in the project root for full license information.


#include "Intersection.h"

#include "../Core/CoreDefs.h"


NIX_USING_NAMESPACE

ION_NAMESPACE_BEGIN


ionBool Intersection::IntersectRayPlane(const Ray& _ray, const Vector4& _planeNormal, const Vector4& _pointOnPlane, Vector4& _outIntersectionPoint)
{
    ionFloat denom = 0;
    _mm_store_ss(&denom, MathFunctions::Dot(_planeNormal, _ray.GetDirection()));

    if (std::abs(denom) > NIX_EPSILON)
    {
        ionFloat t = 0;
        _mm_store_ss(&t, MathFunctions::Dot(_pointOnPlane - _ray.GetOrigin(), _planeNormal));
        t /= denom;

        if (t >= 0.0f)
        {
            _outIntersectionPoint = _ray.GetOrigin() + t * _ray.GetDirection();
            return true;
        }
    }

    return false;
}

ionBool Intersection::IntersectRayBoundingBox(const Ray& _ray, const BoundingBox& _box, ionFloat& _outNearCollisionTime, ionFloat& _outFarCollisionTime)
{
    const Vector4& l1 = (_box.GetMin() - _ray.GetOrigin()) * _ray.GetInverseDirection();
    const Vector4& l2 = (_box.GetMax() - _ray.GetOrigin()) * _ray.GetInverseDirection();

    const Vector4& filtered_l1a = MathFunctions::Min(l1, kPlusInf);
    const Vector4& filtered_l2a = MathFunctions::Min(l2, kPlusInf);

    const Vector4& filtered_l1b = MathFunctions::Max(l1, kMinusInf);
    const Vector4& filtered_l2b = MathFunctions::Max(l2, kMinusInf);

    Vector4 lmax = MathFunctions::Max(filtered_l1a, filtered_l2a);
    Vector4 lmin = MathFunctions::Min(filtered_l1b, filtered_l2b);


    // a,b,c,d -> b,c,d,a
    const float128 lmax0 = _mm_shuffle_ps(lmax, lmax, 0x39);
    const float128 lmin0 = _mm_shuffle_ps(lmin, lmin, 0x39);
    float128 lmaxSimd = _mm_min_ss(lmax, lmax0);
    float128 lminSimd = _mm_min_ss(lmin, lmin0);


    const float128 lmax1 = _mm_movehl_ps(lmaxSimd, lmaxSimd);
    const float128 lmin1 = _mm_movehl_ps(lminSimd, lminSimd);
    lmaxSimd = _mm_min_ss(lmaxSimd, lmax1);
    lminSimd = _mm_min_ss(lminSimd, lmin1);

    const ionBool ret = (_mm_comige_ss(lmaxSimd, _mm_setzero_ps()) & _mm_comige_ss(lmaxSimd, lminSimd)) > 0;

    _mm_store_ss(&_outNearCollisionTime, lminSimd);
    _mm_store_ss(&_outFarCollisionTime, lmaxSimd);

    return ret;
}

ION_NAMESPACE_END