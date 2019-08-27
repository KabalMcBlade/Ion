#include "Animation.h"


EOS_USING_NAMESPACE
NIX_USING_NAMESPACE

ION_NAMESPACE_BEGIN


AnimationChannel::AnimationChannel() : m_node(nullptr), m_samplerIndex(0), m_path(EAnimationPathType_None)
{

}

AnimationChannel::~AnimationChannel()
{
    m_node = nullptr;
}

void AnimationChannel::SetNode(Node* _node)
{
    m_node = _node;
}

void AnimationChannel::SetSamplerIndex(ionU32 _samplerIndex)
{
    m_samplerIndex = _samplerIndex;
}

void AnimationChannel::SetPath(EAnimationPathType _path)
{
    m_path = _path;
}

//////////////////////////////////////////////////////////////////////////

AnimationSampler::AnimationSampler() : m_interpolation(EAnimationInterpolationType_None)
{

}

AnimationSampler::~AnimationSampler()
{
    m_inputs->clear();
    m_outputsLinearPath->clear();
    m_outputsMorphTarget->clear();
}

void AnimationSampler::PushBackInput(ionFloat _input)
{
    m_inputs->push_back(_input);
}

void AnimationSampler::PushBackOutputMorphTarget(ionFloat _input)
{
    m_outputsMorphTarget->push_back(_input);
}

void AnimationSampler::PushBackOutputLinearPath(const Vector& _input)
{
    m_outputsLinearPath->push_back(_input);
}

void AnimationSampler::SetInterpolation(EAnimationInterpolationType _interpolation)
{
    m_interpolation = _interpolation;
}

//////////////////////////////////////////////////////////////////////////

Animation::Animation() : m_name(""), m_hash(-1), m_start(std::numeric_limits<ionFloat>::max()), m_end(std::numeric_limits<ionFloat>::min())
{

}

Animation::~Animation()
{
    m_samplers->clear();
    m_channels->clear();
}

void Animation::SetName(const ionString& _name)
{
    m_name = _name;
    m_hash = std::hash<ionString>{}(m_name);
}

void Animation::PushBackSampler(const AnimationSampler& _sampler)
{
    m_samplers->push_back(_sampler);
}

void Animation::PushBackChannel(const AnimationChannel& _channel)
{
    m_channels->push_back(_channel);
}

void Animation::SetStart(ionFloat _value)
{
    m_start = _value;
}

void Animation::SetEnd(ionFloat _value)
{
    m_end = _value;
}

ION_NAMESPACE_END
