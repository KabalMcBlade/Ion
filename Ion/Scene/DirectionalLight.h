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
    explicit DirectionalLight();
    explicit DirectionalLight(const eosString & _name);
    virtual ~DirectionalLight();

    void SetDirection(const Vector& _dir) { m_direction = _dir; }
    void SetDirection(ionFloat _x, ionFloat _y, ionFloat _z);
    void SetColor(const Vector& _color) { m_color = _color; }
    void SetColor(ionFloat _r, ionFloat _g, ionFloat _b, ionFloat _a);

    const Vector& GetDirection() const { return m_direction; }
    const Vector& GetColor() const { return m_color; }

    Vector GetLightDirection() const;

private:
    DirectionalLight(const DirectionalLight& _Orig) = delete;
    DirectionalLight& operator = (const DirectionalLight&) = delete;

private:
    Vector m_direction;
    Vector m_color;
};

ION_NAMESPACE_END