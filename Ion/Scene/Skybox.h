#pragma once

#include "../Core/CoreDefs.h"

#include "../Dependencies/Eos/Eos/Eos.h"

#include "Entity.h"

#include "../Texture/TextureCommon.h"

EOS_USING_NAMESPACE

ION_NAMESPACE_BEGIN

class ION_DLL Skybox : public Entity
{
public:
    explicit Skybox(const eosString& _filepath, const eosString& _filename, EVertexLayout _layout,
        const eosString& _vertexShaderPath, const eosString& _vertexShadername, const eosString& _fragmentShaderPath, const eosString& _fragmentShadername);

    virtual ~Skybox();

    MaterialState& GetMaterialState();

private:
    Skybox(const Skybox& _Orig) = delete;
    Skybox& operator = (const Skybox&) = delete;
};

ION_NAMESPACE_END