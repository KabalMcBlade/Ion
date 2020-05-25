#pragma once

#include "../Core/CoreDefs.h"

#include "../Scene/Node.h"

#include "../Core/MemorySettings.h"


ION_NAMESPACE_BEGIN

using PrimitiveFactoryAllocator = MemoryAllocator<FreeListBestSearchAllocationPolicy, MultiThreadPolicy, MemoryBoundsCheck, MemoryTag, MemoryLog>;


enum EVertexLayout;
class Entity;

class ION_DLL PrimitiveFactory
{
private:
	static PrimitiveFactoryAllocator* GetAllocator();

public:
	static void GenerateTriangle(EVertexLayout _layout, Entity* _entity, ionFloat _r = 1.0f, ionFloat _g = 1.0f, ionFloat _b = 1.0f, ionFloat _a = 1.0f);
	static void GenerateQuad(EVertexLayout _layout, Entity* _entity, ionFloat _r = 1.0f, ionFloat _g = 1.0f, ionFloat _b = 1.0f, ionFloat _a = 1.0f);
	static void GenerateCube(EVertexLayout _layout, Entity* _entity, ionFloat _r = 1.0f, ionFloat _g = 1.0f, ionFloat _b = 1.0f, ionFloat _a = 1.0f);
	static void GenerateSphere(EVertexLayout _layout, Entity* _entity, ionFloat _r = 1.0f, ionFloat _g = 1.0f, ionFloat _b = 1.0f, ionFloat _a = 1.0f);
	static void GeneratePyramd(EVertexLayout _layout, Entity* _entity, ionFloat _r = 1.0f, ionFloat _g = 1.0f, ionFloat _b = 1.0f, ionFloat _a = 1.0f);
};

ION_NAMESPACE_END