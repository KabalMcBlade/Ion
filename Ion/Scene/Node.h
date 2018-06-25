#pragma once

#include "../Core/CoreDefs.h"
#include "../Core/StandardIncludes.h"

#include "../Dependencies/Eos/Eos/Eos.h"


#include "Transform.h"


EOS_USING_NAMESPACE

ION_NAMESPACE_BEGIN


enum ENodeType
{
    ENodeType_EmptyNode = -1,
    ENodeType_Entity = 0,
    ENodeType_Camera
};


typedef std::function<void(ionFloat)> UpdateCallbackFunction;


class Node;
typedef SmartPointer<Node> NodeHandle;

class ION_DLL Node : public SmartObject
{
public:
    explicit Node();
    explicit Node(const eosString & _name);
    virtual ~Node();


    ENodeType GetNodeType() const { return m_nodeType; }
    void SetName(const eosString & _name);
    const eosString &GetName() const  { return m_name; }
    const eosString &GetNameInternal() const  { return m_nameInternal; }
    ionU32 GetNodeIndex() const { return m_nodeIndex;  }
    ionSize GetHash() const { return m_hash; }

    void AttachToParent(Node& _parent);
    void AttachToParent(NodeHandle& _parent);
    void DetachFromParent();

    virtual void Update(ionFloat _deltaTime);

    const NodeHandle &GetParentHandle() const { return m_parent; }
    const TransformHandle &GetTransformHandle() const { return m_transform; }

    NodeHandle &GetParentHandle() { return m_parent; }
    TransformHandle &GetTransformHandle() { return m_transform; }

    eosVector(NodeHandle) &GetChildren() { return m_children; };

    eosVector(NodeHandle)::const_iterator GetChildIteratorBegin() { return m_children.begin();}
    eosVector(NodeHandle)::const_iterator GetChildIteratorEnd() { return m_children.end(); }
    //////////////////////////////////////////////////////////////////////////

    void ConnectUpdateCallback(UpdateCallbackFunction _cb)
    {
        m_updateFunction = _cb;
    }

    void DisonnectUpdateCallback()
    {
        m_updateFunction = nullptr;
    }


protected:
    ENodeType    m_nodeType;

private:
    Node(const Node& _Orig) = delete;
    Node& operator = (const Node&) = delete;

    ionU32 m_nodeIndex;
    ionSize m_hash;

    NodeHandle m_parent;
    TransformHandle m_transform;

    eosString m_nameInternal;
    eosString m_name;
    
    eosVector(NodeHandle) m_children;

    UpdateCallbackFunction  m_updateFunction;

    static ionU32 g_nextValidNodeIndex;
};

ION_NAMESPACE_END