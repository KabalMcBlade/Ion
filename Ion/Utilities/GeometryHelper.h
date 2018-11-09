#pragma once

#include "../Core/CoreDefs.h"

#include "../Dependencies/Nix/Nix/Nix.h"

#include "../Dependencies/Miscellaneous/mikktspace.h"

NIX_USING_NAMESPACE

ION_NAMESPACE_BEGIN

class ION_DLL GeometryHelper
{
public:
    // the size of _outNormalVectorArray must be equal to _vectorArray
    static void CalculateNormals(const Vector* _vectorArray, const ionU32* _indexList, const ionU32 _indexCount, Vector* _outNormalVectorArray);

    // the size of _outUVUVVectorArray must be equal to _vectorArray
    // is a very simple implementation!
    static void CalculateUVs(const Vector* _vectorArray, const ionU32 _vectorCount, Vector* _outUVUVVectorArray);

    // the size of _outTangentVectorArray must be equal to _vectorArray
    static void CalculateTangents(
        const Vector* _vectorArray, const Vector* _normalArray, const Vector* _textCoordUVUVArray, const ionU32 _vectorCount,   // to iterate and get the value to use
        const ionU32* _indexList, const ionU32 _indexCount,                                                                     // to generate face (triangle)
        Vector* _outTangentVectorArray                                                                                          // output tangent and bi-tangent
    );
    
private:
    static Vector CalculateAvarageVector(const Vector* _vectorArray, const ionU32 _count);

    static Vector CalculateSurfaceNormalTriangle(const Vector* _vectorArray, const ionU32 _index);
    static Vector CalculateSurfaceNormalQuad(const Vector* _vectorArray);

};

ION_NAMESPACE_END