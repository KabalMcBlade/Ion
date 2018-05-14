#pragma once

#include "../Core/CoreDefs.h"

#include "../Dependencies/Eos/Eos/Eos.h"
#include "../Dependencies/Nix/Nix/Nix.h"

#include "Node.h"
#include "BaseCamera.h"
#include "../Geometry/BoundingBox.h"

EOS_USING_NAMESPACE
NIX_USING_NAMESPACE

ION_NAMESPACE_BEGIN

class DirectionalLight;
typedef SmartPointer<DirectionalLight> DirectionalLightHandle;

class ION_DLL DirectionalLight : public Node
{
public:
    explicit DirectionalLight();
    explicit DirectionalLight(const Vector& _color, ionU32 _percentageCloserFilteringSize = 3);
    virtual ~DirectionalLight();

    void UpdateViewMatrix(); // call every time change any of the transform!
    void ComputeScaleAndOffset(const BaseCamera& _camera, const BoundingBox& _boundingBox, ionU32 _shadowMapSize);

    void SetColor(const Vector& _color);
    void SetPercentageCloserFilteringSize(ionU32 _percentageCloserFilteringSize);

    Matrix GetLightViewProjMatrix() const;
    const Vector& GetColor() const { return m_color; }
    ionU32 GetPercentageCloserFilteringKernelSize() const { return m_percentageCloserFilteringKernelSize; }

private:
    DirectionalLight(const DirectionalLight& _Orig) = delete;
    DirectionalLight& operator = (const DirectionalLight&) = delete;

private:
    Vector m_color;
  
    Vector m_cascadeScale;
    Vector m_cascadeOffset;

    Matrix m_view;

    ionU32 m_percentageCloserFilteringKernelSize;
};

ION_NAMESPACE_END