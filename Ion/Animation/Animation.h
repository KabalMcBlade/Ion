#pragma once


#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>

#include "../Dependencies/Eos/Eos/Eos.h"
#include "../Dependencies/Nix/Nix/Nix.h"

#include "../Core/CoreDefs.h"
#include "../Core/MemoryWrapper.h"


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

class Node;
class ION_DLL AnimationChannel final
{
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

class ION_DLL AnimationSampler final
{
public:
    AnimationSampler();
    ~AnimationSampler();

    void PushBackInput(ionFloat _input);
    void PushBackOutputMorphTarget(ionFloat _input);
    void PushBackOutputLinearPath(const Vector& _input);

    void SetInterpolation(EAnimationInterpolationType _interpolation);
    const EAnimationInterpolationType GetInterpolation() const { return m_interpolation; }
    
    const ionVector<ionFloat>& GetInputs() const { return m_inputs; }
    ionVector<ionFloat>& GetInputs() { return m_inputs; }
    ionVector<ionFloat>::const_iterator InputsIteratorBeginConst() { return m_inputs->begin(); }
    ionVector<ionFloat>::const_iterator InputsIteratorEndConst() { return m_inputs->end(); }
    ionVector<ionFloat>::iterator InputsIteratorBegin() { return m_inputs->begin(); }
    ionVector<ionFloat>::iterator InputsIteratorEnd() { return m_inputs->end(); }

    ION_INLINE ionFloat GetMorphTarget(ionU32 _index)
    {
        ionAssertReturnValue(_index >= 0 && _index < m_outputsMorphTarget->size(), "Index out of range", -1.0f);
        return m_outputsMorphTarget[_index];
    }

    const ionVector<ionFloat>& GetMorphTargets() const { return m_outputsMorphTarget; }
    ionVector<ionFloat>& GetMorphTargets() { return m_outputsMorphTarget; }
    ionVector<ionFloat>::const_iterator MorphTargetsIteratorBeginConst() { return m_outputsMorphTarget->begin(); }
    ionVector<ionFloat>::const_iterator MorphTargetsIteratorEndConst() { return m_outputsMorphTarget->end(); }
    ionVector<ionFloat>::iterator MorphTargetsIteratorBegin() { return m_outputsMorphTarget->begin(); }
    ionVector<ionFloat>::iterator MorphTargetsIteratorEnd() { return m_outputsMorphTarget->end(); }

    ION_INLINE Vector GetLinearPath(ionU32 _index)
    {
        ionAssertReturnValue(_index >= 0 && _index < m_outputsLinearPath->size(), "Index out of range", Helper::Splat(-1.0f));
        return m_outputsLinearPath[_index];
    }

    const ionVector<Vector>& GetLinearPaths() const { return m_outputsLinearPath; }
    ionVector<Vector>& GetLinearPaths() { return m_outputsLinearPath; }
    ionVector<Vector>::const_iterator LinearPathsIteratorBeginConst() { return m_outputsLinearPath->begin(); }
    ionVector<Vector>::const_iterator LinearPathsIteratorEndConst() { return m_outputsLinearPath->end(); }
    ionVector<Vector>::iterator LinearPathsIteratorBegin() { return m_outputsLinearPath->begin(); }
    ionVector<Vector>::iterator LinearPathsIteratorEnd() { return m_outputsLinearPath->end(); }

private:
    ionVector<ionFloat>         m_inputs;
    ionVector<ionFloat>         m_outputsMorphTarget;   // can be normalized integer, unified to floats: each output element stores values with a count equal to the number of morph targets.
    ionVector<Vector>           m_outputsLinearPath;
    EAnimationInterpolationType m_interpolation;
};

//////////////////////////////////////////////////////////////////////////

class ION_DLL Animation final
{
public:
    Animation();
    ~Animation();

    void SetName(const ionString& _name);

    ionBool IsValid() const { return m_hash != -1; }

    const ionString& GetName() const { return m_name; }
    eosSize GetHashName() const { return m_hash; }

    void SetStart(ionFloat _value);
    void SetEnd(ionFloat _value);

    ionFloat GetStart() const { return m_start; }
    ionFloat GetEnd() const { return m_end; }

    void PushBackSampler(const AnimationSampler& _sampler);
    void PushBackChannel(const AnimationChannel& _channel);

    const ionVector<AnimationSampler>& GetSamplers() const { return m_samplers; }
    ionVector<AnimationSampler>& GetSamplers() { return m_samplers; }
    ionVector<AnimationSampler>::const_iterator SamplersIteratorBeginConst() { return m_samplers->begin(); }
    ionVector<AnimationSampler>::const_iterator SamplersIteratorEndConst() { return m_samplers->end(); }
    ionVector<AnimationSampler>::iterator SamplersIteratorBegin() { return m_samplers->begin(); }
    ionVector<AnimationSampler>::iterator SamplersIteratorEnd() { return m_samplers->end(); }

    const ionVector<AnimationChannel>& GetChannels() const { return m_channels; }
    ionVector<AnimationChannel>& GetChannels() { return m_channels; }
    ionVector<AnimationChannel>::const_iterator ChannelsIteratorBeginConst() { return m_channels->begin(); }
    ionVector<AnimationChannel>::const_iterator ChannelsIteratorEndConst() { return m_channels->end(); }
    ionVector<AnimationChannel>::iterator ChannelsIteratorBegin() { return m_channels->begin(); }
    ionVector<AnimationChannel>::iterator ChannelsIteratorEnd() { return m_channels->end(); }

private:
    ionVector<AnimationSampler> m_samplers;
    ionVector<AnimationChannel> m_channels;
    ionString                   m_name;
    eosSize                     m_hash;
    ionFloat                    m_start;
    ionFloat                    m_end;
};


ION_NAMESPACE_END
