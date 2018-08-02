#pragma once

#include "../Core/CoreDefs.h"

#include "../Dependencies/Eos/Eos/Eos.h"

#include "Node.h"

#include "../Geometry/BoundingBox.h"
#include "../Geometry/Mesh.h"

EOS_USING_NAMESPACE

ION_NAMESPACE_BEGIN

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

    virtual const BaseMesh* GetMesh(ionU32 _index) const override final { return m_meshes[_index]; }
    virtual BaseMesh* GetMesh(ionU32 _index) override final { return m_meshes[_index]; }
    virtual ionU32  GetMeshCount() const override final;

    virtual BoundingBox* GetBoundingBox() override final { return &m_boundingBox; }

private:
    Entity(const Entity& _Orig) = delete;
    Entity& operator = (const Entity&) = delete;

private:
    BoundingBox             m_boundingBox;
    eosVector(BaseMesh*)    m_meshes;    // 0 to empty entity, logic purpose only
};

ION_NAMESPACE_END