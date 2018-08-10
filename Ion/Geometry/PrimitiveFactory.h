#pragma once

#include "../Core/CoreDefs.h"

#include "../Scene/Node.h"

ION_NAMESPACE_BEGIN

enum EVertexLayout;

class ION_DLL PrimitiveFactory
{
public:
    static void GenerateTriangle(EVertexLayout _layout, ObjectHandler& _entity);
    static void GenerateQuad(EVertexLayout _layout, ObjectHandler& _entity);
    static void GenerateCube(EVertexLayout _layout, ObjectHandler& _entity);
    static void GenerateSphere(EVertexLayout _layout, ObjectHandler& _entity);
};

ION_NAMESPACE_END