#include "Skybox.h"


#include "../Renderer/VertexCacheManager.h"

#include "../Renderer/RenderCore.h"

#include "../Geometry/Mesh.h"

EOS_USING_NAMESPACE
NIX_USING_NAMESPACE

ION_NAMESPACE_BEGIN

Skybox::Skybox()
{
    m_mesh = eosNew(MeshPlain, ION_MEMORY_ALIGNMENT_SIZE);

    GenerateMesh();
}

Skybox::~Skybox()
{
    eosDelete(m_mesh);
}

void Skybox::GenerateMesh()
{
    eosVector(Index) indices;
    indices.resize(36);
    indices = {
        0,1,2,0,2,3,
        4,5,6,4,6,7,
        8,9,10,8,10,11,
        12,13,14,12,14,15,
        16,17,18,16,18,19,
        20,21,22,20,22,23 };

    Vector positions[24] = {
        Vector(-1.0f, 1.0f, 1.0f, 1.0f),
        Vector(-1.0f, -1.0f, 1.0f, 1.0f),
        Vector(1.0f, -1.0f, 1.0f, 1.0f),
        Vector(1.0f, 1.0f, 1.0f, 1.0f),

        Vector(1.0f, 1.0f, -1.0f, 1.0f),
        Vector(1.0f, -1.0f, -1.0f, 1.0f),
        Vector(-1.0f, -1.0f, -1.0f, 1.0f),
        Vector(-1.0f, 1.0f, -1.0f, 1.0f),

        Vector(1.0f, 1.0f, 1.0f, 1.0f),
        Vector(1.0f, -1.0f, 1.0f, 1.0f),
        Vector(1.0f, -1.0f, -1.0f, 1.0f),
        Vector(1.0f, 1.0f, -1.0f, 1.0f),

        Vector(-1.0f, 1.0f, -1.0f, 1.0f),
        Vector(-1.0f, 1.0f, 1.0f, 1.0f),
        Vector(1.0f, 1.0f, 1.0f, 1.0f),
        Vector(1.0f, 1.0f, -1.0f, 1.0f),

        Vector(-1.0f, 1.0f, -1.0f, 1.0f),
        Vector(-1.0f, -1.0f, -1.0f, 1.0f),
        Vector(-1.0f, -1.0f, 1.0f, 1.0f),
        Vector(-1.0f, 1.0f, 1.0f, 1.0f),

        Vector(-1.0f, -1.0f, 1.0f, 1.0f),
        Vector(-1.0f, -1.0f, -1.0f, 1.0f),
        Vector(1.0f, -1.0f, -1.0f, 1.0f),
        Vector(1.0f, -1.0f, 1.0f, 1.0f)
    };

    eosVector(VertexPlain) vertices;
    vertices.resize(24);

    for (ionU32 i = 0; i < 24; ++i)
    {
        vertices[i].SetPosition(positions[i]);
    }
    for (ionU32 i = 0; i < 24; ++i)
    {
        m_mesh->PushBackVertex(vertices[i]);
    }
    for (ionU32 i = 0; i < 36; ++i)
    {
        m_mesh->PushBackIndex(indices[i]);
    }

    m_mesh->SetIndexCount(36);
    m_mesh->SetIndexStart(0);
}

void Skybox::SetMaterial(Material* _material)
{
    m_mesh->SetMaterial(_material);
}

void Skybox::Draw(RenderCore& _renderCore, const Matrix& _projection, const Matrix& _view, const Matrix& _model)
{
    DrawSurface drawSurface;

    _mm_storeu_ps(&drawSurface.m_modelMatrix[0], _model[0]);
    _mm_storeu_ps(&drawSurface.m_modelMatrix[4], _model[1]);
    _mm_storeu_ps(&drawSurface.m_modelMatrix[8], _model[2]);
    _mm_storeu_ps(&drawSurface.m_modelMatrix[12], _model[3]);

    _mm_storeu_ps(&drawSurface.m_viewMatrix[0], _view[0]);
    _mm_storeu_ps(&drawSurface.m_viewMatrix[4], _view[1]);
    _mm_storeu_ps(&drawSurface.m_viewMatrix[8], _view[2]);
    _mm_storeu_ps(&drawSurface.m_viewMatrix[12], _view[3]);

    _mm_storeu_ps(&drawSurface.m_projectionMatrix[0], _projection[0]);
    _mm_storeu_ps(&drawSurface.m_projectionMatrix[4], _projection[1]);
    _mm_storeu_ps(&drawSurface.m_projectionMatrix[8], _projection[2]);
    _mm_storeu_ps(&drawSurface.m_projectionMatrix[12], _projection[3]);

    drawSurface.m_visible = true;
    drawSurface.m_indexStart = m_mesh->GetIndexStart();
    drawSurface.m_indexCount = m_mesh->GetIndexCount();
    drawSurface.m_vertexCache = ionVertexCacheManager().AllocVertex(m_mesh->GetVertexData(), m_mesh->GetVertexSize());
    drawSurface.m_indexCache = ionVertexCacheManager().AllocIndex(m_mesh->GetIndexData(), m_mesh->GetIndexSize());
    drawSurface.m_material = m_mesh->GetMaterial();

    _renderCore.SetState(m_mesh->GetMaterial()->GetState().GetStateBits());
    _renderCore.Draw(drawSurface);
}

ION_NAMESPACE_END