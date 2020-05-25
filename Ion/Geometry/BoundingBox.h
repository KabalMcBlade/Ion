#pragma once


#include "../Core/CoreDefs.h"

#include "../Dependencies/Eos/Eos/Eos.h"
#include "../Dependencies/Nix/Nix/Nix.h"

#include "../Core/MemoryWrapper.h"

#include "../Core/MemorySettings.h"

EOS_USING_NAMESPACE
NIX_USING_NAMESPACE


ION_NAMESPACE_BEGIN

using BoundingBoxAllocator = MemoryAllocator<FreeListBestSearchAllocationPolicy, MultiThreadPolicy, MemoryBoundsCheck, MemoryTag, MemoryLog>;

class Transform;

class ION_DLL BoundingBox
{
public:
	static BoundingBoxAllocator* GetAllocator();

public:
    BoundingBox();
    BoundingBox(const Vector4& _min, const Vector4& _max);
    ~BoundingBox();

    ION_INLINE const Vector4& GetMin() const { return m_min; }
    ION_INLINE const Vector4& GetMax() const { return m_max; }
    ION_INLINE const Vector4& GetHalfExtent() const { return m_halfExtent; }
    ION_INLINE const Vector4& GetCenter() const { return m_center; }
    ION_INLINE const Vector4& GetSize() const { return m_size; }

    ION_INLINE ionBool IsInfinite() const { return m_min == kMinusInf.m_simdf && m_max == kPlusInf.m_simdf; }
    ION_INLINE ionBool IsZero() const { return m_min == kZero.m_simdf && m_max == kZero.m_simdf; }
    ION_INLINE ionBool IsInit() const { return m_min == kPlusInf.m_simdf && m_max == kMinusInf.m_simdf; }
    ION_INLINE ionBool IsValid() const { return !IsInit() && !IsZero() && !IsInfinite(); }

    void Reset();

    void Set(const Vector4& _min, const Vector4& _max);
    void SetInfinite();
    void SetZero();

    void MergePoint(const Vector4& _point);
    void Expande(const BoundingBox& _bbox);
    void Expande(const Vector4& _min, const Vector4& _max);

    void GetCorners(ionVector<Vector4, BoundingBoxAllocator, GetAllocator>& _corners) const;

    BoundingBox GetTransformed(const Matrix& _matrix);

private:
    Vector4 m_min;
    Vector4 m_max;
    Vector4 m_halfExtent;
    Vector4 m_center;
    Vector4 m_size;
};

ION_NAMESPACE_END