// Copyright (c) 2025-2025 Michele Condo'
// File: C:\Projects\Ion\Ion\Scene\Node.cpp
// Licensed under the GPL-3.0 License. See LICENSE file in the project root for full license information.


#include "Node.h"

#include "../Dependencies/Nix/Nix/Nix.h"

#include "../Animation/AnimationRenderer.h"


EOS_USING_NAMESPACE
NIX_USING_NAMESPACE

ION_NAMESPACE_BEGIN

NodeAllocator* Node::GetAllocator()
{
	static HeapArea<Settings::kNodeAllocatorSize> memoryArea;
	static NodeAllocator memoryAllocator(memoryArea, "NodeFreeListAllocator");

	return &memoryAllocator;
}


Node::Node() : m_active(true), m_visible(true), m_renderLayer(ENodeRenderLayer_Default), m_parent(nullptr)
{
    m_uuid.GenerateUUID();
    SetName(ION_BASE_NODE_NAME);
    m_nodeType = ENodeType_EmptyNode;
}

Node::Node(const ionString& _name) : m_active(true), m_visible(true), m_renderLayer(ENodeRenderLayer_Default), m_parent(nullptr)
{
    m_uuid.GenerateUUID();
    SetName(_name);
    m_nodeType = ENodeType_EmptyNode;
}

Node::~Node()
{
	for (ionVector<Node*, NodeAllocator, GetAllocator>::size_type i = 0; i < m_children.size(); ++i)
	{
		Node* node = m_children[i];
		DestroyNode(node);
	}

	DetachFromParent();
}

void Node::SetName(const ionString& _name)
{
    m_name = _name;
}

void Node::AttachToParent(Node* _parent)
{
    if (m_parent != nullptr)
    {
        DetachFromParent();
    }

    m_parent = _parent;
    if (m_parent != nullptr)
    {
		ionVector<Node*, NodeAllocator, GetAllocator> &children = m_parent->GetChildren();

		children.push_back(this);

        OnAttachToParent(_parent);
    }
}

void Node::DetachFromParent()
{
    OnDetachFromParent();

    if (m_parent != nullptr)
    {
        ionVector<Node*, NodeAllocator, GetAllocator>::iterator pendingRemove = std::remove_if(m_parent->GetChildren().begin(), m_parent->GetChildren().end(),
            [&](Node* _object)
        {
            return this == _object;
        }
        );

        if (pendingRemove != m_parent->GetChildren().end())
        {
            m_parent->GetChildren().erase(pendingRemove, m_parent->GetChildren().end());
        }
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

        ionVector<Node*, NodeAllocator, GetAllocator>::const_iterator begin = ChildrenIteratorBeginConst(), end = ChildrenIteratorEndConst(), it = begin;
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

    ionVector<Node*, NodeAllocator, GetAllocator>::const_iterator begin = ChildrenIteratorBeginConst(), end = ChildrenIteratorEndConst(), it = begin;
    for (; it != end; ++it)
    {
        (*it)->SetActive(_isActive);
    }
}

void Node::SetVisible(ionBool _isVisible) 
{
    m_visible = _isVisible; 

    ionVector<Node*, NodeAllocator, GetAllocator>::const_iterator begin = ChildrenIteratorBeginConst(), end = ChildrenIteratorEndConst(), it = begin;
    for (; it != end; ++it)
    {
        (*it)->SetVisible(_isVisible);
    }
}

void Node::AddToRenderLayer(ENodeRenderLayer _layer)
{
    m_renderLayer |= _layer; 

    ionVector<Node*, NodeAllocator, GetAllocator>::const_iterator begin = ChildrenIteratorBeginConst(), end = ChildrenIteratorEndConst(), it = begin;
    for (; it != end; ++it)
    {
        (*it)->AddToRenderLayer(_layer);
    }
}

void Node::RemoveFromRenderLayer(ENodeRenderLayer _layer)
{
    m_renderLayer &= ~_layer; 

    ionVector<Node*, NodeAllocator, GetAllocator>::const_iterator begin = ChildrenIteratorBeginConst(), end = ChildrenIteratorEndConst(), it = begin;
    for (; it != end; ++it)
    {
        (*it)->RemoveFromRenderLayer(_layer);
    }
}


void Node::IterateAll(const std::function< void(Node* _node) >& _lambda /*= nullptr*/)
{
    if (_lambda != nullptr)
    {
        ionVector<Node*, NodeAllocator, GetAllocator>::const_iterator begin = ChildrenIteratorBeginConst(), end = ChildrenIteratorEndConst(), it = begin;
        for (; it != end; ++it)
        {
            _lambda((*it));
            (*it)->IterateAll(_lambda);
        }
    }
}

ION_NAMESPACE_END