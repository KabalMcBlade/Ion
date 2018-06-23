#include "PrimitiveFactory.h"

#include "../Dependencies/Eos/Eos/Eos.h"

#include "../Core/CoreDefs.h"

#include "../Renderer/RenderDefs.h"

#include "../Scene/Entity.h"

#include "../Geometry/Mesh.h"

#include "../Renderer/RenderCommon.h"

#include "../Utilities/GeometryHelper.h"

EOS_USING_NAMESPACE


ION_NAMESPACE_BEGIN

///
///            TRIANGLE
///
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

        Vector positions[3] = { Vector(0.0f, 0.5f, 0.0f, 1.0f), Vector(0.5f, -0.5f, 0.0f, 1.0f), Vector(-0.5f, -0.5f, 0.0f, 1.0f) };

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

        Vector positions[3] = { Vector(0.0f, 0.5f, 0.0f, 1.0f), Vector(0.5f, -0.5f, 0.0f, 1.0f), Vector(-0.5f, -0.5f, 0.0f, 1.0f) };

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

        Vector positions[3] = { Vector(0.0f, 0.5f, 0.0f, 1.0f), Vector(0.5f, -0.5f, 0.0f, 1.0f), Vector(-0.5f, -0.5f, 0.0f, 1.0f) };

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
    }
    break;
    
    case EVertexLayout_Pos_UV_Normal:
    {
        MeshSimple* mesh = _entity.AddMesh<MeshSimple>();

        eosVector(VertexSimple) vertices;
        eosVector(Index) indices;
        vertices.resize(3);
        indices.resize(3);
        indices = { 0, 1, 2 };

        Vector positions[3] = { Vector(0.0f, 0.5f, 0.0f, 1.0f), Vector(0.5f, -0.5f, 0.0f, 1.0f), Vector(-0.5f, -0.5f, 0.0f, 1.0f) };

        vertices[0].SetPosition(positions[0]);
        vertices[1].SetPosition(positions[1]);
        vertices[2].SetPosition(positions[2]);

        vertices[0].SetTexCoordUV(0.0f, 0.0f);
        vertices[1].SetTexCoordUV(1.0f, 0.0f);
        vertices[2].SetTexCoordUV(0.0f, 1.0f);

        Vector normal0 = GeometryHelper::CalculateSurfaceNormalTriangle(&positions[0], 0);
        Vector normal1 = GeometryHelper::CalculateSurfaceNormalTriangle(&positions[0], 1);
        Vector normal2 = GeometryHelper::CalculateSurfaceNormalTriangle(&positions[0], 2);

        vertices[0].SetNormal(normal0);
        vertices[1].SetNormal(normal1);
        vertices[2].SetNormal(normal2);

        mesh->PushBackVertex(vertices[0]);
        mesh->PushBackVertex(vertices[1]);
        mesh->PushBackVertex(vertices[2]);

        mesh->PushBackIndex(indices[0]);
        mesh->PushBackIndex(indices[1]);
        mesh->PushBackIndex(indices[2]);

        mesh->SetIndexCount(3);
        mesh->SetIndexStart(0);
    }
    break;

    case EVertexLayout_Full:
    {
        Mesh* mesh = _entity.AddMesh<Mesh>();

        eosVector(Vertex) vertices;
        eosVector(Index) indices;
        vertices.resize(3);
        indices.resize(3);
        indices = { 0, 1, 2 };

        Vector positions[3] = { Vector(0.0f, 0.5f, 0.0f, 1.0f), Vector(0.5f, -0.5f, 0.0f, 1.0f), Vector(-0.5f, -0.5f, 0.0f, 1.0f) };

        vertices[0].SetPosition(positions[0]);
        vertices[1].SetPosition(positions[1]);
        vertices[2].SetPosition(positions[2]);

        Vector uvuv[3] = { Vector(0.0f, 0.0f, 0.0f, 0.0f),  Vector(1.0f, 0.0f, 1.0f, 0.0f),  Vector(0.0f, 1.0f, 0.0f, 1.0f) };

        vertices[0].SetTexCoordUV0(uvuv[0]);
        vertices[1].SetTexCoordUV0(uvuv[1]);
        vertices[2].SetTexCoordUV0(uvuv[2]);

        Vector normals[3] =
        {
            GeometryHelper::CalculateSurfaceNormalTriangle(&positions[0], 0),
            GeometryHelper::CalculateSurfaceNormalTriangle(&positions[0], 1),
            GeometryHelper::CalculateSurfaceNormalTriangle(&positions[0], 2)
        };

        vertices[0].SetNormal(normals[0]);
        vertices[1].SetNormal(normals[1]);
        vertices[2].SetNormal(normals[2]);

        vertices[0].SetColor(1.0f, 0.0f, 0.0f, 1.0f);
        vertices[1].SetColor(0.0f, 1.0f, 0.0f, 1.0f);
        vertices[2].SetColor(0.0f, 0.0f, 1.0f, 1.0f);

        Vector tangents[3];
        GeometryHelper::CalculateTangent(positions, normals, uvuv, 3, indices.data(), 3, tangents);

        vertices[0].SetTangent(tangents[0]);
        vertices[1].SetTangent(tangents[1]);
        vertices[2].SetTangent(tangents[2]);

        //vertices[0].SetBiTangent(tangents[0]);
        //vertices[1].SetBiTangent(tangents[1]);
        //vertices[2].SetBiTangent(tangents[2]);

        mesh->PushBackVertex(vertices[0]);
        mesh->PushBackVertex(vertices[1]);
        mesh->PushBackVertex(vertices[2]);

        mesh->PushBackIndex(indices[0]);
        mesh->PushBackIndex(indices[1]);
        mesh->PushBackIndex(indices[2]);

        mesh->SetIndexCount(3);
        mesh->SetIndexStart(0);
    }
    break;

    case EVertexLayout_Unknow:
    case EVertexLayout_Empty:
    case EVertexLayout_Count:
        ionAssertReturnVoid(false, "Layout does not exist!");
    break;

    default:
        ionAssertReturnVoid(false, "Layout not yet implemented");
        break;
    }
}


///
///            QUAD
///
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

        Vector positions[4] = { Vector(0.5f, 0.5f, 0.0f, 1.0f), Vector(-0.5f, 0.5f, 0.0f, 1.0f), Vector(-0.5f, -0.5f, 0.0f, 1.0f), Vector(0.5f, -0.5f, 0.0f, 1.0f) };

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

        Vector positions[4] = { Vector(0.5f, 0.5f, 0.0f, 1.0f), Vector(-0.5f, 0.5f, 0.0f, 1.0f), Vector(-0.5f, -0.5f, 0.0f, 1.0f), Vector(0.5f, -0.5f, 0.0f, 1.0f) };

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

        Vector positions[4] = { Vector(0.5f, 0.5f, 0.0f, 1.0f), Vector(-0.5f, 0.5f, 0.0f, 1.0f), Vector(-0.5f, -0.5f, 0.0f, 1.0f), Vector(0.5f, -0.5f, 0.0f, 1.0f) };

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
    }
    break;


    case EVertexLayout_Pos_UV_Normal:
    {
        MeshSimple* mesh = _entity.AddMesh<MeshSimple>();

        eosVector(VertexSimple) vertices;
        eosVector(Index) indices;
        vertices.resize(4);
        indices.resize(6);
        indices = { 0, 1, 2, 2, 3, 0 };

        Vector positions[4] = { Vector(0.5f, 0.5f, 0.0f, 1.0f), Vector(-0.5f, 0.5f, 0.0f, 1.0f), Vector(-0.5f, -0.5f, 0.0f, 1.0f), Vector(0.5f, -0.5f, 0.0f, 1.0f) };

        vertices[0].SetPosition(positions[0]);
        vertices[1].SetPosition(positions[1]);
        vertices[2].SetPosition(positions[2]);
        vertices[3].SetPosition(positions[3]);

        vertices[0].SetTexCoordUV(1.0f, 0.0f);
        vertices[1].SetTexCoordUV(0.0f, 0.0f);
        vertices[2].SetTexCoordUV(0.0f, 1.0f);
        vertices[3].SetTexCoordUV(1.0f, 1.0f);

        Vector normals[4];
        GeometryHelper::CalculateNormalPerVertex(positions, indices.data(), 6, normals);

        vertices[0].SetNormal(normals[0]);
        vertices[1].SetNormal(normals[1]);
        vertices[2].SetNormal(normals[2]);
        vertices[3].SetNormal(normals[3]);

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
    }
    break;

    case EVertexLayout_Full:
    {
        Mesh* mesh = _entity.AddMesh<Mesh>();

        eosVector(Vertex) vertices;
        eosVector(Index) indices;
        vertices.resize(4);
        indices.resize(6);
        indices = { 0, 1, 2, 2, 3, 0 };

        Vector positions[4] = { Vector(0.5f, 0.5f, 0.0f, 1.0f), Vector(-0.5f, 0.5f, 0.0f, 1.0f), Vector(-0.5f, -0.5f, 0.0f, 1.0f), Vector(0.5f, -0.5f, 0.0f, 1.0f) };

        vertices[0].SetPosition(positions[0]);
        vertices[1].SetPosition(positions[1]);
        vertices[2].SetPosition(positions[2]);
        vertices[3].SetPosition(positions[3]);

        vertices[0].SetTexCoordUV0(1.0f, 0.0f);
        vertices[1].SetTexCoordUV0(0.0f, 0.0f);
        vertices[2].SetTexCoordUV0(0.0f, 1.0f);
        vertices[3].SetTexCoordUV0(1.0f, 1.0f);

        Vector normals[4];
        GeometryHelper::CalculateNormalPerVertex(positions, indices.data(), 6, normals);

        vertices[0].SetNormal(normals[0]);
        vertices[1].SetNormal(normals[1]);
        vertices[2].SetNormal(normals[2]);
        vertices[3].SetNormal(normals[3]);

        Vector uvuv[4] = { Vector(0.5f, 0.5f, 0.0f, 1.0f), Vector(-0.5f, 0.5f, 0.0f, 1.0f), Vector(-0.5f, -0.5f, 0.0f, 1.0f), Vector(0.5f, -0.5f, 0.0f, 1.0f) };

        vertices[0].SetTexCoordUV0(uvuv[0]);
        vertices[1].SetTexCoordUV0(uvuv[1]);
        vertices[2].SetTexCoordUV0(uvuv[2]);
        vertices[3].SetTexCoordUV0(uvuv[3]);

        vertices[0].SetColor(1.0f, 0.0f, 0.0f, 1.0f);
        vertices[1].SetColor(0.0f, 1.0f, 0.0f, 1.0f);
        vertices[2].SetColor(0.0f, 0.0f, 1.0f, 1.0f);
        vertices[3].SetColor(1.0f, 1.0f, 1.0f, 1.0f);

        Vector tangents[4];
        GeometryHelper::CalculateTangent(positions, normals, uvuv, 4, indices.data(), 6, tangents);

        vertices[0].SetTangent(tangents[0]);
        vertices[1].SetTangent(tangents[1]);
        vertices[2].SetTangent(tangents[2]);
        vertices[3].SetTangent(tangents[3]);

        //vertices[0].SetBiTangent(tangents[0]);
        //vertices[1].SetBiTangent(tangents[1]);
        //vertices[2].SetBiTangent(tangents[2]);
        //vertices[3].SetBiTangent(tangents[3]);

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
    }
    break;

    case EVertexLayout_Unknow:
    case EVertexLayout_Empty:
    case EVertexLayout_Count:
        ionAssertReturnVoid(false, "Layout does not exist!");
        break;

    default:
        ionAssertReturnVoid(false, "Layout not yet implemented");
        break;
    }
}


///
///            CUBE
///
void PrimitiveFactory::GenerateCube(EVertexLayout _layout, Entity& _entity)
{
    switch (_layout)
    {
    case EVertexLayout_Pos:
    {
        MeshPlain* mesh = _entity.AddMesh<MeshPlain>();

        eosVector(VertexPlain) vertices;
        eosVector(Index) indices;
        vertices.resize(24);
        indices.resize(36);
        indices = { 
            0,1,2,0,2,3,
            4,5,6,4,6,7,
            8,9,10,8,10,11,
            12,13,14,12,14,15,
            16,17,18,16,18,19,
            20,21,22,20,22,23 };

        Vector positions[24] = { 
            Vector(0.0f, 1.0f, 1.0f, 1.0f),
            Vector(0.0f, 0.0f, 1.0f, 1.0f),
            Vector(1.0f, 0.0f, 1.0f, 1.0f),
            Vector(1.0f, 1.0f, 1.0f, 1.0f),

            Vector(1.0f, 1.0f, 0.0f, 1.0f),
            Vector(1.0f, 0.0f, 0.0f, 1.0f),
            Vector(0.0f, 0.0f, 0.0f, 1.0f),
            Vector(0.0f, 1.0f, 0.0f, 1.0f),

            Vector(1.0f, 1.0f, 1.0f, 1.0f),
            Vector(1.0f, 0.0f, 1.0f, 1.0f),
            Vector(1.0f, 0.0f, 0.0f, 1.0f),
            Vector(1.0f, 1.0f, 0.0f, 1.0f),

            Vector(0.0f, 1.0f, 0.0f, 1.0f),
            Vector(0.0f, 1.0f, 1.0f, 1.0f),
            Vector(1.0f, 1.0f, 1.0f, 1.0f),
            Vector(1.0f, 1.0f, 0.0f, 1.0f),

            Vector(0.0f, 1.0f, 0.0f, 1.0f),
            Vector(0.0f, 0.0f, 0.0f, 1.0f),
            Vector(0.0f, 0.0f, 1.0f, 1.0f),
            Vector(0.0f, 1.0f, 1.0f, 1.0f),

            Vector(0.0f, 0.0f, 1.0f, 1.0f),
            Vector(0.0f, 0.0f, 0.0f, 1.0f),
            Vector(1.0f, 0.0f, 0.0f, 1.0f),
            Vector(1.0f, 0.0f, 1.0f, 1.0f)
        };

        for (ionU32 i = 0; i < 24; ++i)
        {
            vertices[i].SetPosition(positions[i]);
        }
        for (ionU32 i = 0; i < 24; ++i)
        {
            mesh->PushBackVertex(vertices[i]);
        }
        for (ionU32 i = 0; i < 36; ++i)
        {
            mesh->PushBackIndex(indices[i]);
        }

        mesh->SetIndexCount(36);
        mesh->SetIndexStart(0);
    }
    break;

    case EVertexLayout_Pos_Color:
    {
        MeshColored* mesh = _entity.AddMesh<MeshColored>();

        eosVector(VertexColored) vertices;
        eosVector(Index) indices;
        vertices.resize(24);
        indices.resize(36);
        indices = {
            0,1,2,0,2,3,
            4,5,6,4,6,7,
            8,9,10,8,10,11,
            12,13,14,12,14,15,
            16,17,18,16,18,19,
            20,21,22,20,22,23 };

        Vector positions[24] = {
            Vector(0.0f, 1.0f, 1.0f, 1.0f),
            Vector(0.0f, 0.0f, 1.0f, 1.0f),
            Vector(1.0f, 0.0f, 1.0f, 1.0f),
            Vector(1.0f, 1.0f, 1.0f, 1.0f),

            Vector(1.0f, 1.0f, 0.0f, 1.0f),
            Vector(1.0f, 0.0f, 0.0f, 1.0f),
            Vector(0.0f, 0.0f, 0.0f, 1.0f),
            Vector(0.0f, 1.0f, 0.0f, 1.0f),

            Vector(1.0f, 1.0f, 1.0f, 1.0f),
            Vector(1.0f, 0.0f, 1.0f, 1.0f),
            Vector(1.0f, 0.0f, 0.0f, 1.0f),
            Vector(1.0f, 1.0f, 0.0f, 1.0f),

            Vector(0.0f, 1.0f, 0.0f, 1.0f),
            Vector(0.0f, 1.0f, 1.0f, 1.0f),
            Vector(1.0f, 1.0f, 1.0f, 1.0f),
            Vector(1.0f, 1.0f, 0.0f, 1.0f),

            Vector(0.0f, 1.0f, 0.0f, 1.0f),
            Vector(0.0f, 0.0f, 0.0f, 1.0f),
            Vector(0.0f, 0.0f, 1.0f, 1.0f),
            Vector(0.0f, 1.0f, 1.0f, 1.0f),

            Vector(0.0f, 0.0f, 1.0f, 1.0f),
            Vector(0.0f, 0.0f, 0.0f, 1.0f),
            Vector(1.0f, 0.0f, 0.0f, 1.0f),
            Vector(1.0f, 0.0f, 1.0f, 1.0f)
        };

        Vector colors[24] = {
            Vector(0.0f, 0.0f, 1.0f, 1.0f),
            Vector(0.0f, 1.0f, 1.0f, 1.0f),
            Vector(1.0f, 0.0f, 1.0f, 1.0f),
            Vector(1.0f, 1.0f, 1.0f, 1.0f),

            Vector(1.0f, 1.0f, 1.0f, 1.0f),
            Vector(1.0f, 0.0f, 0.0f, 1.0f),
            Vector(0.0f, 1.0f, 0.0f, 1.0f),
            Vector(0.0f, 0.0f, 1.0f, 1.0f),

            Vector(0.0f, 0.0f, 1.0f, 1.0f),
            Vector(0.0f, 1.0f, 0.0f, 1.0f),
            Vector(1.0f, 0.0f, 0.0f, 1.0f),
            Vector(0.0f, 0.0f, 0.0f, 1.0f),

            Vector(0.0f, 0.0f, 0.0f, 1.0f),
            Vector(0.0f, 1.0f, 1.0f, 1.0f),
            Vector(1.0f, 1.0f, 0.0f, 1.0f),
            Vector(0.0f, 1.0f, 0.0f, 1.0f),

            Vector(0.0f, 1.0f, 0.0f, 1.0f),
            Vector(0.0f, 1.0f, 1.0f, 1.0f),
            Vector(1.0f, 1.0f, 0.0f, 1.0f),
            Vector(1.0f, 0.0f, 1.0f, 1.0f),

            Vector(1.0f, 0.0f, 1.0f, 1.0f),
            Vector(0.0f, 1.0f, 0.0f, 1.0f),
            Vector(1.0f, 1.0f, 0.0f, 1.0f),
            Vector(0.0f, 0.0f, 1.0f, 1.0f)
        };

        for (ionU32 i = 0; i < 24; ++i)
        {
            vertices[i].SetPosition(positions[i]);
            vertices[i].SetColor(colors[i]);
        }
        for (ionU32 i = 0; i < 24; ++i)
        {
            mesh->PushBackVertex(vertices[i]);
        }
        for (ionU32 i = 0; i < 36; ++i)
        {
            mesh->PushBackIndex(indices[i]);
        }

        mesh->SetIndexCount(36);
        mesh->SetIndexStart(0);
    }
    break;

    case EVertexLayout_Pos_UV:
    {
        MeshUV* mesh = _entity.AddMesh<MeshUV>();

        eosVector(VertexUV) vertices;
        eosVector(Index) indices;
        vertices.resize(24);
        indices.resize(36);
        indices = {
            0,1,2,0,2,3,
            4,5,6,4,6,7,
            8,9,10,8,10,11,
            12,13,14,12,14,15,
            16,17,18,16,18,19,
            20,21,22,20,22,23 };

        Vector positions[24] = {
            Vector(0.0f, 1.0f, 1.0f, 1.0f),
            Vector(0.0f, 0.0f, 1.0f, 1.0f),
            Vector(1.0f, 0.0f, 1.0f, 1.0f),
            Vector(1.0f, 1.0f, 1.0f, 1.0f),

            Vector(1.0f, 1.0f, 0.0f, 1.0f),
            Vector(1.0f, 0.0f, 0.0f, 1.0f),
            Vector(0.0f, 0.0f, 0.0f, 1.0f),
            Vector(0.0f, 1.0f, 0.0f, 1.0f),

            Vector(1.0f, 1.0f, 1.0f, 1.0f),
            Vector(1.0f, 0.0f, 1.0f, 1.0f),
            Vector(1.0f, 0.0f, 0.0f, 1.0f),
            Vector(1.0f, 1.0f, 0.0f, 1.0f),

            Vector(0.0f, 1.0f, 0.0f, 1.0f),
            Vector(0.0f, 1.0f, 1.0f, 1.0f),
            Vector(1.0f, 1.0f, 1.0f, 1.0f),
            Vector(1.0f, 1.0f, 0.0f, 1.0f),

            Vector(0.0f, 1.0f, 0.0f, 1.0f),
            Vector(0.0f, 0.0f, 0.0f, 1.0f),
            Vector(0.0f, 0.0f, 1.0f, 1.0f),
            Vector(0.0f, 1.0f, 1.0f, 1.0f),

            Vector(0.0f, 0.0f, 1.0f, 1.0f),
            Vector(0.0f, 0.0f, 0.0f, 1.0f),
            Vector(1.0f, 0.0f, 0.0f, 1.0f),
            Vector(1.0f, 0.0f, 1.0f, 1.0f)
        };

        ionFloat texCoords[] = {
            0.0f, 1.0f,
            0.0f, 0.0f,
            1.0f, 0.0f,
            1.0f, 1.0f,

            0.0f, 1.0f,
            0.0f, 0.0f,
            1.0f, 0.0f,
            1.0f, 1.0f,

            0.0f, 1.0f,
            0.0f, 0.0f,
            1.0f, 0.0f,
            1.0f, 1.0f,

            0.0f, 1.0f,
            0.0f, 0.0f,
            1.0f, 0.0f,
            1.0f, 1.0f,

            0.0f, 1.0f,
            0.0f, 0.0f,
            1.0f, 0.0f,
            1.0f, 1.0f,

            0.0f, 1.0f,
            0.0f, 0.0f,
            1.0f, 0.0f,
            1.0f, 1.0f,
        };


        for (ionU32 i = 0, j = 0; i < 24; ++i, j += 2)
        {
            vertices[i].SetPosition(positions[i]);
            vertices[i].SetTexCoordUV(texCoords[j], texCoords[j + 1]);
        }
        for (ionU32 i = 0; i < 24; ++i)
        {
            mesh->PushBackVertex(vertices[i]);
        }
        for (ionU32 i = 0; i < 36; ++i)
        {
            mesh->PushBackIndex(indices[i]);
        }

        mesh->SetIndexCount(36);
        mesh->SetIndexStart(0);
    }
    break;


    case EVertexLayout_Pos_UV_Normal:
    {
        MeshSimple* mesh = _entity.AddMesh<MeshSimple>();

        eosVector(VertexSimple) vertices;
        eosVector(Index) indices;
        vertices.resize(24);
        indices.resize(36);
        indices = {
            0,1,2,0,2,3,
            4,5,6,4,6,7,
            8,9,10,8,10,11,
            12,13,14,12,14,15,
            16,17,18,16,18,19,
            20,21,22,20,22,23 };

        Vector positions[24] = {
            Vector(0.0f, 1.0f, 1.0f, 1.0f),
            Vector(0.0f, 0.0f, 1.0f, 1.0f),
            Vector(1.0f, 0.0f, 1.0f, 1.0f),
            Vector(1.0f, 1.0f, 1.0f, 1.0f),

            Vector(1.0f, 1.0f, 0.0f, 1.0f),
            Vector(1.0f, 0.0f, 0.0f, 1.0f),
            Vector(0.0f, 0.0f, 0.0f, 1.0f),
            Vector(0.0f, 1.0f, 0.0f, 1.0f),

            Vector(1.0f, 1.0f, 1.0f, 1.0f),
            Vector(1.0f, 0.0f, 1.0f, 1.0f),
            Vector(1.0f, 0.0f, 0.0f, 1.0f),
            Vector(1.0f, 1.0f, 0.0f, 1.0f),

            Vector(0.0f, 1.0f, 0.0f, 1.0f),
            Vector(0.0f, 1.0f, 1.0f, 1.0f),
            Vector(1.0f, 1.0f, 1.0f, 1.0f),
            Vector(1.0f, 1.0f, 0.0f, 1.0f),

            Vector(0.0f, 1.0f, 0.0f, 1.0f),
            Vector(0.0f, 0.0f, 0.0f, 1.0f),
            Vector(0.0f, 0.0f, 1.0f, 1.0f),
            Vector(0.0f, 1.0f, 1.0f, 1.0f),

            Vector(0.0f, 0.0f, 1.0f, 1.0f),
            Vector(0.0f, 0.0f, 0.0f, 1.0f),
            Vector(1.0f, 0.0f, 0.0f, 1.0f),
            Vector(1.0f, 0.0f, 1.0f, 1.0f)
        };

        ionFloat texCoords[] = {
            0.0f, 1.0f,
            0.0f, 0.0f,
            1.0f, 0.0f,
            1.0f, 1.0f,

            0.0f, 1.0f,
            0.0f, 0.0f,
            1.0f, 0.0f,
            1.0f, 1.0f,

            0.0f, 1.0f,
            0.0f, 0.0f,
            1.0f, 0.0f,
            1.0f, 1.0f,

            0.0f, 1.0f,
            0.0f, 0.0f,
            1.0f, 0.0f,
            1.0f, 1.0f,

            0.0f, 1.0f,
            0.0f, 0.0f,
            1.0f, 0.0f,
            1.0f, 1.0f,

            0.0f, 1.0f,
            0.0f, 0.0f,
            1.0f, 0.0f,
            1.0f, 1.0f,
        };

        Vector normals[24] = {
            Vector(0.0f, 0.0f, 1.0f, 1.0f),
            Vector(0.0f, 0.0f, 1.0f, 1.0f),
            Vector(0.0f, 0.0f, 1.0f, 1.0f),
            Vector(0.0f, 0.0f, 1.0f, 1.0f),

            Vector(0.0f, 0.0f, -1.0f, 1.0f),
            Vector(0.0f, 0.0f, -1.0f, 1.0f),
            Vector(0.0f, 0.0f, -1.0f, 1.0f),
            Vector(0.0f, 0.0f, -1.0f, 1.0f),

            Vector(1.0f, 0.0f, 0.0f, 1.0f),
            Vector(1.0f, 0.0f, 0.0f, 1.0f),
            Vector(1.0f, 0.0f, 0.0f, 1.0f),
            Vector(1.0f, 0.0f, 0.0f, 1.0f),

            Vector(0.0f, 1.0f, 0.0f, 1.0f),
            Vector(0.0f, 1.0f, 0.0f, 1.0f),
            Vector(0.0f, 1.0f, 0.0f, 1.0f),
            Vector(0.0f, 1.0f, 0.0f, 1.0f),

            Vector(-1.0f, 0.0f, 0.0f, 1.0f),
            Vector(-1.0f, 0.0f, 0.0f, 1.0f),
            Vector(-1.0f, 0.0f, 0.0f, 1.0f),
            Vector(-1.0f, 0.0f, 0.0f, 1.0f),

            Vector(0.0f, -1.0f, 0.0f, 1.0f),
            Vector(0.0f, -1.0f, 0.0f, 1.0f),
            Vector(0.0f, -1.0f, 0.0f, 1.0f),
            Vector(0.0f, -1.0f, 0.0f, 1.0f)
        };

        for (ionU32 i = 0, j = 0; i < 24; ++i, j += 2)
        {
            vertices[i].SetPosition(positions[i]);
            vertices[i].SetTexCoordUV(texCoords[j], texCoords[j + 1]);
            vertices[i].SetNormal(normals[i]);
        }
        for (ionU32 i = 0; i < 24; ++i)
        {
            mesh->PushBackVertex(vertices[i]);
        }
        for (ionU32 i = 0; i < 36; ++i)
        {
            mesh->PushBackIndex(indices[i]);
        }

        mesh->SetIndexCount(36);
        mesh->SetIndexStart(0);
    }
    break;

    case EVertexLayout_Full:
    {
        Mesh* mesh = _entity.AddMesh<Mesh>();

        eosVector(Vertex) vertices;
        eosVector(Index) indices;
        vertices.resize(24);
        indices.resize(36);
        indices = {
            0,1,2,0,2,3,
            4,5,6,4,6,7,
            8,9,10,8,10,11,
            12,13,14,12,14,15,
            16,17,18,16,18,19,
            20,21,22,20,22,23 };

        Vector positions[24] = {
            Vector(0.0f, 1.0f, 1.0f, 1.0f),
            Vector(0.0f, 0.0f, 1.0f, 1.0f),
            Vector(1.0f, 0.0f, 1.0f, 1.0f),
            Vector(1.0f, 1.0f, 1.0f, 1.0f),

            Vector(1.0f, 1.0f, 0.0f, 1.0f),
            Vector(1.0f, 0.0f, 0.0f, 1.0f),
            Vector(0.0f, 0.0f, 0.0f, 1.0f),
            Vector(0.0f, 1.0f, 0.0f, 1.0f),

            Vector(1.0f, 1.0f, 1.0f, 1.0f),
            Vector(1.0f, 0.0f, 1.0f, 1.0f),
            Vector(1.0f, 0.0f, 0.0f, 1.0f),
            Vector(1.0f, 1.0f, 0.0f, 1.0f),

            Vector(0.0f, 1.0f, 0.0f, 1.0f),
            Vector(0.0f, 1.0f, 1.0f, 1.0f),
            Vector(1.0f, 1.0f, 1.0f, 1.0f),
            Vector(1.0f, 1.0f, 0.0f, 1.0f),

            Vector(0.0f, 1.0f, 0.0f, 1.0f),
            Vector(0.0f, 0.0f, 0.0f, 1.0f),
            Vector(0.0f, 0.0f, 1.0f, 1.0f),
            Vector(0.0f, 1.0f, 1.0f, 1.0f),

            Vector(0.0f, 0.0f, 1.0f, 1.0f),
            Vector(0.0f, 0.0f, 0.0f, 1.0f),
            Vector(1.0f, 0.0f, 0.0f, 1.0f),
            Vector(1.0f, 0.0f, 1.0f, 1.0f)
        };

        Vector uvuv[24] = {
            Vector(0.0f, 1.0f, 0.0f, 1.0f),
            Vector(0.0f, 0.0f, 0.0f, 0.0f),
            Vector(1.0f, 0.0f, 1.0f, 0.0f),
            Vector(1.0f, 1.0f, 1.0f, 1.0f),

            Vector(0.0f, 1.0f, 0.0f, 1.0f),
            Vector(0.0f, 0.0f, 0.0f, 0.0f),
            Vector(1.0f, 0.0f, 1.0f, 0.0f),
            Vector(1.0f, 1.0f, 1.0f, 1.0f),

            Vector(0.0f, 1.0f, 0.0f, 1.0f),
            Vector(0.0f, 0.0f, 0.0f, 0.0f),
            Vector(1.0f, 0.0f, 1.0f, 0.0f),
            Vector(1.0f, 1.0f, 1.0f, 1.0f),

            Vector(0.0f, 1.0f, 0.0f, 1.0f),
            Vector(0.0f, 0.0f, 0.0f, 0.0f),
            Vector(1.0f, 0.0f, 1.0f, 0.0f),
            Vector(1.0f, 1.0f, 1.0f, 1.0f),

            Vector(0.0f, 1.0f, 0.0f, 1.0f),
            Vector(0.0f, 0.0f, 0.0f, 0.0f),
            Vector(1.0f, 0.0f, 1.0f, 0.0f),
            Vector(1.0f, 1.0f, 1.0f, 1.0f),

            Vector(0.0f, 1.0f, 0.0f, 1.0f),
            Vector(0.0f, 0.0f, 0.0f, 0.0f),
            Vector(1.0f, 0.0f, 1.0f, 0.0f),
            Vector(1.0f, 1.0f, 1.0f, 1.0f)
        };

        Vector normals[24] = {
            Vector(0.0f, 0.0f, 1.0f, 1.0f),
            Vector(0.0f, 0.0f, 1.0f, 1.0f),
            Vector(0.0f, 0.0f, 1.0f, 1.0f),
            Vector(0.0f, 0.0f, 1.0f, 1.0f),

            Vector(0.0f, 0.0f, -1.0f, 1.0f),
            Vector(0.0f, 0.0f, -1.0f, 1.0f),
            Vector(0.0f, 0.0f, -1.0f, 1.0f),
            Vector(0.0f, 0.0f, -1.0f, 1.0f),

            Vector(1.0f, 0.0f, 0.0f, 1.0f),
            Vector(1.0f, 0.0f, 0.0f, 1.0f),
            Vector(1.0f, 0.0f, 0.0f, 1.0f),
            Vector(1.0f, 0.0f, 0.0f, 1.0f),

            Vector(0.0f, 1.0f, 0.0f, 1.0f),
            Vector(0.0f, 1.0f, 0.0f, 1.0f),
            Vector(0.0f, 1.0f, 0.0f, 1.0f),
            Vector(0.0f, 1.0f, 0.0f, 1.0f),

            Vector(-1.0f, 0.0f, 0.0f, 1.0f),
            Vector(-1.0f, 0.0f, 0.0f, 1.0f),
            Vector(-1.0f, 0.0f, 0.0f, 1.0f),
            Vector(-1.0f, 0.0f, 0.0f, 1.0f),

            Vector(0.0f, -1.0f, 0.0f, 1.0f),
            Vector(0.0f, -1.0f, 0.0f, 1.0f),
            Vector(0.0f, -1.0f, 0.0f, 1.0f),
            Vector(0.0f, -1.0f, 0.0f, 1.0f)
        };

        Vector colors[24] = {
            Vector(0.0f, 0.0f, 1.0f, 1.0f),
            Vector(0.0f, 1.0f, 1.0f, 1.0f),
            Vector(1.0f, 0.0f, 1.0f, 1.0f),
            Vector(1.0f, 1.0f, 1.0f, 1.0f),

            Vector(1.0f, 1.0f, 1.0f, 1.0f),
            Vector(1.0f, 0.0f, 0.0f, 1.0f),
            Vector(0.0f, 1.0f, 0.0f, 1.0f),
            Vector(0.0f, 0.0f, 1.0f, 1.0f),

            Vector(0.0f, 0.0f, 1.0f, 1.0f),
            Vector(0.0f, 1.0f, 0.0f, 1.0f),
            Vector(1.0f, 0.0f, 0.0f, 1.0f),
            Vector(0.0f, 0.0f, 0.0f, 1.0f),

            Vector(0.0f, 0.0f, 0.0f, 1.0f),
            Vector(0.0f, 1.0f, 1.0f, 1.0f),
            Vector(1.0f, 1.0f, 0.0f, 1.0f),
            Vector(0.0f, 1.0f, 0.0f, 1.0f),

            Vector(0.0f, 1.0f, 0.0f, 1.0f),
            Vector(0.0f, 1.0f, 1.0f, 1.0f),
            Vector(1.0f, 1.0f, 0.0f, 1.0f),
            Vector(1.0f, 0.0f, 1.0f, 1.0f),

            Vector(1.0f, 0.0f, 1.0f, 1.0f),
            Vector(0.0f, 1.0f, 0.0f, 1.0f),
            Vector(1.0f, 1.0f, 0.0f, 1.0f),
            Vector(0.0f, 0.0f, 1.0f, 1.0f)
        };

        Vector tangents[24];
        GeometryHelper::CalculateTangent(positions, normals, uvuv, 24, indices.data(), 36, tangents);

        for (ionU32 i = 0; i < 24; ++i)
        {
            vertices[i].SetPosition(positions[i]);
            vertices[i].SetTexCoordUV0(uvuv[i]);
            vertices[i].SetNormal(normals[i]);
            vertices[i].SetColor(colors[i]);
            vertices[i].SetTangent(tangents[i]);
        }
        for (ionU32 i = 0; i < 24; ++i)
        {
            mesh->PushBackVertex(vertices[i]);
        }
        for (ionU32 i = 0; i < 36; ++i)
        {
            mesh->PushBackIndex(indices[i]);
        }

        mesh->SetIndexCount(36);
        mesh->SetIndexStart(0);
    }
    break;

    case EVertexLayout_Unknow:
    case EVertexLayout_Empty:
    case EVertexLayout_Count:
        ionAssertReturnVoid(false, "Layout does not exist!");
        break;

    default:
        ionAssertReturnVoid(false, "Layout not yet implemented");
        break;
    }
}


///
///            SPHERE
///
void PrimitiveFactory::GenerateSphere(EVertexLayout _layout, Entity& _entity)
{
    static const ionFloat radius = 1.0f;
    static const ionU32 rings = 24;
    static const ionU32 sectors = 48;

    switch (_layout)
    {
    case EVertexLayout_Pos:
    {
        MeshPlain* mesh = _entity.AddMesh<MeshPlain>();

        eosVector(VertexPlain) vertices;
        eosVector(Index) indices;

        eosVector(Vector) positions;

        const ionU32 verticesSize = rings * sectors;
        const ionU32 indicesSize = rings * sectors * 6;

        const ionFloat R = 1.0f / static_cast<ionFloat>(rings - 1);
        const ionFloat S = 1.0f / static_cast<ionFloat>(sectors - 1);
        ionS32 r, s;

        vertices.resize(verticesSize);
        indices.resize(indicesSize);

        positions.resize(verticesSize);

        std::vector<Vector>::iterator v = positions.begin();

        for (r = 0; r < rings; ++r)
        {
            for (s = 0; s < sectors; ++s)
            {
                const ionFloat y = std::sin(-MathHelper::kHalfPI + MathHelper::kPI * r * R);
                const ionFloat x = std::cos(2 * MathHelper::kPI * s * S) * std::sin(MathHelper::kPI * r * R);
                const ionFloat z = std::sin(2 * MathHelper::kPI * s * S) * std::sin(MathHelper::kPI * r * R);

                *v++ = VectorHelper::Set(x * radius, y * radius, z * radius, 1.0f);
            }
        }

        std::vector<Index>::iterator i = indices.begin();
        for (r = 0; r < rings; ++r)
        {
            for (s = 0; s < sectors; ++s)
            {
                *i++ = r * sectors + s;
                *i++ = (r + 1) * sectors + s;
                *i++ = (r + 1) * sectors + (s + 1) % sectors;
                *i++ = r * sectors + s;
                *i++ = (r + 1) * sectors + (s + 1) % sectors;
                *i++ = r * sectors + (s + 1) % sectors;
            }
        }

        for (ionU32 i = 0; i < verticesSize; ++i)
        {
            vertices[i].SetPosition(positions[i]);
        }
        for (ionU32 i = 0; i < verticesSize; ++i)
        {
            mesh->PushBackVertex(vertices[i]);
        }
        for (ionU32 i = 0; i < indicesSize; ++i)
        {
            mesh->PushBackIndex(indices[i]);
        }

        mesh->SetIndexCount(indicesSize);
        mesh->SetIndexStart(0);
    }
    break;

    case EVertexLayout_Pos_Color:
    {
        MeshColored* mesh = _entity.AddMesh<MeshColored>();

        eosVector(VertexColored) vertices;
        eosVector(Index) indices;

        eosVector(Vector) positions;
        eosVector(Vector) normals;
        eosVector(Vector) colors;

        const ionU32 verticesSize = rings * sectors;
        const ionU32 indicesSize = rings * sectors * 6;

        const ionFloat R = 1.0f / static_cast<ionFloat>(rings - 1);
        const ionFloat S = 1.0f / static_cast<ionFloat>(sectors - 1);
        ionS32 r, s;

        vertices.resize(verticesSize);
        indices.resize(indicesSize);

        positions.resize(verticesSize);
        normals.resize(verticesSize);
        colors.resize(verticesSize);

        std::vector<Vector>::iterator v = positions.begin();
        std::vector<Vector>::iterator n = normals.begin();
        std::vector<Vector>::iterator c = colors.begin();


        for (r = 0; r < rings; ++r)
        {
            for (s = 0; s < sectors; ++s)
            {
                const ionFloat y = std::sin(-MathHelper::kHalfPI + MathHelper::kPI * r * R);
                const ionFloat x = std::cos(2 * MathHelper::kPI * s * S) * std::sin(MathHelper::kPI * r * R);
                const ionFloat z = std::sin(2 * MathHelper::kPI * s * S) * std::sin(MathHelper::kPI * r * R);

                *v++ = VectorHelper::Set(x * radius, y * radius, z * radius, 1.0f);
                *n++ = VectorHelper::Set(x, y, z, 1.0f);
                *c++ = VectorHelper::Set(x, y, z, 1.0f);
            }
        }

        std::vector<Index>::iterator i = indices.begin();
        for (r = 0; r < rings; ++r)
        {
            for (s = 0; s < sectors; ++s)
            {
                *i++ = r * sectors + s;
                *i++ = (r + 1) * sectors + s;
                *i++ = (r + 1) * sectors + (s + 1) % sectors;
                *i++ = r * sectors + s;
                *i++ = (r + 1) * sectors + (s + 1) % sectors;
                *i++ = r * sectors + (s + 1) % sectors;
            }
        }

        for (ionU32 i = 0; i < verticesSize; ++i)
        {
            vertices[i].SetPosition(positions[i]);
            vertices[i].SetColor(colors[i]);
        }
        for (ionU32 i = 0; i < verticesSize; ++i)
        {
            mesh->PushBackVertex(vertices[i]);
        }
        for (ionU32 i = 0; i < indicesSize; ++i)
        {
            mesh->PushBackIndex(indices[i]);
        }

        mesh->SetIndexCount(indicesSize);
        mesh->SetIndexStart(0);
    }
    break;

    case EVertexLayout_Pos_UV:
    {
        MeshUV* mesh = _entity.AddMesh<MeshUV>();

        eosVector(VertexUV) vertices;
        eosVector(Index) indices;

        eosVector(Vector) positions;
        eosVector(Vector) uvuv;

        const ionU32 verticesSize = rings * sectors;
        const ionU32 indicesSize = rings * sectors * 6;

        const ionFloat R = 1.0f / static_cast<ionFloat>(rings - 1);
        const ionFloat S = 1.0f / static_cast<ionFloat>(sectors - 1);
        ionS32 r, s;

        vertices.resize(verticesSize);
        indices.resize(indicesSize);

        positions.resize(verticesSize);
        uvuv.resize(verticesSize);

        std::vector<Vector>::iterator v = positions.begin();
        std::vector<Vector>::iterator t = uvuv.begin();

        for (r = 0; r < rings; ++r)
        {
            for (s = 0; s < sectors; ++s)
            {
                const ionFloat y = std::sin(-MathHelper::kHalfPI + MathHelper::kPI * r * R);
                const ionFloat x = std::cos(2 * MathHelper::kPI * s * S) * std::sin(MathHelper::kPI * r * R);
                const ionFloat z = std::sin(2 * MathHelper::kPI * s * S) * std::sin(MathHelper::kPI * r * R);

                *v++ = VectorHelper::Set(x * radius, y * radius, z * radius, 1.0f);
                *t++ = VectorHelper::Set(s * S, r * R, s * S, r * R);
            }
        }

        std::vector<Index>::iterator i = indices.begin();
        for (r = 0; r < rings; ++r)
        {
            for (s = 0; s < sectors; ++s)
            {
                *i++ = r * sectors + s;
                *i++ = (r + 1) * sectors + s;
                *i++ = (r + 1) * sectors + (s + 1) % sectors;
                *i++ = r * sectors + s;
                *i++ = (r + 1) * sectors + (s + 1) % sectors;
                *i++ = r * sectors + (s + 1) % sectors;
            }
        }

        for (ionU32 i = 0; i < verticesSize; ++i)
        {
            vertices[i].SetPosition(positions[i]);
            vertices[i].SetTexCoordUV(uvuv[i]);
        }
        for (ionU32 i = 0; i < verticesSize; ++i)
        {
            mesh->PushBackVertex(vertices[i]);
        }
        for (ionU32 i = 0; i < indicesSize; ++i)
        {
            mesh->PushBackIndex(indices[i]);
        }

        mesh->SetIndexCount(indicesSize);
        mesh->SetIndexStart(0);
    }
    break;


    case EVertexLayout_Pos_UV_Normal:
    {
        MeshSimple* mesh = _entity.AddMesh<MeshSimple>();

        eosVector(VertexSimple) vertices;
        eosVector(Index) indices;

        eosVector(Vector) positions;
        eosVector(Vector) normals;
        eosVector(Vector) uvuv;

        const ionU32 verticesSize = rings * sectors;
        const ionU32 indicesSize = rings * sectors * 6;

        const ionFloat R = 1.0f / static_cast<ionFloat>(rings - 1);
        const ionFloat S = 1.0f / static_cast<ionFloat>(sectors - 1);
        ionS32 r, s;

        vertices.resize(verticesSize);
        indices.resize(indicesSize);

        positions.resize(verticesSize);
        normals.resize(verticesSize);
        uvuv.resize(verticesSize);

        std::vector<Vector>::iterator v = positions.begin();
        std::vector<Vector>::iterator n = normals.begin();
        std::vector<Vector>::iterator t = uvuv.begin();

        for (r = 0; r < rings; ++r)
        {
            for (s = 0; s < sectors; ++s)
            {
                const ionFloat y = std::sin(-MathHelper::kHalfPI + MathHelper::kPI * r * R);
                const ionFloat x = std::cos(2 * MathHelper::kPI * s * S) * std::sin(MathHelper::kPI * r * R);
                const ionFloat z = std::sin(2 * MathHelper::kPI * s * S) * std::sin(MathHelper::kPI * r * R);

                *v++ = VectorHelper::Set(x * radius, y * radius, z * radius, 1.0f);
                *n++ = VectorHelper::Set(x, y, z, 1.0f);
                *t++ = VectorHelper::Set(s * S, r * R, s * S, r * R);
            }
        }

        std::vector<Index>::iterator i = indices.begin();
        for (r = 0; r < rings; ++r)
        {
            for (s = 0; s < sectors; ++s)
            {
                *i++ = r * sectors + s;
                *i++ = (r + 1) * sectors + s;
                *i++ = (r + 1) * sectors + (s + 1) % sectors;
                *i++ = r * sectors + s;
                *i++ = (r + 1) * sectors + (s + 1) % sectors;
                *i++ = r * sectors + (s + 1) % sectors;
            }
        }

        for (ionU32 i = 0; i < verticesSize; ++i)
        {
            vertices[i].SetPosition(positions[i]);
            vertices[i].SetTexCoordUV(uvuv[i]);
            vertices[i].SetNormal(normals[i]);
        }
        for (ionU32 i = 0; i < verticesSize; ++i)
        {
            mesh->PushBackVertex(vertices[i]);
        }
        for (ionU32 i = 0; i < indicesSize; ++i)
        {
            mesh->PushBackIndex(indices[i]);
        }

        mesh->SetIndexCount(indicesSize);
        mesh->SetIndexStart(0);
    }
    break;

    case EVertexLayout_Full:
    {
        Mesh* mesh = _entity.AddMesh<Mesh>();

        eosVector(Vertex) vertices;
        eosVector(Index) indices;

        eosVector(Vector) positions;
        eosVector(Vector) normals;
        eosVector(Vector) uvuv;
        eosVector(Vector) colors;

        const ionU32 verticesSize = rings * sectors;
        const ionU32 indicesSize = rings * sectors * 6;

        const ionFloat R = 1.0f / static_cast<ionFloat>(rings - 1);
        const ionFloat S = 1.0f / static_cast<ionFloat>(sectors - 1);
        ionS32 r, s;

        vertices.resize(verticesSize);
        indices.resize(indicesSize);

        positions.resize(verticesSize);
        normals.resize(verticesSize);
        uvuv.resize(verticesSize);
        colors.resize(verticesSize);

        std::vector<Vector>::iterator v = positions.begin();
        std::vector<Vector>::iterator n = normals.begin();
        std::vector<Vector>::iterator t = uvuv.begin();
        std::vector<Vector>::iterator c = colors.begin();

        for (r = 0; r < rings; ++r)
        {
            for (s = 0; s < sectors; ++s)
            {
                const ionFloat y = std::sin(-MathHelper::kHalfPI + MathHelper::kPI * r * R);
                const ionFloat x = std::cos(2 * MathHelper::kPI * s * S) * std::sin(MathHelper::kPI * r * R);
                const ionFloat z = std::sin(2 * MathHelper::kPI * s * S) * std::sin(MathHelper::kPI * r * R);

                *v++ = VectorHelper::Set(x * radius, y * radius, z * radius, 1.0f);
                *n++ = VectorHelper::Set(x, y, z, 1.0f);
                *t++ = VectorHelper::Set(s * S, r * R, s * S, r * R);
                *c++ = VectorHelper::Set(x, y, z, 1.0f);
            }
        }

        std::vector<Index>::iterator i = indices.begin();
        for (r = 0; r < rings; ++r)
        {
            for (s = 0; s < sectors; ++s)
            {
                *i++ = r * sectors + s;
                *i++ = (r + 1) * sectors + s;
                *i++ = (r + 1) * sectors + (s + 1) % sectors;
                *i++ = r * sectors + s;
                *i++ = (r + 1) * sectors + (s + 1) % sectors;
                *i++ = r * sectors + (s + 1) % sectors;
            }
        }

        Vector tangents[verticesSize];
        GeometryHelper::CalculateTangent(positions.data(), normals.data(), uvuv.data(), verticesSize, indices.data(), indicesSize, tangents);

        for (ionU32 i = 0; i < verticesSize; ++i)
        {
            vertices[i].SetPosition(positions[i]);
            vertices[i].SetTexCoordUV0(uvuv[i]);
            vertices[i].SetNormal(normals[i]);
            vertices[i].SetColor(colors[i]);
            vertices[i].SetTangent(tangents[i]);
        }
        for (ionU32 i = 0; i < verticesSize; ++i)
        {
            mesh->PushBackVertex(vertices[i]);
        }
        for (ionU32 i = 0; i < indicesSize; ++i)
        {
            mesh->PushBackIndex(indices[i]);
        }

        mesh->SetIndexCount(indicesSize);
        mesh->SetIndexStart(0);
    }
    break;

    case EVertexLayout_Unknow:
    case EVertexLayout_Empty:
    case EVertexLayout_Count:
        ionAssertReturnVoid(false, "Layout does not exist!");
        break;

    default:
        ionAssertReturnVoid(false, "Layout not yet implemented");
        break;
    }
}

ION_NAMESPACE_END