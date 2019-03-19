#include "Serializer.h"

#include "../Dependencies/Miscellaneous/json.hpp"

#include "../Renderer/RenderCommon.h"

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
        // NODES
        json mainObj;
        mainObj = json
        {
            { "uuid", _inData.GetUUID().ToString().c_str() },
            { "name", _inData.GetName().c_str() },
            { "nodeType", _inData.GetNodeType() },
            { "renderLayer", _inData.GetRenderLayer() },
            { "meshCount", meshCount },
            { "meshes", meshesJson },
            { "child", json() }

        };


        json& child = mainObj.at("child");

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

        _json += mainObj;
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