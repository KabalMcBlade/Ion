#pragma once

#include "../Core/CoreDefs.h"

#include "../Dependencies/Eos/Eos/Eos.h"

#include "Transform.h"


EOS_USING_NAMESPACE

ION_NAMESPACE_BEGIN

class Node;
typedef SmartPointer<Node> NodeHandle;

class ION_DLL Node : public SmartObject
{
public:
    explicit Node();
    explicit Node(const eosString & _name);
    virtual ~Node();

    void SetName(const eosString & _name);
    const eosString &GetName() const  { return m_name; }
    const eosString &GetNameInternal() const  { return m_nameInternal; }
    ionU32 GetNodeIndex() const { return m_nodeIndex;  }
    ionSize GetHash() const { return m_hash; }

    void AttachToParent(NodeHandle& _parent);
    void DetachFromParent();

    const NodeHandle &GetParentHandle() const { return m_parent; }
    const TransformHandle &GetTransformHandle() const { return m_transform; }

    eosVector(NodeHandle) &GetChildren() { return m_children; };

private:
    Node(const Node& _Orig) = delete;
    Node& operator = (const Node&) = delete;

    ionU32 m_nodeIndex;
    ionSize m_hash;

    NodeHandle m_parent;
    TransformHandle m_transform;
    //Node* m_parent;
    //Transform* m_transform;

    eosString m_nameInternal;
    eosString m_name;
    
    eosVector(NodeHandle) m_children;
    //eosVector(Node*) m_children;

    static ionU32 g_nextValidNodeIndex;
};

ION_NAMESPACE_END