#pragma once


#include "../Core/CoreDefs.h"


#include "../Dependencies/Eos/Eos/Eos.h"
#include "../Dependencies/Nix/Nix/Nix.h"


EOS_USING_NAMESPACE
NIX_USING_NAMESPACE

ION_NAMESPACE_BEGIN


class ION_DLL Ray
{
public:
    Ray();
    Ray(const Vector4& _origin, const Vector4& _direction);
    Ray(ionFloat _relMousePosX, ionFloat _relMousePosY, const Vector4& _worldRayOrigin, const Matrix4x4& _inverseMatrix);
    ~Ray();

    ION_INLINE const Vector4& GetOrigin() const { return m_origin;}
    ION_INLINE const Vector4& GetDirection() const { return m_direction; }
    ION_INLINE const Vector4& GetInverseDirection() const { return m_inverseDirection; }

    void Set(const Vector4& _origin, const Vector4& _direction);
    void Set(ionFloat _relMousePosX, ionFloat _relMousePosY, const Vector4& _worldRayOrigin, const Matrix4x4& _inverseMatrix);

private:
    Vector4 m_origin;
    Vector4 m_direction;
    Vector4 m_inverseDirection;
};

ION_NAMESPACE_END