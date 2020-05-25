#pragma once

#include "../Core/CoreDefs.h"

#include "../Dependencies/Eos/Eos/Eos.h"
#include "../Dependencies/Nix/Nix/Nix.h"

#include "Node.h"

EOS_USING_NAMESPACE
NIX_USING_NAMESPACE

ION_NAMESPACE_BEGIN

class ION_DLL DirectionalLight : public Node
{
public:
    DirectionalLight();
    explicit DirectionalLight(const ionString& _name);
    virtual ~DirectionalLight();

    void SetColor(const Vector4& _color) { m_color = _color; }
    void SetColor(ionFloat _r, ionFloat _g, ionFloat _b, ionFloat _a);

    const Vector4& GetColor() const { return m_color; }

    Vector4 GetLightDirection() const;

private:
    DirectionalLight(const DirectionalLight& _Orig) = delete;
    DirectionalLight& operator = (const DirectionalLight&) = delete;

private:
    Vector4 m_color;
};

ION_NAMESPACE_END