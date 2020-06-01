#include "Serializer.h"

#include "../Dependencies/Miscellaneous/json.hpp"

#include "../Renderer/RenderCommon.h"
#include "../Animation/AnimationRenderer.h"

#include "../Core/MemorySettings.h"

using nlohmann::json;

ION_NAMESPACE_BEGIN


using SerializerAllocator = MemoryAllocator<FreeListBestSearchAllocationPolicy, MultiThreadPolicy, MemoryBoundsCheck, MemoryTag, MemoryLog>;

SerializerAllocator* GetAllocator()
{
	static HeapArea<Settings::kSerializeAllocatorSize> memoryArea;
	static SerializerAllocator memoryAllocator(memoryArea, "SerializeListAllocator");

	return &memoryAllocator;
}


namespace _private
{
    static ionU32 g_serializationLevel = 1;

    void RendererToJSON(json& _json, const BaseMeshRenderer* _inMeshRenderer)
    {
        json meshRendererIndices;
        json meshRendererVertices;

        ionVector<Index, SerializerAllocator, GetAllocator> indexes;
        
        const ionU32 indexCount = _inMeshRenderer->GetIndexDataCount();
        const void* indexData = _inMeshRenderer->GetIndexData();
        //const ionSize indexSize = _inMeshRenderer->GetSizeOfIndex();

        for (ionU32 i = 0; i < indexCount; ++i)
        {
            const Index value = *((reinterpret_cast<const Index*&>(indexData)) + i);
            indexes.push_back(value);
        }

        meshRendererIndices = indexes;

		ionVector<VertexPlain, SerializerAllocator, GetAllocator> verticesPlain;
		ionVector<VertexColored, SerializerAllocator, GetAllocator> verticesColored;
		ionVector<VertexNormal, SerializerAllocator, GetAllocator> verticesNormal;
		ionVector<VertexUV, SerializerAllocator, GetAllocator> verticesUV;
		ionVector<VertexSimple, SerializerAllocator, GetAllocator> verticesSimple;
		ionVector<Vertex, SerializerAllocator, GetAllocator> verticesFull;

        const EVertexLayout vertexLayout = _inMeshRenderer->GetLayout();
        const ionU32 vertexCount = _inMeshRenderer->GetVertexDataCount();
        const void* vertexData = _inMeshRenderer->GetVertexData();
        //const ionSize vertexSize = _inMeshRenderer->GetSizeOfVertex();


        for (ionU32 i = 0; i < vertexCount; ++i)
        {
            switch (vertexLayout)
            {
            case EVertexLayout_Pos:
            {
                const VertexPlain value = *((reinterpret_cast<const VertexPlain*&>(vertexData)) + i);
                verticesPlain.push_back(value);
            }
            break;
            case EVertexLayout_Pos_Color:
            {
                const VertexColored value = *((reinterpret_cast<const VertexColored*&>(vertexData)) + i);
                verticesColored.push_back(value);
            }
            break;
            case EVertexLayout_Pos_Normal:
            {
                const VertexNormal value = *((reinterpret_cast<const VertexNormal*&>(vertexData)) + i);
                verticesNormal.push_back(value);
            }
            break;
            case EVertexLayout_Pos_UV:
            {
                const VertexUV value = *((reinterpret_cast<const VertexUV*&>(vertexData)) + i);
                verticesUV.push_back(value);
            }
            break;
            case EVertexLayout_Pos_UV_Normal:
            {
                const VertexSimple value = *((reinterpret_cast<const VertexSimple*&>(vertexData)) + i);
                verticesSimple.push_back(value);
            }
            break;
            case EVertexLayout_Full:
            {
                const Vertex value = *((reinterpret_cast<const Vertex*&>(vertexData)) + i);
                verticesFull.push_back(value);
            }
            break;
            default:
                break;
            }
        }

        // set the array
        switch (vertexLayout)
        {
        case EVertexLayout_Pos:
        {
            meshRendererVertices = verticesPlain;
        }
        break;
        case EVertexLayout_Pos_Color:
        {
            meshRendererVertices = verticesColored;
        }
        break;
        case EVertexLayout_Pos_Normal:
        {
            meshRendererVertices = verticesNormal;
        }
        break;
        case EVertexLayout_Pos_UV:
        {
            meshRendererVertices = verticesUV;
        }
        break;
        case EVertexLayout_Pos_UV_Normal:
        {
            meshRendererVertices = verticesSimple;
        }
        break;
        case EVertexLayout_Full:
        {
            meshRendererVertices = verticesFull;
        }
        break;
        default:
            break;
        }

        json meshRendererMorphTarget;

        const bool isUsingMorphTarget = _inMeshRenderer->IsUsingMorphTarget();
        if (isUsingMorphTarget)
        {
			ionVector<VertexMorphTarget, SerializerAllocator, GetAllocator> vertexMorphTarget;

            const ionU32 morphTargetCount = _inMeshRenderer->GetMorphTargetDataCount();
            const void* morphTargetData = _inMeshRenderer->GetMorphTargetData();
            //const ionSize morphTargetSize = _inMeshRenderer->GetSizeOfMorphTarget();

            for (ionU32 i = 0; i < morphTargetCount; ++i)
            {
                const VertexMorphTarget value = *((reinterpret_cast<const VertexMorphTarget*&>(morphTargetData)) + i);
                vertexMorphTarget.push_back(value);
            }

            meshRendererMorphTarget = vertexMorphTarget;
        }

        json meshRendererJSON = json
        {
            { "indicesCount", indexCount },
            { "indices", meshRendererIndices },
            { "vertexLayout", vertexLayout },
            { "vertexCount", vertexCount },
            { "vertex", meshRendererVertices },
            { "usingMorphTarget", isUsingMorphTarget },
            { "morphTarget", meshRendererMorphTarget },
        };

        _json = meshRendererJSON;

    }


    void NodeToJSON(json& _json, Entity& _inData)
    {
        //
        //
        // MESHES
		ionVector<json, SerializerAllocator, GetAllocator> meshesJson;
        const ionU32 meshCount = _inData.GetMeshCount();
        for (ionU32 i = 0; i < meshCount; ++i)
        {
            const Mesh* mesh = _inData.GetMesh(i);
            json meshJson = json
            {
                { "indexMesh", i },
                { "indexStart", mesh->GetIndexStart() },
                { "indexCount", mesh->GetIndexCount() }
                // material missing for now into the serialization
            };

            meshesJson.push_back(meshJson);
        }

        //
        //
        // TRANSFORM
        json transformJson = json
        {
            { "position", _inData.GetTransform().GetPosition() },
            { "rotation", _inData.GetTransform().GetRotation() },
            { "scale", _inData.GetTransform().GetScale() }
        };
        
        //
        //
        // MORPH TARGET WEIGHTS
		ionVector<ionFloat, SerializerAllocator, GetAllocator> initialMorphTargetWeight;
        const ionU32 initialMorphTargetCount = _inData.GetInitialMorphTargetWeightCount();
        for (ionU32 i = 0; i < initialMorphTargetCount; ++i)
        {
            const ionFloat weight = _inData.GetInitialMorphTargetWeight(i);
            initialMorphTargetWeight.push_back(weight);
        }
        /*
        ionVector<ionFloat> morphTargetWeight;
        const ionU32 morphTargetCount = _inData.GetMorphTargetWeightCount();
        for (ionU32 i = 0; i < morphTargetCount; ++i)
        {
            const ionFloat weight = _inData.GetMorphTargetWeight(i);
            morphTargetWeight->push_back(weight);
        }
        */

        //
        //
        // ANIMATION
        json animationsJson;

        // if the serialization level is greater then mini (0) serialize also the animation
        if (_private::g_serializationLevel > 0)
        {
            if (const AnimationRenderer* animationRenderer = _inData.GetAnimationRenderer())
            {
				ionVector<json, SerializerAllocator, GetAllocator> animationJson;
                const ionSize animationCount = animationRenderer->GetAnimationCount();
                for (ionSize i = 0; i < animationCount; ++i)
                {
                    const Animation& animation = animationRenderer->GetAnimation((ionU32)i);

                    const ionFloat start = animation.GetStart();
                    const ionFloat end = animation.GetEnd();
                    const ionString& name = animation.GetName();
                    const ionSize hash = animation.GetHashName();

                    const ionVector<AnimationChannel, AnimationAllocator, Animation::GetAllocator>& channels = animation.GetChannels();
                    const ionVector<AnimationSampler, AnimationAllocator, Animation::GetAllocator>& samplers = animation.GetSamplers();

                    ionVector<AnimationChannel, AnimationAllocator, Animation::GetAllocator>::size_type channelsCount = channels.size();
                    ionVector<AnimationSampler, AnimationAllocator, Animation::GetAllocator>::size_type samplersCount = samplers.size();

					ionVector<json, SerializerAllocator, GetAllocator> channelsJson;
                    for (ionVector<AnimationChannel, AnimationAllocator, Animation::GetAllocator>::size_type j = 0; j < channelsCount; ++j)
                    {
                        const AnimationChannel& channel = channels[j];

                        const ionString& belongingNodeName = channel.GetNode()->GetName();
                        const ionString& belongingNodeUUID = channel.GetNode()->GetUUID().ToString();
                        const EAnimationPathType& animationPath = channel.GetPath();
                        const ionU32 samplerIndex = channel.GetSamplerIndex();

                        json channelJson = json
                        {
                            { "belongingNode", belongingNodeName.c_str() },
                            { "belongingNodeUUID", belongingNodeUUID.c_str() },
                            { "animationPath", animationPath },
                            { "samplerIndex", samplerIndex },
                        };

                        channelsJson.push_back(channelJson);
                    }

					ionVector<json, SerializerAllocator, GetAllocator> samplersJson;
                    for (ionVector<AnimationSampler, AnimationAllocator, Animation::GetAllocator>::size_type j = 0; j < samplersCount; ++j)
                    {
                        const AnimationSampler& sampler = samplers[j];

                        const EAnimationInterpolationType& interpolation = sampler.GetInterpolation();
                        const ionVector<ionFloat, AnimationSamplerAllocator, AnimationSampler::GetAllocator>& inputs = sampler.GetInputs();
                        const ionVector<Vector4, AnimationSamplerAllocator, AnimationSampler::GetAllocator>& linearPath = sampler.GetLinearPaths();
                        const ionVector<ionFloat, AnimationSamplerAllocator, AnimationSampler::GetAllocator>& morphTarget = sampler.GetMorphTargets();

						ionVector<ionFloat, SerializerAllocator, GetAllocator> _input;
                        ionVector<ionFloat, AnimationSamplerAllocator, AnimationSampler::GetAllocator>::size_type inputCount = inputs.size();
                        for (ionVector<ionFloat, AnimationSamplerAllocator, AnimationSampler::GetAllocator>::size_type a = 0; a < inputCount; ++a)
                        {
                            const ionFloat& v = inputs[a];
                            _input.push_back(v);
                        }

						ionVector<Vector4, SerializerAllocator, GetAllocator> _linearPath;
                        ionVector<Vector4, AnimationSamplerAllocator, AnimationSampler::GetAllocator>::size_type linearPathCount = linearPath.size();
                        for (ionVector<Vector4, AnimationSamplerAllocator, AnimationSampler::GetAllocator>::size_type a = 0; a < linearPathCount; ++a)
                        {
                            const Vector4& v = linearPath[a];
                            _linearPath.push_back(v);
                        }

						ionVector<ionFloat, SerializerAllocator, GetAllocator> _morphTarget;
                        ionVector<ionFloat, AnimationSamplerAllocator, AnimationSampler::GetAllocator>::size_type morphTargetCount = morphTarget.size();
                        for (ionVector<ionFloat, AnimationSamplerAllocator, AnimationSampler::GetAllocator>::size_type a = 0; a < morphTargetCount; ++a)
                        {
                            const ionFloat& v = morphTarget[a];
                            _morphTarget.push_back(v);
                        }


                        json samplerJson = json
                        {
                            { "interpolationType", interpolation },
                            { "inputCount", inputCount },
                            { "inputs", _input },
                            { "linearPathCount", linearPathCount },
                            { "linearPath", _linearPath },
                            { "morphTargetCount", morphTargetCount },
                            { "morphTarget", _morphTarget },
                        };

                        samplersJson.push_back(samplerJson);
                    }


                    //
                    json animationJson = json
                    {
                        { "name", name.c_str() },
                        { "hash", hash },
                        { "startAt", start },
                        { "endAt", end },
                        { "channelCount", channelsCount },
                        { "channels", channelsJson },
                        { "samplerCount", samplersCount },
                        { "samplers", samplersJson },
                    };

                    animationsJson.push_back(animationJson);
                }

                //
                animationsJson = json
                {
                    { "animationCount", animationCount },
                    { "animations", animationsJson },
                };
            }
        }

        //
        //
        // NODES
        json nodeObj;
        nodeObj = json
        {
            { "uuid", _inData.GetUUID().ToString().c_str() },
            { "name", _inData.GetName().c_str() },
            { "transform", transformJson },
            { "nodeType", _inData.GetNodeType() },
            { "renderLayer", _inData.GetRenderLayer() },
            { "meshCount", meshCount },
            { "meshes", meshesJson },
            { "initialMorphTargetCount", initialMorphTargetCount },
            { "initialMorphTargetWeight", initialMorphTargetWeight },
            //{ "morphTargetCount", morphTargetCount },
            //{ "morphTargetWeight", morphTargetWeight },
            { "animations", animationsJson },
            { "child", json() }
        };


        json& child = nodeObj.at("child");

        _inData.IterateAll(
            [&](Node* _node)
        {
            if (_node->GetNodeType() == ENodeType_Entity)
            {
                if (Entity* entityPtr = dynamic_cast<Entity*>(_node))
                {
                    _private::NodeToJSON(child, *entityPtr);
                }
            }
            else if (_node->GetNodeType() == ENodeType_Camera)
            {

            }
            else if (_node->GetNodeType() == ENodeType_DirectionalLight)
            {

            }
            else if (_node->GetNodeType() == ENodeType_PointLight)
            {

            }
            else if (_node->GetNodeType() == ENodeType_SpotLight)
            {

            }
            else if (_node->GetNodeType() == ENodeType_EmptyNode)
            {

            }
            // nothing else
        }
        );

        _json += nodeObj;
    }

    void FromJSON(const json& _json, Entity& _outData)
    {

    }
}

//////////////////////////////////////////////////////////////////////////

void to_json(json& _json, const Index& _input)
{
    _json = json
    {
        { "index", _input }
    };
}
void from_json(const json& _json, Index& _output)
{

}

void to_json(json& _json, const VertexPlain& _input)
{
    _json = json
    {
        { "position", _input.GetPosition() }
    };
}
void from_json(const json& _json, VertexPlain& _output)
{

}
void to_json(json& _json, const VertexColored& _input)
{
    _json = json
    {
        { "position", _input.GetPosition() },
        { "color", _input.GetColorV() }
    };
}
void from_json(const json& _json, VertexColored& _output)
{

}
void to_json(json& _json, const VertexNormal& _input)
{
    _json = json
    {
        { "position", _input.GetPosition() },
        { "normal", _input.GetNormal() }
    };
}
void from_json(const json& _json, VertexNormal& _output)
{

}
void to_json(json& _json, const VertexUV& _input)
{
    _json = json
    {
        { "position", _input.GetPosition() },
        { "texCoordU", _input.GetTexCoordU() },
        { "texCoordV", _input.GetTexCoordV() }
    };
}
void from_json(const json& _json, VertexUV& _output)
{

}
void to_json(json& _json, const VertexSimple& _input)
{
    _json = json
    {
        { "position", _input.GetPosition() },
        { "normal", _input.GetNormal() },
        { "texCoordU", _input.GetTexCoordU() },
        { "texCoordV", _input.GetTexCoordV() }
    };
}
void from_json(const json& _json, VertexSimple& _output)
{

}
void to_json(json& _json, const Vertex& _input)
{
    _json = json
    {
        { "position", _input.GetPosition() },
        { "normal", _input.GetNormal() },
        { "color", _input.GetColorV() },
        { "tangent", _input.GetTangent() },
        { "bitangent", _input.GetBiTangent() },
        { "bitangentSign", _input.GetBiTangentSign() },
        { "weights", _input.GetWeightsV() },
        { "joint0", _input.GetJoint0() },
        { "joint1", _input.GetJoint1() },
        { "joint2", _input.GetJoint2() },
        { "joint3", _input.GetJoint3() },
        { "texCoordU0", _input.GetTexCoordU0() },
        { "texCoordV0", _input.GetTexCoordV0() },
        { "texCoordU1", _input.GetTexCoordU1() },
        { "texCoordV1", _input.GetTexCoordV1() }
    };
}
void from_json(const json& _json, Vertex& _output)
{

}
void to_json(json& _json, const VertexMorphTarget& _input)
{
    _json = json
    {
        { "position", _input.GetPosition() },
        { "normal", _input.GetNormal() },
        { "tangent", _input.GetTangent() }
    };
}
void from_json(const json& _json, VertexMorphTarget& _output)
{

}
//////////////////////////////////////////////////////////////////////////

void to_json(json& _json, Node* _input)
{
    json rendererJSON;
    json nodesJSON;

    if (_input->GetNodeType() == ENodeType_Entity)
    {
        if (Entity* entityPtr = dynamic_cast<Entity*>(_input))
        {
            // if the serialization level is the max I serialize the vertex either
            if (_private::g_serializationLevel > 1)
            {
                // this exist only in the root node of type ENodeType_Entity
                if (const BaseMeshRenderer* meshRenderer = entityPtr->GetMeshRenderer())
                {
                    _private::RendererToJSON(rendererJSON, meshRenderer);
                }
            }
            _private::NodeToJSON(nodesJSON, *entityPtr);
        }
    }
    else if (_input->GetNodeType() == ENodeType_Camera)
    {

    }
    else if (_input->GetNodeType() == ENodeType_DirectionalLight)
    {

    }
    else if (_input->GetNodeType() == ENodeType_PointLight)
    {

    }
    else if (_input->GetNodeType() == ENodeType_SpotLight)
    {

    }
    else if (_input->GetNodeType() == ENodeType_EmptyNode)
    {

    }

    _json = json
    {
        { "renderer", rendererJSON },
        { "nodes", nodesJSON },
    };
}

void from_json(const json& _json, Node*& _output)
{

}


std::string Serialize(Node* _input, ionU32 _level /*= 1*/)
{
    _level = _level > 2 ? 1 : _level;

    _private::g_serializationLevel = _level;

    json j(_input);
    std::string jsonArray = j.dump();

    _private::g_serializationLevel = 1;

    return jsonArray;
}


ION_NAMESPACE_END