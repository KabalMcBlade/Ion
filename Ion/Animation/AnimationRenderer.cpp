#include "AnimationRenderer.h"

#include "../Dependencies/Nix/Nix/Nix.h"

#include "../Scene/Node.h"

EOS_USING_NAMESPACE
NIX_USING_NAMESPACE

ION_NAMESPACE_BEGIN

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

void AnimationRenderer::OnUpate(ionFloat _deltaTime)
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

void AnimationRenderer::UpdateAnimation(ionU32 _animationIndex, ionFloat _animationTimer)
{
    Animation& anim = m_aninimations[_animationIndex];

    eosVector(AnimationChannel)::iterator begin = anim.ChannelsIteratorBegin(), end = anim.ChannelsIteratorEnd(), it = begin;
    for (; it != end; ++it)
    {
        AnimationChannel& channel = *it;

        eosVector(AnimationSampler)& samplers = anim.GetSamplers();

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

        // logic
        eosVector(ionFloat)& inputs = sampler.GetInputs();
        const ionU32 inputSize = static_cast<ionU32>(inputs.size());
        for (ionU32 i = 0; i < inputSize - 1; ++i)
        {
            if ((_animationTimer >= inputs[i]) && (_animationTimer <= inputs[i + 1]))
            {
                ionFloat u = std::max(0.0f, _animationTimer - inputs[i]) / (inputs[i + 1] - inputs[i]);
                if (u <= 1.0f)
                {
                    switch (channel.GetPath()) 
                    {
                    case EAnimationPathType_Translation:
                    {
                        Vector position = sampler.GetLinearPath(i).LerpTo(sampler.GetLinearPath(i + 1), u);
                        channel.GetNode()->GetTransform().SetPosition(position);
                        break;
                    }
                    case EAnimationPathType_Scale:
                    {
                        Vector scale = sampler.GetLinearPath(i).LerpTo(sampler.GetLinearPath(i + 1), u);
                        channel.GetNode()->GetTransform().SetScale(scale);
                        break;
                    }
                    case EAnimationPathType_Rotation:
                    {
                        Quaternion rot1 = sampler.GetLinearPath(i);
                        Quaternion rot2 = sampler.GetLinearPath(i + 1);

                        Quaternion rotation = rot1.Slerp(rot2, u);
                        channel.GetNode()->GetTransform().SetRotation(rotation);
                        break;
                    }
                    case EAnimationPathType_WeightMorphTarget:
                    {
                        // NOT YET IMPLEMENTED
                        break;
                    }
                    }
                }
            }
        }
    }
}

ION_NAMESPACE_END


