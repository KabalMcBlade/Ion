#pragma once

#include "../Core/CoreDefs.h"

#include "../Scene/Node.h"

ION_NAMESPACE_BEGIN

enum EVertexLayout;

class ION_DLL PrimitiveFactory
{
public:
    static void GenerateTriangle(EVertexLayout _layout, ObjectHandler& _entity, ionFloat _r = 1.0f, ionFloat _g = 1.0f, ionFloat _b = 1.0f, ionFloat _a = 1.0f);
    static void GenerateQuad(EVertexLayout _layout, ObjectHandler& _entity, ionFloat _r = 1.0f, ionFloat _g = 1.0f, ionFloat _b = 1.0f, ionFloat _a = 1.0f);
    static void GenerateCube(EVertexLayout _layout, ObjectHandler& _entity, ionFloat _r = 1.0f, ionFloat _g = 1.0f, ionFloat _b = 1.0f, ionFloat _a = 1.0f);
    static void GenerateSphere(EVertexLayout _layout, ObjectHandler& _entity, ionFloat _r = 1.0f, ionFloat _g = 1.0f, ionFloat _b = 1.0f, ionFloat _a = 1.0f);
};

ION_NAMESPACE_END