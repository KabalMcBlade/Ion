#include "PrimitiveFactory.h"

#include "../Dependencies/Eos/Eos/Eos.h"

#include "../Core/CoreDefs.h"

#include "../Renderer/RenderDefs.h"

#include "../Scene/Entity.h"

#include "../Geometry/Mesh.h"

#include "../Renderer/RenderCommon.h"


EOS_USING_NAMESPACE


ION_NAMESPACE_BEGIN


void PrimitiveFactory::GenerateTriangle(EVertexLayout _layout, Entity& _entity)
{
    switch(_layout)
    {
    case EVertexLayout_Pos:
    {
        MeshPlain* mesh = _entity.AddMesh<MeshPlain>();

        eosVector(VertexPlain) vertices;
        eosVector(Index) indices;
        vertices.resize(3);
        indices.resize(3);
        indices = { 0, 1, 2 };

        Vector positions[3] = { Vector(0.0f, -0.5f, 0.0f, 1.0f), Vector(0.5f, 0.5f, 0.0f, 1.0f), Vector(-0.5f, 0.5f, 0.0f, 1.0f) };

        vertices[0].SetPosition(positions[0]);
        vertices[1].SetPosition(positions[1]);
        vertices[2].SetPosition(positions[2]);

        mesh->PushBackVertex(vertices[0]);
        mesh->PushBackVertex(vertices[1]);
        mesh->PushBackVertex(vertices[2]);

        mesh->PushBackIndex(indices[0]);
        mesh->PushBackIndex(indices[1]);
        mesh->PushBackIndex(indices[2]);

        mesh->SetIndexCount(3);
        mesh->SetIndexStart(0);
        mesh->SetIndexType(VK_INDEX_TYPE_UINT32);
    }
    break;

    case EVertexLayout_Pos_Color:
    {
        MeshColored* mesh = _entity.AddMesh<MeshColored>();

        eosVector(VertexColored) vertices;
        eosVector(Index) indices;
        vertices.resize(3);
        indices.resize(3);
        indices = { 0, 1, 2 };

        Vector positions[3] = { Vector(0.0f, -0.5f, 0.0f, 1.0f), Vector(0.5f, 0.5f, 0.0f, 1.0f), Vector(-0.5f, 0.5f, 0.0f, 1.0f) };

        vertices[0].SetPosition(positions[0]);
        vertices[1].SetPosition(positions[1]);
        vertices[2].SetPosition(positions[2]);

        vertices[0].SetColor(1.0f, 0.0f, 0.0f, 1.0f);
        vertices[1].SetColor(0.0f, 1.0f, 0.0f, 1.0f);
        vertices[2].SetColor(0.0f, 0.0f, 1.0f, 1.0f);

        mesh->PushBackVertex(vertices[0]);
        mesh->PushBackVertex(vertices[1]);
        mesh->PushBackVertex(vertices[2]);

        mesh->PushBackIndex(indices[0]);
        mesh->PushBackIndex(indices[1]);
        mesh->PushBackIndex(indices[2]);

        mesh->SetIndexCount(3);
        mesh->SetIndexStart(0);
        mesh->SetIndexType(VK_INDEX_TYPE_UINT32);
    }
    break;

    case EVertexLayout_Pos_UV:
    {
        MeshUV* mesh = _entity.AddMesh<MeshUV>();

        eosVector(VertexUV) vertices;
        eosVector(Index) indices;
        vertices.resize(3);
        indices.resize(3);
        indices = { 0, 1, 2 };

        Vector positions[3] = { Vector(0.0f, -0.5f, 0.0f, 1.0f), Vector(0.5f, 0.5f, 0.0f, 1.0f), Vector(-0.5f, 0.5f, 0.0f, 1.0f) };

        vertices[0].SetPosition(positions[0]);
        vertices[1].SetPosition(positions[1]);
        vertices[2].SetPosition(positions[2]);

        vertices[0].SetTexCoordUV(0.0f, 0.0f);
        vertices[1].SetTexCoordUV(1.0f, 0.0f);
        vertices[2].SetTexCoordUV(0.0f, 1.0f);

        mesh->PushBackVertex(vertices[0]);
        mesh->PushBackVertex(vertices[1]);
        mesh->PushBackVertex(vertices[2]);

        mesh->PushBackIndex(indices[0]);
        mesh->PushBackIndex(indices[1]);
        mesh->PushBackIndex(indices[2]);

        mesh->SetIndexCount(3);
        mesh->SetIndexStart(0);
        mesh->SetIndexType(VK_INDEX_TYPE_UINT32);
    }
    break;

    default:
        ionAssertReturnVoid(false, "Layout not yet implemented");
        break;
    }
}

void PrimitiveFactory::GenerateQuad(EVertexLayout _layout, Entity& _entity)
{
    switch (_layout)
    {
    case EVertexLayout_Pos:
    {
        MeshPlain* mesh = _entity.AddMesh<MeshPlain>();

        eosVector(VertexPlain) vertices;
        eosVector(Index) indices;
        vertices.resize(4);
        indices.resize(6);
        indices = { 0, 1, 2, 2, 3, 0 };

        Vector positions[4] = { Vector(-0.5f, -0.5f, 0.0f, 1.0f), Vector(0.5f, -0.5f, 0.0f, 1.0f), Vector(0.5f, 0.5f, 0.0f, 1.0f), Vector(-0.5f, 0.5f, 0.0f, 1.0f) };

        vertices[0].SetPosition(positions[0]);
        vertices[1].SetPosition(positions[1]);
        vertices[2].SetPosition(positions[2]);
        vertices[3].SetPosition(positions[3]);

        mesh->PushBackVertex(vertices[0]);
        mesh->PushBackVertex(vertices[1]);
        mesh->PushBackVertex(vertices[2]);
        mesh->PushBackVertex(vertices[3]);

        mesh->PushBackIndex(indices[0]);
        mesh->PushBackIndex(indices[1]);
        mesh->PushBackIndex(indices[2]);
        mesh->PushBackIndex(indices[3]);
        mesh->PushBackIndex(indices[4]);
        mesh->PushBackIndex(indices[5]);

        mesh->SetIndexCount(6);
        mesh->SetIndexStart(0);
        mesh->SetIndexType(VK_INDEX_TYPE_UINT32);
    }
    break;
    
    case EVertexLayout_Pos_Color:
    {
        MeshColored* mesh = _entity.AddMesh<MeshColored>();

        eosVector(VertexColored) vertices;
        eosVector(Index) indices;
        vertices.resize(4);
        indices.resize(6);
        indices = { 0, 1, 2, 2, 3, 0 };

        Vector positions[4] = { Vector(-0.5f, -0.5f, 0.0f, 1.0f), Vector(0.5f, -0.5f, 0.0f, 1.0f), Vector(0.5f, 0.5f, 0.0f, 1.0f), Vector(-0.5f, 0.5f, 0.0f, 1.0f) };

        vertices[0].SetPosition(positions[0]);
        vertices[1].SetPosition(positions[1]);
        vertices[2].SetPosition(positions[2]);
        vertices[3].SetPosition(positions[3]);

        vertices[0].SetColor(1.0f, 0.0f, 0.0f, 1.0f);
        vertices[1].SetColor(0.0f, 1.0f, 0.0f, 1.0f);
        vertices[2].SetColor(0.0f, 0.0f, 1.0f, 1.0f);
        vertices[3].SetColor(1.0f, 1.0f, 1.0f, 1.0f);

        mesh->PushBackVertex(vertices[0]);
        mesh->PushBackVertex(vertices[1]);
        mesh->PushBackVertex(vertices[2]);
        mesh->PushBackVertex(vertices[3]);

        mesh->PushBackIndex(indices[0]);
        mesh->PushBackIndex(indices[1]);
        mesh->PushBackIndex(indices[2]);
        mesh->PushBackIndex(indices[3]);
        mesh->PushBackIndex(indices[4]);
        mesh->PushBackIndex(indices[5]);

        mesh->SetIndexCount(6);
        mesh->SetIndexStart(0);
        mesh->SetIndexType(VK_INDEX_TYPE_UINT32);
    }
    break;

    case EVertexLayout_Pos_UV:
    {
        MeshUV* mesh = _entity.AddMesh<MeshUV>();

        eosVector(VertexUV) vertices;
        eosVector(Index) indices;
        vertices.resize(4);
        indices.resize(6);
        indices = { 0, 1, 2, 2, 3, 0 };

        Vector positions[4] = { Vector(-0.5f, -0.5f, 0.0f, 1.0f), Vector(0.5f, -0.5f, 0.0f, 1.0f), Vector(0.5f, 0.5f, 0.0f, 1.0f), Vector(-0.5f, 0.5f, 0.0f, 1.0f) };

        vertices[0].SetPosition(positions[0]);
        vertices[1].SetPosition(positions[1]);
        vertices[2].SetPosition(positions[2]);
        vertices[3].SetPosition(positions[3]);

        vertices[0].SetTexCoordUV(1.0f, 0.0f);
        vertices[1].SetTexCoordUV(0.0f, 0.0f);
        vertices[2].SetTexCoordUV(0.0f, 1.0f);
        vertices[3].SetTexCoordUV(1.0f, 1.0f);

        mesh->PushBackVertex(vertices[0]);
        mesh->PushBackVertex(vertices[1]);
        mesh->PushBackVertex(vertices[2]);
        mesh->PushBackVertex(vertices[3]);

        mesh->PushBackIndex(indices[0]);
        mesh->PushBackIndex(indices[1]);
        mesh->PushBackIndex(indices[2]);
        mesh->PushBackIndex(indices[3]);
        mesh->PushBackIndex(indices[4]);
        mesh->PushBackIndex(indices[5]);

        mesh->SetIndexCount(6);
        mesh->SetIndexStart(0);
        mesh->SetIndexType(VK_INDEX_TYPE_UINT32);
    }
    break;

    default:
        ionAssertReturnVoid(false, "Layout not yet implemented");
        break;
    }
}


ION_NAMESPACE_END