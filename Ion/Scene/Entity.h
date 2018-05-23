#pragma once

#include "../Core/CoreDefs.h"

#include "../Dependencies/Eos/Eos/Eos.h"

#include "Node.h"

#include "../Geometry/BoundingBox.h"
#include "../Geometry/Mesh.h"

EOS_USING_NAMESPACE

ION_NAMESPACE_BEGIN

class Entity;
typedef SmartPointer<Entity> EntityHandle;

class Material;

class ION_DLL Entity : public Node
{
public:
    explicit Entity();
    explicit Entity(const eosString & _name);
    virtual ~Entity();

    const eosVector(Mesh)& GetMeshList() const { return m_meshes; }
    eosVector(Mesh)& GetMeshList() { return m_meshes; }

    const BoundingBox& GetBoundingBox() const { return m_boundingBox; }
    BoundingBox& GetBoundingBox() { return m_boundingBox; }

    BoundingBox GetTransformedBoundingBox();

    ionU32  GetMeshCount() const;
    ionU32  GetPrimitiveCount(ionU32 _meshIndex) const;

    const void* GetVertexBuffer(ionU32 _meshIndex, ionU32 _primitiveIndex) const;
    ionSize GetVertexBufferSize(ionU32 _meshIndex, ionU32 _primitiveIndex) const;

    const void* GetIndexBuffer(ionU32 _meshIndex, ionU32 _primitiveIndex) const;
    ionSize GetIndexBufferSize(ionU32 _meshIndex, ionU32 _primitiveIndex) const;

    const Material* GetMaterial(ionU32 _meshIndex, ionU32 _primitiveIndex) const;
    Material* GetMaterial(ionU32 _meshIndex, ionU32 _primitiveIndex);
    /*
    void*   GetJointBuffer(ionU32 _meshIndex, ionU32 _primitiveIndex) const;
    ionSize GetJointBufferSize(ionU32 _meshIndex, ionU32 _primitiveIndex) const;
    */
private:
    Entity(const Entity& _Orig) = delete;
    Entity& operator = (const Entity&) = delete;

private:
    BoundingBox     m_boundingBox;
    eosVector(Mesh) m_meshes;    // 0 to empty entity, logic purpose only
};

ION_NAMESPACE_END