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

    template<typename T>
    T* AddMesh()
    {
        T* mesh = eosNew(T, ION_MEMORY_ALIGNMENT_SIZE);
        m_meshes.push_back(mesh);
        return mesh;
    }

    const BaseMesh* GetMesh(ionU32 _index) const { return m_meshes[_index]; }
    BaseMesh* GetMesh(ionU32 _index) { return m_meshes[_index]; }

    const BoundingBox& GetBoundingBox() const { return m_boundingBox; }
    BoundingBox& GetBoundingBox() { return m_boundingBox; }

    BoundingBox GetTransformedBoundingBox();

    ionU32  GetMeshCount() const;

private:
    Entity(const Entity& _Orig) = delete;
    Entity& operator = (const Entity&) = delete;

private:
    BoundingBox             m_boundingBox;
    eosVector(BaseMesh*)    m_meshes;    // 0 to empty entity, logic purpose only
};

ION_NAMESPACE_END