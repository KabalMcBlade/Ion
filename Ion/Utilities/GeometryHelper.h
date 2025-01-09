// Copyright (c) 2025-2025 Michele Condo'
// File: C:\Projects\Ion\Ion\Utilities\GeometryHelper.h
// Licensed under the GPL-3.0 License. See LICENSE file in the project root for full license information.


#pragma once

#include "../Core/CoreDefs.h"

#include "../Core/MemoryWrapper.h"

#include "../Dependencies/Nix/Nix/Nix.h"

NIX_USING_NAMESPACE

ION_NAMESPACE_BEGIN

using GeometryHelperAllocator = MemoryAllocator<FreeListBestSearchAllocationPolicy, MultiThreadPolicy, MemoryBoundsCheck, MemoryTag, MemoryLog>;


class ION_DLL GeometryHelper
{
public:
	static GeometryHelperAllocator* GetAllocator();

public:
    // The size of _outNormalVectorArray must be equal to _vectorArray
    static void CalculateNormals(const Vector4* _vectorArray, const ionU32 _vectorCount, const ionU32* _indexList, const ionU32 _indexCount, Vector4* _outNormalVectorArray);

    // The size of _outUVUVVectorArray must be equal to _vectorArray
    // It is a very simple implementation and is not optimized, I don't care at this stage of development!
    // It can be used ONLY for simple primitive geometry!!
    static void CalculateUVs(const Vector4* _vectorArray, const ionU32 _vectorCount, Vector4* _outUVUVVectorArray);

    // The size of _outTangentVectorArray must be equal to _vectorArray and the other input array this internally use the mikktspace algorithm
    static void CalculateTangents(
        const Vector4* _vectorArray, const Vector4* _normalArray, const Vector4* _textCoordUVUVArray, const ionU32 _vectorCount,   // to iterate and get the value to use
        const ionU32* _indexList, const ionU32 _indexCount,                                                                     // to generate face (triangle)
        Vector4* _outTangentVectorArray, ionFloat* _outTangentSignArray                                                          // output tangent and bi-tangent sign
    );
};

ION_NAMESPACE_END