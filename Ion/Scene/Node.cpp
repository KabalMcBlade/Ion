#include "Node.h"

#include "../Dependencies/Nix/Nix/Nix.h"

#include "../Animation/AnimationRenderer.h"

#define ION_BASE_NODE_NAME "Node"

EOS_USING_NAMESPACE
NIX_USING_NAMESPACE

ION_NAMESPACE_BEGIN

Node::Node() : m_active(true), m_visible(true), m_renderLayer(ENodeRenderLayer_Default), m_parent(nullptr)
{
    m_uuid.GenerateUUID();
    SetName(ION_BASE_NODE_NAME);
    m_nodeType = ENodeType_EmptyNode;
}

Node::Node(const eosString & _name) : m_active(true), m_visible(true), m_renderLayer(ENodeRenderLayer_Default), m_parent(nullptr)
{
    m_uuid.GenerateUUID();
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
        eosVector<ObjectHandler>::iterator pendingRemove = std::remove_if(m_parent->GetChildren().begin(), m_parent->GetChildren().end(), 
            [&](ObjectHandler& _object)
        {
            return this == _object;
        }
        );

        if (pendingRemove != m_parent->GetChildren().end())
        {
            m_parent->GetChildren().erase(pendingRemove, m_parent->GetChildren().end());
        }

        //m_parent->GetChildren().erase(std::remove(m_parent->GetChildren().begin(), m_parent->GetChildren().end(), this), m_parent->GetChildren().end());
    }
}

void Node::Update(ionFloat _deltaTime)
{
    if (m_active)
    {
        OnUpdate(_deltaTime);
        
        AnimationRenderer* animationRenderer = GetAnimationRenderer();
        if (animationRenderer != nullptr)
        {
            if (animationRenderer->IsEnabled())
            {
                animationRenderer->OnUpdate(_deltaTime);
            }
        }
        
        if (m_parent != nullptr)
        {
            m_transform.SetMatrixWS(m_transform.GetMatrix() * m_parent->GetTransform().GetMatrixWS());
        }
        else
        {
            m_transform.SetMatrixWS(m_transform.GetMatrix());
        }

        eosVector<ObjectHandler>::const_iterator begin = ChildrenIteratorBeginConst(), end = ChildrenIteratorEndConst(), it = begin;
        for (; it != end; ++it)
        {
            (*it)->Update(_deltaTime);
        }

        OnLateUpdate(_deltaTime);
    }
}

// all these methods set from "this" parenting object to all their children in cascade
void Node::SetActive(ionBool _isActive)
{
    m_active = _isActive;

    eosVector<ObjectHandler>::const_iterator begin = ChildrenIteratorBeginConst(), end = ChildrenIteratorEndConst(), it = begin;
    for (; it != end; ++it)
    {
        (*it)->SetActive(_isActive);
    }
}

void Node::SetVisible(ionBool _isVisible) 
{
    m_visible = _isVisible; 

    eosVector<ObjectHandler>::const_iterator begin = ChildrenIteratorBeginConst(), end = ChildrenIteratorEndConst(), it = begin;
    for (; it != end; ++it)
    {
        (*it)->SetVisible(_isVisible);
    }
}

void Node::AddToRenderLayer(ENodeRenderLayer _layer)
{
    m_renderLayer |= _layer; 

    eosVector<ObjectHandler>::const_iterator begin = ChildrenIteratorBeginConst(), end = ChildrenIteratorEndConst(), it = begin;
    for (; it != end; ++it)
    {
        (*it)->AddToRenderLayer(_layer);
    }
}

void Node::RemoveFromRenderLayer(ENodeRenderLayer _layer)
{
    m_renderLayer &= ~_layer; 

    eosVector<ObjectHandler>::const_iterator begin = ChildrenIteratorBeginConst(), end = ChildrenIteratorEndConst(), it = begin;
    for (; it != end; ++it)
    {
        (*it)->RemoveFromRenderLayer(_layer);
    }
}


void Node::IterateAll(const std::function< void(const ObjectHandler& _node) >& _lambda /*= nullptr*/)
{
    if (_lambda != nullptr)
    {
        eosVector<ObjectHandler>::const_iterator begin = ChildrenIteratorBeginConst(), end = ChildrenIteratorEndConst(), it = begin;
        for (; it != end; ++it)
        {
            _lambda((*it));
            (*it)->IterateAll(_lambda);
        }
    }
}

ION_NAMESPACE_END