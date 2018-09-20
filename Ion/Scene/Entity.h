#pragma once

#include "../Core/CoreDefs.h"

#include "../Dependencies/Eos/Eos/Eos.h"

#include "Node.h"

#include "../Geometry/BoundingBox.h"
#include "../Geometry/Mesh.h"
#include "../Geometry/MeshRenderer.h"


EOS_USING_NAMESPACE

ION_NAMESPACE_BEGIN

class AnimationRenderer;
class ION_DLL Entity : public Node
{
public:
    explicit Entity();
    explicit Entity(const eosString & _name);
    virtual ~Entity();

    template<typename T>
    T* AddMeshRenderer()
    {
        m_boundingBox.Reset();
        m_meshes.clear();
        if (m_meshRenderer != nullptr)
        {
            eosDelete(m_meshRenderer);
            m_meshRenderer = nullptr;
        }
        m_meshRenderer = eosNew(T, ION_MEMORY_ALIGNMENT_SIZE);
        return dynamic_cast<T*>(m_meshRenderer);
    }

    AnimationRenderer* AddAnimationRenderer();

    const AnimationRenderer* GetAnimationRenderer() const override final { return m_animationRenderer; }
    AnimationRenderer* GetAnimationRenderer()  override final { return m_animationRenderer; }

    const BaseMeshRenderer* GetMeshRenderer() const override final { return m_meshRenderer; }
    BaseMeshRenderer* GetMeshRenderer() override final { return m_meshRenderer; }

    void PushBackMesh(const Mesh& _mesh);
    virtual const Mesh* GetMesh(ionU32 _index) const override final { return &m_meshes[_index]; }
    virtual Mesh* GetMesh(ionU32 _index) override final { return &m_meshes[_index]; }
    virtual ionU32  GetMeshCount() const override final;

    virtual BoundingBox* GetBoundingBox() override final { return &m_boundingBox; }

private:
    Entity(const Entity& _Orig) = delete;
    Entity& operator = (const Entity&) = delete;

private:
    BoundingBox             m_boundingBox;
    BaseMeshRenderer*       m_meshRenderer; // if has this one, means that this one is the root and any other children nodes can potentially contains meshes
    AnimationRenderer*      m_animationRenderer;
    eosVector(Mesh)         m_meshes;
};

ION_NAMESPACE_END