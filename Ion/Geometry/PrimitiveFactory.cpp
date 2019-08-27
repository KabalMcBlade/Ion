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
	Entity* entityPtr = dynamic_cast<Entity*>(_entity());

	GenerateTriangle(_layout, entityPtr, _r, _g, _b, _a);
}

///
///            QUAD
///
void PrimitiveFactory::GenerateQuad(EVertexLayout _layout, ObjectHandler& _entity, ionFloat _r /*= 1.0f*/, ionFloat _g /*= 1.0f*/, ionFloat _b /*= 1.0f*/, ionFloat _a /*= 1.0f*/)
{
	Entity* entityPtr = dynamic_cast<Entity*>(_entity());

	GenerateQuad(_layout, entityPtr, _r, _g, _b, _a);
}

///
///            CUBE
///
void PrimitiveFactory::GenerateCube(EVertexLayout _layout, ObjectHandler& _entity, ionFloat _r /*= 1.0f*/, ionFloat _g /*= 1.0f*/, ionFloat _b /*= 1.0f*/, ionFloat _a /*= 1.0f*/)
{
	Entity* entityPtr = dynamic_cast<Entity*>(_entity());

	GenerateCube(_layout, entityPtr, _r, _g, _b, _a);
}

///
///            SPHERE
///
void PrimitiveFactory::GenerateSphere(EVertexLayout _layout, ObjectHandler& _entity, ionFloat _r /*= 1.0f*/, ionFloat _g /*= 1.0f*/, ionFloat _b /*= 1.0f*/, ionFloat _a /*= 1.0f*/)
{
	Entity* entityPtr = dynamic_cast<Entity*>(_entity());

	GenerateSphere(_layout, entityPtr, _r, _g, _b, _a);
}

///
///            PYRAMID
///
void PrimitiveFactory::GeneratePyramd(EVertexLayout _layout, ObjectHandler& _entity, ionFloat _r /*= 1.0f*/, ionFloat _g /*= 1.0f*/, ionFloat _b /*= 1.0f*/, ionFloat _a /*= 1.0f*/)
{
	Entity* entityPtr = dynamic_cast<Entity*>(_entity());

	GeneratePyramd(_layout, entityPtr, _r, _g, _b, _a);
}



//////////////////////////////////////////////////////////////////////////


///
///            TRIANGLE
///
void PrimitiveFactory::GenerateTriangle(EVertexLayout _layout, Entity* _entity, ionFloat _r /*= 1.0f*/, ionFloat _g /*= 1.0f*/, ionFloat _b /*= 1.0f*/, ionFloat _a /*= 1.0f*/)
{
	ionVector<Index> indices;
	indices->resize(3);
	indices = { 0, 1, 2 };

	Vector positions[3] = { Vector(0.0f, 0.5f, 0.0f, 1.0f), Vector(0.5f, -0.5f, 0.0f, 1.0f), Vector(-0.5f, -0.5f, 0.0f, 1.0f) };

	switch (_layout)
	{
	case EVertexLayout_Pos:
	{
		MeshRendererPlain* meshRenderer = _entity->AddMeshRenderer<MeshRendererPlain>();

		ionVector<VertexPlain> vertices;
		vertices->resize(3);

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

		_entity->PushBackMesh(mesh);
	}
	break;

	case EVertexLayout_Pos_Color:
	{
		MeshRendererColored* meshRenderer = _entity->AddMeshRenderer<MeshRendererColored>();

		ionVector<VertexColored> vertices;
		vertices->resize(3);

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

		_entity->PushBackMesh(mesh);
	}
	break;

	case EVertexLayout_Pos_UV:
	{
		MeshRendererUV* meshRenderer = _entity->AddMeshRenderer<MeshRendererUV>();

		ionVector<VertexUV> vertices;
		vertices->resize(3);

		vertices[0].SetPosition(positions[0]);
		vertices[1].SetPosition(positions[1]);
		vertices[2].SetPosition(positions[2]);

		Vector uvuv[3];
		GeometryHelper::CalculateUVs(positions, 3, uvuv);
		for (ionU32 i = 0; i < vertices->size(); ++i)
		{
			vertices[i].SetTexCoordUV(uvuv[i]);
		}

		meshRenderer->PushBackVertex(vertices[0]);
		meshRenderer->PushBackVertex(vertices[1]);
		meshRenderer->PushBackVertex(vertices[2]);

		meshRenderer->PushBackIndex(indices[0]);
		meshRenderer->PushBackIndex(indices[1]);
		meshRenderer->PushBackIndex(indices[2]);

		Mesh mesh;
		mesh.SetIndexCount(3);
		mesh.SetIndexStart(0);

		_entity->PushBackMesh(mesh);
	}
	break;

	case EVertexLayout_Pos_UV_Normal:
	{
		MeshRendererSimple* meshRenderer = _entity->AddMeshRenderer<MeshRendererSimple>();

		ionVector<VertexSimple> vertices;
		vertices->resize(3);

		vertices[0].SetPosition(positions[0]);
		vertices[1].SetPosition(positions[1]);
		vertices[2].SetPosition(positions[2]);

		Vector uvuv[3];
		GeometryHelper::CalculateUVs(positions, 3, uvuv);
		for (ionU32 i = 0; i < vertices->size(); ++i)
		{
			vertices[i].SetTexCoordUV(uvuv[i]);
		}

		Vector normals[3];
		GeometryHelper::CalculateNormals(positions, 3, indices->data(), 3, normals);
		for (ionU32 i = 0; i < vertices->size(); ++i)
		{
			vertices[i].SetNormal(normals[i]);
		}

		meshRenderer->PushBackVertex(vertices[0]);
		meshRenderer->PushBackVertex(vertices[1]);
		meshRenderer->PushBackVertex(vertices[2]);

		meshRenderer->PushBackIndex(indices[0]);
		meshRenderer->PushBackIndex(indices[1]);
		meshRenderer->PushBackIndex(indices[2]);

		Mesh mesh;
		mesh.SetIndexCount(3);
		mesh.SetIndexStart(0);

		_entity->PushBackMesh(mesh);
	}
	break;

	case EVertexLayout_Pos_Normal:
	{
		MeshRendererNormal* meshRenderer = _entity->AddMeshRenderer<MeshRendererNormal>();

		ionVector<VertexNormal> vertices;
		vertices->resize(3);

		vertices[0].SetPosition(positions[0]);
		vertices[1].SetPosition(positions[1]);
		vertices[2].SetPosition(positions[2]);

		Vector normals[3];
		GeometryHelper::CalculateNormals(positions, 3, indices->data(), 3, normals);
		for (ionU32 i = 0; i < vertices->size(); ++i)
		{
			vertices[i].SetNormal(normals[i]);
		}

		meshRenderer->PushBackVertex(vertices[0]);
		meshRenderer->PushBackVertex(vertices[1]);
		meshRenderer->PushBackVertex(vertices[2]);

		meshRenderer->PushBackIndex(indices[0]);
		meshRenderer->PushBackIndex(indices[1]);
		meshRenderer->PushBackIndex(indices[2]);

		Mesh mesh;
		mesh.SetIndexCount(3);
		mesh.SetIndexStart(0);

		_entity->PushBackMesh(mesh);
	}
	break;

	case EVertexLayout_Full:
	{
		MeshRenderer* meshRenderer = _entity->AddMeshRenderer<MeshRenderer>();

		ionVector<Vertex> vertices;
		vertices->resize(3);

		vertices[0].SetPosition(positions[0]);
		vertices[1].SetPosition(positions[1]);
		vertices[2].SetPosition(positions[2]);

		Vector uvuv[3];
		GeometryHelper::CalculateUVs(positions, 3, uvuv);
		for (ionU32 i = 0; i < vertices->size(); ++i)
		{
			vertices[i].SetTexCoordUV0(uvuv[i]);
			vertices[i].SetTexCoordUV1(uvuv[i]);
		}

		Vector normals[3];
		GeometryHelper::CalculateNormals(positions, 3, indices->data(), 3, normals);
		for (ionU32 i = 0; i < vertices->size(); ++i)
		{
			vertices[i].SetNormal(normals[i]);
		}

		vertices[0].SetColor(_r, _g, _b, _a);
		vertices[1].SetColor(_r, _g, _b, _a);
		vertices[2].SetColor(_r, _g, _b, _a);

		Vector tangents[3];
		ionFloat bitangentsign[3];
		GeometryHelper::CalculateTangents(positions, normals, uvuv, 3, indices->data(), 3, tangents, bitangentsign);

		vertices[0].SetTangent(tangents[0]);
		vertices[1].SetTangent(tangents[1]);
		vertices[2].SetTangent(tangents[2]);

		vertices[0].SetBiTangentSign(bitangentsign[0]);
		vertices[1].SetBiTangentSign(bitangentsign[1]);
		vertices[2].SetBiTangentSign(bitangentsign[2]);

		meshRenderer->PushBackVertex(vertices[0]);
		meshRenderer->PushBackVertex(vertices[1]);
		meshRenderer->PushBackVertex(vertices[2]);

		meshRenderer->PushBackIndex(indices[0]);
		meshRenderer->PushBackIndex(indices[1]);
		meshRenderer->PushBackIndex(indices[2]);

		Mesh mesh;
		mesh.SetIndexCount(3);
		mesh.SetIndexStart(0);

		_entity->PushBackMesh(mesh);
	}
	break;

	case EVertexLayout_Empty:
	{
		_entity->AddMeshRenderer<BaseMeshRenderer>();

		Mesh mesh;
		mesh.SetIndexCount(0);
		mesh.SetIndexStart(0);

		_entity->PushBackMesh(mesh);
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

	_entity->GetBoundingBox()->Expande(positions[0], positions[1]);
	_entity->GetBoundingBox()->Expande(positions[0], positions[2]);
}

///
///            QUAD
///
void PrimitiveFactory::GenerateQuad(EVertexLayout _layout, Entity* _entity, ionFloat _r /*= 1.0f*/, ionFloat _g /*= 1.0f*/, ionFloat _b /*= 1.0f*/, ionFloat _a /*= 1.0f*/)
{
	ionVector<Index> indices;
	indices->resize(6);
	indices = { 0, 1, 2, 2, 3, 0 };

	Vector positions[4] = { Vector(0.5f, 0.5f, 0.0f, 1.0f), Vector(-0.5f, 0.5f, 0.0f, 1.0f), Vector(-0.5f, -0.5f, 0.0f, 1.0f), Vector(0.5f, -0.5f, 0.0f, 1.0f) };

	switch (_layout)
	{
	case EVertexLayout_Pos:
	{
		MeshRendererPlain* meshRenderer = _entity->AddMeshRenderer<MeshRendererPlain>();

		ionVector<VertexPlain> vertices;
		vertices->resize(4);

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

		_entity->PushBackMesh(mesh);
	}
	break;

	case EVertexLayout_Pos_Color:
	{
		MeshRendererColored* meshRenderer = _entity->AddMeshRenderer<MeshRendererColored>();

		ionVector<VertexColored> vertices;
		vertices->resize(4);

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

		_entity->PushBackMesh(mesh);
	}
	break;

	case EVertexLayout_Pos_UV:
	{
		MeshRendererUV* meshRenderer = _entity->AddMeshRenderer<MeshRendererUV>();

		ionVector<VertexUV> vertices;
		vertices->resize(4);

		vertices[0].SetPosition(positions[0]);
		vertices[1].SetPosition(positions[1]);
		vertices[2].SetPosition(positions[2]);
		vertices[3].SetPosition(positions[3]);

		Vector uvuv[4];
		GeometryHelper::CalculateUVs(positions, 4, uvuv);
		for (ionU32 i = 0; i < vertices->size(); ++i)
		{
			vertices[i].SetTexCoordUV(uvuv[i]);
		}

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

		_entity->PushBackMesh(mesh);
	}
	break;

	case EVertexLayout_Pos_Normal:
	{
		MeshRendererNormal* meshRenderer = _entity->AddMeshRenderer<MeshRendererNormal>();

		ionVector<VertexNormal> vertices;
		vertices->resize(4);

		vertices[0].SetPosition(positions[0]);
		vertices[1].SetPosition(positions[1]);
		vertices[2].SetPosition(positions[2]);
		vertices[3].SetPosition(positions[3]);

		Vector normals[4];
		GeometryHelper::CalculateNormals(positions, 4, indices->data(), 6, normals);
		for (ionU32 i = 0; i < vertices->size(); ++i)
		{
			vertices[i].SetNormal(normals[i]);
		}

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

		_entity->PushBackMesh(mesh);
	}
	break;

	case EVertexLayout_Pos_UV_Normal:
	{
		MeshRendererSimple* meshRenderer = _entity->AddMeshRenderer<MeshRendererSimple>();

		ionVector<VertexSimple> vertices;
		vertices->resize(4);

		vertices[0].SetPosition(positions[0]);
		vertices[1].SetPosition(positions[1]);
		vertices[2].SetPosition(positions[2]);
		vertices[3].SetPosition(positions[3]);

		Vector uvuv[4];
		GeometryHelper::CalculateUVs(positions, 4, uvuv);
		for (ionU32 i = 0; i < vertices->size(); ++i)
		{
			vertices[i].SetTexCoordUV(uvuv[i]);
		}

		Vector normals[4];
		GeometryHelper::CalculateNormals(positions, 4, indices->data(), 6, normals);
		for (ionU32 i = 0; i < vertices->size(); ++i)
		{
			vertices[i].SetNormal(normals[i]);
		}

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

		_entity->PushBackMesh(mesh);
	}
	break;

	case EVertexLayout_Full:
	{
		MeshRenderer* meshRenderer = _entity->AddMeshRenderer<MeshRenderer>();

		ionVector<Vertex> vertices;
		vertices->resize(4);

		vertices[0].SetPosition(positions[0]);
		vertices[1].SetPosition(positions[1]);
		vertices[2].SetPosition(positions[2]);
		vertices[3].SetPosition(positions[3]);

		Vector uvuv[4];
		GeometryHelper::CalculateUVs(positions, 4, uvuv);
		for (ionU32 i = 0; i < vertices->size(); ++i)
		{
			vertices[i].SetTexCoordUV0(uvuv[i]);
			vertices[i].SetTexCoordUV1(uvuv[i]);
		}

		Vector normals[4];
		GeometryHelper::CalculateNormals(positions, 4, indices->data(), 6, normals);
		for (ionU32 i = 0; i < vertices->size(); ++i)
		{
			vertices[i].SetNormal(normals[i]);
		}

		vertices[0].SetColor(_r, _g, _b, _a);
		vertices[1].SetColor(_r, _g, _b, _a);
		vertices[2].SetColor(_r, _g, _b, _a);
		vertices[3].SetColor(_r, _g, _b, _a);

		Vector tangents[4];
		ionFloat bitangentsign[4];
		GeometryHelper::CalculateTangents(positions, normals, uvuv, 4, indices->data(), 6, tangents, bitangentsign);

		vertices[0].SetTangent(tangents[0]);
		vertices[1].SetTangent(tangents[1]);
		vertices[2].SetTangent(tangents[2]);
		vertices[3].SetTangent(tangents[3]);

		vertices[0].SetBiTangentSign(bitangentsign[0]);
		vertices[1].SetBiTangentSign(bitangentsign[1]);
		vertices[2].SetBiTangentSign(bitangentsign[2]);
		vertices[3].SetBiTangentSign(bitangentsign[3]);

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

		_entity->PushBackMesh(mesh);
	}
	break;

	case EVertexLayout_Empty:
	{
		_entity->AddMeshRenderer<BaseMeshRenderer>();

		Mesh mesh;
		mesh.SetIndexCount(0);
		mesh.SetIndexStart(0);

		_entity->PushBackMesh(mesh);
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

	_entity->GetBoundingBox()->Expande(positions[0], positions[1]);
	_entity->GetBoundingBox()->Expande(positions[2], positions[3]);
}

///
///            CUBE
///
void PrimitiveFactory::GenerateCube(EVertexLayout _layout, Entity* _entity, ionFloat _r /*= 1.0f*/, ionFloat _g /*= 1.0f*/, ionFloat _b /*= 1.0f*/, ionFloat _a /*= 1.0f*/)
{
	ionVector<Index> indices;
	indices->resize(36);
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

	switch (_layout)
	{
	case EVertexLayout_Pos:
	{
		MeshRendererPlain* meshRenderer = _entity->AddMeshRenderer<MeshRendererPlain>();

		ionVector<VertexPlain> vertices;
		vertices->resize(24);

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

		_entity->PushBackMesh(mesh);
	}
	break;

	case EVertexLayout_Pos_Color:
	{
		MeshRendererColored* meshRenderer = _entity->AddMeshRenderer<MeshRendererColored>();

		ionVector<VertexColored> vertices;
		vertices->resize(24);

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

		_entity->PushBackMesh(mesh);
	}
	break;

	case EVertexLayout_Pos_UV:
	{
		MeshRendererUV* meshRenderer = _entity->AddMeshRenderer<MeshRendererUV>();

		ionVector<VertexUV> vertices;
		vertices->resize(24);

		for (ionU32 i = 0; i < 24; ++i)
		{
			vertices[i].SetPosition(positions[i]);
		}

		Vector uvuv[24];
		GeometryHelper::CalculateUVs(positions, 24, uvuv);
		for (ionU32 i = 0; i < vertices->size(); ++i)
		{
			vertices[i].SetTexCoordUV(uvuv[i]);
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

		_entity->PushBackMesh(mesh);
	}
	break;

	case EVertexLayout_Pos_Normal:
	{
		MeshRendererNormal* meshRenderer = _entity->AddMeshRenderer<MeshRendererNormal>();

		ionVector<VertexNormal> vertices;
		vertices->resize(24);

		for (ionU32 i = 0; i < 24; ++i)
		{
			vertices[i].SetPosition(positions[i]);
		}

		Vector normals[24];
		GeometryHelper::CalculateNormals(positions, 24, indices->data(), 36, normals);
		for (ionU32 i = 0; i < vertices->size(); ++i)
		{
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

		_entity->PushBackMesh(mesh);
	}
	break;

	case EVertexLayout_Pos_UV_Normal:
	{
		MeshRendererSimple* meshRenderer = _entity->AddMeshRenderer<MeshRendererSimple>();

		ionVector<VertexSimple> vertices;
		vertices->resize(24);

		for (ionU32 i = 0; i < 24; ++i)
		{
			vertices[i].SetPosition(positions[i]);
		}

		Vector uvuv[24];
		GeometryHelper::CalculateUVs(positions, 24, uvuv);
		for (ionU32 i = 0; i < vertices->size(); ++i)
		{
			vertices[i].SetTexCoordUV(uvuv[i]);
		}

		Vector normals[24];
		GeometryHelper::CalculateNormals(positions, 24, indices->data(), 36, normals);
		for (ionU32 i = 0; i < vertices->size(); ++i)
		{
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

		_entity->PushBackMesh(mesh);
	}
	break;

	case EVertexLayout_Full:
	{
		MeshRenderer* meshRenderer = _entity->AddMeshRenderer<MeshRenderer>();

		ionVector<Vertex> vertices;
		vertices->resize(24);

		for (ionU32 i = 0; i < 24; ++i)
		{
			vertices[i].SetPosition(positions[i]);
			vertices[i].SetColor(_r, _g, _b, _a);
		}

		Vector uvuv[24];
		GeometryHelper::CalculateUVs(positions, 24, uvuv);
		for (ionU32 i = 0; i < vertices->size(); ++i)
		{
			vertices[i].SetTexCoordUV0(uvuv[i]);
			vertices[i].SetTexCoordUV1(uvuv[i]);
		}

		Vector normals[24];
		GeometryHelper::CalculateNormals(positions, 24, indices->data(), 36, normals);
		for (ionU32 i = 0; i < vertices->size(); ++i)
		{
			vertices[i].SetNormal(normals[i]);
		}

		Vector tangents[24];
		ionFloat bitangentsign[24];
		GeometryHelper::CalculateTangents(positions, normals, uvuv, 24, indices->data(), 36, tangents, bitangentsign);
		for (ionU32 i = 0; i < vertices->size(); ++i)
		{
			vertices[i].SetTangent(tangents[i]);
			vertices[i].SetBiTangentSign(bitangentsign[i]);
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

		_entity->PushBackMesh(mesh);
	}
	break;

	case EVertexLayout_Empty:
	{
		_entity->AddMeshRenderer<BaseMeshRenderer>();

		Mesh mesh;
		mesh.SetIndexCount(0);
		mesh.SetIndexStart(0);

		_entity->PushBackMesh(mesh);
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
		_entity->GetBoundingBox()->Expande(positions[i - 1], positions[i]);
	}
}

///
///            SPHERE
///
void PrimitiveFactory::GenerateSphere(EVertexLayout _layout, Entity* _entity, ionFloat _r /*= 1.0f*/, ionFloat _g /*= 1.0f*/, ionFloat _b /*= 1.0f*/, ionFloat _a /*= 1.0f*/)
{
	const ionU32 stacks = 24;   //20;
	const ionU32 slices = 48;   //20

	ionVector<Vector> positions;
	ionVector<Index> indices;

	for (ionU32 i = 0; i <= stacks; ++i)
	{
		ionFloat V = (ionFloat)i / (ionFloat)stacks;
		ionFloat phi = V * kfPI;

		// loop through the slices.
		for (ionU32 j = 0; j <= slices; ++j)
		{

			ionFloat U = (ionFloat)j / (ionFloat)slices;
			ionFloat theta = U * (kfPI * 2);

			// use spherical coordinates to calculate the positions.
			ionFloat x = cos(theta) * sin(phi);
			ionFloat y = cos(phi);
			ionFloat z = sin(theta) * sin(phi);

			positions->push_back(Vector(x, y, z, 1.0f));
		}
	}

	for (ionU32 i = 0; i < slices * stacks + slices; ++i)
	{
		indices->push_back(Index(i));
		indices->push_back(Index(i + slices + 1));
		indices->push_back(Index(i + slices));

		indices->push_back(Index(i + slices + 1));
		indices->push_back(Index(i));
		indices->push_back(Index(i + 1));
	}

	switch (_layout)
	{
	case EVertexLayout_Pos:
	{
		MeshRendererPlain* meshRenderer = _entity->AddMeshRenderer<MeshRendererPlain>();

		ionVector<VertexPlain> vertices;

		const ionU32 verticesSize = static_cast<ionU32>(positions->size());
		const ionU32 indicesSize = static_cast<ionU32>(indices->size());

		vertices->resize(verticesSize);

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

		_entity->PushBackMesh(mesh);
	}
	break;

	case EVertexLayout_Pos_Color:
	{
		MeshRendererColored* meshRenderer = _entity->AddMeshRenderer<MeshRendererColored>();

		ionVector<VertexColored> vertices;

		const ionU32 verticesSize = static_cast<ionU32>(positions->size());
		const ionU32 indicesSize = static_cast<ionU32>(indices->size());

		vertices->resize(verticesSize);

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

		_entity->PushBackMesh(mesh);
	}
	break;

	case EVertexLayout_Pos_UV:
	{
		MeshRendererUV* meshRenderer = _entity->AddMeshRenderer<MeshRendererUV>();

		ionVector<VertexUV> vertices;
		ionVector<Vector> uvuv;

		const ionU32 verticesSize = static_cast<ionU32>(positions->size());
		const ionU32 indicesSize = static_cast<ionU32>(indices->size());

		vertices->resize(verticesSize);

		uvuv->resize(verticesSize);

		GeometryHelper::CalculateUVs(positions->data(), verticesSize, &uvuv[0]);

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

		_entity->PushBackMesh(mesh);
	}
	break;

	case EVertexLayout_Pos_Normal:
	{
		MeshRendererNormal* meshRenderer = _entity->AddMeshRenderer<MeshRendererNormal>();

		ionVector<VertexNormal> vertices;
		ionVector<Vector> normals;

		const ionU32 verticesSize = static_cast<ionU32>(positions->size());
		const ionU32 indicesSize = static_cast<ionU32>(indices->size());

		vertices->resize(verticesSize);

		normals->resize(verticesSize);

		GeometryHelper::CalculateNormals(positions->data(), verticesSize, indices->data(), indicesSize, &normals[0]);

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

		_entity->PushBackMesh(mesh);
	}
	break;

	case EVertexLayout_Pos_UV_Normal:
	{
		MeshRendererSimple* meshRenderer = _entity->AddMeshRenderer<MeshRendererSimple>();

		ionVector<VertexSimple> vertices;
		ionVector<Vector> uvuv;
		ionVector<Vector> normals;

		const ionU32 verticesSize = static_cast<ionU32>(positions->size());
		const ionU32 indicesSize = static_cast<ionU32>(indices->size());

		vertices->resize(verticesSize);

		uvuv->resize(verticesSize);
		normals->resize(verticesSize);

		GeometryHelper::CalculateUVs(positions->data(), verticesSize, &uvuv[0]);
		GeometryHelper::CalculateNormals(positions->data(), verticesSize, indices->data(), indicesSize, &normals[0]);

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

		_entity->PushBackMesh(mesh);
	}
	break;

	case EVertexLayout_Full:
	{
		MeshRenderer* meshRenderer = _entity->AddMeshRenderer<MeshRenderer>();

		ionVector<Vertex> vertices;
		ionVector<Vector> uvuv;
		ionVector<Vector> normals;
		ionVector<Vector> tangents;
		ionVector<ionFloat> bitangentsign;

		const ionU32 verticesSize = static_cast<ionU32>(positions->size());
		const ionU32 indicesSize = static_cast<ionU32>(indices->size());

		vertices->resize(verticesSize);

		uvuv->resize(verticesSize);
		normals->resize(verticesSize);
		tangents->resize(verticesSize);
		bitangentsign->resize(verticesSize);

		GeometryHelper::CalculateUVs(positions->data(), verticesSize, &uvuv[0]);
		GeometryHelper::CalculateNormals(positions->data(), verticesSize, indices->data(), indicesSize, &normals[0]);
		GeometryHelper::CalculateTangents(positions->data(), normals->data(), uvuv->data(), verticesSize, indices->data(), indicesSize, &tangents[0], &bitangentsign[0]);

		for (ionU32 i = 0; i < verticesSize; ++i)
		{
			vertices[i].SetPosition(positions[i]);
			vertices[i].SetTexCoordUV0(uvuv[i]);
			vertices[i].SetTexCoordUV1(uvuv[i]);
			vertices[i].SetNormal(normals[i]);
			vertices[i].SetColor(_r, _g, _b, _a);
			vertices[i].SetTangent(tangents[i]);
			vertices[i].SetBiTangentSign(bitangentsign[i]);
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

		_entity->PushBackMesh(mesh);
	}
	break;

	case EVertexLayout_Empty:
	{
		_entity->AddMeshRenderer<BaseMeshRenderer>();

		Mesh mesh;
		mesh.SetIndexCount(0);
		mesh.SetIndexStart(0);

		_entity->PushBackMesh(mesh);
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
	const ionSize count = positions->size();
	for (ionU32 i = 1; i < count; ++i)
	{
		_entity->GetBoundingBox()->Expande(positions[i - 1], positions[i]);
	}
}

///
///            PYRAMID
///
void PrimitiveFactory::GeneratePyramd(EVertexLayout _layout, Entity* _entity, ionFloat _r /*= 1.0f*/, ionFloat _g /*= 1.0f*/, ionFloat _b /*= 1.0f*/, ionFloat _a /*= 1.0f*/)
{
	ionVector<Index> indices;
	indices->resize(18);
	indices = {
		0, 1, 2,
		2, 3, 0,

		4, 5, 6,
		7, 8, 9,
		10, 11, 12,
		13, 14, 15
	};

	Vector positions[16] = {

		// 1 square base
		Vector(0.5f, -0.5f, 0.5f, 1.0f),
		Vector(-0.5f, -0.5f, 0.5f, 1.0f),
		Vector(-0.5f, -0.5f, -0.5f, 1.0f),
		Vector(0.5f, -0.5f, -0.5f, 1.0f),

		// 4 triangles
		Vector(0.5f, -0.5f, 0.5f, 1.0f),
		Vector(0.0f, 0.5f, 0.0f, 1.0f),
		Vector(-0.5f, -0.5f, 0.5f, 1.0f),

		Vector(-0.5f, -0.5f, 0.5f, 1.0f),
		Vector(0.0f, 0.5f, 0.0f, 1.0f),
		Vector(-0.5f, -0.5f, -0.5f, 1.0f),

		Vector(-0.5f, -0.5f, -0.5f, 1.0f),
		Vector(0.0f, 0.5f, 0.0f, 1.0f),
		Vector(0.5f, -0.5f, -0.5f, 1.0f),

		Vector(0.5f, -0.5f, -0.5f, 1.0f),
		Vector(0.0f, 0.5f, 0.0f, 1.0f),
		Vector(0.5f, -0.5f, 0.5f, 1.0f)
	};

	switch (_layout)
	{
	case EVertexLayout_Pos:
	{
		MeshRendererPlain* meshRenderer = _entity->AddMeshRenderer<MeshRendererPlain>();

		ionVector<VertexPlain> vertices;
		vertices->resize(16);

		for (ionU32 i = 0; i < 16; ++i)
		{
			vertices[i].SetPosition(positions[i]);
		}
		for (ionU32 i = 0; i < 16; ++i)
		{
			meshRenderer->PushBackVertex(vertices[i]);
		}
		for (ionU32 i = 0; i < 18; ++i)
		{
			meshRenderer->PushBackIndex(indices[i]);
		}

		Mesh mesh;
		mesh.SetIndexCount(18);
		mesh.SetIndexStart(0);

		_entity->PushBackMesh(mesh);
	}
	break;

	case EVertexLayout_Pos_Color:
	{
		MeshRendererColored* meshRenderer = _entity->AddMeshRenderer<MeshRendererColored>();

		ionVector<VertexColored> vertices;
		vertices->resize(16);

		for (ionU32 i = 0; i < 16; ++i)
		{
			vertices[i].SetPosition(positions[i]);
			vertices[i].SetColor(_r, _g, _b, _a);
		}
		for (ionU32 i = 0; i < 16; ++i)
		{
			meshRenderer->PushBackVertex(vertices[i]);
		}
		for (ionU32 i = 0; i < 18; ++i)
		{
			meshRenderer->PushBackIndex(indices[i]);
		}

		Mesh mesh;
		mesh.SetIndexCount(18);
		mesh.SetIndexStart(0);

		_entity->PushBackMesh(mesh);
	}
	break;

	case EVertexLayout_Pos_UV:
	{
		MeshRendererUV* meshRenderer = _entity->AddMeshRenderer<MeshRendererUV>();

		ionVector<VertexUV> vertices;
		vertices->resize(16);

		for (ionU32 i = 0; i < 16; ++i)
		{
			vertices[i].SetPosition(positions[i]);
		}

		Vector uvuv[16];
		GeometryHelper::CalculateUVs(positions, 16, uvuv);
		for (ionU32 i = 0; i < vertices->size(); ++i)
		{
			vertices[i].SetTexCoordUV(uvuv[i]);
		}

		for (ionU32 i = 0; i < 16; ++i)
		{
			meshRenderer->PushBackVertex(vertices[i]);
		}
		for (ionU32 i = 0; i < 18; ++i)
		{
			meshRenderer->PushBackIndex(indices[i]);
		}

		Mesh mesh;
		mesh.SetIndexCount(18);
		mesh.SetIndexStart(0);

		_entity->PushBackMesh(mesh);
	}
	break;

	case EVertexLayout_Pos_Normal:
	{
		MeshRendererNormal* meshRenderer = _entity->AddMeshRenderer<MeshRendererNormal>();

		ionVector<VertexNormal> vertices;
		vertices->resize(16);

		for (ionU32 i = 0; i < 16; ++i)
		{
			vertices[i].SetPosition(positions[i]);
		}

		Vector normals[16];
		GeometryHelper::CalculateNormals(positions, 16, indices->data(), 18, normals);
		for (ionU32 i = 0; i < vertices->size(); ++i)
		{
			vertices[i].SetNormal(normals[i]);
		}

		for (ionU32 i = 0; i < 16; ++i)
		{
			meshRenderer->PushBackVertex(vertices[i]);
		}
		for (ionU32 i = 0; i < 18; ++i)
		{
			meshRenderer->PushBackIndex(indices[i]);
		}

		Mesh mesh;
		mesh.SetIndexCount(18);
		mesh.SetIndexStart(0);

		_entity->PushBackMesh(mesh);
	}
	break;

	case EVertexLayout_Pos_UV_Normal:
	{
		MeshRendererSimple* meshRenderer = _entity->AddMeshRenderer<MeshRendererSimple>();

		ionVector<VertexSimple> vertices;
		vertices->resize(16);

		for (ionU32 i = 0; i < 16; ++i)
		{
			vertices[i].SetPosition(positions[i]);
		}

		Vector uvuv[16];
		GeometryHelper::CalculateUVs(positions, 16, uvuv);
		for (ionU32 i = 0; i < vertices->size(); ++i)
		{
			vertices[i].SetTexCoordUV(uvuv[i]);
		}

		Vector normals[16];
		GeometryHelper::CalculateNormals(positions, 16, indices->data(), 18, normals);
		for (ionU32 i = 0; i < vertices->size(); ++i)
		{
			vertices[i].SetNormal(normals[i]);
		}

		for (ionU32 i = 0; i < 16; ++i)
		{
			meshRenderer->PushBackVertex(vertices[i]);
		}
		for (ionU32 i = 0; i < 18; ++i)
		{
			meshRenderer->PushBackIndex(indices[i]);
		}

		Mesh mesh;
		mesh.SetIndexCount(18);
		mesh.SetIndexStart(0);

		_entity->PushBackMesh(mesh);
	}
	break;

	case EVertexLayout_Full:
	{
		MeshRenderer* meshRenderer = _entity->AddMeshRenderer<MeshRenderer>();

		ionVector<Vertex> vertices;
		vertices->resize(16);

		for (ionU32 i = 0; i < 16; ++i)
		{
			vertices[i].SetPosition(positions[i]);
			vertices[i].SetColor(_r, _g, _b, _a);
		}

		Vector uvuv[16];
		GeometryHelper::CalculateUVs(positions, 16, uvuv);
		for (ionU32 i = 0; i < vertices->size(); ++i)
		{
			vertices[i].SetTexCoordUV0(uvuv[i]);
			vertices[i].SetTexCoordUV1(uvuv[i]);
		}

		Vector normals[16];
		GeometryHelper::CalculateNormals(positions, 16, indices->data(), 18, normals);
		for (ionU32 i = 0; i < vertices->size(); ++i)
		{
			vertices[i].SetNormal(normals[i]);
		}

		Vector tangents[16];
		ionFloat bitangentsign[16];
		GeometryHelper::CalculateTangents(positions, normals, uvuv, 16, indices->data(), 18, tangents, bitangentsign);
		for (ionU32 i = 0; i < vertices->size(); ++i)
		{
			vertices[i].SetTangent(tangents[i]);
			vertices[i].SetBiTangentSign(bitangentsign[i]);
		}


		for (ionU32 i = 0; i < 16; ++i)
		{
			meshRenderer->PushBackVertex(vertices[i]);
		}
		for (ionU32 i = 0; i < 18; ++i)
		{
			meshRenderer->PushBackIndex(indices[i]);
		}

		Mesh mesh;
		mesh.SetIndexCount(18);
		mesh.SetIndexStart(0);

		_entity->PushBackMesh(mesh);
	}
	break;

	case EVertexLayout_Empty:
	{
		_entity->AddMeshRenderer<BaseMeshRenderer>();

		Mesh mesh;
		mesh.SetIndexCount(0);
		mesh.SetIndexStart(0);

		_entity->PushBackMesh(mesh);
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

	for (ionU32 i = 1; i < 16; ++i)
	{
		_entity->GetBoundingBox()->Expande(positions[i - 1], positions[i]);
	}
}




ION_NAMESPACE_END