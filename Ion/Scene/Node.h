#pragma once

#include "../Core/CoreDefs.h"
#include "../Core/StandardIncludes.h"

#include "../Dependencies/Eos/Eos/Eos.h"


#include "Transform.h"

#include "../App/Mode.h"


EOS_USING_NAMESPACE

ION_NAMESPACE_BEGIN


enum ENodeType
{
    ENodeType_EmptyNode = -1,
    ENodeType_Entity = 0,
    ENodeType_Camera,
    ENodeType_DirectionalLight,
    ENodeType_SpotLight,
    ENodeType_PointLight,
};


// this is used to group the entities under the same render camera
enum ENodeRenderLayer : ionU32
{
    ENodeRenderLayer_Default = 0b00000000000000000000000000000001,
    ENodeRenderLayer_1 = 0b00000000000000000000000000000010,
    ENodeRenderLayer_2 = 0b00000000000000000000000000000100,
    ENodeRenderLayer_3 = 0b00000000000000000000000000001000,
    ENodeRenderLayer_4 = 0b00000000000000000000000000010000,
    ENodeRenderLayer_5 = 0b00000000000000000000000000100000,
    ENodeRenderLayer_6 = 0b00000000000000000000000001000000,
    ENodeRenderLayer_7 = 0b00000000000000000000000010000000,

    // for now 8 camera are enough.
    // if need more, add here!
};


class Node;
typedef SmartPointer<Node> ObjectHandler;

class BoundingBox;
class Mesh;
class BaseMeshRenderer;
class AnimationRenderer;
class ION_DLL Node : public SmartObject
{
public:
    explicit Node();
    explicit Node(const eosString & _name);
    virtual ~Node();

    //////////////////////////////////////////////////////////////////////////

    //
    // USER VIRTUAL CALLS
    virtual void OnAttachToParent(ObjectHandler& _parent) {}
    virtual void OnDetachFromParent() {}

    virtual void OnBegin() {}
    virtual void OnEnd() {}

    virtual void OnUpdate(ionFloat _deltaTime) {}
    virtual void OnLateUpdate(ionFloat _deltaTime) {}

    virtual void OnMouseInput(const MouseState& _mouseState, ionFloat _deltaTime) {}
    virtual void OnKeyboardInput(const KeyboardState& _keyboardState, ionFloat _deltaTime) {}

    // those need to conformity
    virtual const BaseMeshRenderer* GetMeshRenderer() const { return nullptr; }
    virtual BaseMeshRenderer* GetMeshRenderer() { return nullptr; }

    virtual const AnimationRenderer* GetAnimationRenderer() const { return nullptr; }
    virtual AnimationRenderer* GetAnimationRenderer()  { return nullptr; }

    virtual void PushBackMesh(const Mesh& _mesh) {}
    virtual const Mesh* GetMesh(ionU32 _index) const { return nullptr; }
    virtual Mesh* GetMesh(ionU32 _index) { return nullptr; }
    virtual ionU32  GetMeshCount() const { return 0; }

    virtual void PushBackInitialMorphTargetWeight(ionFloat _weight) {}
    virtual ionFloat GetInitialMorphTargetWeight(ionU32 _index) const { return -1.0f; }
    virtual ionU32  GetInitialMorphTargetWeightCount() const { return 0; };

    virtual void ResizeMorphTargetWeight(ionU32 _size) {}
    virtual ionFloat GetMorphTargetWeight(ionU32 _index) const { return -1.0f; }
    virtual void SetMorphTargetWeight(ionU32 _index, ionFloat _value) {}
    virtual ionU32 GetMorphTargetWeightCount() const { return 0; }

    virtual BoundingBox* GetBoundingBox()  { return nullptr; }

    //
    // USER CALLS

    void SetName(const eosString & _name);
    void SetActive(ionBool _isActive);
    ionBool IsActive() const { return m_active; }

    void SetVisible(ionBool _isVisible);
    ionBool IsVisible() const { return m_visible; }

    void AttachToParent(ObjectHandler& _parent);
    void DetachFromParent();

    void AddToRenderLayer(ENodeRenderLayer _layer);
    void RemoveFromRenderLayer(ENodeRenderLayer _layer);

    ionU32 GetRenderLayer() { return m_renderLayer; }
    ionBool IsInRenderLayer(ionU32 _layers) { return (m_renderLayer & _layers) != 0; }
    ionBool IsInRenderLayer(ENodeRenderLayer _layer) { return (m_renderLayer & static_cast<ionU32>(_layer)) != 0; }

    //////////////////////////////////////////////////////////////////////////

    ENodeType GetNodeType() const { return m_nodeType; }
    const eosString &GetName() const  { return m_name; }
    const eosString &GetNameInternal() const  { return m_nameInternal; }
    ionU32 GetNodeIndex() const { return m_nodeIndex;  }
    ionSize GetHash() const { return m_hash; }

    void Update(ionFloat _deltaTime);

    const Node* GetParent() const { return m_parent; }

    const Transform &GetTransform() const { return m_transform; }
    Transform &GetTransform() { return m_transform; }

    eosVector(ObjectHandler) &GetChildren() { return m_children; };

    eosVector(ObjectHandler)::const_iterator ChildrenIteratorBeginConst() { return m_children.begin();}
    eosVector(ObjectHandler)::const_iterator ChildrenIteratorEndConst() { return m_children.end(); }
    eosVector(ObjectHandler)::iterator ChildrenIteratorBegin() { return m_children.begin(); }
    eosVector(ObjectHandler)::iterator ChildrenIteratorEnd() { return m_children.end(); }

    //////////////////////////////////////////////////////////////////////////

    // "Spcial accessor call"

    void IterateAll(const std::function< void(const ObjectHandler& _node) >& _lambda = nullptr);

protected:
    ENodeType    m_nodeType;

private:
    Node(const Node& _Orig) = delete;
    Node& operator = (const Node&) = delete;

    ionU32 m_renderLayer;
    ionU32 m_nodeIndex;
    ionSize m_hash;

    Node* m_parent; // to avoid cross reference is a ptr instead of a smart object
    Transform m_transform;

    eosString m_nameInternal;
    eosString m_name;
    
    eosVector(ObjectHandler) m_children;

    ionBool m_active;
    ionBool m_visible;

    static ionU32 g_nextValidNodeIndex;
};

ION_NAMESPACE_END