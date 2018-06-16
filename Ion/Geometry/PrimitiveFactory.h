#pragma once

#include "../Core/CoreDefs.h"

ION_NAMESPACE_BEGIN

enum EVertexLayout;
class Entity;

class ION_DLL PrimitiveFactory
{
public:
    static void GenerateTriangle(EVertexLayout _layout, Entity& _entity);
    static void GenerateQuad(EVertexLayout _layout, Entity& _entity);
    static void GenerateCube(EVertexLayout _layout, Entity& _entity);
	static void GenerateSphere(EVertexLayout _layout, Entity& _entity);
};

ION_NAMESPACE_END