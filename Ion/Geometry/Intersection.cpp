#include "Intersection.h"

#include "../Core/CoreDefs.h"


NIX_USING_NAMESPACE

ION_NAMESPACE_BEGIN


ionBool Intersection::IntersectRayPlane(const Ray& _ray, const Vector& _planeNormal, const Vector& _pointOnPlane, Vector& _outIntersectionPoint)
{
    nixFloat denom = 0;
    _mm_store_ss(&denom, Helper::Dot(_planeNormal, _ray.GetDirection()));

    if (std::abs(denom) > kfEpsilon)
    {
        ionFloat t = 0;
        _mm_store_ss(&t, Helper::Dot(_pointOnPlane - _ray.GetOrigin(), _planeNormal));
        t /= denom;

        if (t >= 0.0f)
        {
            _outIntersectionPoint = _ray.GetOrigin() + t * _ray.GetDirection();
            return true;
        }
    }

    return false;
}

ionBool IntersectRayBoundingBox(const Ray& _ray, const BoundingBox& _box, ionFloat& _outNearCollisionTime, ionFloat& _outFarCollisionTime)
{
    const Vector& l1 = (_box.GetMin() - _ray.GetOrigin()) * _ray.GetInverseDirection();
    const Vector& l2 = (_box.GetMax() - _ray.GetOrigin()) * _ray.GetInverseDirection();

    const Vector& filtered_l1a = Helper::Min(l1, kPlusInf);
    const Vector& filtered_l2a = Helper::Min(l2, kPlusInf);

    const Vector& filtered_l1b = Helper::Max(l1, kMinusInf);
    const Vector& filtered_l2b = Helper::Max(l2, kMinusInf);

    Vector lmax = Helper::Max(filtered_l1a, filtered_l2a);
    Vector lmin = Helper::Min(filtered_l1b, filtered_l2b);


    // a,b,c,d -> b,c,d,a
    const nixFloat4 lmax0 = _mm_shuffle_ps(lmax, lmax, 0x39);
    const nixFloat4 lmin0 = _mm_shuffle_ps(lmin, lmin, 0x39);
    nixFloat4 lmaxSimd = _mm_min_ss(lmax, lmax0);
    nixFloat4 lminSimd = _mm_min_ss(lmin, lmin0);


    const nixFloat4 lmax1 = _mm_movehl_ps(lmaxSimd, lmaxSimd);
    const nixFloat4 lmin1 = _mm_movehl_ps(lminSimd, lminSimd);
    lmaxSimd = _mm_min_ss(lmaxSimd, lmax1);
    lminSimd = _mm_min_ss(lminSimd, lmin1);

    const ionBool ret = (_mm_comige_ss(lmaxSimd, _mm_setzero_ps()) & _mm_comige_ss(lmaxSimd, lminSimd)) > 0;

    _mm_store_ss(&_outNearCollisionTime, lminSimd);
    _mm_store_ss(&_outFarCollisionTime, lmaxSimd);

    return ret;
}

ION_NAMESPACE_END