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
    Ray(const Vector& _origin, const Vector& _direction);
    Ray(ionFloat _relMousePosX, ionFloat _relMousePosY, const Vector& _worldRayOrigin, const Matrix& _inverseMatrix);
    ~Ray();

    ION_INLINE const Vector& GetOrigin() const { return m_origin;}
    ION_INLINE const Vector& GetDirection() const { return m_direction; }
    ION_INLINE const Vector& GetInverseDirection() const { return m_inverseDirection; }

    void Set(const Vector& _origin, const Vector& _direction);
    void Set(ionFloat _relMousePosX, ionFloat _relMousePosY, const Vector& _worldRayOrigin, const Matrix& _inverseMatrix);

private:
    Vector m_origin;
    Vector m_direction;
    Vector m_inverseDirection;
};

ION_NAMESPACE_END