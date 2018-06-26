#include "Node.h"


#include "../Dependencies/Nix/Nix/Nix.h"

#define ION_BASE_NODE_NAME "Node"

EOS_USING_NAMESPACE
NIX_USING_NAMESPACE

ION_NAMESPACE_BEGIN

ionU32 Node::g_nextValidNodeIndex = 0;

Node::Node() : m_transform(eosNew(Transform, ION_MEMORY_ALIGNMENT_SIZE)), m_active(true), m_renderLayer(ENodeRenderLayer_Default)
{
    m_nodeIndex = g_nextValidNodeIndex;
    ++g_nextValidNodeIndex;
    m_parent = nullptr;
    SetName(ION_BASE_NODE_NAME);
    m_nodeType = ENodeType_EmptyNode;
}

Node::Node(const eosString & _name) : m_transform(eosNew(Transform, ION_MEMORY_ALIGNMENT_SIZE)), m_active(true), m_renderLayer(ENodeRenderLayer_Default)
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

        OnAttachToParent(m_parent);
    }
}

void Node::DetachFromParent()
{
    OnDetachFromParent();

    m_parent->GetChildren().erase(std::remove(m_parent->GetChildren().begin(), m_parent->GetChildren().end(), this), m_parent->GetChildren().end());
    m_parent = nullptr; // still need? 
}

void Node::Update(ionFloat _deltaTime)
{
    if (m_active)
    {
        Matrix worldTransform;
        if (m_parent.IsValid())
        {
            m_transform->SetMatrixWS(m_parent->GetTransformHandle()->GetMatrixWS() * m_transform->GetMatrix());
            if (GetNodeType() == ENodeType_Camera)
            {
                m_transform->SetMatrixInverseWS(m_parent->GetTransformHandle()->GetMatrixInverseWS() * m_transform->GetMatrixInverse());
            }
        }
        else
        {
            m_transform->SetMatrixWS(m_transform->GetMatrix());
            if (GetNodeType() == ENodeType_Camera)
            {
                m_transform->SetMatrixInverseWS(m_transform->GetMatrixInverse());
            }
        }

        OnUpdate(_deltaTime);

        eosVector(NodeHandle)::const_iterator begin = GetChildIteratorBegin(), end = GetChildIteratorEnd(), it = begin;
        for (; it != end; ++it)
        {
            (*it)->Update(_deltaTime);
        }

        OnLateUpdate(_deltaTime);
    }
}

ION_NAMESPACE_END