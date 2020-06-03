#include "AnimationRenderer.h"

#include "../Dependencies/Nix/Nix/Nix.h"

#include "../Scene/Node.h"

#include "../Shader/ShaderProgramManager.h"

EOS_USING_NAMESPACE
NIX_USING_NAMESPACE

ION_NAMESPACE_BEGIN

AnimationRendererAllocator* AnimationRenderer::GetAllocator()
{
	static HeapArea<Settings::kAnimationRendererAllocatorSize> memoryArea;
	static AnimationRendererAllocator memoryAllocator(memoryArea, "AnimationRendererFreeListAllocator");

	return &memoryAllocator;
}

AnimationRenderer::AnimationRenderer() : m_enabled(true), m_timer(0.0f), m_animationSpeedMultiplier(1.0f), m_animationIndex(0)
{
}

AnimationRenderer::~AnimationRenderer()
{
    m_aninimations.clear();
}

void AnimationRenderer::PushBackAnimation(const Animation& _animation)
{
    m_aninimations.push_back(_animation);
}

void AnimationRenderer::OnUpateAll(ionFloat _deltaTime)
{
    const ionU32 count = static_cast<ionU32>(GetAnimationCount());
    for (ionU32 i = 0; i < count; ++i)
    {
        m_timer += _deltaTime * m_animationSpeedMultiplier;
        if (m_timer > m_aninimations[i].GetEnd())
        {
            m_timer -= m_aninimations[i].GetEnd();
        }

        UpdateAnimation(i, m_timer);
    }
}

void AnimationRenderer::OnUpdate(ionFloat _deltaTime)
{
    if (GetAnimationCount() == 0)
    {
        return;
    }

    m_timer += _deltaTime * m_animationSpeedMultiplier;
    if (m_timer > m_aninimations[m_animationIndex].GetEnd())
    {
        m_timer -= m_aninimations[m_animationIndex].GetEnd();
    }

    UpdateAnimation(m_animationIndex, m_timer);
}

ionFloat AnimationRenderer::Lerp(ionFloat v0, ionFloat v1, ionFloat t)
{
    return (1.0f - t) * v0 + t * v1;
}

ionFloat AnimationRenderer::Step(ionFloat v0, ionFloat v1, ionFloat t)
{
    if (t < 0.500001f)
    {
        return v0;
    }
    else
    {
        return v1;
    }
}

ionFloat AnimationRenderer::HermiteCubicSpline(ionFloat _p0, ionFloat _m0, ionFloat _p1, ionFloat _m1, ionFloat _t)
{
    const ionFloat p0c = (2.0f * powf(_t, 3.0f)) - (3.0f * powf(_t, 2.0f)) + 1.0f;
    const ionFloat m0c = powf(_t, 3.0f) - (2 * powf(_t, 2.0f)) + _t;
    const ionFloat p1c = (-2 * powf(_t, 3.0f)) + (3 * powf(_t, 2.0f));
    const ionFloat m1c = powf(_t, 3.0f) - powf(_t, 2.0f);

    const ionFloat p0 = p0c * _p0;
    const ionFloat m0 = m0c * _m0;
    const ionFloat p1 = p1c * _p1;
    const ionFloat m1 = m1c * _m1;

    return p0 + m0 + p1 + m0;
}

void AnimationRenderer::UpdateAnimation(ionU32 _animationIndex, ionFloat _animationTimer)
{
    Animation& anim = m_aninimations[_animationIndex];

    ionVector<AnimationChannel, AnimationChannelAllocator, AnimationChannel::GetAllocator>::iterator begin = anim.ChannelsIteratorBegin(), end = anim.ChannelsIteratorEnd(), it = begin;
    for (; it != end; ++it)
    {
        AnimationChannel& channel = *it;

        ionVector<AnimationSampler, AnimationAllocator, Animation::GetAllocator>& samplers = anim.GetSamplers();

        // Sanity check
        AnimationSampler& sampler = samplers[channel.GetSamplerIndex()];
        if (channel.GetPath() == EAnimationPathType_WeightMorphTarget)
        {
            if (sampler.GetInputs().size() > sampler.GetMorphTargets().size())
            {
                continue;
            }
        }
        else
        {
            if (sampler.GetInputs().size() > sampler.GetLinearPaths().size())
            {
                continue;
            }
        }

        ionU32 weightCount = channel.GetNode()->GetInitialMorphTargetWeightCount();
        channel.GetNode()->ResizeMorphTargetWeight(weightCount);

        for (ionU32 w = 0; w < weightCount; ++w)
        {
            channel.GetNode()->SetMorphTargetWeight(w, channel.GetNode()->GetInitialMorphTargetWeight(w));
        }

        // logic
        ionVector<ionFloat, AnimationSamplerAllocator, AnimationSampler::GetAllocator>& inputs = sampler.GetInputs();
        const ionU32 inputSize = static_cast<ionU32>(inputs.size());
        for (ionU32 i = 0; i < inputSize - 1; ++i)
        {
            if ((_animationTimer >= inputs[i]) && (_animationTimer <= inputs[i + 1]))
            {
                const ionFloat td = (inputs[i + 1] - inputs[i]);
                const ionFloat t = std::max(0.0f, _animationTimer - inputs[i]) / td;
                if (t <= 1.0f)
                {
                    switch(sampler.GetInterpolation())
                    {
                    case EAnimationInterpolationType_Linear:
                    {
                        switch (channel.GetPath())
                        {
                        case EAnimationPathType_Translation:
                        {
                            Vector4 position = sampler.GetLinearPath(i).LerpTo(sampler.GetLinearPath(i + 1), t);
                            channel.GetNode()->GetTransform().SetPosition(position);
                            break;
                        }
                        case EAnimationPathType_Scale:
                        {
                            Vector4 scale = sampler.GetLinearPath(i).LerpTo(sampler.GetLinearPath(i + 1), t);
                            channel.GetNode()->GetTransform().SetScale(scale);
                            break;
                        }
                        case EAnimationPathType_Rotation:
                        {
                            Quaternion rot1 = sampler.GetLinearPath(i);
                            Quaternion rot2 = sampler.GetLinearPath(i + 1);

                            Quaternion rotation = rot1.Slerp(rot2, t);
                            rotation = rotation.Normalize();
                            channel.GetNode()->GetTransform().SetRotation(rotation);
                            break;
                        }
                        case EAnimationPathType_WeightMorphTarget:
                        {
                            ionU32 weightCount = channel.GetNode()->GetInitialMorphTargetWeightCount();
                            ionU32 vertexIndex0 = (i * weightCount * 3) + weightCount;
                            ionU32 vertexIndex1 = ((i + 1) * weightCount * 3) + weightCount;

                            for (ionU32 w = 0; w < weightCount; ++w)
                            {
                                const ionFloat a = sampler.GetMorphTarget(vertexIndex0 + w);
                                const ionFloat b = sampler.GetMorphTarget(vertexIndex1 + w);

                                const ionFloat weight = Lerp(a, b, t);
                                channel.GetNode()->SetMorphTargetWeight(w, weight);
                            }

                            break;
                        }
                        }
                        break;
                    }
                    case EAnimationInterpolationType_Step:
                    {
                        switch (channel.GetPath())
                        {
                        case EAnimationPathType_Translation:
                        {
                            Vector4 position = sampler.GetLinearPath(i).StepTo(sampler.GetLinearPath(i + 1), t);
                            channel.GetNode()->GetTransform().SetPosition(position);
                            break;
                        }
                        case EAnimationPathType_Scale:
                        {
                            Vector4 scale = sampler.GetLinearPath(i).StepTo(sampler.GetLinearPath(i + 1), t);
                            channel.GetNode()->GetTransform().SetScale(scale);
                            break;
                        }
                        case EAnimationPathType_Rotation:
                        {
                            Quaternion rot1 = sampler.GetLinearPath(i);
                            Quaternion rot2 = sampler.GetLinearPath(i + 1);

                            Quaternion rotation = rot1.StepTo(rot2, t);
                            rotation = rotation.Normalize();
                            channel.GetNode()->GetTransform().SetRotation(rotation);
                            break;
                        }
                        case EAnimationPathType_WeightMorphTarget:
                        {
                            ionU32 weightCount = channel.GetNode()->GetInitialMorphTargetWeightCount();
                            ionU32 vertexIndex0 = (i * weightCount * 3) + weightCount;
                            ionU32 vertexIndex1 = ((i + 1) * weightCount * 3) + weightCount;

                            for (ionU32 w = 0; w < weightCount; ++w)
                            {
                                const ionFloat a = sampler.GetMorphTarget(vertexIndex0 + w);
                                const ionFloat b = sampler.GetMorphTarget(vertexIndex1 + w);

                                const ionFloat weight = Step(a, b, t);
                                channel.GetNode()->SetMorphTargetWeight(w, weight);
                            }

                            break;
                        }
                        }
                        break;
                    }
                    case EAnimationInterpolationType_CubicSpline:
                    {
                        switch (channel.GetPath())
                        {
                        case EAnimationPathType_Translation:
                        {
                            Vector4 p0 = sampler.GetLinearPath(i);
                            Vector4 m0 = sampler.GetLinearPath(i) * td;
                            Vector4 p1 = sampler.GetLinearPath(i + 1);
                            Vector4 m1 = sampler.GetLinearPath(i + 1) * td;
                            Vector4 u(t);

                            Vector4 position = MathFunctions::HermiteCubicSpline(p0, m0, p1, m1, u);

                            channel.GetNode()->GetTransform().SetPosition(position);
                            break;
                        }
                        case EAnimationPathType_Scale:
                        {
                            Vector4 p0 = sampler.GetLinearPath(i);
                            Vector4 m0 = sampler.GetLinearPath(i) * td;
                            Vector4 p1 = sampler.GetLinearPath(i + 1);
                            Vector4 m1 = sampler.GetLinearPath(i + 1) * td;
                            Vector4 u(t);

                            Vector4 scale = MathFunctions::HermiteCubicSpline(p0, m0, p1, m1, u);

                            channel.GetNode()->GetTransform().SetScale(scale);
                            break;
                        }
                        case EAnimationPathType_Rotation:
                        {
                            // not very meaningful... just to have "some code" in here... for now..
                            Quaternion p0 = sampler.GetLinearPath(i);
                            Quaternion m0 = sampler.GetLinearPath(i) * td;
                            Quaternion p1 = sampler.GetLinearPath(i + 1);
                            Quaternion m1 = sampler.GetLinearPath(i + 1) * td;
                            Vector4 u(t);

                            Quaternion rotation = MathFunctions::HermiteCubicSpline(p0, m0, p1, m1, u);
                            rotation = rotation.Normalize();

                            channel.GetNode()->GetTransform().SetRotation(rotation);
                            break;
                        }
                        case EAnimationPathType_WeightMorphTarget:
                        {
                            ionU32 weightCount = channel.GetNode()->GetInitialMorphTargetWeightCount();

                            ionU32 vertexIndex0 = (i * weightCount * 3) + weightCount;
                            ionU32 outTangent0 = (i * weightCount * 3) + (weightCount * 2);
                            ionU32 vertexIndex1 = ((i + 1) * weightCount * 3) + weightCount;
                            ionU32 inTangent1 = (i + 1) * weightCount * 3;

                            for (ionU32 w = 0; w < weightCount; ++w)
                            {
                                const ionFloat p0 = sampler.GetMorphTarget(vertexIndex0 + w);
                                const ionFloat m0 = sampler.GetMorphTarget(outTangent0 + w) * td;
                                const ionFloat p1 = sampler.GetMorphTarget(vertexIndex1 + w);
                                const ionFloat m1 = sampler.GetMorphTarget(inTangent1 + w) * td;

                                const ionFloat weight = HermiteCubicSpline(p0, m0, p1, m1, t);
                                channel.GetNode()->SetMorphTargetWeight(w, weight);
                            }

                            break;
                        }
                        }
                        break;
                    }
                    case EAnimationInterpolationType_None:
                    default:
                    {
                        break;
                    }
                    }
                }
            }
        }
    }
}

void AnimationRenderer::Draw(const Node* _node)
{
    ionFloat weights[ION_MAX_WEIGHT_COUNT];
    memset(&weights, 0, sizeof(weights));

    const ionU32 count = _node->GetMorphTargetWeightCount();
    for (ionU32 i = 0; i < count && i < ION_MAX_WEIGHT_COUNT; ++i)
    {
        weights[i] = _node->GetMorphTargetWeight(i);
    }

    if (count > 0)
    {
        ionShaderProgramManager().SetRenderParamsFloat(ION_WEIGHTS_FLOATS_ARRAY_PARAM, &weights[0], ION_MAX_WEIGHT_COUNT);
    }
}


ION_NAMESPACE_END


