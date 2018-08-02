#pragma once

#include "../Core/CoreDefs.h"

#include "../Dependencies/Eos/Eos/Eos.h"

#include "Entity.h"

#include "../Texture/TextureCommon.h"

EOS_USING_NAMESPACE

ION_NAMESPACE_BEGIN

class RenderCore;
class ION_DLL Skybox
{
public:
    explicit Skybox();
    virtual ~Skybox();

    EVertexLayout GetVertexLayout() const { return EVertexLayout_Pos; }

    void SetMaterial(Material* _material);

    void Draw(RenderCore& _renderCore, const Matrix& _projection, const Matrix& _view, const Matrix& _model);

private:
    Skybox(const Skybox& _Orig) = delete;
    Skybox& operator = (const Skybox&) = delete;

private:
    void GenerateMesh();

private:
    MeshPlain* m_mesh;
};

ION_NAMESPACE_END