#include "Ray.h"

#include "../Core/CoreDefs.h"


NIX_USING_NAMESPACE

ION_NAMESPACE_BEGIN

Ray::Ray()
{
    Set(VectorHelper::GetZero(), VectorHelper::GetOne());
}

Ray::Ray(const Vector& _origin, const Vector& _direction)
{
    Set(_origin, _direction);
}

Ray::Ray(ionFloat _relMousePosX, ionFloat _relMousePosY, const Vector& _worldRayOrigin, const Matrix& _inverseMatrix)
{
    Set(_relMousePosX, _relMousePosY, _worldRayOrigin, _inverseMatrix);
}

Ray::~Ray()
{

}

void Ray::Set(const Vector& _origin, const Vector& _direction)
{
    m_origin = _origin;
    m_direction = _direction;
    m_inverseDirection = VectorHelper::GetOne() / _direction;
}

void Ray::Set(ionFloat _relMousePosX, ionFloat _relMousePosY, const Vector& _worldRayOrigin, const Matrix& _inverseMatrix)
{
    Vector screenPos(_relMousePosX * 2.0f - 1.0f, _relMousePosY * 2.0f - 1.0f, 1.0f, 1.0f);
    Vector worldPos = _inverseMatrix * screenPos;
    Vector worldPosWWWW = VectorHelper::ExtractElement_3(worldPos);
    worldPos /= worldPosWWWW;
    Set(_worldRayOrigin, VectorHelper::Normalize(worldPos - _worldRayOrigin));
}

ION_NAMESPACE_END