// Copyright (c) 2025-2025 Michele Condo'
// File: C:\Projects\Ion\Ion\Animation\Animation.h
// Licensed under the GPL-3.0 License. See LICENSE file in the project root for full license information.


#pragma once


#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>

#include "../Dependencies/Eos/Eos/Eos.h"
#include "../Dependencies/Nix/Nix/Nix.h"

#include "../Core/CoreDefs.h"
#include "../Core/MemoryWrapper.h"

#include "../Core/MemorySettings.h"


EOS_USING_NAMESPACE
NIX_USING_NAMESPACE

ION_NAMESPACE_BEGIN


enum EAnimationPathType
{
    EAnimationPathType_None,
    EAnimationPathType_Translation,
    EAnimationPathType_Rotation,
    EAnimationPathType_Scale,
    EAnimationPathType_WeightMorphTarget
};

enum EAnimationInterpolationType 
{
    EAnimationInterpolationType_None,
    EAnimationInterpolationType_Linear,
    EAnimationInterpolationType_Step,
    EAnimationInterpolationType_CubicSpline
};

//////////////////////////////////////////////////////////////////////////

using AnimationChannelAllocator = MemoryAllocator<FreeListBestSearchAllocationPolicy, MultiThreadPolicy, MemoryBoundsCheck, MemoryTag, MemoryLog>;

class Node;
class ION_DLL AnimationChannel final
{
public:
	static AnimationChannelAllocator* GetAllocator();

public:
    AnimationChannel();
    ~AnimationChannel();

    void SetNode(Node* _node);
    void SetSamplerIndex(ionU32 _samplerIndex);
    void SetPath(EAnimationPathType _path);

    Node* GetNode() { return m_node; }
    const Node* GetNode() const { return m_node; }
    const ionU32 GetSamplerIndex() const { return m_samplerIndex; }
    const EAnimationPathType GetPath() const { return m_path; }

private:
    Node*               m_node;
    ionU32              m_samplerIndex;
    EAnimationPathType  m_path;
};

//////////////////////////////////////////////////////////////////////////


using AnimationSamplerAllocator = MemoryAllocator<FreeListBestSearchAllocationPolicy, MultiThreadPolicy, MemoryBoundsCheck, MemoryTag, MemoryLog>;

class ION_DLL AnimationSampler final
{
public:
	static AnimationSamplerAllocator* GetAllocator();

public:
    AnimationSampler();
    ~AnimationSampler();

    void PushBackInput(ionFloat _input);
    void PushBackOutputMorphTarget(ionFloat _input);
    void PushBackOutputLinearPath(const Vector4& _input);

    void SetInterpolation(EAnimationInterpolationType _interpolation);
    const EAnimationInterpolationType GetInterpolation() const { return m_interpolation; }
    
    const ionVector<ionFloat, AnimationSamplerAllocator, GetAllocator>& GetInputs() const { return m_inputs; }
    ionVector<ionFloat, AnimationSamplerAllocator, GetAllocator>& GetInputs() { return m_inputs; }
    ionVector<ionFloat, AnimationSamplerAllocator, GetAllocator>::const_iterator InputsIteratorBeginConst() { return m_inputs.begin(); }
    ionVector<ionFloat, AnimationSamplerAllocator, GetAllocator>::const_iterator InputsIteratorEndConst() { return m_inputs.end(); }
    ionVector<ionFloat, AnimationSamplerAllocator, GetAllocator>::iterator InputsIteratorBegin() { return m_inputs.begin(); }
    ionVector<ionFloat, AnimationSamplerAllocator, GetAllocator>::iterator InputsIteratorEnd() { return m_inputs.end(); }

    ION_INLINE ionFloat GetMorphTarget(ionU32 _index)
    {
        ionAssertReturnValue(_index >= 0 && _index < m_outputsMorphTarget.size(), "Index out of range", -1.0f);
        return m_outputsMorphTarget[_index];
    }

    const ionVector<ionFloat, AnimationSamplerAllocator, GetAllocator>& GetMorphTargets() const { return m_outputsMorphTarget; }
    ionVector<ionFloat, AnimationSamplerAllocator, GetAllocator>& GetMorphTargets() { return m_outputsMorphTarget; }
    ionVector<ionFloat, AnimationSamplerAllocator, GetAllocator>::const_iterator MorphTargetsIteratorBeginConst() { return m_outputsMorphTarget.begin(); }
    ionVector<ionFloat, AnimationSamplerAllocator, GetAllocator>::const_iterator MorphTargetsIteratorEndConst() { return m_outputsMorphTarget.end(); }
    ionVector<ionFloat, AnimationSamplerAllocator, GetAllocator>::iterator MorphTargetsIteratorBegin() { return m_outputsMorphTarget.begin(); }
    ionVector<ionFloat, AnimationSamplerAllocator, GetAllocator>::iterator MorphTargetsIteratorEnd() { return m_outputsMorphTarget.end(); }

    ION_INLINE Vector4 GetLinearPath(ionU32 _index)
    {
        ionAssertReturnValue(_index >= 0 && _index < m_outputsLinearPath.size(), "Index out of range", MathFunctions::Splat(-1.0f));
        return m_outputsLinearPath[_index];
    }

    const ionVector<Vector4, AnimationSamplerAllocator, GetAllocator>& GetLinearPaths() const { return m_outputsLinearPath; }
    ionVector<Vector4, AnimationSamplerAllocator, GetAllocator>& GetLinearPaths() { return m_outputsLinearPath; }
    ionVector<Vector4, AnimationSamplerAllocator, GetAllocator>::const_iterator LinearPathsIteratorBeginConst() { return m_outputsLinearPath.begin(); }
    ionVector<Vector4, AnimationSamplerAllocator, GetAllocator>::const_iterator LinearPathsIteratorEndConst() { return m_outputsLinearPath.end(); }
    ionVector<Vector4, AnimationSamplerAllocator, GetAllocator>::iterator LinearPathsIteratorBegin() { return m_outputsLinearPath.begin(); }
    ionVector<Vector4, AnimationSamplerAllocator, GetAllocator>::iterator LinearPathsIteratorEnd() { return m_outputsLinearPath.end(); }

private:
    ionVector<ionFloat, AnimationSamplerAllocator, GetAllocator> m_inputs;
    ionVector<ionFloat, AnimationSamplerAllocator, GetAllocator> m_outputsMorphTarget;   // can be normalized integer, unified to floats: each output element stores values with a count equal to the number of morph targets.
    ionVector<Vector4, AnimationSamplerAllocator, GetAllocator> m_outputsLinearPath;
    EAnimationInterpolationType m_interpolation;
};

//////////////////////////////////////////////////////////////////////////

using AnimationAllocator = MemoryAllocator<FreeListBestSearchAllocationPolicy, MultiThreadPolicy, MemoryBoundsCheck, MemoryTag, MemoryLog>;

class ION_DLL Animation final
{
public:
	static AnimationAllocator* GetAllocator();

public:
    Animation();
    ~Animation();

    void SetName(const ionString& _name);

    ionBool IsValid() const { return m_hash != -1; }

    const ionString& GetName() const { return m_name; }
    ionSize GetHashName() const { return m_hash; }

    void SetStart(ionFloat _value);
    void SetEnd(ionFloat _value);

    ionFloat GetStart() const { return m_start; }
    ionFloat GetEnd() const { return m_end; }

    void PushBackSampler(const AnimationSampler& _sampler);
    void PushBackChannel(const AnimationChannel& _channel);

    const ionVector<AnimationSampler, AnimationAllocator, GetAllocator>& GetSamplers() const { return m_samplers; }
    ionVector<AnimationSampler, AnimationAllocator, GetAllocator>& GetSamplers() { return m_samplers; }
    ionVector<AnimationSampler, AnimationAllocator, GetAllocator>::const_iterator SamplersIteratorBeginConst() { return m_samplers.begin(); }
    ionVector<AnimationSampler, AnimationAllocator, GetAllocator>::const_iterator SamplersIteratorEndConst() { return m_samplers.end(); }
    ionVector<AnimationSampler, AnimationAllocator, GetAllocator>::iterator SamplersIteratorBegin() { return m_samplers.begin(); }
    ionVector<AnimationSampler, AnimationAllocator, GetAllocator>::iterator SamplersIteratorEnd() { return m_samplers.end(); }

    const ionVector<AnimationChannel, AnimationAllocator, GetAllocator>& GetChannels() const { return m_channels; }
    ionVector<AnimationChannel, AnimationAllocator, GetAllocator>& GetChannels() { return m_channels; }
    ionVector<AnimationChannel, AnimationAllocator, GetAllocator>::const_iterator ChannelsIteratorBeginConst() { return m_channels.begin(); }
    ionVector<AnimationChannel, AnimationAllocator, GetAllocator>::const_iterator ChannelsIteratorEndConst() { return m_channels.end(); }
    ionVector<AnimationChannel, AnimationAllocator, GetAllocator>::iterator ChannelsIteratorBegin() { return m_channels.begin(); }
    ionVector<AnimationChannel, AnimationAllocator, GetAllocator>::iterator ChannelsIteratorEnd() { return m_channels.end(); }

private:
    ionVector<AnimationSampler, AnimationAllocator, GetAllocator> m_samplers;
	ionVector<AnimationChannel, AnimationAllocator, GetAllocator> m_channels;
    ionString m_name;
    ionSize                     m_hash;
    ionFloat                    m_start;
    ionFloat                    m_end;
};


ION_NAMESPACE_END
