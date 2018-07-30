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
    explicit Skybox();
    explicit Skybox(const eosString& _name);
    virtual ~Skybox();


    void SetLayout(EVertexLayout _layout);
    void SetCubemap(const Texture* _cubemap);
    void SetShaders(const eosString& _shaderProgramName, const ConstantsBindingDef& _constants, ionS32 _vertexIndex = -1, ionS32 _fragmentIndex = -1, ionS32 _tessellationControlIndex = -1, ionS32 _tessellationEvaluationIndex = -1, ionS32 _geometryIndex = -1);

    const Texture* GetCubemapTexture() const;

    const MaterialState& GetMaterialState() const;
    MaterialState& GetMaterialState();

private:
    Skybox(const Skybox& _Orig) = delete;
    Skybox& operator = (const Skybox&) = delete;

};

ION_NAMESPACE_END