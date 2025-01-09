// Copyright (c) 2025-2025 Michele Condo'
// File: C:\Projects\Ion\Ion\Animation\AnimationRenderer.h
// Licensed under the GPL-3.0 License. See LICENSE file in the project root for full license information.


#pragma once

#include "../Dependencies/Eos/Eos/Eos.h"

#include "../Core/CoreDefs.h"

#include "../Animation/Animation.h"

#include "../Core/MemorySettings.h"

EOS_USING_NAMESPACE

ION_NAMESPACE_BEGIN


using AnimationRendererAllocator = MemoryAllocator<FreeListBestSearchAllocationPolicy, MultiThreadPolicy, MemoryBoundsCheck, MemoryTag, MemoryLog>;

class Node;
class ION_DLL AnimationRenderer final
{
private:
	static AnimationRendererAllocator* GetAllocator();

public:
    AnimationRenderer();
    ~AnimationRenderer();

    void SetEnabled(ionBool _enabled) { m_enabled = _enabled; }
    ionBool IsEnabled() const { return m_enabled; }

    void SetAnimationToPlay(ionU32 _animationIndex) { m_animationIndex = _animationIndex; }
    ionU32 GetAnimationPlayed() const { return m_animationIndex; }

    void PushBackAnimation(const Animation& _animation);

    ionFloat GetAnimationTimer() const { return m_timer; }
    void ResetAnimationTimer() { m_timer = 0.0f; }

    ionFloat GetAnimationSpeedMultiplier() const { return m_animationSpeedMultiplier; }
    void SetAnimationSpeedMultiplier(ionFloat _multiplier) { m_animationSpeedMultiplier = _multiplier; }

    ionSize GetAnimationCount() const { return m_aninimations.size(); }
    const ionVector<Animation, AnimationRendererAllocator, GetAllocator>& GetAnimations() const { return m_aninimations; }
    const Animation& GetAnimation(ionU32 _index) const { return m_aninimations[_index]; }
    Animation& GetAnimation(ionU32 _index) { return m_aninimations[_index]; }
    ionVector<Animation, AnimationRendererAllocator, GetAllocator>& GetAnimations() { return m_aninimations; }
    ionVector<Animation, AnimationRendererAllocator, GetAllocator>::const_iterator AnimationsIteratorBeginConst() { return m_aninimations.begin(); }
    ionVector<Animation, AnimationRendererAllocator, GetAllocator>::const_iterator AnimationsIteratorEndConst() { return m_aninimations.end(); }
    ionVector<Animation, AnimationRendererAllocator, GetAllocator>::iterator AnimationsIteratorBegin() { return m_aninimations.begin(); }
    ionVector<Animation, AnimationRendererAllocator, GetAllocator>::iterator AnimationsIteratorEnd() { return m_aninimations.end(); }

    void OnUpateAll(ionFloat _deltaTime);
    void OnUpdate(ionFloat _deltaTime);

    // actually just set the render parameters...
    void Draw(const Node* _node);

private:
    void UpdateAnimation(ionU32 _animationIndex, ionFloat _animationTimer);

    ionFloat Lerp(ionFloat v0, ionFloat v1, ionFloat t);
    ionFloat Step(ionFloat v0, ionFloat v1, ionFloat t);
    ionFloat HermiteCubicSpline(ionFloat _p0, ionFloat _m0, ionFloat _p1, ionFloat _m1, ionFloat _t);

private:
    ionVector<Animation, AnimationRendererAllocator, GetAllocator>    m_aninimations;
    ionFloat                m_timer;
    ionFloat                m_animationSpeedMultiplier;
    ionU32                  m_animationIndex;
    ionBool                 m_enabled;
};


ION_NAMESPACE_END