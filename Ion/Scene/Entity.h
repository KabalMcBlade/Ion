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

private:
    Entity(const Entity& _Orig) = delete;
    Entity& operator = (const Entity&) = delete;

private:
    BoundingBox     m_boundingBox;
    eosVector(Mesh) m_meshes;    // 0 to empty entity, logic purpose only
};

ION_NAMESPACE_END