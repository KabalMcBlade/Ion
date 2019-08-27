#pragma once


#include "../Core/CoreDefs.h"

#include "../Dependencies/Eos/Eos/Eos.h"
#include "../Dependencies/Nix/Nix/Nix.h"

#include "../Core/MemoryWrapper.h"


EOS_USING_NAMESPACE
NIX_USING_NAMESPACE


ION_NAMESPACE_BEGIN

class Transform;

class ION_DLL BoundingBox
{
public:
    BoundingBox();
    BoundingBox(const Vector& _min, const Vector& _max);
    ~BoundingBox();

    ION_INLINE const Vector& GetMin() const { return m_min; }
    ION_INLINE const Vector& GetMax() const { return m_max; }
    ION_INLINE const Vector& GetHalfExtent() const { return m_halfExtent; }
    ION_INLINE const Vector& GetCenter() const { return m_center; }
    ION_INLINE const Vector& GetSize() const { return m_size; }

    ION_INLINE ionBool IsInfinite() const { return m_min == kMinusInf.m_simdf && m_max == kPlusInf.m_simdf; }
    ION_INLINE ionBool IsZero() const { return m_min == kZero.m_simdf && m_max == kZero.m_simdf; }
    ION_INLINE ionBool IsInit() const { return m_min == kPlusInf.m_simdf && m_max == kMinusInf.m_simdf; }
    ION_INLINE ionBool IsValid() const { return !IsInit() && !IsZero() && !IsInfinite(); }

    void Reset();

    void Set(const Vector& _min, const Vector& _max);
    void SetInfinite();
    void SetZero();

    void MergePoint(const Vector& _point);
    void Expande(const BoundingBox& _bbox);
    void Expande(const Vector& _min, const Vector& _max);

    void GetCorners(ionVector<Vector>& _corners) const;

    BoundingBox GetTransformed(const Matrix& _matrix);

private:
    Vector m_min;
    Vector m_max;
    Vector m_halfExtent;
    Vector m_center;
    Vector m_size;
};

ION_NAMESPACE_END