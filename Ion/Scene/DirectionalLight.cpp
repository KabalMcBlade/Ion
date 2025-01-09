// Copyright (c) 2025-2025 Michele Condo'
// File: C:\Projects\Ion\Ion\Scene\DirectionalLight.cpp
// Licensed under the GPL-3.0 License. See LICENSE file in the project root for full license information.


#include "DirectionalLight.h"

#define ION_BASE_CAMERA_NAME "DirectionalLight"


EOS_USING_NAMESPACE
NIX_USING_NAMESPACE

ION_NAMESPACE_BEGIN

#define ION_BASE_DIRECTIONAL_LIGHT  "DirectionalLight"

DirectionalLight::DirectionalLight() :  Node(ION_BASE_DIRECTIONAL_LIGHT)
{
    m_nodeType = ENodeType_DirectionalLight;
    m_color = Vector4(kOneVec4);
}

DirectionalLight::DirectionalLight(const ionString & _name) : Node(_name)
{
    m_nodeType = ENodeType_DirectionalLight;
    m_color = Vector4(kOneVec4);
}

DirectionalLight::~DirectionalLight()
{

}

void DirectionalLight::SetColor(ionFloat _r, ionFloat _g, ionFloat _b, ionFloat _a)
{
    m_color = MathFunctions::Set(_r, _g, _b, _a);
}

Vector4 DirectionalLight::GetLightDirection() const
{
    static const Vector4 forward(0.0f, 0.0f, 1.0, 0.0f);
    return GetTransform().GetRotation() * forward;
}


ION_NAMESPACE_END