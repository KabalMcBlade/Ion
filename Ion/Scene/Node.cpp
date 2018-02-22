#include "Node.h"


#define ION_BASE_NODE_NAME "Node"

EOS_USING_NAMESPACE

ION_NAMESPACE_BEGIN

ionU32 Node::g_nextValidNodeIndex = 0;

Node::Node()
{
    m_nodeIndex = g_nextValidNodeIndex;
    ++g_nextValidNodeIndex;
    m_parent = nullptr;
    SetName(ION_BASE_NODE_NAME);
}

Node::Node(const eosString & _name)
{
    m_nodeIndex = g_nextValidNodeIndex;
    ++g_nextValidNodeIndex;
    m_parent = nullptr;
    SetName(_name);
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

ION_NAMESPACE_END