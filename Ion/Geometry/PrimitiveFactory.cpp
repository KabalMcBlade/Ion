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
    
    case EVertexLayout_Pos_UV_Normal:
    {
        MeshSimple* mesh = _entity.AddMesh<MeshSimple>();

        eosVector(VertexSimple) vertices;
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
        mesh->SetIndexType(VK_INDEX_TYPE_UINT32);
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

        Vector positions[3] = { Vector(0.0f, -0.5f, 0.0f, 1.0f), Vector(0.5f, 0.5f, 0.0f, 1.0f), Vector(-0.5f, 0.5f, 0.0f, 1.0f) };

        vertices[0].SetPosition(positions[0]);
        vertices[1].SetPosition(positions[1]);
        vertices[2].SetPosition(positions[2]);

        Vector uvuv[3] = { Vector(0.0f, 0.0f, 0.0f, 0.0f),  Vector(1.0f, 0.0f, 1.0f, 0.0f),  Vector(0.0f, 1.0f, 0.0f, 1.0f) };

        vertices[0].SetTexCoordUV(uvuv[0]);
        vertices[1].SetTexCoordUV(uvuv[1]);
        vertices[2].SetTexCoordUV(uvuv[2]);

        Vector normals[3] =
        {
            GeometryHelper::CalculateSurfaceNormalTriangle(&positions[0], 0),
            GeometryHelper::CalculateSurfaceNormalTriangle(&positions[0], 1),
            GeometryHelper::CalculateSurfaceNormalTriangle(&positions[0], 2)
        };

        vertices[0].SetNormal(normals[0]);
        vertices[1].SetNormal(normals[1]);
        vertices[2].SetNormal(normals[2]);

        vertices[0].SetColor1(1.0f, 0.0f, 0.0f, 1.0f);
        vertices[1].SetColor1(0.0f, 1.0f, 0.0f, 1.0f);
        vertices[2].SetColor1(0.0f, 0.0f, 1.0f, 1.0f);

        vertices[0].SetColor2(0.5f, 0.5f, 0.5f, 1.0f);
        vertices[1].SetColor2(0.5f, 0.5f, 0.5f, 1.0f);
        vertices[2].SetColor2(0.5f, 0.5f, 0.5f, 1.0f);

        Vector tangents[3];
        GeometryHelper::CalculateTangent(positions, normals, uvuv, 3, indices.data(), 3, tangents);

        vertices[0].SetTangent(tangents[0]);
        vertices[1].SetTangent(tangents[1]);
        vertices[2].SetTangent(tangents[2]);

        vertices[0].SetBiTangent(tangents[0]);
        vertices[1].SetBiTangent(tangents[1]);
        vertices[2].SetBiTangent(tangents[2]);

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


    case EVertexLayout_Pos_UV_Normal:
    {
        MeshSimple* mesh = _entity.AddMesh<MeshSimple>();

        eosVector(VertexSimple) vertices;
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
        mesh->SetIndexType(VK_INDEX_TYPE_UINT32);
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

        Vector positions[4] = { Vector(-0.5f, -0.5f, 0.0f, 1.0f), Vector(0.5f, -0.5f, 0.0f, 1.0f), Vector(0.5f, 0.5f, 0.0f, 1.0f), Vector(-0.5f, 0.5f, 0.0f, 1.0f) };

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

        Vector uvuv[4] = { Vector(0.0f, 0.0f, 0.0f, 0.0f),  Vector(1.0f, 0.0f, 1.0f, 0.0f),  Vector(0.0f, 1.0f, 0.0f, 1.0f), Vector(1.0f, 1.0f, 0.0f, 1.0f) };

        vertices[0].SetTexCoordUV(uvuv[0]);
        vertices[1].SetTexCoordUV(uvuv[1]);
        vertices[2].SetTexCoordUV(uvuv[2]);
        vertices[3].SetTexCoordUV(uvuv[3]);

        vertices[0].SetColor1(1.0f, 0.0f, 0.0f, 1.0f);
        vertices[1].SetColor1(0.0f, 1.0f, 0.0f, 1.0f);
        vertices[2].SetColor1(0.0f, 0.0f, 1.0f, 1.0f);
        vertices[3].SetColor1(1.0f, 1.0f, 1.0f, 1.0f);

        vertices[0].SetColor2(0.5f, 0.5f, 0.5f, 1.0f);
        vertices[1].SetColor2(0.5f, 0.5f, 0.5f, 1.0f);
        vertices[2].SetColor2(0.5f, 0.5f, 0.5f, 1.0f);
        vertices[3].SetColor2(0.5f, 0.5f, 0.5f, 1.0f);

        Vector tangents[4];
        GeometryHelper::CalculateTangent(positions, normals, uvuv, 4, indices.data(), 6, tangents);

        vertices[0].SetTangent(tangents[0]);
        vertices[1].SetTangent(tangents[1]);
        vertices[2].SetTangent(tangents[2]);
        vertices[3].SetTangent(tangents[3]);

        vertices[0].SetBiTangent(tangents[0]);
        vertices[1].SetBiTangent(tangents[1]);
        vertices[2].SetBiTangent(tangents[2]);
        vertices[3].SetBiTangent(tangents[3]);

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

        mesh->SetIndexCount(36);
        mesh->SetIndexStart(0);
        mesh->SetIndexType(VK_INDEX_TYPE_UINT32);
    }
    break;

    case EVertexLayout_Pos_Color:
    {
      
    }
    break;

    case EVertexLayout_Pos_UV:
    {
        /*
        float texCoords[] = {
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
        */
    }
    break;


    case EVertexLayout_Pos_UV_Normal:
    {
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
    }
    break;

    case EVertexLayout_Full:
    {
       
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