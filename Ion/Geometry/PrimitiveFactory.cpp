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
void PrimitiveFactory::GenerateTriangle(EVertexLayout _layout, ObjectHandler& _entity, ionFloat _r /*= 1.0f*/, ionFloat _g /*= 1.0f*/, ionFloat _b /*= 1.0f*/, ionFloat _a /*= 1.0f*/)
{
    eosVector(Index) indices;
    indices.resize(3);
    indices = { 0, 1, 2 };

    Vector positions[3] = { Vector(0.0f, 0.5f, 0.0f, 1.0f), Vector(0.5f, -0.5f, 0.0f, 1.0f), Vector(-0.5f, -0.5f, 0.0f, 1.0f) };

    Entity* entityPtr = dynamic_cast<Entity*>(_entity.GetPtr());

    switch(_layout)
    {
    case EVertexLayout_Pos:
    {
        MeshRendererPlain* meshRenderer = entityPtr->AddMeshRenderer<MeshRendererPlain>();

        eosVector(VertexPlain) vertices;
        vertices.resize(3);

        vertices[0].SetPosition(positions[0]);
        vertices[1].SetPosition(positions[1]);
        vertices[2].SetPosition(positions[2]);

        meshRenderer->PushBackVertex(vertices[0]);
        meshRenderer->PushBackVertex(vertices[1]);
        meshRenderer->PushBackVertex(vertices[2]);

        meshRenderer->PushBackIndex(indices[0]);
        meshRenderer->PushBackIndex(indices[1]);
        meshRenderer->PushBackIndex(indices[2]);

        Mesh mesh;
        mesh.SetIndexCount(3);
        mesh.SetIndexStart(0);

        entityPtr->PushBackMesh(mesh);
    }
    break;

    case EVertexLayout_Pos_Color:
    {
        MeshRendererColored* meshRenderer = entityPtr->AddMeshRenderer<MeshRendererColored>();

        eosVector(VertexColored) vertices;
        vertices.resize(3);

        vertices[0].SetPosition(positions[0]);
        vertices[1].SetPosition(positions[1]);
        vertices[2].SetPosition(positions[2]);

        vertices[0].SetColor(_r, _g, _b, _a);
        vertices[1].SetColor(_r, _g, _b, _a);
        vertices[2].SetColor(_r, _g, _b, _a);

        meshRenderer->PushBackVertex(vertices[0]);
        meshRenderer->PushBackVertex(vertices[1]);
        meshRenderer->PushBackVertex(vertices[2]);

        meshRenderer->PushBackIndex(indices[0]);
        meshRenderer->PushBackIndex(indices[1]);
        meshRenderer->PushBackIndex(indices[2]);

        Mesh mesh;
        mesh.SetIndexCount(3);
        mesh.SetIndexStart(0);

        entityPtr->PushBackMesh(mesh);
    }
    break;

    case EVertexLayout_Pos_UV:
    {
        MeshRendererUV* meshRenderer = entityPtr->AddMeshRenderer<MeshRendererUV>();

        eosVector(VertexUV) vertices;
        vertices.resize(3);

        vertices[0].SetPosition(positions[0]);
        vertices[1].SetPosition(positions[1]);
        vertices[2].SetPosition(positions[2]);

        vertices[0].SetTexCoordUV(0.0f, 0.0f);
        vertices[1].SetTexCoordUV(1.0f, 0.0f);
        vertices[2].SetTexCoordUV(0.0f, 1.0f);

        meshRenderer->PushBackVertex(vertices[0]);
        meshRenderer->PushBackVertex(vertices[1]);
        meshRenderer->PushBackVertex(vertices[2]);

        meshRenderer->PushBackIndex(indices[0]);
        meshRenderer->PushBackIndex(indices[1]);
        meshRenderer->PushBackIndex(indices[2]);

        Mesh mesh;
        mesh.SetIndexCount(3);
        mesh.SetIndexStart(0);

        entityPtr->PushBackMesh(mesh);
    }
    break;
    
    case EVertexLayout_Pos_UV_Normal:
    {
        MeshRendererSimple* meshRenderer = entityPtr->AddMeshRenderer<MeshRendererSimple>();

        eosVector(VertexSimple) vertices;
        vertices.resize(3);

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

        meshRenderer->PushBackVertex(vertices[0]);
        meshRenderer->PushBackVertex(vertices[1]);
        meshRenderer->PushBackVertex(vertices[2]);

        meshRenderer->PushBackIndex(indices[0]);
        meshRenderer->PushBackIndex(indices[1]);
        meshRenderer->PushBackIndex(indices[2]);

        Mesh mesh;
        mesh.SetIndexCount(3);
        mesh.SetIndexStart(0);

        entityPtr->PushBackMesh(mesh);
    }
    break;

    case EVertexLayout_Pos_Normal:
    {
        MeshRendererNormal* meshRenderer = entityPtr->AddMeshRenderer<MeshRendererNormal>();

        eosVector(VertexNormal) vertices;
        vertices.resize(3);

        vertices[0].SetPosition(positions[0]);
        vertices[1].SetPosition(positions[1]);
        vertices[2].SetPosition(positions[2]);

        Vector normal0 = GeometryHelper::CalculateSurfaceNormalTriangle(&positions[0], 0);
        Vector normal1 = GeometryHelper::CalculateSurfaceNormalTriangle(&positions[0], 1);
        Vector normal2 = GeometryHelper::CalculateSurfaceNormalTriangle(&positions[0], 2);

        vertices[0].SetNormal(normal0);
        vertices[1].SetNormal(normal1);
        vertices[2].SetNormal(normal2);

        meshRenderer->PushBackVertex(vertices[0]);
        meshRenderer->PushBackVertex(vertices[1]);
        meshRenderer->PushBackVertex(vertices[2]);

        meshRenderer->PushBackIndex(indices[0]);
        meshRenderer->PushBackIndex(indices[1]);
        meshRenderer->PushBackIndex(indices[2]);

        Mesh mesh;
        mesh.SetIndexCount(3);
        mesh.SetIndexStart(0);

        entityPtr->PushBackMesh(mesh);
    }
    break;

    case EVertexLayout_Full:
    {
        MeshRenderer* meshRenderer = entityPtr->AddMeshRenderer<MeshRenderer>();

        eosVector(Vertex) vertices;
        vertices.resize(3);

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

        vertices[0].SetColor(_r, _g, _b, _a);
        vertices[1].SetColor(_r, _g, _b, _a);
        vertices[2].SetColor(_r, _g, _b, _a);

        Vector tangents[3];
        GeometryHelper::CalculateTangent(positions, normals, uvuv, 3, indices.data(), 3, tangents);

        vertices[0].SetTangent(tangents[0]);
        vertices[1].SetTangent(tangents[1]);
        vertices[2].SetTangent(tangents[2]);

        //vertices[0].SetBiTangent(tangents[0]);
        //vertices[1].SetBiTangent(tangents[1]);
        //vertices[2].SetBiTangent(tangents[2]);

        meshRenderer->PushBackVertex(vertices[0]);
        meshRenderer->PushBackVertex(vertices[1]);
        meshRenderer->PushBackVertex(vertices[2]);

        meshRenderer->PushBackIndex(indices[0]);
        meshRenderer->PushBackIndex(indices[1]);
        meshRenderer->PushBackIndex(indices[2]);

        Mesh mesh;
        mesh.SetIndexCount(3);
        mesh.SetIndexStart(0);

        entityPtr->PushBackMesh(mesh);
    }
    break;

    case EVertexLayout_Empty:
    {
        entityPtr->AddMeshRenderer<BaseMeshRenderer>();

        Mesh mesh;
        mesh.SetIndexCount(0);
        mesh.SetIndexStart(0);

        entityPtr->PushBackMesh(mesh);
    }
    break;

    case EVertexLayout_Unknow:
    case EVertexLayout_Count:
        ionAssertReturnVoid(false, "Layout does not exist!");
    break;

    default:
        ionAssertReturnVoid(false, "Layout not yet implemented");
        break;
    }

    entityPtr->GetBoundingBox()->Expande(positions[0], positions[1]);
    entityPtr->GetBoundingBox()->Expande(positions[0], positions[2]);
}


///
///            QUAD
///
void PrimitiveFactory::GenerateQuad(EVertexLayout _layout, ObjectHandler& _entity, ionFloat _r /*= 1.0f*/, ionFloat _g /*= 1.0f*/, ionFloat _b /*= 1.0f*/, ionFloat _a /*= 1.0f*/)
{
    eosVector(Index) indices;
    indices.resize(6);
    indices = { 0, 1, 2, 2, 3, 0 };

    Vector positions[4] = { Vector(0.5f, 0.5f, 0.0f, 1.0f), Vector(-0.5f, 0.5f, 0.0f, 1.0f), Vector(-0.5f, -0.5f, 0.0f, 1.0f), Vector(0.5f, -0.5f, 0.0f, 1.0f) };

    Entity* entityPtr = dynamic_cast<Entity*>(_entity.GetPtr());

    switch (_layout)
    {
    case EVertexLayout_Pos:
    {
        MeshRendererPlain* meshRenderer = entityPtr->AddMeshRenderer<MeshRendererPlain>();

        eosVector(VertexPlain) vertices;
        vertices.resize(4);

        vertices[0].SetPosition(positions[0]);
        vertices[1].SetPosition(positions[1]);
        vertices[2].SetPosition(positions[2]);
        vertices[3].SetPosition(positions[3]);

        meshRenderer->PushBackVertex(vertices[0]);
        meshRenderer->PushBackVertex(vertices[1]);
        meshRenderer->PushBackVertex(vertices[2]);
        meshRenderer->PushBackVertex(vertices[3]);

        meshRenderer->PushBackIndex(indices[0]);
        meshRenderer->PushBackIndex(indices[1]);
        meshRenderer->PushBackIndex(indices[2]);
        meshRenderer->PushBackIndex(indices[3]);
        meshRenderer->PushBackIndex(indices[4]);
        meshRenderer->PushBackIndex(indices[5]);

        Mesh mesh;
        mesh.SetIndexCount(6);
        mesh.SetIndexStart(0);

        entityPtr->PushBackMesh(mesh);
    }
    break;
    
    case EVertexLayout_Pos_Color:
    {
        MeshRendererColored* meshRenderer = entityPtr->AddMeshRenderer<MeshRendererColored>();

        eosVector(VertexColored) vertices;
        vertices.resize(4);

        vertices[0].SetPosition(positions[0]);
        vertices[1].SetPosition(positions[1]);
        vertices[2].SetPosition(positions[2]);
        vertices[3].SetPosition(positions[3]);

        vertices[0].SetColor(_r, _g, _b, _a);
        vertices[1].SetColor(_r, _g, _b, _a);
        vertices[2].SetColor(_r, _g, _b, _a);
        vertices[3].SetColor(_r, _g, _b, _a);

        meshRenderer->PushBackVertex(vertices[0]);
        meshRenderer->PushBackVertex(vertices[1]);
        meshRenderer->PushBackVertex(vertices[2]);
        meshRenderer->PushBackVertex(vertices[3]);

        meshRenderer->PushBackIndex(indices[0]);
        meshRenderer->PushBackIndex(indices[1]);
        meshRenderer->PushBackIndex(indices[2]);
        meshRenderer->PushBackIndex(indices[3]);
        meshRenderer->PushBackIndex(indices[4]);
        meshRenderer->PushBackIndex(indices[5]);

        Mesh mesh;
        mesh.SetIndexCount(6);
        mesh.SetIndexStart(0);

        entityPtr->PushBackMesh(mesh);
    }
    break;

    case EVertexLayout_Pos_UV:
    {
        MeshRendererUV* meshRenderer = entityPtr->AddMeshRenderer<MeshRendererUV>();

        eosVector(VertexUV) vertices;
        vertices.resize(4);

        vertices[0].SetPosition(positions[0]);
        vertices[1].SetPosition(positions[1]);
        vertices[2].SetPosition(positions[2]);
        vertices[3].SetPosition(positions[3]);

        vertices[0].SetTexCoordUV(1.0f, 0.0f);
        vertices[1].SetTexCoordUV(0.0f, 0.0f);
        vertices[2].SetTexCoordUV(0.0f, 1.0f);
        vertices[3].SetTexCoordUV(1.0f, 1.0f);

        meshRenderer->PushBackVertex(vertices[0]);
        meshRenderer->PushBackVertex(vertices[1]);
        meshRenderer->PushBackVertex(vertices[2]);
        meshRenderer->PushBackVertex(vertices[3]);

        meshRenderer->PushBackIndex(indices[0]);
        meshRenderer->PushBackIndex(indices[1]);
        meshRenderer->PushBackIndex(indices[2]);
        meshRenderer->PushBackIndex(indices[3]);
        meshRenderer->PushBackIndex(indices[4]);
        meshRenderer->PushBackIndex(indices[5]);

        Mesh mesh;
        mesh.SetIndexCount(6);
        mesh.SetIndexStart(0);

        entityPtr->PushBackMesh(mesh);
    }
    break;

    case EVertexLayout_Pos_Normal:
    {
        MeshRendererNormal* meshRenderer = entityPtr->AddMeshRenderer<MeshRendererNormal>();

        eosVector(VertexNormal) vertices;
        vertices.resize(4);

        vertices[0].SetPosition(positions[0]);
        vertices[1].SetPosition(positions[1]);
        vertices[2].SetPosition(positions[2]);
        vertices[3].SetPosition(positions[3]);

        Vector normals[4];
        GeometryHelper::CalculateNormalPerVertex(positions, indices.data(), 6, normals);

        vertices[0].SetNormal(normals[0]);
        vertices[1].SetNormal(normals[1]);
        vertices[2].SetNormal(normals[2]);
        vertices[3].SetNormal(normals[3]);

        meshRenderer->PushBackVertex(vertices[0]);
        meshRenderer->PushBackVertex(vertices[1]);
        meshRenderer->PushBackVertex(vertices[2]);
        meshRenderer->PushBackVertex(vertices[3]);

        meshRenderer->PushBackIndex(indices[0]);
        meshRenderer->PushBackIndex(indices[1]);
        meshRenderer->PushBackIndex(indices[2]);
        meshRenderer->PushBackIndex(indices[3]);
        meshRenderer->PushBackIndex(indices[4]);
        meshRenderer->PushBackIndex(indices[5]);

        Mesh mesh;
        mesh.SetIndexCount(6);
        mesh.SetIndexStart(0);

        entityPtr->PushBackMesh(mesh);
    }
    break;

    case EVertexLayout_Pos_UV_Normal:
    {
        MeshRendererSimple* meshRenderer = entityPtr->AddMeshRenderer<MeshRendererSimple>();

        eosVector(VertexSimple) vertices;
        vertices.resize(4);

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

        meshRenderer->PushBackVertex(vertices[0]);
        meshRenderer->PushBackVertex(vertices[1]);
        meshRenderer->PushBackVertex(vertices[2]);
        meshRenderer->PushBackVertex(vertices[3]);

        meshRenderer->PushBackIndex(indices[0]);
        meshRenderer->PushBackIndex(indices[1]);
        meshRenderer->PushBackIndex(indices[2]);
        meshRenderer->PushBackIndex(indices[3]);
        meshRenderer->PushBackIndex(indices[4]);
        meshRenderer->PushBackIndex(indices[5]);

        Mesh mesh;
        mesh.SetIndexCount(6);
        mesh.SetIndexStart(0);

        entityPtr->PushBackMesh(mesh);
    }
    break;

    case EVertexLayout_Full:
    {
        MeshRenderer* meshRenderer = entityPtr->AddMeshRenderer<MeshRenderer>();

        eosVector(Vertex) vertices;
        vertices.resize(4);

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
        
        Vector uvuv[4] = { Vector(0.0f, 0.0f, 1.0f, 1.0f), Vector(0.0f, 0.0f, 1.0f, 1.0f), Vector(0.0f, 0.0f, 1.0f, 1.0f), Vector(0.0f, 0.0f, 1.0f, 1.0f) };

        vertices[0].SetTexCoordUV0(uvuv[0]);
        vertices[1].SetTexCoordUV0(uvuv[1]);
        vertices[2].SetTexCoordUV0(uvuv[2]);
        vertices[3].SetTexCoordUV0(uvuv[3]);

        vertices[0].SetColor(_r, _g, _b, _a);
        vertices[1].SetColor(_r, _g, _b, _a);
        vertices[2].SetColor(_r, _g, _b, _a);
        vertices[3].SetColor(_r, _g, _b, _a);

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

        meshRenderer->PushBackVertex(vertices[0]);
        meshRenderer->PushBackVertex(vertices[1]);
        meshRenderer->PushBackVertex(vertices[2]);
        meshRenderer->PushBackVertex(vertices[3]);

        meshRenderer->PushBackIndex(indices[0]);
        meshRenderer->PushBackIndex(indices[1]);
        meshRenderer->PushBackIndex(indices[2]);
        meshRenderer->PushBackIndex(indices[3]);
        meshRenderer->PushBackIndex(indices[4]);
        meshRenderer->PushBackIndex(indices[5]);

        Mesh mesh;
        mesh.SetIndexCount(6);
        mesh.SetIndexStart(0);

        entityPtr->PushBackMesh(mesh);
    }
    break;

    case EVertexLayout_Empty:
    {
        entityPtr->AddMeshRenderer<BaseMeshRenderer>();

        Mesh mesh;
        mesh.SetIndexCount(0);
        mesh.SetIndexStart(0);

        entityPtr->PushBackMesh(mesh);
    }
    break;

    case EVertexLayout_Unknow:
    case EVertexLayout_Count:
        ionAssertReturnVoid(false, "Layout does not exist!");
        break;


    default:
        ionAssertReturnVoid(false, "Layout not yet implemented");
        break;
    }

    entityPtr->GetBoundingBox()->Expande(positions[0], positions[1]);
    entityPtr->GetBoundingBox()->Expande(positions[2], positions[3]);
}


///
///            CUBE
///
void PrimitiveFactory::GenerateCube(EVertexLayout _layout, ObjectHandler& _entity, ionFloat _r /*= 1.0f*/, ionFloat _g /*= 1.0f*/, ionFloat _b /*= 1.0f*/, ionFloat _a /*= 1.0f*/)
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
        Vector(-0.5f, 0.5f, 0.5f, 1.0f),
        Vector(-0.5f, -0.5f, 0.5f, 1.0f),
        Vector(0.5f, -0.5f, 0.5f, 1.0f),
        Vector(0.5f, 0.5f, 0.5f, 1.0f),

        Vector(0.5f, 0.5f, -0.5f, 1.0f),
        Vector(0.5f, -0.5f, -0.5f, 1.0f),
        Vector(-0.5f, -0.5f, -0.5f, 1.0f),
        Vector(-0.5f, 0.5f, -0.5f, 1.0f),

        Vector(0.5f, 0.5f, 0.5f, 1.0f),
        Vector(0.5f, -0.5f, 0.5f, 1.0f),
        Vector(0.5f, -0.5f, -0.5f, 1.0f),
        Vector(0.5f, 0.5f, -0.5f, 1.0f),

        Vector(-0.5f, 0.5f, -0.5f, 1.0f),
        Vector(-0.5f, 0.5f, 0.5f, 1.0f),
        Vector(0.5f, 0.5f, 0.5f, 1.0f),
        Vector(0.5f, 0.5f, -0.5f, 1.0f),

        Vector(-0.5f, 0.5f, -0.5f, 1.0f),
        Vector(-0.5f, -0.5f, -0.5f, 1.0f),
        Vector(-0.5f, -0.5f, 0.5f, 1.0f),
        Vector(-0.5f, 0.5f, 0.5f, 1.0f),

        Vector(-0.5f, -0.5f, 0.5f, 1.0f),
        Vector(-0.5f, -0.5f, -0.5f, 1.0f),
        Vector(0.5f, -0.5f, -0.5f, 1.0f),
        Vector(0.5f, -0.5f, 0.5f, 1.0f)
    };

    ionFloat texCoords[] = {
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
        0.0f, 1.0f,
        0.0f, 0.0f,
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

    Vector uvuv[24] = {
        Vector(1.0f, 0.0f, 0.0f, 1.0f),
        Vector(1.0f, 1.0f, 0.0f, 0.0f),
        Vector(0.0f, 1.0f, 1.0f, 0.0f),
        Vector(0.0f, 0.0f, 1.0f, 1.0f),

        Vector(1.0f, 0.0f, 0.0f, 1.0f),
        Vector(1.0f, 1.0f, 0.0f, 0.0f),
        Vector(0.0f, 1.0f, 1.0f, 0.0f),
        Vector(0.0f, 0.0f, 1.0f, 1.0f),

        Vector(1.0f, 0.0f, 0.0f, 1.0f),
        Vector(1.0f, 1.0f, 0.0f, 0.0f),
        Vector(0.0f, 1.0f, 1.0f, 0.0f),
        Vector(0.0f, 0.0f, 1.0f, 1.0f),

        Vector(1.0f, 0.0f, 0.0f, 1.0f),
        Vector(1.0f, 1.0f, 0.0f, 0.0f),
        Vector(0.0f, 1.0f, 1.0f, 0.0f),
        Vector(0.0f, 0.0f, 1.0f, 1.0f),

        Vector(1.0f, 0.0f, 0.0f, 1.0f),
        Vector(1.0f, 1.0f, 0.0f, 0.0f),
        Vector(0.0f, 1.0f, 1.0f, 0.0f),
        Vector(0.0f, 0.0f, 1.0f, 1.0f),

        Vector(1.0f, 0.0f, 0.0f, 1.0f),
        Vector(1.0f, 1.0f, 0.0f, 0.0f),
        Vector(0.0f, 1.0f, 1.0f, 0.0f),
        Vector(0.0f, 0.0f, 1.0f, 1.0f),
    };

    Entity* entityPtr = dynamic_cast<Entity*>(_entity.GetPtr());

    switch (_layout)
    {
    case EVertexLayout_Pos:
    {
        MeshRendererPlain* meshRenderer = entityPtr->AddMeshRenderer<MeshRendererPlain>();

        eosVector(VertexPlain) vertices;
        vertices.resize(24);

        for (ionU32 i = 0; i < 24; ++i)
        {
            vertices[i].SetPosition(positions[i]);
        }
        for (ionU32 i = 0; i < 24; ++i)
        {
            meshRenderer->PushBackVertex(vertices[i]);
        }
        for (ionU32 i = 0; i < 36; ++i)
        {
            meshRenderer->PushBackIndex(indices[i]);
        }

        Mesh mesh;
        mesh.SetIndexCount(36);
        mesh.SetIndexStart(0);

        entityPtr->PushBackMesh(mesh);
    }
    break;

    case EVertexLayout_Pos_Color:
    {
        MeshRendererColored* meshRenderer = entityPtr->AddMeshRenderer<MeshRendererColored>();

        eosVector(VertexColored) vertices;
        vertices.resize(24); 

        for (ionU32 i = 0; i < 24; ++i)
        {
            vertices[i].SetPosition(positions[i]);
            vertices[i].SetColor(_r, _g, _b, _a);
        }
        for (ionU32 i = 0; i < 24; ++i)
        {
            meshRenderer->PushBackVertex(vertices[i]);
        }
        for (ionU32 i = 0; i < 36; ++i)
        {
            meshRenderer->PushBackIndex(indices[i]);
        }

        Mesh mesh;
        mesh.SetIndexCount(36);
        mesh.SetIndexStart(0);

        entityPtr->PushBackMesh(mesh);
    }
    break;

    case EVertexLayout_Pos_UV:
    {
        MeshRendererUV* meshRenderer = entityPtr->AddMeshRenderer<MeshRendererUV>();

        eosVector(VertexUV) vertices;
        vertices.resize(24);

        for (ionU32 i = 0, j = 0; i < 24; ++i, j += 2)
        {
            vertices[i].SetPosition(positions[i]);
            vertices[i].SetTexCoordUV(texCoords[j], texCoords[j + 1]);
        }
        for (ionU32 i = 0; i < 24; ++i)
        {
            meshRenderer->PushBackVertex(vertices[i]);
        }
        for (ionU32 i = 0; i < 36; ++i)
        {
            meshRenderer->PushBackIndex(indices[i]);
        }

        Mesh mesh;
        mesh.SetIndexCount(36);
        mesh.SetIndexStart(0);

        entityPtr->PushBackMesh(mesh);
    }
    break;

    case EVertexLayout_Pos_Normal:
    {
        MeshRendererNormal* meshRenderer = entityPtr->AddMeshRenderer<MeshRendererNormal>();

        eosVector(VertexNormal) vertices;
        vertices.resize(24);

        for (ionU32 i = 0, j = 0; i < 24; ++i, j += 2)
        {
            vertices[i].SetPosition(positions[i]);
            vertices[i].SetNormal(normals[i]);
        }
        for (ionU32 i = 0; i < 24; ++i)
        {
            meshRenderer->PushBackVertex(vertices[i]);
        }
        for (ionU32 i = 0; i < 36; ++i)
        {
            meshRenderer->PushBackIndex(indices[i]);
        }

        Mesh mesh;
        mesh.SetIndexCount(36);
        mesh.SetIndexStart(0);

        entityPtr->PushBackMesh(mesh);
    }
    break;

    case EVertexLayout_Pos_UV_Normal:
    {
        MeshRendererSimple* meshRenderer = entityPtr->AddMeshRenderer<MeshRendererSimple>();

        eosVector(VertexSimple) vertices;
        vertices.resize(24);

        for (ionU32 i = 0, j = 0; i < 24; ++i, j += 2)
        {
            vertices[i].SetPosition(positions[i]);
            vertices[i].SetTexCoordUV(texCoords[j], texCoords[j + 1]);
            vertices[i].SetNormal(normals[i]);
        }
        for (ionU32 i = 0; i < 24; ++i)
        {
            meshRenderer->PushBackVertex(vertices[i]);
        }
        for (ionU32 i = 0; i < 36; ++i)
        {
            meshRenderer->PushBackIndex(indices[i]);
        }

        Mesh mesh;
        mesh.SetIndexCount(36);
        mesh.SetIndexStart(0);

        entityPtr->PushBackMesh(mesh);
    }
    break;

    case EVertexLayout_Full:
    {
        MeshRenderer* meshRenderer = entityPtr->AddMeshRenderer<MeshRenderer>();

        eosVector(Vertex) vertices;
        vertices.resize(24);

        Vector tangents[24];
        GeometryHelper::CalculateTangent(positions, normals, uvuv, 24, indices.data(), 36, tangents);

        for (ionU32 i = 0; i < 24; ++i)
        {
            vertices[i].SetPosition(positions[i]);
            vertices[i].SetTexCoordUV0(uvuv[i]);
            vertices[i].SetNormal(normals[i]);
            vertices[i].SetColor(_r, _g, _b, _a);
            vertices[i].SetTangent(tangents[i]);
        }
        for (ionU32 i = 0; i < 24; ++i)
        {
            meshRenderer->PushBackVertex(vertices[i]);
        }
        for (ionU32 i = 0; i < 36; ++i)
        {
            meshRenderer->PushBackIndex(indices[i]);
        }

        Mesh mesh;
        mesh.SetIndexCount(36);
        mesh.SetIndexStart(0);

        entityPtr->PushBackMesh(mesh);
    }
    break;

    case EVertexLayout_Empty:
    {
        entityPtr->AddMeshRenderer<BaseMeshRenderer>();

        Mesh mesh;
        mesh.SetIndexCount(0);
        mesh.SetIndexStart(0);

        entityPtr->PushBackMesh(mesh);
    }
    break;

    case EVertexLayout_Unknow:
    case EVertexLayout_Count:
        ionAssertReturnVoid(false, "Layout does not exist!");
        break;


    default:
        ionAssertReturnVoid(false, "Layout not yet implemented");
        break;
    }

    for (ionU32 i = 1; i < 24; ++i)
    {
        entityPtr->GetBoundingBox()->Expande(positions[i - 1], positions[i]);
    }
}


///
///            SPHERE
///
void PrimitiveFactory::GenerateSphere(EVertexLayout _layout, ObjectHandler& _entity, ionFloat _r /*= 1.0f*/, ionFloat _g /*= 1.0f*/, ionFloat _b /*= 1.0f*/, ionFloat _a /*= 1.0f*/)
{
    static const ionFloat radius = 1.0f;
    static const ionU32 rings = 24;
    static const ionU32 sectors = 48;

    eosVector(Vector) positions;
    eosVector(Vector) normals;
    eosVector(Vector) uvuv;
    eosVector(Index) indices;

    Entity* entityPtr = dynamic_cast<Entity*>(_entity.GetPtr());

    switch (_layout)
    {
    case EVertexLayout_Pos:
    {
        MeshRendererPlain* meshRenderer = entityPtr->AddMeshRenderer<MeshRendererPlain>();

        eosVector(VertexPlain) vertices; 

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
            meshRenderer->PushBackVertex(vertices[i]);
        }
        for (ionU32 i = 0; i < indicesSize; ++i)
        {
            meshRenderer->PushBackIndex(indices[i]);
        }


        Mesh mesh;
        mesh.SetIndexCount(indicesSize);
        mesh.SetIndexStart(0);

        entityPtr->PushBackMesh(mesh);
    }
    break;

    case EVertexLayout_Pos_Color:
    {
        MeshRendererColored* meshRenderer = entityPtr->AddMeshRenderer<MeshRendererColored>();

        eosVector(VertexColored) vertices;

        const ionU32 verticesSize = rings * sectors;
        const ionU32 indicesSize = rings * sectors * 6;

        const ionFloat R = 1.0f / static_cast<ionFloat>(rings - 1);
        const ionFloat S = 1.0f / static_cast<ionFloat>(sectors - 1);
        ionS32 r, s;

        vertices.resize(verticesSize);
        indices.resize(indicesSize);

        positions.resize(verticesSize);
        normals.resize(verticesSize);

        std::vector<Vector>::iterator v = positions.begin();
        std::vector<Vector>::iterator n = normals.begin();


        for (r = 0; r < rings; ++r)
        {
            for (s = 0; s < sectors; ++s)
            {
                const ionFloat y = std::sin(-MathHelper::kHalfPI + MathHelper::kPI * r * R);
                const ionFloat x = std::cos(2 * MathHelper::kPI * s * S) * std::sin(MathHelper::kPI * r * R);
                const ionFloat z = std::sin(2 * MathHelper::kPI * s * S) * std::sin(MathHelper::kPI * r * R);

                *v++ = VectorHelper::Set(x * radius, y * radius, z * radius, 1.0f);
                *n++ = VectorHelper::Set(x, y, z, 1.0f);
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
            vertices[i].SetColor(_r, _g, _b, _a);
        }
        for (ionU32 i = 0; i < verticesSize; ++i)
        {
            meshRenderer->PushBackVertex(vertices[i]);
        }
        for (ionU32 i = 0; i < indicesSize; ++i)
        {
            meshRenderer->PushBackIndex(indices[i]);
        }

        Mesh mesh;
        mesh.SetIndexCount(indicesSize);
        mesh.SetIndexStart(0);

        entityPtr->PushBackMesh(mesh);
    }
    break;

    case EVertexLayout_Pos_UV:
    {
        MeshRendererUV* meshRenderer = entityPtr->AddMeshRenderer<MeshRendererUV>();

        eosVector(VertexUV) vertices;

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
                *t++ = VectorHelper::Set(1.0f - (s * S), 1.0f - (r * R), 1.0f - (s * S), 1.0f - (r * R));
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
            meshRenderer->PushBackVertex(vertices[i]);
        }
        for (ionU32 i = 0; i < indicesSize; ++i)
        {
            meshRenderer->PushBackIndex(indices[i]);
        }

        Mesh mesh;
        mesh.SetIndexCount(indicesSize);
        mesh.SetIndexStart(0);

        entityPtr->PushBackMesh(mesh);
    }
    break;

    case EVertexLayout_Pos_Normal:
    {
        MeshRendererNormal* meshRenderer = entityPtr->AddMeshRenderer<MeshRendererNormal>();

        eosVector(VertexNormal) vertices;

        const ionU32 verticesSize = rings * sectors;
        const ionU32 indicesSize = rings * sectors * 6;

        const ionFloat R = 1.0f / static_cast<ionFloat>(rings - 1);
        const ionFloat S = 1.0f / static_cast<ionFloat>(sectors - 1);
        ionS32 r, s;

        vertices.resize(verticesSize);
        indices.resize(indicesSize);

        positions.resize(verticesSize);
        normals.resize(verticesSize);

        std::vector<Vector>::iterator v = positions.begin();
        std::vector<Vector>::iterator n = normals.begin();

        for (r = 0; r < rings; ++r)
        {
            for (s = 0; s < sectors; ++s)
            {
                const ionFloat y = std::sin(-MathHelper::kHalfPI + MathHelper::kPI * r * R);
                const ionFloat x = std::cos(2 * MathHelper::kPI * s * S) * std::sin(MathHelper::kPI * r * R);
                const ionFloat z = std::sin(2 * MathHelper::kPI * s * S) * std::sin(MathHelper::kPI * r * R);

                *v++ = VectorHelper::Set(x * radius, y * radius, z * radius, 1.0f);
                *n++ = VectorHelper::Set(x, y, z, 1.0f);
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
            vertices[i].SetNormal(normals[i]);
        }
        for (ionU32 i = 0; i < verticesSize; ++i)
        {
            meshRenderer->PushBackVertex(vertices[i]);
        }
        for (ionU32 i = 0; i < indicesSize; ++i)
        {
            meshRenderer->PushBackIndex(indices[i]);
        }

        Mesh mesh;
        mesh.SetIndexCount(indicesSize);
        mesh.SetIndexStart(0);

        entityPtr->PushBackMesh(mesh);
    }
    break;

    case EVertexLayout_Pos_UV_Normal:
    {
        MeshRendererSimple* meshRenderer = entityPtr->AddMeshRenderer<MeshRendererSimple>();

        eosVector(VertexSimple) vertices;

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
                *t++ = VectorHelper::Set(1.0f - (s * S), 1.0f - (r * R), 1.0f - (s * S), 1.0f - (r * R));
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
            meshRenderer->PushBackVertex(vertices[i]);
        }
        for (ionU32 i = 0; i < indicesSize; ++i)
        {
            meshRenderer->PushBackIndex(indices[i]);
        }

        Mesh mesh;
        mesh.SetIndexCount(indicesSize);
        mesh.SetIndexStart(0);

        entityPtr->PushBackMesh(mesh);
    }
    break;

    case EVertexLayout_Full:
    {
        MeshRenderer* meshRenderer = entityPtr->AddMeshRenderer<MeshRenderer>();

        eosVector(Vertex) vertices;

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
                *t++ = VectorHelper::Set(1.0f - (s * S), 1.0f - (r * R), 1.0f - (s * S), 1.0f - (r * R));
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
            vertices[i].SetColor(_r, _g, _b, _a);
            vertices[i].SetTangent(tangents[i]);
        }
        for (ionU32 i = 0; i < verticesSize; ++i)
        {
            meshRenderer->PushBackVertex(vertices[i]);
        }
        for (ionU32 i = 0; i < indicesSize; ++i)
        {
            meshRenderer->PushBackIndex(indices[i]);
        }

        Mesh mesh;
        mesh.SetIndexCount(indicesSize);
        mesh.SetIndexStart(0);

        entityPtr->PushBackMesh(mesh);
    }
    break;

    case EVertexLayout_Empty:
    {
        entityPtr->AddMeshRenderer<BaseMeshRenderer>();

        Mesh mesh;
        mesh.SetIndexCount(0);
        mesh.SetIndexStart(0);

        entityPtr->PushBackMesh(mesh);
    }
    break;

    case EVertexLayout_Unknow:
    case EVertexLayout_Count:
        ionAssertReturnVoid(false, "Layout does not exist!");
        break;


    default:
        ionAssertReturnVoid(false, "Layout not yet implemented");
        break;
    }

    const ionSize count = positions.size();
    for (ionU32 i = 1; i < count; ++i)
    {
        entityPtr->GetBoundingBox()->Expande(positions[i - 1], positions[i]);
    }
}

ION_NAMESPACE_END