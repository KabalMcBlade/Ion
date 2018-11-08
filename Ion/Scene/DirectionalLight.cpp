#include "DirectionalLight.h"

#define ION_BASE_CAMERA_NAME "DirectionalLight"


EOS_USING_NAMESPACE
NIX_USING_NAMESPACE

ION_NAMESPACE_BEGIN

#define ION_BASE_DIRECTIONAL_LIGHT  "DirectionalLight"

DirectionalLight::DirectionalLight() :  Node(ION_BASE_DIRECTIONAL_LIGHT)
{
    m_nodeType = ENodeType_DirectionalLight;
    m_color = kOne.m_simdf;
}

DirectionalLight::DirectionalLight(const eosString & _name) : Node(_name)
{
    m_nodeType = ENodeType_DirectionalLight;
    m_color = kOne.m_simdf;
}

DirectionalLight::~DirectionalLight()
{

}

void DirectionalLight::SetColor(ionFloat _r, ionFloat _g, ionFloat _b, ionFloat _a)
{
    m_color = Helper::Set(_r, _g, _b, _a);
}

Vector DirectionalLight::GetLightDirection() const
{
    static const Vector forward(0.0f, 0.0f, 1.0, 0.0f);
    return GetTransform().GetRotation() * forward;
}


ION_NAMESPACE_END