// Copyright (c) 2025-2025 Michele Condo'
// File: C:\Projects\Ion\Ion\Geometry\Intersection.h
// Licensed under the GPL-3.0 License. See LICENSE file in the project root for full license information.


#pragma once


#include "../Core/CoreDefs.h"


#include "../Dependencies/Eos/Eos/Eos.h"
#include "../Dependencies/Nix/Nix/Nix.h"

#include "Ray.h"
#include "BoundingBox.h"

EOS_USING_NAMESPACE
NIX_USING_NAMESPACE

ION_NAMESPACE_BEGIN


class ION_DLL Intersection
{
    static ionBool IntersectRayPlane(const Ray& _ray, const Vector4& _planeNormal, const Vector4& _pointOnPlane, Vector4& _outIntersectionPoint);
    static ionBool IntersectRayBoundingBox(const Ray& _ray, const BoundingBox& _box, ionFloat& _outNearCollisionTime, ionFloat& _outFarCollisionTime);
};

ION_NAMESPACE_END