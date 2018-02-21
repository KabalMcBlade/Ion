#include "Transform.h"

EOS_USING_NAMESPACE
NIX_USING_NAMESPACE

ION_NAMESPACE_BEGIN

Transform::Transform() : m_rotation(0.0f, 0.0f, 0.0f, 0.0f), m_scale(1.0f, 1.0f, 1.0f, 1.0f), m_position()
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

void Transform::UpdateTransform(const Matrix& _parentMatrix /* = Matrix() */)
{
    m_matrix = Matrix() * _parentMatrix;
    m_matrix = m_matrix.Translate(m_position);
    m_matrix = m_matrix * m_rotation.ToMatrix();
    m_matrix = m_matrix.Scale(m_scale);
}

void Transform::UpdateTransformInverse(const Matrix& _parentMatrix/* = Matrix() */)
{
    m_matrixInverse = _parentMatrix.Scale(1.0f / m_scale);
    m_matrixInverse = m_rotation.ToMatrix().Transpose() * m_matrixInverse;
    m_matrixInverse = m_matrixInverse.Translate(m_position);
}

const Matrix& Transform::GetMatrix() const
{
    return m_matrix;
}
const Matrix& Transform::GetMatrixInverse() const
{
    return m_matrixInverse;
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
    m_position = _position;
}

void Transform::SetScale(const nixFloat& _scale)
{
#   if NIX_ARCH & NIX_ARCH_AVX512_FLAG

    m_scale = VectorHelper::Splat512(_scale);

#   elif NIX_ARCH & NIX_ARCH_AVX_FLAG

    m_scale = VectorHelper::Splat256(_scale);

#   else 

    m_scale = VectorHelper::Splat(_scale);

#   endif
}

void Transform::SetScale(const Vector& _scale)
{
    m_scale = _scale;
}

void Transform::SetRotation(const Quaternion& _rotation)
{
    m_rotation = _rotation;
}

void Transform::SetRotation(const nixFloat& _radians, const Vector& _axis)
{
    m_rotation = Quaternion(_radians, _axis);
}


ION_NAMESPACE_END