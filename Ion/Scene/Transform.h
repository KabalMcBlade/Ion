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
    Transform();

    Transform(const Vector4& _position, const ionFloat& _scale, const Quaternion& _rotation);
    Transform(const Vector4& _position, const ionFloat& _scale, const ionFloat& _radians, const Vector4& _axis);

    Transform(const Vector4& _position, const Vector4& _scale, const Quaternion& _rotation);
    Transform(const Vector4& _position, const Vector4& _scale, const ionFloat& _radians, const Vector4& _axis);

    ~Transform();

    void SetMatrixWS(const Matrix& _matrixWS) { m_matrixWS = _matrixWS; }

    const Matrix& GetMatrixWS() const;
    const Matrix& GetMatrix();

    const Vector4& GetPosition() const;
    const Vector4& GetScale() const;
    const Quaternion& GetRotation() const;

    void SetPosition(const Vector4& _position);
    void SetPosition(const ionFloat& _x, const ionFloat& _y, const ionFloat& _z);
    void SetScale(const ionFloat& _scale);
    void SetScale(const ionFloat& _x, const ionFloat& _y, const ionFloat& _z);
    void SetScale(const Vector4& _scale);
    void SetRotation(const Quaternion& _rotation);
    void SetRotation(const ionFloat& _radians, const Vector4& _axis);
    void SetRotation(const ionFloat& _radians, const ionFloat& _x, const ionFloat& _y, const ionFloat& _z);
    void SetRotation(const ionFloat& _pitch, const ionFloat& _yaw, const ionFloat& _roll);

private:
    Transform(const Transform& _orig) = delete;    
    Transform& operator = (const Transform&) = delete;

    Matrix m_matrixWS;
    Matrix m_matrix;
    Vector4 m_position;
    Vector4 m_scale;
    Quaternion m_rotation;

    ionBool m_dirty;
};

ION_NAMESPACE_END