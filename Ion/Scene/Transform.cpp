#include "Transform.h"

EOS_USING_NAMESPACE
NIX_USING_NAMESPACE

ION_NAMESPACE_BEGIN

Transform::Transform() : m_rotation(0.0f, 0.0f, 0.0f, 1.0f), m_scale(1.0f, 1.0f, 1.0f, 0.0f), m_position(0.0f, 0.0f, 0.0f, 1.0f), m_dirty(true)
{

}

Transform::Transform(const Vector& _position, const nixFloat& _scale, const Quaternion& _rotation)
{
    SetPosition(_position);
    SetScale(_scale);
    SetRotation(_rotation);
}

Transform::Transform(const Vector& _position, const nixFloat& _scale, const nixFloat& _radians, const Vector& _axis)
{
    SetPosition(_position);
    SetScale(_scale);
    SetRotation(_radians, _axis);
}

Transform::Transform(const Vector& _position, const Vector& _scale, const Quaternion& _rotation)
{
    SetPosition(_position);
    SetScale(_scale);
    SetRotation(_rotation);
}

Transform::Transform(const Vector& _position, const Vector& _scale, const nixFloat& _radians, const Vector& _axis)
{
    SetPosition(_position);
    SetScale(_scale);
    SetRotation(_radians, _axis);
}

Transform::~Transform()
{

}

const Matrix& Transform::GetMatrixWS() const
{
    return m_matrixWS;
}

const Matrix& Transform::GetMatrix()
{
    if (m_dirty)
    {
        static const Matrix identity;

        const Matrix scale = identity.Scale(m_scale);
        const Matrix rotate = m_rotation.ToMatrix();
        const Matrix translate = identity.Translate(m_position);
        m_matrix = scale * rotate * translate;

        m_rotation.SetFromMatrix(m_matrix);
        m_rotation = m_rotation.Normalize();

        m_dirty = false;
    }
    return m_matrix;
}

const Vector& Transform::GetPosition() const
{
    return m_position;
}
const Vector& Transform::GetScale() const
{
    return m_scale;
}

const Quaternion& Transform::GetRotation() const
{
    return m_rotation;
}

void Transform::SetPosition(const Vector& _position)
{
    m_dirty = true;
    m_position = _position;
}

void Transform::SetScale(const nixFloat& _scale)
{
    m_dirty = true;
    m_scale = Helper::Splat(_scale);
}

void Transform::SetScale(const Vector& _scale)
{
    m_dirty = true;
    m_scale = _scale;
}

void Transform::SetRotation(const Quaternion& _rotation)
{
    m_dirty = true;
    m_rotation = _rotation;
}

void Transform::SetRotation(const nixFloat& _radians, const Vector& _axis)
{
    m_dirty = true;
    m_rotation = Quaternion(_radians, _axis);
}


ION_NAMESPACE_END