#include "DirectionalLight.h"

#include "../Geometry/BoundingBox.h"

EOS_USING_NAMESPACE
NIX_USING_NAMESPACE

ION_NAMESPACE_BEGIN

DirectionalLight::DirectionalLight() : m_color(1.0f)
{
    m_nodeType = ENodeType_DirectionalLight;
}

DirectionalLight::DirectionalLight(const Vector& _color, ionU32 _percentageCloserFilteringSize /*= 3*/) : m_color(_color), m_percentageCloserFilteringKernelSize(_percentageCloserFilteringSize)
{
    m_nodeType = ENodeType_DirectionalLight;
}

DirectionalLight::~DirectionalLight()
{

}

void DirectionalLight::UpdateViewMatrix()
{
    Matrix parent;
    if (GetParentHandle().IsValid())
    {
        parent = GetParentHandle()->GetTransformHandle()->GetMatrix();
    }

    GetTransformHandle()->UpdateTransformInverse(parent);
    m_view = GetTransformHandle()->GetMatrixInverse();
}

void DirectionalLight::ComputeScaleAndOffset(const BaseCamera& _camera, const BoundingBox& _sceneBoundingBox, ionU32 _shadowMapSize)
{
    const ionFloat shadowMapSize = static_cast<float>(_shadowMapSize);

    Vector min = _sceneBoundingBox.GetMin();
    Vector max = _sceneBoundingBox.GetMax();

    ionFloat minX = VectorHelper::ExtractElement_0(min);
    ionFloat minY = VectorHelper::ExtractElement_1(min);
    ionFloat minZ = VectorHelper::ExtractElement_2(min);

    ionFloat maxX = VectorHelper::ExtractElement_0(max);
    ionFloat maxY = VectorHelper::ExtractElement_1(max);
    ionFloat maxZ = VectorHelper::ExtractElement_2(max);

    Vector worldSpaceSceneBoundingBoxCorners[EFrustumCorner_Count] =
    {
        Vector(maxX, maxY, minZ, 1.f),
        Vector(minX, maxY, minZ, 1.f),
        Vector(minX, minY, minZ, 1.f),
        Vector(maxX, minY, minZ, 1.f),
        Vector(maxX, maxY, maxZ, 1.f),
        Vector(minX, maxY, maxZ, 1.f),
        Vector(minX, minY, maxZ, 1.f),
        Vector(maxX, minY, maxZ, 1.f)
    };

    ionFloat fZNear = -std::numeric_limits<ionFloat>::max();
    for (uint32_t i = 0; i < 8; ++i)
    {
        auto sceneAABBCornerLVS = m_view * worldSpaceSceneBoundingBoxCorners[i];
        ionFloat cornerNearZ = VectorHelper::ExtractElement_2(sceneAABBCornerLVS);
        fZNear = fmax(fZNear, cornerNearZ);
    }


    Vector lightViewSpaceMin(std::numeric_limits<ionFloat>::max());
    Vector lightViewSpaceMax(-std::numeric_limits<ionFloat>::max());

    // I use the world space and then multiple with the view of the light because I need the space view of the light and not of the camera
    Frustum::Corners frustumCornersWS = _camera.GetFrustum().GetFrustumCornersWorldSpace();
    for (ionU32 cornerIdx = 0; cornerIdx < EFrustumCorner_Count; ++cornerIdx)
    {
        auto lightViewSpaceCorner = m_view * frustumCornersWS.m_corners[cornerIdx];
        lightViewSpaceMin = VectorHelper::Min(lightViewSpaceMin, lightViewSpaceCorner);
        lightViewSpaceMax = VectorHelper::Max(lightViewSpaceMax, lightViewSpaceCorner);
    }

    ionFloat depth = _camera.GetFar() - _camera.GetNear();
    ionFloat diagLen = VectorHelper::ExtractElement_0(frustumCornersWS.m_corners[4].Distance(frustumCornersWS.m_corners[6]));

    ionFloat paddingX = (diagLen - (VectorHelper::ExtractElement_0(lightViewSpaceMax) - VectorHelper::ExtractElement_0(lightViewSpaceMin))) * 0.5f;
    ionFloat paddingY = (diagLen - (VectorHelper::ExtractElement_1(lightViewSpaceMax) - VectorHelper::ExtractElement_1(lightViewSpaceMin))) * 0.5f;

    ionAssertReturnVoid(paddingX >= 0.f && paddingY >= 0.f, "Must be greater or equal 0!");
    Vector padding(paddingX, paddingY, 0.f);
    lightViewSpaceMin -= padding;
    lightViewSpaceMax += padding;

    // fit the X and Y so that percentage closer filtering kernel will not sample outside of shadow maps
    ionFloat pcfPaddingTexelCount = static_cast<ionFloat>(m_percentageCloserFilteringKernelSize >> 1);
    Vector worldUnitsPerTexel = lightViewSpaceMax - lightViewSpaceMin;  // considering just X and Y!
    Vector clearZW(1.0f, 1.0f, 0.0f, 0.0f);
    worldUnitsPerTexel *= clearZW;
    worldUnitsPerTexel /= (_shadowMapSize - 2.0f * pcfPaddingTexelCount);

    padding = Vector(worldUnitsPerTexel * pcfPaddingTexelCount);
    lightViewSpaceMin -= padding;
    lightViewSpaceMax += padding;

    // Snap the orthographic frustum to pixel boundaries so that shadow edges will not shimmer
    Vector floorMinWorldsUnit = VectorHelper::Floor(lightViewSpaceMin / worldUnitsPerTexel);
    Vector xyLightMin = floorMinWorldsUnit * worldUnitsPerTexel;
    ionFloat xyLightMinX = VectorHelper::ExtractElement_0(xyLightMin);
    ionFloat xyLightMinY = VectorHelper::ExtractElement_1(xyLightMin);
    ionFloat Zmin = VectorHelper::ExtractElement_2(lightViewSpaceMin);
    lightViewSpaceMin = Vector(xyLightMinX, xyLightMinY, Zmin);

    Vector floorMaxWorldsUnit = VectorHelper::Floor(lightViewSpaceMax / worldUnitsPerTexel);
    Vector xyLightMax = floorMaxWorldsUnit * worldUnitsPerTexel;
    ionFloat xyLightMaxX = VectorHelper::ExtractElement_0(xyLightMax);
    ionFloat xyLightMaxY = VectorHelper::ExtractElement_1(xyLightMax);
    lightViewSpaceMax = Vector(xyLightMaxX, xyLightMaxY, fZNear);

    ionFloat lightViewSpaceMinX = VectorHelper::ExtractElement_0(lightViewSpaceMin);
    ionFloat lightViewSpaceMinY = VectorHelper::ExtractElement_1(lightViewSpaceMin);
    ionFloat lightViewSpaceMinZ = VectorHelper::ExtractElement_2(lightViewSpaceMin);
    ionFloat lightViewSpaceMaxX = VectorHelper::ExtractElement_0(lightViewSpaceMax);
    ionFloat lightViewSpaceMaxY = VectorHelper::ExtractElement_1(lightViewSpaceMax);
    ionFloat lightViewSpaceMaxZ = VectorHelper::ExtractElement_2(lightViewSpaceMax);

    m_cascadeScale = Vector(2.f / (lightViewSpaceMaxX - lightViewSpaceMinX), -2.f / (lightViewSpaceMaxY - lightViewSpaceMinY), -1.f / (lightViewSpaceMaxZ - lightViewSpaceMinZ));

    ionFloat cascadeScaleX = VectorHelper::ExtractElement_0(m_cascadeScale);
    ionFloat cascadeScaleY = VectorHelper::ExtractElement_1(m_cascadeScale);
    ionFloat cascadeScaleZ = VectorHelper::ExtractElement_2(m_cascadeScale);

    m_cascadeOffset = Vector(-0.5f * (lightViewSpaceMaxX + lightViewSpaceMinX) * cascadeScaleX, -0.5f * (lightViewSpaceMaxY + lightViewSpaceMinY) * cascadeScaleY, -lightViewSpaceMaxZ * cascadeScaleZ);
}

void DirectionalLight::SetColor(const Vector& _color)
{
    m_color = _color;
}

void DirectionalLight::SetPercentageCloserFilteringSize(ionU32 _percentageCloserFilteringSize)
{
    m_percentageCloserFilteringKernelSize = _percentageCloserFilteringSize;
}


Matrix DirectionalLight::GetLightViewProjMatrix() const
{
    /*
    ionFloat cascadeOffsetX = VectorHelper::ExtractElement_0(m_cascadeOffset);
    ionFloat cascadeOffsetY = VectorHelper::ExtractElement_1(m_cascadeOffset);
    ionFloat cascadeOffsetZ = VectorHelper::ExtractElement_2(m_cascadeOffset);

    ionFloat cascadeScaleX = VectorHelper::ExtractElement_0(m_cascadeScale);
    ionFloat cascadeScaleY = VectorHelper::ExtractElement_1(m_cascadeScale);
    ionFloat cascadeScaleZ = VectorHelper::ExtractElement_2(m_cascadeScale);

    Matrix projection(  cascadeScaleX,  0.0f,           0.0f,           0.0f,
                        0.0f,           cascadeScaleY,  0.0f,           0.0f,
                        0.0f,           0.0f,           cascadeScaleZ,  0.0f,
                        cascadeOffsetX, cascadeOffsetY, cascadeOffsetZ, 1.0f);

    return projection * m_view;
    */

    Quaternion rotation;
    Transform lightViewProjMatrix(m_cascadeOffset, m_cascadeScale, rotation);

    lightViewProjMatrix.UpdateTransform();
    return lightViewProjMatrix.GetMatrix();
}


ION_NAMESPACE_END