#include "Serializer.h"

#include "../Dependencies/Miscellaneous/json.hpp"

#include "../Renderer/RenderCommon.h"
#include "../Animation/AnimationRenderer.h"

using nlohmann::json;

ION_NAMESPACE_BEGIN

namespace _private
{
    void RendererToJSON(json& _json, const BaseMeshRenderer* _inMeshRenderer)
    {
        json meshRendererIndices;
        json meshRendererVertices;

        std::vector<Index> indexes;
        
        const ionU32 indexCount = _inMeshRenderer->GetIndexDataCount();
        const void* indexData = _inMeshRenderer->GetIndexData();
        //const ionSize indexSize = _inMeshRenderer->GetSizeOfIndex();

        for (ionU32 i = 0; i < indexCount; ++i)
        {
            const Index value = *((reinterpret_cast<const Index*&>(indexData)) + i);
            indexes.push_back(value);
        }

        meshRendererIndices = indexes;

        std::vector<VertexPlain> verticesPlain;
        std::vector<VertexColored> verticesColored;
        std::vector<VertexNormal> verticesNormal;
        std::vector<VertexUV> verticesUV;
        std::vector<VertexSimple> verticesSimple;
        std::vector<Vertex> verticesFull;

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
            std::vector<VertexMorphTarget> vertexMorphTarget;

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
        std::vector<json> meshesJson;
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
        std::vector<ionFloat> initialMorphTargetWeight;
        const ionU32 initialMorphTargetCount = _inData.GetInitialMorphTargetWeightCount();
        for (ionU32 i = 0; i < initialMorphTargetCount; ++i)
        {
            const ionFloat weight = _inData.GetInitialMorphTargetWeight(i);
            initialMorphTargetWeight.push_back(weight);
        }

        std::vector<ionFloat> morphTargetWeight;
        const ionU32 morphTargetCount = _inData.GetMorphTargetWeightCount();
        for (ionU32 i = 0; i < morphTargetCount; ++i)
        {
            const ionFloat weight = _inData.GetMorphTargetWeight(i);
            morphTargetWeight.push_back(weight);
        }

        //
        //
        // ANIMATION
        json animationsJson;
        if (const AnimationRenderer* animationRenderer = _inData.GetAnimationRenderer())
        {
            std::vector<json> animationJson;
            const ionSize animationCount = animationRenderer->GetAnimationCount();
            for (ionSize i = 0; i < animationCount; ++i)
            {
                const Animation& animation = animationRenderer->GetAnimation((ionU32)i);

                const ionFloat start = animation.GetStart();
                const ionFloat end = animation.GetEnd();
                const eosString& name = animation.GetName();
                const eosSize hash = animation.GetHashName();

                const eosVector(AnimationChannel)& channels = animation.GetChannels();
                const eosVector(AnimationSampler)& samplers = animation.GetSamplers();

                eosVector(AnimationChannel)::size_type channelsCount = channels.size();
                eosVector(AnimationSampler)::size_type samplersCount = samplers.size();

                std::vector<json> channelsJson;
                for (eosVector(AnimationChannel)::size_type j = 0; j < channelsCount; ++j)
                {
                    const AnimationChannel& channel = channels[j];

                    const eosString& belongingNodeName = channel.GetNode()->GetName();
                    const eosString& belongingNodeUUID = channel.GetNode()->GetUUID().ToString();
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

                std::vector<json> samplersJson;
                for (eosVector(AnimationSampler)::size_type j = 0; j < samplersCount; ++j)
                {
                    const AnimationSampler& sampler = samplers[j];

                    const EAnimationInterpolationType& interpolation = sampler.GetInterpolation();
                    const eosVector(ionFloat)& inputs = sampler.GetInputs();
                    const eosVector(Vector)& linearPath = sampler.GetLinearPaths();
                    const eosVector(ionFloat)& morphTarget = sampler.GetMorphTargets();

                    std::vector<ionFloat> _input;
                    eosVector(ionFloat)::size_type inputCount = inputs.size();
                    for (eosVector(ionFloat)::size_type a = 0; a < inputCount; ++a)
                    {
                        const ionFloat& v = inputs[a];
                        _input.push_back(v);
                    }

                    std::vector<Vector> _linearPath;
                    eosVector(Vector)::size_type linearPathCount = linearPath.size();
                    for (eosVector(Vector)::size_type a = 0; a < linearPathCount; ++a)
                    {
                        const Vector& v = linearPath[a];
                        _linearPath.push_back(v);
                    }

                    std::vector<ionFloat> _morphTarget;
                    eosVector(ionFloat)::size_type morphTargetCount = morphTarget.size();
                    for (eosVector(ionFloat)::size_type a = 0; a < morphTargetCount; ++a)
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
            { "morphTargetCount", morphTargetCount },
            { "morphTargetWeight", morphTargetWeight },
            { "animations", animationsJson },
            { "child", json() }
        };


        json& child = nodeObj.at("child");

        _inData.IterateAll(
            [&](const ObjectHandler& _node)
        {
            if (_node->GetNodeType() == ENodeType_Entity)
            {
                if (Entity* entityPtr = dynamic_cast<Entity*>(_node.GetPtr()))
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

void to_json(json& _json, const ObjectHandler& _input)
{
    json rendererJSON;
    json nodesJSON;

    if (_input->GetNodeType() == ENodeType_Entity)
    {
        if (Entity* entityPtr = dynamic_cast<Entity*>(_input.GetPtr()))
        {
            // this exist only in the root node of type ENodeType_Entity
            if (const BaseMeshRenderer* meshRenderer = entityPtr->GetMeshRenderer())
            {
                _private::RendererToJSON(rendererJSON, meshRenderer);
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

void from_json(const json& _json, ObjectHandler& _output)
{

}


std::string Serialize(const ObjectHandler& _input)
{
    json j(_input);
    std::string jsonArray = j.dump();
    return jsonArray;
}


ION_NAMESPACE_END