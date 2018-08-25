#include "Node.h"


#include "../Dependencies/Nix/Nix/Nix.h"

#define ION_BASE_NODE_NAME "Node"

EOS_USING_NAMESPACE
NIX_USING_NAMESPACE

ION_NAMESPACE_BEGIN

ionU32 Node::g_nextValidNodeIndex = 0;

Node::Node() : m_active(true), m_visible(true), m_renderLayer(ENodeRenderLayer_Default), m_parent(nullptr)
{
    m_nodeIndex = g_nextValidNodeIndex;
    ++g_nextValidNodeIndex;
    SetName(ION_BASE_NODE_NAME);
    m_nodeType = ENodeType_EmptyNode;
}

Node::Node(const eosString & _name) : m_active(true), m_visible(true), m_renderLayer(ENodeRenderLayer_Default), m_parent(nullptr)
{
    m_nodeIndex = g_nextValidNodeIndex;
    ++g_nextValidNodeIndex;
    SetName(_name);
    m_nodeType = ENodeType_EmptyNode;
}

Node::~Node()
{
    DetachFromParent();
}

void Node::SetName(const eosString& _name)
{
    m_name = _name;
    eosString tmp(std::to_string(m_nodeIndex).c_str()); // I know.. but I don't want to implement everything!
    m_nameInternal = m_name + tmp;
    m_hash = std::hash<eosString>{}(m_nameInternal);
}

void Node::AttachToParent(ObjectHandler& _parent)
{
    if (m_parent != nullptr)
    {
        DetachFromParent();
    }

    m_parent = _parent.GetPtr();
    if (m_parent != nullptr)
    {
        m_parent->GetChildren().push_back(this);

        OnAttachToParent(_parent);
    }
}

void Node::DetachFromParent()
{
    OnDetachFromParent();

    if (m_parent != nullptr)
    {
        m_parent->GetChildren().erase(std::remove(m_parent->GetChildren().begin(), m_parent->GetChildren().end(), this), m_parent->GetChildren().end());
    }
}

void Node::Update(ionFloat _deltaTime)
{
    if (m_active)
    {
        OnUpdate(_deltaTime);

        if (m_parent != nullptr)
        {
            m_transform.SetMatrixWS(m_transform.GetMatrix() * m_parent->GetTransform().GetMatrixWS());
        }
        else
        {
            m_transform.SetMatrixWS(m_transform.GetMatrix());
        }

        eosVector(ObjectHandler)::const_iterator begin = GetChildIteratorBegin(), end = GetChildIteratorEnd(), it = begin;
        for (; it != end; ++it)
        {
            (*it)->Update(_deltaTime);
        }

        OnLateUpdate(_deltaTime);
    }
}

ION_NAMESPACE_END