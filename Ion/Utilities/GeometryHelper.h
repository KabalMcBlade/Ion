#pragma once

#include "../Core/CoreDefs.h"

#include "../Dependencies/Nix/Nix/Nix.h"


NIX_USING_NAMESPACE

ION_NAMESPACE_BEGIN

class ION_DLL GeometryHelper
{
public:
    static Vector CalculateAvarageVector(Vector* _vectorArray, ionU32 _count);

    static Vector CalculateSurfaceNormalTriangle(Vector* _vectorArray);
    static Vector CalculateSurfaceNormalQuad(Vector* _vectorArray);
};

ION_NAMESPACE_END