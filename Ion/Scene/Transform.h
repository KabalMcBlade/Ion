#pragma once

#include "../Core/CoreDefs.h"

#include "../Dependencies/Eos/Eos/Eos.h"
#include "../Dependencies/Nix/Nix/Nix.h"


EOS_USING_NAMESPACE
NIX_USING_NAMESPACE


ION_NAMESPACE_BEGIN

class ION_DLL Transform final
{
public:
    explicit Transform();

    explicit Transform(const Vector& _position, const nixFloat& _scale, const Quaternion& _rotation);
    explicit Transform(const Vector& _position, const nixFloat& _scale, const nixFloat& _radians, const Vector& _axis);

    explicit Transform(const Vector& _position, const Vector& _scale, const Quaternion& _rotation);
    explicit Transform(const Vector& _position, const Vector& _scale, const nixFloat& _radians, const Vector& _axis);

    ~Transform();

    void SetMatrixWS(const Matrix& _matrixWS) { m_matrixWS = _matrixWS; }

    const Matrix& GetMatrixWS() const;
    const Matrix& GetMatrix();

    const Vector& GetPosition() const;
    const Vector& GetScale() const;
    const Quaternion& GetRotation() const;

    void SetPosition(const Vector& _position);
    void SetScale(const nixFloat& _scale);
    void SetScale(const Vector& _scale);
    void SetRotation(const Quaternion& _rotation);
    void SetRotation(const nixFloat& _radians, const Vector& _axis);

private:
    Transform(const Transform& _orig) = delete;    
    Transform& operator = (const Transform&) = delete;

    Matrix m_matrixWS;
    Matrix m_matrix;
    Vector m_position;
    Vector m_scale;
    Quaternion m_rotation;

    ionBool m_dirty;
};

ION_NAMESPACE_END