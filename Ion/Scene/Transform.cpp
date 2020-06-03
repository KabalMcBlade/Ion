#include "Transform.h"

EOS_USING_NAMESPACE
NIX_USING_NAMESPACE

ION_NAMESPACE_BEGIN

Transform::Transform() : m_rotation(0.0f, 0.0f, 0.0f, 1.0f), m_scale(1.0f, 1.0f, 1.0f, 0.0f), m_position(0.0f, 0.0f, 0.0f, 1.0f), m_dirty(true)
{

}

Transform::Transform(const Vector4& _position, const ionFloat& _scale, const Quaternion& _rotation)
{
    SetPosition(_position);
    SetScale(_scale);
    SetRotation(_rotation);
}

Transform::Transform(const Vector4& _position, const ionFloat& _scale, const ionFloat& _radians, const Vector4& _axis)
{
    SetPosition(_position);
    SetScale(_scale);
    SetRotation(_radians, _axis);
}

Transform::Transform(const Vector4& _position, const Vector4& _scale, const Quaternion& _rotation)
{
    SetPosition(_position);
    SetScale(_scale);
    SetRotation(_rotation);
}

Transform::Transform(const Vector4& _position, const Vector4& _scale, const ionFloat& _radians, const Vector4& _axis)
{
    SetPosition(_position);
    SetScale(_scale);
    SetRotation(_radians, _axis);
}

Transform::~Transform()
{

}

const Matrix4x4& Transform::GetMatrixWS() const
{
    return m_matrixWS;
}

const Matrix4x4& Transform::GetMatrix()
{
    if (m_dirty)
    {
        static const Matrix4x4 identity;

        const Matrix4x4 rotate = m_rotation.ToMatrix();
        const Matrix4x4 translate = identity.Translate(m_position);
        m_matrix =  rotate * translate;
        
        m_rotation.SetFromMatrix(m_matrix);
        m_rotation = m_rotation.Normalize();

        const Matrix4x4 scale = identity.Scale(m_scale);
        m_matrix = scale * m_matrix;

        m_dirty = false;
    }
    return m_matrix;
}

const Vector4& Transform::GetPosition() const
{
    return m_position;
}
const Vector4& Transform::GetScale() const
{
    return m_scale;
}

const Quaternion& Transform::GetRotation() const
{
    return m_rotation;
}

void Transform::SetPosition(const Vector4& _position)
{
    m_dirty = true;
    m_position = _position;
}

void Transform::SetPosition(const ionFloat& _x, const ionFloat& _y, const ionFloat& _z)
{
    m_dirty = true;
    m_position = Helper::Set(_x, _y, _z, 0.0f);
}

void Transform::SetScale(const ionFloat& _scale)
{
    m_dirty = true;
    m_scale = Helper::Splat(_scale);
}

void Transform::SetScale(const Vector4& _scale)
{
    m_dirty = true;
    m_scale = _scale;
}

void Transform::SetScale(const ionFloat& _x, const ionFloat& _y, const ionFloat& _z)
{
    m_dirty = true;
    m_scale = Helper::Set(_x, _y, _z, 0.0f);
}

void Transform::SetRotation(const Quaternion& _rotation)
{
    m_dirty = true;
    m_rotation = _rotation;
}

void Transform::SetRotation(const ionFloat& _radians, const Vector4& _axis)
{
    m_dirty = true;
    m_rotation.SetFromAngleAxis(_radians, _axis);
}

void Transform::SetRotation(const ionFloat& _radians, const ionFloat& _x, const ionFloat& _y, const ionFloat& _z)
{
    m_dirty = true;
    m_rotation.SetFromAngleAxis(_radians, _x, _y, _z);
}

void Transform::SetRotation(const ionFloat& _pitch, const ionFloat& _yaw, const ionFloat& _roll)
{
    m_dirty = true;
    m_rotation.SetFromPitchYawRoll(_pitch, _yaw, _roll);
}


ION_NAMESPACE_END