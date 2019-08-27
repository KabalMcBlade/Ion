#include "BoundingBox.h"

#include "../Core/CoreDefs.h"

#include "../Scene/Transform.h"

EOS_USING_NAMESPACE
NIX_USING_NAMESPACE

ION_NAMESPACE_BEGIN

BoundingBox::BoundingBox()
{
    Reset();
}

BoundingBox::BoundingBox(const Vector& _min, const Vector& _max)
{
    Set(_min, _max);
}

BoundingBox::~BoundingBox()
{

}

void BoundingBox::Reset()
{
    Set(kPlusInf.m_simdf, kMinusInf.m_simdf);
}

void BoundingBox::Set(const Vector& _min, const Vector& _max)
{
    m_min = _min;
    m_max = _max;

	ionAssertReturnVoid((((uintptr_t)(&m_min)) & 15) == 0, "Pointer is misaligned");
	ionAssertReturnVoid((((uintptr_t)(&m_max)) & 15) == 0, "Pointer is misaligned");

    m_halfExtent = (m_max - m_min) * 0.5f;
    m_center = (m_max + m_min) * 0.5f;
    m_size = m_max - m_min;

    /*
    // was used to clear the result, maybe useless
    const Vector halfExtent((m_max - m_min) * 0.5f);
    const Vector center((m_min + m_max) * 0.5f);
    m_halfExtent = Helper::MulAdd(halfExtent, k1110, k0001);
    m_center = Helper::MulAdd(center, k1110, k0001);
    */
}

void BoundingBox::SetInfinite()
{
    m_min = kMinusInf.m_simdf;
    m_max = kPlusInf.m_simdf;
}

void BoundingBox::SetZero()
{
    m_min = kZero.m_simdf;
    m_max = kZero.m_simdf;
}

void BoundingBox::MergePoint(const Vector& _point)
{
    Set(Helper::Min(m_min, _point), Helper::Max(m_max, _point));
}

void BoundingBox::Expande(const BoundingBox& _bbox)
{
    Set(Helper::Min(m_min, _bbox.GetMin()), Helper::Max(m_max, _bbox.GetMax()));
}

void BoundingBox::Expande(const Vector& _min, const Vector& _max)
{
    Set(Helper::Min(m_min, _min), Helper::Max(m_max, _max));
}

void BoundingBox::GetCorners(ionVector<Vector>& _corners) const
{
    const nixFloat4 min = m_min;
    const nixFloat4 max = m_max;

    const nixFloat4 _minX_maxX_minY_maxY = _mm_unpacklo_ps(min, max);
    const nixFloat4 _minZ_maxZ_minW_maxW = _mm_unpackhi_ps(min, max);

    _corners->clear();
    _corners->reserve(8);

    // corner 0
    _corners->push_back(m_min);

    // corner 1
    const nixFloat4 _minX_maxY_minZ_maxW = _mm_shuffle_ps(_minX_maxX_minY_maxY, _minZ_maxZ_minW_maxW, _MM_SHUFFLE(0, 3, 0, 3));
    _corners->push_back(_minX_maxY_minZ_maxW);

    // corner 2
    const nixFloat4 _maxX_maxY_minZ_maxW = _mm_shuffle_ps(_minX_maxX_minY_maxY, _minZ_maxZ_minW_maxW, _MM_SHUFFLE(1, 3, 0, 3));
    _corners->push_back(_maxX_maxY_minZ_maxW);

    // corner 3
    const nixFloat4 _maxX_minY_minZ_maxW = _mm_shuffle_ps(_minX_maxX_minY_maxY, _minZ_maxZ_minW_maxW, _MM_SHUFFLE(1, 2, 0, 3));
    _corners->push_back(_maxX_minY_minZ_maxW);

    // corner 4
    _corners->push_back(m_max);

    // corner 5
    const nixFloat4 _minX_maxY_maxZ_maxW = _mm_shuffle_ps(_minX_maxX_minY_maxY, _minZ_maxZ_minW_maxW, _MM_SHUFFLE(0, 3, 1, 3));
    _corners->push_back(_minX_maxY_maxZ_maxW);

    // corner 6
    const nixFloat4 _minX_minY_maxZ_maxW = _mm_shuffle_ps(_minX_maxX_minY_maxY, _minZ_maxZ_minW_maxW, _MM_SHUFFLE(0, 2, 1, 3));
    _corners->push_back(_minX_minY_maxZ_maxW);

    // corner 7
    const nixFloat4 _maxX_minY_maxZ_maxW = _mm_shuffle_ps(_minX_maxX_minY_maxY, _minZ_maxZ_minW_maxW, _MM_SHUFFLE(1, 2, 1, 3));
    _corners->push_back(_maxX_minY_maxZ_maxW);
}

BoundingBox BoundingBox::GetTransformed(const Matrix& _matrix)
{
    BoundingBox transformedBoundingBox;

    ionVector<Vector> corners;
    GetCorners(corners);

    for (ionU32 i = 0; i < 8; ++i)
    {
        Vector tv = _matrix * corners[i];

        transformedBoundingBox.MergePoint(tv);
    }

    return transformedBoundingBox;
}

ION_NAMESPACE_END