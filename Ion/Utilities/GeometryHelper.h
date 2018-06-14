#pragma once

#include "../Core/CoreDefs.h"

#include "../Dependencies/Nix/Nix/Nix.h"



NIX_USING_NAMESPACE

ION_NAMESPACE_BEGIN

class ION_DLL GeometryHelper
{
public:
    static Vector CalculateAvarageVector(const Vector* _vectorArray, const ionU32 _count);

    static Vector CalculateSurfaceNormalTriangle(const Vector* _vectorArray);
    static Vector CalculateSurfaceNormalTriangle(const Vector* _vectorArray, const ionU32 _index);
    static Vector CalculateSurfaceNormalQuad(const Vector* _vectorArray);

    // the size of _outNormalVectorArray must be equal to _vectorArray
    static void CalculateNormalPerVertex(const Vector* _vectorArray, const ionU32* _indexList, const ionU32 _indexCount, Vector* _outNormalVectorArray);

    // the size of _outTangentVectorArray must be equal to _vectorArray
    static void CalculateTangent(
        const Vector* _vectorArray, const Vector* _normalArray, const Vector* _textCoordUVUVArray, const ionU32 _vectorCount,   /* to iterate and get the value to use */
        const ionU32* _indexList, const ionU32 _indexCount,                                                                     /* to generate face (triangle) */
        Vector* _outTangentVectorArray                                                                                          /* output tangent and bi-tangent */
    );
};

ION_NAMESPACE_END