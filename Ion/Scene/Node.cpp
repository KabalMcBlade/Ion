#include "Node.h"


#include "../Dependencies/Nix/Nix/Nix.h"

#define ION_BASE_NODE_NAME "Node"

EOS_USING_NAMESPACE
NIX_USING_NAMESPACE

ION_NAMESPACE_BEGIN

ionU32 Node::g_nextValidNodeIndex = 0;

Node::Node() : m_transform(eosNew(Transform, ION_MEMORY_ALIGNMENT_SIZE))
{
    m_nodeIndex = g_nextValidNodeIndex;
    ++g_nextValidNodeIndex;
    m_parent = nullptr;
    SetName(ION_BASE_NODE_NAME);
    m_nodeType = ENodeType_EmptyNode;
}

Node::Node(const eosString & _name) : m_transform(eosNew(Transform, ION_MEMORY_ALIGNMENT_SIZE))
{
    m_nodeIndex = g_nextValidNodeIndex;
    ++g_nextValidNodeIndex;
    m_parent = nullptr;
    SetName(_name);
    m_nodeType = ENodeType_EmptyNode;
}

Node::~Node()
{

}

void Node::SetName(const eosString& _name)
{
    m_name = _name;
    eosString tmp(std::to_string(m_nodeIndex).c_str()); // I know.. but I don't want to implement everything!
    m_nameInternal = m_name + tmp;
    m_hash = std::hash<eosString>{}(m_nameInternal);
}

void Node::AttachToParent(Node& _parent)
{
    SmartPointer<Node> parentHandle(&_parent);
    AttachToParent(parentHandle);
}

void Node::AttachToParent(NodeHandle& _parent)
{
    if (m_parent.IsValid())
    {
        DetachFromParent();
    }

    m_parent = _parent;
    if (m_parent.IsValid())
    {
        SmartPointer<Node> thisHandle(this);
        m_parent->GetChildren().push_back(thisHandle);
    }
}

void Node::DetachFromParent()
{
    m_parent->GetChildren().erase(std::remove(m_parent->GetChildren().begin(), m_parent->GetChildren().end(), this), m_parent->GetChildren().end());
    m_parent = nullptr; // still need? 
}

void Node::Update(ionFloat _deltaTime)
{
    Matrix currentParentTransform;
    if (m_parent.IsValid())
    {
        // I just get here, because if has a parent, means that in the prev iteration, someone called the UpdateTransform
        // with the parent matrix
        currentParentTransform = m_transform->GetMatrix();  
    }
    else
    {
        // update because is the root for sure and no other can update this object at this point!
        m_transform->UpdateTransform(); 
        currentParentTransform = m_transform->GetMatrix();
    }

    if (m_updateFunction != nullptr)
    {
        m_updateFunction(_deltaTime);
    }

    eosVector(NodeHandle)::const_iterator begin = GetChildIteratorBegin(), end = GetChildIteratorEnd(), it = begin;
    for (; it != end; ++it)
    {
        (*it)->GetTransformHandle()->UpdateTransform(currentParentTransform);
        (*it)->Update(_deltaTime);
    }
}

ION_NAMESPACE_END