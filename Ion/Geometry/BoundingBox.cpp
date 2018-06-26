#include "BoundingBox.h"

#include "../Core/CoreDefs.h"

#include "../Scene/Transform.h"

EOS_USING_NAMESPACE
NIX_USING_NAMESPACE

ION_NAMESPACE_BEGIN

BoundingBox::BoundingBox()
{
    Set(VectorHelper::GetPlusInf(), VectorHelper::GetMinusInf());
}

BoundingBox::BoundingBox(const Vector& _min, const Vector& _max)
{
    Set(_min, _max);
}

BoundingBox::~BoundingBox()
{

}

void BoundingBox::Set(const Vector& _min, const Vector& _max)
{
    m_min = _min;
    m_max = _max;
    m_halfExtent = (m_max - m_min) * 0.5f;
    m_center = (m_max + m_min) * 0.5f;

    /*
    // was used to clear the result, maybe useless
    const Vector halfExtent((m_max - m_min) * 0.5f);
    const Vector center((m_min + m_max) * 0.5f);
    m_halfExtent = VectorHelper::MulAdd(halfExtent, VectorHelper::Get1110(), VectorHelper::Get0001());
    m_center = VectorHelper::MulAdd(center, VectorHelper::Get1110(), VectorHelper::Get0001());
    */
}

void BoundingBox::SetInfinite()
{
    m_min = VectorHelper::GetMinusInf();
    m_max = VectorHelper::GetPlusInf();
}

void BoundingBox::SetZero()
{
    m_min = VectorHelper::GetZero();
    m_max = VectorHelper::GetZero();
}

void BoundingBox::MergePoint(const Vector& _point)
{
    Set(VectorHelper::Min(m_min, _point), VectorHelper::Max(m_max, _point));
}

void BoundingBox::Expande(const BoundingBox& _bbox)
{
    Set(VectorHelper::Min(m_min, _bbox.GetMin()), VectorHelper::Max(m_max, _bbox.GetMax()));
}

void BoundingBox::Expande(const Vector& _min, const Vector& _max)
{
    Set(VectorHelper::Min(m_min, _min), VectorHelper::Max(m_max, _max));
}

void BoundingBox::GetCorners(eosVector(Vector)& _corners) const
{
    const __nixFloat4 min = m_min;
    const __nixFloat4 max = m_max;

    const __nixFloat4 _minX_maxX_minY_maxY = _mm_unpacklo_ps(min, max);
    const __nixFloat4 _minZ_maxZ_minW_maxW = _mm_unpackhi_ps(min, max);

    _corners.clear();
    _corners.reserve(8);

    // corner 0
    _corners.push_back(m_min);

    // corner 1
    const __nixFloat4 _minX_maxY_minZ_maxW = _mm_shuffle_ps(_minX_maxX_minY_maxY, _minZ_maxZ_minW_maxW, _MM_SHUFFLE(0, 3, 0, 3));
    _corners.push_back(_minX_maxY_minZ_maxW);

    // corner 2
    const __nixFloat4 _maxX_maxY_minZ_maxW = _mm_shuffle_ps(_minX_maxX_minY_maxY, _minZ_maxZ_minW_maxW, _MM_SHUFFLE(1, 3, 0, 3));
    _corners.push_back(_maxX_maxY_minZ_maxW);

    // corner 3
    const __nixFloat4 _maxX_minY_minZ_maxW = _mm_shuffle_ps(_minX_maxX_minY_maxY, _minZ_maxZ_minW_maxW, _MM_SHUFFLE(1, 2, 0, 3));
    _corners.push_back(_maxX_minY_minZ_maxW);

    // corner 4
    _corners.push_back(m_max);

    // corner 5
    const __nixFloat4 _minX_maxY_maxZ_maxW = _mm_shuffle_ps(_minX_maxX_minY_maxY, _minZ_maxZ_minW_maxW, _MM_SHUFFLE(0, 3, 1, 3));
    _corners.push_back(_minX_maxY_maxZ_maxW);

    // corner 6
    const __nixFloat4 _minX_minY_maxZ_maxW = _mm_shuffle_ps(_minX_maxX_minY_maxY, _minZ_maxZ_minW_maxW, _MM_SHUFFLE(0, 2, 1, 3));
    _corners.push_back(_minX_minY_maxZ_maxW);

    // corner 7
    const __nixFloat4 _maxX_minY_maxZ_maxW = _mm_shuffle_ps(_minX_maxX_minY_maxY, _minZ_maxZ_minW_maxW, _MM_SHUFFLE(1, 2, 1, 3));
    _corners.push_back(_maxX_minY_maxZ_maxW);
}

BoundingBox BoundingBox::GetTransformed(const Matrix& _matrix)
{
    BoundingBox transformedBoundingBox;

    eosVector(Vector) corners;
    GetCorners(corners);

    for (ionU32 i = 0; i < 8; ++i)
    {
        Vector tv = _matrix * corners[i];

        transformedBoundingBox.MergePoint(tv);
    }

    return transformedBoundingBox;
}

ION_NAMESPACE_END