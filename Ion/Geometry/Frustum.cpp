#include "Frustum.h"


EOS_USING_NAMESPACE
NIX_USING_NAMESPACE

ION_NAMESPACE_BEGIN


Frustum::Frustum()
{
 
}

Frustum::~Frustum()
{

}

void Frustum::Update(const Matrix& _projection, const Matrix& _view)
{
    m_viewProjectionMatrix = _projection * _view;
 
    ExtractFrustumPlanes(m_viewProjectionMatrix, m_frustumPlanesViewSpace);

    m_inverseProjectionMatrix = _projection.InverseNoScale();
    m_inverseViewProjectionMatrix = m_viewProjectionMatrix.InverseNoScale();

    ExtractFrustumsCorners(m_inverseProjectionMatrix, m_frustumCornersViewSpace);
    ExtractFrustumsCorners(m_inverseViewProjectionMatrix, m_frustumCornersWorldSpace);
}

void Frustum::ExtractFrustumsCorners(const Matrix& _inverseMatrix, Corners& _outCorners)
{
    _outCorners.m_corners[EFrustumCorner::EFrustumCorner_NearBottomLeft] = Vector(-1.0f, -1.0f, 0.0f, 1.0f);
    _outCorners.m_corners[EFrustumCorner::EFrustumCorner_NearTopLeft] = Vector(-1.0f, 1.0f, 0.0f, 1.0f);
    _outCorners.m_corners[EFrustumCorner::EFrustumCorner_NearTopRight] = Vector(1.0f, 1.0f, 0.0f, 1.0f);
    _outCorners.m_corners[EFrustumCorner::EFrustumCorner_NearBottomRight] = Vector(1.0f, -1.0f, 0.0f, 1.0f);

    _outCorners.m_corners[EFrustumCorner::EFrustumCorner_FarBottomLeft] = Vector(-1.0f, -1.0f, 1.0f, 1.0f);
    _outCorners.m_corners[EFrustumCorner::EFrustumCorner_FarTopLeft] = Vector(-1.0f, 1.0f, 1.0f, 1.0f);
    _outCorners.m_corners[EFrustumCorner::EFrustumCorner_FarTopRight] = Vector(1.0f, 1.0f, 1.0f, 1.0f);
    _outCorners.m_corners[EFrustumCorner::EFrustumCorner_FarBottomRight] = Vector(1.0f, -1.0f, 1.0f, 1.0f);

    for (ionU8 i = 0; i < EFrustumCorner::EFrustumCorner_Count; ++i)
    {
        Vector corner = _inverseMatrix * _outCorners.m_corners[i];
        Vector cornerW = Helper::ExtractW(corner);

        _outCorners.m_corners[i] = corner / cornerW;
    }
}

void Frustum::ExtractFrustumPlanes(const Matrix& _viewProjMatrix, Planes& _outFrustumPlanes)
{
    Vector viewProjMatrixAxisX = _viewProjMatrix[0];
    Vector viewProjMatrixAxisY = _viewProjMatrix[1];
    Vector viewProjMatrixAxisZ = _viewProjMatrix[2];
    Vector viewProjMatrixAxisW = _viewProjMatrix[3];


    Vector addWX = viewProjMatrixAxisW + viewProjMatrixAxisX;
    Vector subWX = viewProjMatrixAxisW - viewProjMatrixAxisX;
    Vector addWY = viewProjMatrixAxisW + viewProjMatrixAxisY;
    Vector subWY = viewProjMatrixAxisW - viewProjMatrixAxisY;
    Vector addWZ = viewProjMatrixAxisW + viewProjMatrixAxisZ;
    Vector subWZ = viewProjMatrixAxisW - viewProjMatrixAxisZ;


    //
    // normals
    _outFrustumPlanes.m_normals[EFrustumPlane::EFrustumPlane_Left] = addWX;
    _outFrustumPlanes.m_normals[EFrustumPlane::EFrustumPlane_Right] = subWX;
    _outFrustumPlanes.m_normals[EFrustumPlane::EFrustumPlane_Top] = subWY;
    _outFrustumPlanes.m_normals[EFrustumPlane::EFrustumPlane_Bottom] = addWY;
    _outFrustumPlanes.m_normals[EFrustumPlane::EFrustumPlane_Far] = addWZ;
    _outFrustumPlanes.m_normals[EFrustumPlane::EFrustumPlane_Near] = subWZ;

    //
    // distance
    _mm_store_ss(&_outFrustumPlanes.m_distances[EFrustumPlane::EFrustumPlane_Left], _mm_shuffle_ps(addWX, addWX, _MM_SHUFFLE(3, 3, 3, 3)));
    _mm_store_ss(&_outFrustumPlanes.m_distances[EFrustumPlane::EFrustumPlane_Right], _mm_shuffle_ps(subWX, subWX, _MM_SHUFFLE(3, 3, 3, 3)));

    _mm_store_ss(&_outFrustumPlanes.m_distances[EFrustumPlane::EFrustumPlane_Top], _mm_shuffle_ps(subWY, subWY, _MM_SHUFFLE(3, 3, 3, 3)));
    _mm_store_ss(&_outFrustumPlanes.m_distances[EFrustumPlane::EFrustumPlane_Bottom], _mm_shuffle_ps(addWY, addWY, _MM_SHUFFLE(3, 3, 3, 3)));

    _mm_store_ss(&_outFrustumPlanes.m_distances[EFrustumPlane::EFrustumPlane_Far], _mm_shuffle_ps(addWZ, addWZ, _MM_SHUFFLE(3, 3, 3, 3)));
    _mm_store_ss(&_outFrustumPlanes.m_distances[EFrustumPlane::EFrustumPlane_Near], _mm_shuffle_ps(subWZ, subWZ, _MM_SHUFFLE(3, 3, 3, 3)));

    for (ionU8 planeIdx = 0; planeIdx < EFrustumPlane::EFrustumPlane_Count; ++planeIdx)
    {
        const Vector len4 = _outFrustumPlanes.m_normals[planeIdx].Length();
        ionFloat len = 0.0f;

        _mm_store_ss(&len, len4);

        _outFrustumPlanes.m_normals[planeIdx] /= len4;
        _outFrustumPlanes.m_distances[planeIdx] /= len;
    }
}


ION_NAMESPACE_END