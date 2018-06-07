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

    VkIndexType GetIndexType(ionU32 _meshIndex) const;

    ionSize GetIndexStart(ionU32 _meshIndex) const;
    ionSize GetIndexCount(ionU32 _meshIndex) const;

    const void* GetVertexBuffer(ionU32 _meshIndex) const;
    ionSize GetVertexBufferSize(ionU32 _meshIndex) const;

    const void* GetIndexBuffer(ionU32 _meshIndex) const;
    ionSize GetIndexBufferSize(ionU32 _meshIndex) const;

    const Material* GetMaterial(ionU32 _meshIndex) const;
    Material* GetMaterial(ionU32 _meshIndex);
    /*
    void*   GetJointBuffer(ionU32 _meshIndex) const;
    ionSize GetJointBufferSize(ionU32 _meshIndex) const;
    */
private:
    Entity(const Entity& _Orig) = delete;
    Entity& operator = (const Entity&) = delete;

private:
    BoundingBox     m_boundingBox;
    eosVector(Mesh) m_meshes;    // 0 to empty entity, logic purpose only
};

ION_NAMESPACE_END