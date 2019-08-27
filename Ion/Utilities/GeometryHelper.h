#pragma once

#include "../Core/CoreDefs.h"

#include "../Core/MemoryWrapper.h"

#include "../Dependencies/Nix/Nix/Nix.h"

NIX_USING_NAMESPACE

ION_NAMESPACE_BEGIN

class ION_DLL GeometryHelper
{
public:
    // The size of _outNormalVectorArray must be equal to _vectorArray
    static void CalculateNormals(const Vector* _vectorArray, const ionU32 _vectorCount, const ionU32* _indexList, const ionU32 _indexCount, Vector* _outNormalVectorArray);

    // The size of _outUVUVVectorArray must be equal to _vectorArray
    // It is a very simple implementation and is not optimized, I don't care at this stage of development!
    // It can be used ONLY for simple primitive geometry!!
    static void CalculateUVs(const Vector* _vectorArray, const ionU32 _vectorCount, Vector* _outUVUVVectorArray);

    // The size of _outTangentVectorArray must be equal to _vectorArray and the other input array this internally use the mikktspace algorithm
    static void CalculateTangents(
        const Vector* _vectorArray, const Vector* _normalArray, const Vector* _textCoordUVUVArray, const ionU32 _vectorCount,   // to iterate and get the value to use
        const ionU32* _indexList, const ionU32 _indexCount,                                                                     // to generate face (triangle)
        Vector* _outTangentVectorArray, ionFloat* _outTangentSignArray                                                          // output tangent and bi-tangent sign
    );
};

ION_NAMESPACE_END