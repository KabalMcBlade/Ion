#include "DirectionalLight.h"

#define ION_BASE_CAMERA_NAME "DirectionalLight"


EOS_USING_NAMESPACE
NIX_USING_NAMESPACE

ION_NAMESPACE_BEGIN

#define ION_BASE_DIRECTIONAL_LIGHT  "DirectionalLight"

DirectionalLight::DirectionalLight() :  Node(ION_BASE_DIRECTIONAL_LIGHT)
{
    m_nodeType = ENodeType_DirectionalLight;
    m_color = VectorHelper::GetOne();
    m_direction = VectorHelper::Get0001();
}

DirectionalLight::DirectionalLight(const eosString & _name) : Node(_name)
{
    m_nodeType = ENodeType_DirectionalLight;
    m_color = VectorHelper::GetOne();
    m_direction = VectorHelper::Get0001();
}

DirectionalLight::~DirectionalLight()
{

}

void DirectionalLight::SetDirection(ionFloat _x, ionFloat _y, ionFloat _z)
{
    m_direction = Vector(_x, _y, _z, 1.0f);
}

void DirectionalLight::SetColor(ionFloat _r, ionFloat _g, ionFloat _b, ionFloat _a)
{
    m_color = Vector(_r, _g, _b, _a);
}

Vector DirectionalLight::GetLightDirection() const
{
    return GetTransform().GetRotation() * m_direction;
}


ION_NAMESPACE_END