#pragma once

#include "../Core/CoreDefs.h"
#include "../Core/StandardIncludes.h"

#include "../Dependencies/Eos/Eos/Eos.h"

#include "Transform.h"

#include "../App/Mode.h"

#include "../Core/UUID.h"

#include "../Core/MemorySettings.h"


#define ION_BASE_NODE_NAME "Node"


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

using NodeAllocator = MemoryAllocator<FreeListBestSearchAllocationPolicy, MultiThreadPolicy, MemoryBoundsCheck, MemoryTag, MemoryLog>;


class BoundingBox;
class Mesh;
class BaseMeshRenderer;
class AnimationRenderer;
class ION_DLL Node
{
public:
	static NodeAllocator* GetAllocator();

public:
    Node();
    explicit Node(const ionString & _name);
    virtual ~Node();

	//////////////////////////////////////////////////////////////////////////
	template <typename T, typename... Args>
	static T* __CreateNode(Args&&... args)
	{
		T* temp = nullptr;
		ionAssertReturnValue((Node const*)&temp, "Template must inherit from Node", nullptr);
		temp = ionNew(T, GetAllocator(), std::forward<Args>(args)...);
		return temp;
	}

	template <class T>
	static void __DestroyNode(T* _node)
	{
		ionAssertReturnVoid((Node const*)&_node, "Template must inherit from Node");
		ionDelete(_node, GetAllocator());
	}

#define CreateNode(Type, ...)	 Node::__CreateNode<Type>(__VA_ARGS__)
#define DestroyNode(Object)	 Node::__DestroyNode(Object)


    //////////////////////////////////////////////////////////////////////////

    //
    // USER VIRTUAL CALLS
    virtual void OnAttachToParent(Node* _parent) {}
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

    void SetName(const ionString & _name);
    void SetActive(ionBool _isActive);
    ionBool IsActive() const { return m_active; }

    void SetVisible(ionBool _isVisible);
    ionBool IsVisible() const { return m_visible; }

    void AttachToParent(Node* _parent);
    void DetachFromParent();

    void AddToRenderLayer(ENodeRenderLayer _layer);
    void RemoveFromRenderLayer(ENodeRenderLayer _layer);

    ionU32 GetRenderLayer() const { return m_renderLayer; }
    ionBool IsInRenderLayer(ionU32 _layers) const { return (m_renderLayer & _layers) != 0; }
    ionBool IsInRenderLayer(ENodeRenderLayer _layer) const { return (m_renderLayer & static_cast<ionU32>(_layer)) != 0; }

    //////////////////////////////////////////////////////////////////////////

    const UUID& GetUUID() const { return m_uuid;  }

    ENodeType GetNodeType() const { return m_nodeType; }
    const ionString &GetName() const  { return m_name; }

    void Update(ionFloat _deltaTime);

	const Node* GetParent() const { return m_parent; }
	Node* GetParent() { return m_parent; }

    const Transform &GetTransform() const { return m_transform; }
    Transform &GetTransform() { return m_transform; }

    ionVector<Node*, NodeAllocator, GetAllocator> &GetChildren() { return m_children; };

    ionVector<Node*, NodeAllocator, GetAllocator>::const_iterator ChildrenIteratorBeginConst() { return m_children.cbegin();}
    ionVector<Node*, NodeAllocator, GetAllocator>::const_iterator ChildrenIteratorEndConst() { return m_children.cend(); }
    ionVector<Node*, NodeAllocator, GetAllocator>::iterator ChildrenIteratorBegin() { return m_children.begin(); }
    ionVector<Node*, NodeAllocator, GetAllocator>::iterator ChildrenIteratorEnd() { return m_children.end(); }

    //////////////////////////////////////////////////////////////////////////

    // "Special accessor call"

    void IterateAll(const std::function< void(Node* _node) >& _lambda = nullptr);

protected:
    ENodeType    m_nodeType;

private:
    Node(const Node& _Orig) = delete;
    Node& operator = (const Node&) = delete;

    UUID m_uuid;

    ionU32 m_renderLayer;

    Node* m_parent; 
    Transform m_transform;

    ionString m_name;
    
    ionVector<Node*, NodeAllocator, GetAllocator> m_children;

    ionBool m_active;
    ionBool m_visible;
};

ION_NAMESPACE_END