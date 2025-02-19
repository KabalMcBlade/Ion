// Copyright (c) 2025-2025 Michele Condo'
// File: C:\Projects\Ion\Ion\Geometry\Ray.cpp
// Licensed under the GPL-3.0 License. See LICENSE file in the project root for full license information.


#include "Ray.h"

#include "../Core/CoreDefs.h"


NIX_USING_NAMESPACE

ION_NAMESPACE_BEGIN

Ray::Ray()
{
    Set(Vector4(kZero), Vector4(kOneVec4));
}

Ray::Ray(const Vector4& _origin, const Vector4& _direction)
{
    Set(_origin, _direction);
}

Ray::Ray(ionFloat _relMousePosX, ionFloat _relMousePosY, const Vector4& _worldRayOrigin, const Matrix4x4& _inverseMatrix)
{
    Set(_relMousePosX, _relMousePosY, _worldRayOrigin, _inverseMatrix);
}

Ray::~Ray()
{

}

void Ray::Set(const Vector4& _origin, const Vector4& _direction)
{
    static const Vector4 one = Vector4(kOneVec4);
    m_origin = _origin;
    m_direction = _direction;
    m_inverseDirection = one / _direction;
}

void Ray::Set(ionFloat _relMousePosX, ionFloat _relMousePosY, const Vector4& _worldRayOrigin, const Matrix4x4& _inverseMatrix)
{
    Vector4 screenPos(_relMousePosX * 2.0f - 1.0f, _relMousePosY * 2.0f - 1.0f, 1.0f, 1.0f);
    Vector4 worldPos = _inverseMatrix * screenPos;
    Vector4 worldPosWWWW = MathFunctions::ExtractW(worldPos);
    worldPos /= worldPosWWWW;
    Set(_worldRayOrigin, MathFunctions::Normalize(worldPos - _worldRayOrigin));
}

ION_NAMESPACE_END