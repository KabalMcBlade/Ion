// Copyright (c) 2025-2025 Michele Condo'
// File: C:\Projects\Ion\Ion\Geometry\Frustum.h
// Licensed under the GPL-3.0 License. See LICENSE file in the project root for full license information.


#pragma once


#include "../Core/CoreDefs.h"

#include "../Dependencies/Eos/Eos/Eos.h"
#include "../Dependencies/Nix/Nix/Nix.h"


EOS_USING_NAMESPACE
NIX_USING_NAMESPACE


ION_NAMESPACE_BEGIN


enum EFrustumPlane
{
    EFrustumPlane_Near = 0,
    EFrustumPlane_Far = 1,
    EFrustumPlane_Left = 2,
    EFrustumPlane_Right = 3,
    EFrustumPlane_Top = 4,
    EFrustumPlane_Bottom = 5,
    EFrustumPlane_Count
};

enum EFrustumCorner
{
    EFrustumCorner_NearTopRight = 0,
    EFrustumCorner_NearTopLeft = 1,
    EFrustumCorner_NearBottomLeft = 2,
    EFrustumCorner_NearBottomRight = 3,
    EFrustumCorner_FarTopRight = 4,
    EFrustumCorner_FarTopLeft = 5,
    EFrustumCorner_FarBottomLeft = 6,
    EFrustumCorner_FarBottomRight = 7,
    EFrustumCorner_Count
};

class ION_DLL Frustum final
{
public:
    struct Planes
    {
        Vector4 m_normals[EFrustumPlane::EFrustumPlane_Count];
        ionFloat m_distances[EFrustumPlane::EFrustumPlane_Count];
    };

    struct Corners
    {
        Vector4 m_corners[EFrustumCorner::EFrustumCorner_Count];
    };

public:
    Frustum();
    ~Frustum();

    void Update(const Matrix4x4& _projection, const Matrix4x4& _view);

    const Planes& GetFrustumPlanesViewSpace() const { return m_frustumPlanesViewSpace; }
    const Corners& GetFrustumCornersViewSpace() const { return m_frustumCornersViewSpace; }
    const Corners& GetFrustumCornersWorldSpace() const { return m_frustumCornersWorldSpace; }

private:
    void ExtractFrustumsCorners(const Matrix4x4& _inverseMatrix, Corners& _outCorners);
    void ExtractFrustumPlanes(const Matrix4x4& _viewProjMatrix, Planes& _outFrustumPlanes);

private:
    Planes  m_frustumPlanesViewSpace;
    Corners m_frustumCornersViewSpace;
    Corners m_frustumCornersWorldSpace;

    Matrix4x4  m_viewMatrix;
    Matrix4x4  m_projectionMatrix;
    Matrix4x4  m_inverseProjectionMatrix;
    Matrix4x4  m_viewProjectionMatrix;
    Matrix4x4  m_inverseViewProjectionMatrix;
};


ION_NAMESPACE_END