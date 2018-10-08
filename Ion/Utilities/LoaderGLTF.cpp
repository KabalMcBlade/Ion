#include "LoaderGLTF.h"

#include "../Scene/Entity.h"

#include "../Texture/TextureManager.h"
#include "../Material/MaterialManager.h"

#include "../Dependencies/Eos/Eos/Eos.h"
#include "../Dependencies/Nix/Nix/Nix.h"

#include "../Utilities/GeometryHelper.h"

#include "../Renderer/RenderCommon.h"
#include "../Renderer/RenderManager.h"
#include "../Shader/ShaderProgramManager.h"
#include "../Core/FileSystemManager.h"

#include "../Scene/Camera.h"

#include "../Geometry/MeshRenderer.h"
#include "../Geometry/Mesh.h"

#include "../Animation/AnimationRenderer.h"

#define TINYGLTF_IMPLEMENTATION
// #define TINYGLTF_NOEXCEPTION // optional. disable exception handling.
#include "../Dependencies/Miscellaneous/tiny_gltf.h"

EOS_USING_NAMESPACE
NIX_USING_NAMESPACE

ION_NAMESPACE_BEGIN

LoaderGLTF::LoaderGLTF()
{
}

LoaderGLTF::~LoaderGLTF()
{
}


void UpdateBoundingBox(const ObjectHandler& _node, BoundingBox& _mainBoundingBoxToUpdate)
{
    if (_node->GetNodeType() == ENodeType_Entity)
    {
        Entity* entity = dynamic_cast<Entity*>(_node.GetPtr());

        if (entity->GetBoundingBox()->IsValid())
        {
            _mainBoundingBoxToUpdate.Expande(*entity->GetBoundingBox());
        }
    }

    if (_node->GetChildren().empty())
    {
        return;
    }

    const eosVector(ObjectHandler)& children = _node->GetChildren();
    eosVector(ObjectHandler)::const_iterator begin = children.cbegin(), end = children.cend(), it = begin;
    for (; it != end; ++it)
    {
        ObjectHandler nh = (*it);
        UpdateBoundingBox(nh, _mainBoundingBoxToUpdate);
    }
}

// for some reasons, tinygltf must be declared in source file: I was unable to declare any of its structures in header file
void LoadNode(const tinygltf::Node& _node, const tinygltf::Model& _model, MeshRenderer* _meshRenderer, ObjectHandler& _entityHandle, eosMap(ionU32, Node*)& _nodeIndexToNodePointer, eosMap(ionS32, eosString)& _textureIndexToTextureName, eosMap(ionS32, eosString)& _materialIndexToMaterialName, ionBool _generateNormalWhenMissing, ionBool _generateTangentWhenMissing, ionBool _setBitangentSign)
{
    Vector position(0.0f, 0.0f, 0.0f, 1.0f);
    Quaternion rotation;
    Vector scale(1.0f);

    if (_node.matrix.size() == 16)
    {
        ionFloat scaleFactor = std::sqrtf((ionFloat)_node.matrix.data()[0] * (ionFloat)_node.matrix.data()[0] + (ionFloat)_node.matrix.data()[1] * (ionFloat)_node.matrix.data()[1] + (ionFloat)_node.matrix.data()[2] * (ionFloat)_node.matrix.data()[2]);
        scale = VectorHelper::Set(scaleFactor, scaleFactor, scaleFactor, 1.0f);

        Matrix mat = Matrix(
            (ionFloat)_node.matrix.data()[0], (ionFloat)_node.matrix.data()[1], (ionFloat)_node.matrix.data()[2], (ionFloat)_node.matrix.data()[3],
            (ionFloat)_node.matrix.data()[4], (ionFloat)_node.matrix.data()[5], (ionFloat)_node.matrix.data()[6], (ionFloat)_node.matrix.data()[7],
            (ionFloat)_node.matrix.data()[8], (ionFloat)_node.matrix.data()[9], (ionFloat)_node.matrix.data()[10], (ionFloat)_node.matrix.data()[11],
            (ionFloat)_node.matrix.data()[12], (ionFloat)_node.matrix.data()[13], (ionFloat)_node.matrix.data()[14], (ionFloat)_node.matrix.data()[15]);

        rotation.SetFromMatrix(mat);

        position = mat[3];
    }
    else 
    {   
        if (_node.translation.size() == 3)
        {
            position = Vector((ionFloat)_node.translation[0], (ionFloat)_node.translation[1], (ionFloat)_node.translation[2], 1.0f);
        }

        if (_node.rotation.size() == 4)
        {
            rotation = Quaternion((ionFloat)_node.rotation[0], (ionFloat)_node.rotation[1], (ionFloat)_node.rotation[2], (ionFloat)_node.rotation[3]);
        }

        if (_node.scale.size() == 3)
        {
            scale = Vector((ionFloat)_node.scale[0], (ionFloat)_node.scale[1], (ionFloat)_node.scale[2]);
        }
    }
    
    _entityHandle->GetTransform().SetPosition(position);
    _entityHandle->GetTransform().SetRotation(rotation);
    _entityHandle->GetTransform().SetScale(scale);

    const Matrix& localTransform = _entityHandle->GetTransform().GetMatrix();

    //
    //  This one is the "NodeIndex" use to track the animation nodes
    //
    //  _node.children[i] 
    //

    // calculate matrix for all children if any
    for (ionSize i = 0; i < _node.children.size(); ++i)
    {
        Entity* child = eosNew(Entity, ION_MEMORY_ALIGNMENT_SIZE, _model.nodes[_node.children[i]].name.c_str());
        ObjectHandler childHandle(child);
        child->AttachToParent(_entityHandle);

        _nodeIndexToNodePointer.insert(std::pair<ionU32, Node*>((ionU32)_node.children[i], child));
        LoadNode(_model.nodes[_node.children[i]], _model, _meshRenderer, childHandle, _nodeIndexToNodePointer, _textureIndexToTextureName, _materialIndexToMaterialName, _generateNormalWhenMissing, _generateTangentWhenMissing, _setBitangentSign);
    }
    
    if (_node.mesh > -1) 
    {
        const tinygltf::Mesh mesh = _model.meshes[_node.mesh];

        Entity* entityPtr = dynamic_cast<Entity*>(_entityHandle.GetPtr());

        ionBool usingMorphTarget = false;
        ionBool usingSkinningMesh = false;

        // load weights if any!
        // the weights here are for the morph target animation, not for the skinning animation!
        // the morph target are per mesh/node: first check node, because node override the mesh.
        if (_node.weights.size() > 0)
        {
            usingMorphTarget = true;
            ionSize weightCount = _node.weights.size();
            for (ionSize i = 0; i < weightCount; ++i)
            {
                ionFloat weight = static_cast<ionFloat>(_node.weights[i]);
                entityPtr->PushBackInitialMorphTargetWeight(weight);
            }
        }
        else
        {
            if (mesh.weights.size() > 0)
            {
                usingMorphTarget = true;
                ionSize weightCount = mesh.weights.size();
                for (ionSize i = 0; i < weightCount; ++i)
                {
                    ionFloat weight = static_cast<ionFloat>(mesh.weights[i]);
                    entityPtr->PushBackInitialMorphTargetWeight(weight);
                }
            }
        }

        for (ionSize j = 0; j < mesh.primitives.size(); ++j)
        {
            const tinygltf::Primitive &primitive = mesh.primitives[j];
            if (primitive.indices < 0)
            {
                continue;
            }

            Mesh ionMesh;
            ionMesh.SetIndexStart(_meshRenderer->GetIndexDataCount());
            ionU32 vertexStart = _meshRenderer->GetVertexDataCount();       // 0?

            eosVector(Vector) positionToBeNormalized;
            eosVector(Vector) normalForTangent;
            eosVector(Vector) uvuvForTangents;

            // Vertices
            {
                const ionFloat *bufferPos = nullptr;
                const ionFloat *bufferNormals = nullptr;
                const ionFloat *bufferTangent = nullptr;

                const ionFloat *bufferTexCoordsFloat0 = nullptr;
                const ionU16 *bufferTexCoordsU160 = nullptr;
                const ionU8 *bufferTexCoordsU80 = nullptr;
    
                const ionFloat *bufferTexCoordsFloat1 = nullptr;
                const ionU16 *bufferTexCoordsU161 = nullptr;
                const ionU8 *bufferTexCoordsU81 = nullptr;

                const ionFloat *bufferColorFloat = nullptr;
                const ionU16 *bufferColorU16 = nullptr;
                const ionU8 *bufferColorU8 = nullptr;

                const ionFloat *bufferWeightsFloat = nullptr;
                const ionU16 *bufferWeightsU16 = nullptr;
                const ionU8 *bufferWeightsU8 = nullptr;

                const ionU16 *bufferJointsU16 = nullptr;
                const ionU8 *bufferJointsU8 = nullptr;

                // Position attribute is required
                ionAssertReturnVoid(primitive.attributes.find("POSITION") != primitive.attributes.end(), "POSITION ATTRIBUTE MISSING!");

                const tinygltf::Accessor &posAccessor = _model.accessors[primitive.attributes.find("POSITION")->second];
                const tinygltf::BufferView &posView = _model.bufferViews[posAccessor.bufferView];
                bufferPos = reinterpret_cast<const ionFloat *>(&(_model.buffers[posView.buffer].data[posAccessor.byteOffset + posView.byteOffset]));

                if (primitive.attributes.find("NORMAL") != primitive.attributes.end()) 
                {
                    const tinygltf::Accessor &normAccessor = _model.accessors[primitive.attributes.find("NORMAL")->second];
                    const tinygltf::BufferView &normView = _model.bufferViews[normAccessor.bufferView];
                    bufferNormals = reinterpret_cast<const ionFloat *>(&(_model.buffers[normView.buffer].data[normAccessor.byteOffset + normView.byteOffset]));

                    _generateNormalWhenMissing = false;
                }

                if (primitive.attributes.find("TANGENT") != primitive.attributes.end())
                {
                    const tinygltf::Accessor &tangentAccessor = _model.accessors[primitive.attributes.find("TANGENT")->second];
                    const tinygltf::BufferView &tangentView = _model.bufferViews[tangentAccessor.bufferView];
                    bufferTangent = reinterpret_cast<const ionFloat *>(&(_model.buffers[tangentView.buffer].data[tangentAccessor.byteOffset + tangentView.byteOffset]));

                    _generateTangentWhenMissing = false;
                }

                if (usingMorphTarget)
                {
                    eosVector(const ionFloat*) morphTargetBufferPos;
                    eosVector(const ionFloat*) morphTargetBufferNormals;
                    eosVector(const ionFloat*) morphTargetBufferTangent;

                    eosVector(ionSize) accessorCounts;
                    const ionSize morphTargetCount = primitive.targets.size();
                    for (ionSize t = 0; t < morphTargetCount; ++t)
                    {
                        if (primitive.targets[t].find("POSITION") != primitive.targets[t].end())
                        {
                            const tinygltf::Accessor &posAccessor = _model.accessors[primitive.targets[t].find("POSITION")->second];
                            const tinygltf::BufferView &posView = _model.bufferViews[posAccessor.bufferView];
                            morphTargetBufferPos.push_back(reinterpret_cast<const ionFloat *>(&(_model.buffers[posView.buffer].data[posAccessor.byteOffset + posView.byteOffset])));
                            accessorCounts.push_back(posAccessor.count);
                        }
                    }

                    for (size_t t = 0; t < primitive.targets.size(); ++t) 
                    {
                        if (primitive.targets[t].find("NORMAL") != primitive.targets[t].end()) 
                        {
                            const tinygltf::Accessor &normAccessor = _model.accessors[primitive.targets[t].find("NORMAL")->second];
                            const tinygltf::BufferView &normView = _model.bufferViews[normAccessor.bufferView];
                            morphTargetBufferNormals.push_back(reinterpret_cast<const ionFloat *>(&(_model.buffers[normView.buffer].data[normAccessor.byteOffset + normView.byteOffset])));
                        }
                    }

                    for (size_t t = 0; t < primitive.targets.size(); ++t)
                    {
                        if (primitive.targets[t].find("TANGENT") != primitive.targets[t].end()) 
                        {
                            const tinygltf::Accessor &tangentAccessor = _model.accessors[primitive.targets[t].find("TANGENT")->second];
                            const tinygltf::BufferView &tangentView = _model.bufferViews[tangentAccessor.bufferView];
                            morphTargetBufferTangent.push_back(reinterpret_cast<const ionFloat *>(&(_model.buffers[tangentView.buffer].data[tangentAccessor.byteOffset + tangentView.byteOffset])));
                        }
                    }

                    for (ionSize t = 0; t < morphTargetCount; ++t)
                    {
                        const ionSize& count = accessorCounts[t];
                        for (ionSize v = 0; v < count; ++v)
                        {
                            VertexMorphTarget vert;

                            Vector pos((&morphTargetBufferPos[t][v * 3])[0], ((&morphTargetBufferPos[t][v * 3])[1]), (&morphTargetBufferPos[t][v * 3])[2], 1.0f);
                            vert.SetPosition(pos);

                            Vector normal;
                            if (morphTargetBufferNormals.size() > 0)
                            {
                                normal = VectorHelper::Set((&morphTargetBufferNormals[t][v * 3])[0], ((&morphTargetBufferNormals[t][v * 3])[1]), (&morphTargetBufferNormals[t][v * 3])[2], 1.0f);
                            }
                            else
                            {
                                normal = VectorHelper::Set(0.0f, 0.0f, 0.0f, 1.0f);
                            }
                            vert.SetNormal(normal);

                            Vector tangent;
                            if (morphTargetBufferTangent.size() > 0)
                            {
                                tangent = VectorHelper::Set((&morphTargetBufferTangent[t][v * 3])[0], ((&morphTargetBufferTangent[t][v * 3])[1]), (&morphTargetBufferTangent[t][v * 3])[2], 1.0f);
                            }
                            else
                            {
                                tangent = VectorHelper::Set(0.0f, 0.0f, 0.0f, 1.0f);
                            }

                            vert.SetTangent(tangent);

                            _meshRenderer->PushBackMorphTarget(vert);
                        }
                    }

                    // TODO: FINISH HERE
                }

                if (primitive.attributes.find("TEXCOORD_0") != primitive.attributes.end()) 
                {
                    const tinygltf::Accessor &uvAccessor = _model.accessors[primitive.attributes.find("TEXCOORD_0")->second];
                    const tinygltf::BufferView &uvView = _model.bufferViews[uvAccessor.bufferView];

                    switch (uvAccessor.componentType)
                    {
                    case TINYGLTF_PARAMETER_TYPE_FLOAT:
                    {
                        bufferTexCoordsFloat0 = reinterpret_cast<const ionFloat *>(&(_model.buffers[uvView.buffer].data[uvAccessor.byteOffset + uvView.byteOffset]));
                        break;
                    }

                    case TINYGLTF_PARAMETER_TYPE_UNSIGNED_SHORT:
                    {
                        bufferTexCoordsU160 = reinterpret_cast<const ionU16 *>(&(_model.buffers[uvView.buffer].data[uvAccessor.byteOffset + uvView.byteOffset]));
                        break;
                    }

                    case TINYGLTF_PARAMETER_TYPE_UNSIGNED_BYTE:
                    {
                        bufferTexCoordsU80 = reinterpret_cast<const ionU8 *>(&(_model.buffers[uvView.buffer].data[uvAccessor.byteOffset + uvView.byteOffset]));
                        break;
                    }

                    default:
                        ionAssertReturnVoid(false, "Component type is not supported!");
                    }
                }

                if (primitive.attributes.find("TEXCOORD_1") != primitive.attributes.end())
                {
                    const tinygltf::Accessor &uvAccessor = _model.accessors[primitive.attributes.find("TEXCOORD_1")->second];
                    const tinygltf::BufferView &uvView = _model.bufferViews[uvAccessor.bufferView];

                    switch (uvAccessor.componentType)
                    {
                    case TINYGLTF_PARAMETER_TYPE_FLOAT:
                    {
                        bufferTexCoordsFloat1 = reinterpret_cast<const ionFloat *>(&(_model.buffers[uvView.buffer].data[uvAccessor.byteOffset + uvView.byteOffset]));
                        break;
                    }

                    case TINYGLTF_PARAMETER_TYPE_UNSIGNED_SHORT:
                    {
                        bufferTexCoordsU161 = reinterpret_cast<const ionU16 *>(&(_model.buffers[uvView.buffer].data[uvAccessor.byteOffset + uvView.byteOffset]));
                        break;
                    }

                    case TINYGLTF_PARAMETER_TYPE_UNSIGNED_BYTE:
                    {
                        bufferTexCoordsU81 = reinterpret_cast<const ionU8 *>(&(_model.buffers[uvView.buffer].data[uvAccessor.byteOffset + uvView.byteOffset]));
                        break;
                    }

                    default:
                        ionAssertReturnVoid(false, "Component type is not supported!");
                    }
                }

                if (primitive.attributes.find("COLOR_0") != primitive.attributes.end())
                {
                    const tinygltf::Accessor &colorAccessor = _model.accessors[primitive.attributes.find("COLOR_0")->second];
                    const tinygltf::BufferView &colorView = _model.bufferViews[colorAccessor.bufferView];

                    switch (colorAccessor.componentType)
                    {
                    case TINYGLTF_PARAMETER_TYPE_FLOAT:
                    {
                        bufferColorFloat = reinterpret_cast<const ionFloat *>(&(_model.buffers[colorView.buffer].data[colorAccessor.byteOffset + colorView.byteOffset]));
                        break;
                    }

                    case TINYGLTF_PARAMETER_TYPE_UNSIGNED_SHORT:
                    {
                        bufferColorU16 = reinterpret_cast<const ionU16 *>(&(_model.buffers[colorView.buffer].data[colorAccessor.byteOffset + colorView.byteOffset]));
                        break;
                    }

                    case TINYGLTF_PARAMETER_TYPE_UNSIGNED_BYTE:
                    {
                        bufferColorU8 = reinterpret_cast<const ionU8 *>(&(_model.buffers[colorView.buffer].data[colorAccessor.byteOffset + colorView.byteOffset]));
                        break;
                    }

                    default:
                        ionAssertReturnVoid(false, "Component type is not supported!");
                    }
                }

                if (primitive.attributes.find("WEIGHTS_0") != primitive.attributes.end())
                {
                    usingSkinningMesh = true;

                    const tinygltf::Accessor &weightsAccessor = _model.accessors[primitive.attributes.find("WEIGHTS_0")->second];
                    const tinygltf::BufferView &weightsView = _model.bufferViews[weightsAccessor.bufferView];

                    switch (weightsAccessor.componentType)
                    {
                    case TINYGLTF_PARAMETER_TYPE_FLOAT:
                    {
                        bufferWeightsFloat = reinterpret_cast<const ionFloat *>(&(_model.buffers[weightsView.buffer].data[weightsAccessor.byteOffset + weightsView.byteOffset]));
                        break;
                    }

                    case TINYGLTF_PARAMETER_TYPE_UNSIGNED_SHORT:
                    {
                        bufferWeightsU16 = reinterpret_cast<const ionU16 *>(&(_model.buffers[weightsView.buffer].data[weightsAccessor.byteOffset + weightsView.byteOffset]));
                        break;
                    }

                    case TINYGLTF_PARAMETER_TYPE_UNSIGNED_BYTE:
                    {
                        bufferWeightsU8 = reinterpret_cast<const ionU8 *>(&(_model.buffers[weightsView.buffer].data[weightsAccessor.byteOffset + weightsView.byteOffset]));
                        break;
                    }

                    default:
                        ionAssertReturnVoid(false, "Component type is not supported!");
                    }
                }

                if (primitive.attributes.find("JOINTS_0") != primitive.attributes.end())
                {
                    const tinygltf::Accessor &jointsAccessor = _model.accessors[primitive.attributes.find("JOINTS_0")->second];
                    const tinygltf::BufferView &jointsView = _model.bufferViews[jointsAccessor.bufferView];

                    switch (jointsAccessor.componentType)
                    {
                    case TINYGLTF_PARAMETER_TYPE_UNSIGNED_SHORT:
                    {
                        bufferJointsU16 = reinterpret_cast<const ionU16 *>(&(_model.buffers[jointsView.buffer].data[jointsAccessor.byteOffset + jointsView.byteOffset]));
                        break;
                    }

                    case TINYGLTF_PARAMETER_TYPE_UNSIGNED_BYTE:
                    {
                        bufferJointsU8 = reinterpret_cast<const ionU8 *>(&(_model.buffers[jointsView.buffer].data[jointsAccessor.byteOffset + jointsView.byteOffset]));
                        break;
                    }

                    default:
                        ionAssertReturnVoid(false, "Component type is not supported!");
                    } 
                }
                else
                {
                    // for skinning mesh need weights and joints!
                    usingSkinningMesh = false;
                }

                for (ionSize v = 0; v < posAccessor.count; v++) 
                {
                    // the Y value of all vectors is inverted due Vulkan coordinate system

                    Vertex vert;

                    Vector pos((&bufferPos[v * 3])[0], ((&bufferPos[v * 3])[1]), (&bufferPos[v * 3])[2], 1.0f);

                    if (_generateNormalWhenMissing || _generateTangentWhenMissing)
                    {
                        positionToBeNormalized.push_back(pos);
                    }
                    pos = localTransform * pos;
                    vert.SetPosition(pos);

                    _entityHandle->GetBoundingBox()->Expande(vert.GetPosition(), vert.GetPosition());
                    
                    Vector normal;
                    if (bufferNormals != nullptr)
                    {
                        normal = VectorHelper::Set((&bufferNormals[v * 3])[0], ((&bufferNormals[v * 3])[1]), (&bufferNormals[v * 3])[2], 1.0f);

                        if (_generateTangentWhenMissing)
                        {
                            normalForTangent.push_back(normal);
                        }
                    }
                    else
                    {
                        normal = VectorHelper::Set(0.0f, 0.0f, 0.0f, 1.0f);

                        // if no normal, after all iteration the normalToBeTangent will be empty, so we will know how to do
                    }
                    normal = localTransform * normal;
                    vert.SetNormal(normal);

                    Vector tangent;
                    if (bufferTangent != nullptr)
                    {
                        tangent = VectorHelper::Set((&bufferTangent[v * 3])[0], ((&bufferTangent[v * 3])[1]), (&bufferTangent[v * 3])[2], 1.0f);
                    }
                    else
                    {
                        tangent = VectorHelper::Set(0.0f, 0.0f, 0.0f, 1.0f);
                    }
                    vert.SetTangent(tangent);

                    if (bufferTexCoordsFloat0 != nullptr)
                    {
                        if (_generateTangentWhenMissing)
                        {
                            Vector uvuv = VectorHelper::Set((&bufferTexCoordsFloat0[v * 2])[0], (&bufferTexCoordsFloat0[v * 2])[1], (&bufferTexCoordsFloat0[v * 2])[0], (&bufferTexCoordsFloat0[v * 2])[1]);
                            uvuvForTangents.push_back(uvuv);
                        }

                        vert.SetTexCoordUV0((&bufferTexCoordsFloat0[v * 2])[0], (&bufferTexCoordsFloat0[v * 2])[1]);
                    }
                    else
                    {
                        if (bufferTexCoordsU160 != nullptr)
                        {
                            if (_generateTangentWhenMissing)
                            {
                                Vector uvuv = VectorHelper::Set(ionFloat((&bufferTexCoordsU160[v * 2])[0]) / 65535.0f, ionFloat((&bufferTexCoordsU160[v * 2])[1]) / 65535.0f, ionFloat((&bufferTexCoordsU160[v * 2])[0]) / 65535.0f, ionFloat((&bufferTexCoordsU160[v * 2])[1]) / 65535.0f);
                                uvuvForTangents.push_back(uvuv);
                            }

                            vert.SetTexCoordUV0(ionFloat((&bufferTexCoordsU160[v * 2])[0]) / 65535.0f, ionFloat((&bufferTexCoordsU160[v * 2])[1]) / 65535.0f);
                        }
                        else
                        {
                            if(bufferTexCoordsU80 != nullptr)
                            {
                                if (_generateTangentWhenMissing)
                                {
                                    Vector uvuv = VectorHelper::Set(ionFloat((&bufferTexCoordsU80[v * 2])[0]) / 255.0f, ionFloat((&bufferTexCoordsU80[v * 2])[1]) / 255.0f, ionFloat((&bufferTexCoordsU80[v * 2])[0]) / 255.0f, ionFloat((&bufferTexCoordsU80[v * 2])[1]) / 255.0f);
                                    uvuvForTangents.push_back(uvuv);
                                }

                                vert.SetTexCoordUV0(ionFloat((&bufferTexCoordsU80[v * 2])[0]) / 255.0f, ionFloat((&bufferTexCoordsU80[v * 2])[1]) / 255.0f);
                            }
                            else
                            {
                                vert.SetTexCoordUV0(0.0f, 0.0f);
                            }
                        }
                    }
        
                    if (bufferTexCoordsFloat1 != nullptr)
                    {
                        vert.SetTexCoordUV1((&bufferTexCoordsFloat1[v * 2])[0], (&bufferTexCoordsFloat1[v * 2])[1]);
                    }
                    else
                    {
                        if (bufferTexCoordsU161 != nullptr)
                        {
                            vert.SetTexCoordUV1(ionFloat((&bufferTexCoordsU161[v * 2])[0]) / 65535.0f, ionFloat((&bufferTexCoordsU161[v * 2])[1]) / 65535.0f);
                        }
                        else
                        {
                            if (bufferTexCoordsU81 != nullptr)
                            {
                                vert.SetTexCoordUV1(ionFloat((&bufferTexCoordsU81[v * 2])[0]) / 255.0f, ionFloat((&bufferTexCoordsU81[v * 2])[1]) / 255.0f);
                            }
                            else
                            {
                                vert.SetTexCoordUV1(0.0f, 0.0f);
                            }
                        }
                    }
                    
                    if (bufferColorFloat != nullptr)
                    {
                        vert.SetColor((&bufferColorFloat[v * 4])[0], (&bufferColorFloat[v * 4])[1], (&bufferColorFloat[v * 4])[2], (&bufferColorFloat[v * 4])[3]);
                    }
                    else
                    {
                        if (bufferColorU16 != nullptr)
                        {
                            vert.SetColor(ionFloat((&bufferColorU16[v * 4])[0]) / 65535.0f, ionFloat((&bufferColorU16[v * 4])[1]) / 65535.0f, ionFloat((&bufferColorU16[v * 4])[2]) / 65535.0f, ionFloat((&bufferColorU16[v * 4])[3]) / 65535.0f);
                        }
                        else
                        {
                            if (bufferColorU8 != nullptr)
                            {
                                vert.SetColor(ionFloat((&bufferColorU8[v * 4])[0]) / 255.0f, ionFloat((&bufferColorU8[v * 4])[1]) / 255.0f, ionFloat((&bufferColorU8[v * 4])[2]) / 255.0f, ionFloat((&bufferColorU8[v * 4])[3]) / 255.0f);
                            }
                            else
                            {
                                vert.SetColor(1.0f, 1.0f, 1.0f, 1.0f);
                            }
                        }
                    }

                    if (bufferWeightsFloat != nullptr)
                    {
                        vert.SetWeights((&bufferWeightsFloat[v * 4])[0], (&bufferWeightsFloat[v * 4])[1], (&bufferWeightsFloat[v * 4])[2], (&bufferWeightsFloat[v * 4])[3]);
                    }
                    else
                    {
                        if (bufferWeightsU16 != nullptr)
                        {
                            vert.SetWeights(ionFloat((&bufferWeightsU16[v * 4])[0]) / 65535.0f, ionFloat((&bufferWeightsU16[v * 4])[1]) / 65535.0f, ionFloat((&bufferWeightsU16[v * 4])[2]) / 65535.0f, ionFloat((&bufferWeightsU16[v * 4])[3]) / 65535.0f);
                        }
                        else
                        {
                            if (bufferWeightsU8 != nullptr)
                            {
                                vert.SetWeights(ionFloat((&bufferWeightsU8[v * 4])[0]) / 255.0f, ionFloat((&bufferWeightsU8[v * 4])[1]) / 255.0f, ionFloat((&bufferWeightsU8[v * 4])[2]) / 255.0f, ionFloat((&bufferWeightsU8[v * 4])[3]) / 255.0f);
                            }
                            else
                            {
                                vert.SetWeights(1.0f, 1.0f, 1.0f, 1.0f);
                            }
                        }
                    }

                    if (bufferJointsU16 != nullptr)
                    {
                        vert.SetJoint((&bufferJointsU16[v * 4])[0], (&bufferJointsU16[v * 4])[1], (&bufferJointsU16[v * 4])[2], (&bufferJointsU16[v * 4])[3]);
                    }
                    else
                    {
                        if (bufferJointsU8 != nullptr)
                        {
                            vert.SetJoint((&bufferJointsU8[v * 4])[0], (&bufferJointsU8[v * 4])[1], (&bufferJointsU8[v * 4])[2], (&bufferJointsU8[v * 4])[3]);
                        }
                        else
                        {
                            vert.SetJoint(0, 0, 0, 0);
                        }
                    }

                    _meshRenderer->PushBackVertex(vert);
                }
            }


            eosVector(Index) indexToBeUsedDuringNormalization;

            // Indices
            {
                const tinygltf::Accessor &accessor = _model.accessors[primitive.indices];
                const tinygltf::BufferView &bufferView = _model.bufferViews[accessor.bufferView];
                const tinygltf::Buffer &buffer = _model.buffers[bufferView.buffer];

                ionMesh.SetIndexCount(static_cast<ionU32>(accessor.count));

                switch (accessor.componentType)
                {
                case TINYGLTF_PARAMETER_TYPE_UNSIGNED_INT:
                {
                    ionU32 *buf = (ionU32 *)eosNewRaw(sizeof(ionU32) * accessor.count, ION_MEMORY_ALIGNMENT_SIZE);
                    memcpy(buf, &buffer.data[accessor.byteOffset + bufferView.byteOffset], accessor.count * sizeof(ionU32));
                    for (ionSize index = 0; index < accessor.count; index++)
                    {
                        if (_generateNormalWhenMissing || _generateTangentWhenMissing)
                        {
                            indexToBeUsedDuringNormalization.push_back((Index)(buf[index] + vertexStart));
                        }
                        _meshRenderer->PushBackIndex((Index)(buf[index] + vertexStart));
                    }
                    eosDeleteRaw(buf);
                    break;
                }

                case TINYGLTF_PARAMETER_TYPE_UNSIGNED_SHORT:
                {
                    ionU16 *buf = (ionU16 *)eosNewRaw(sizeof(ionU16) * accessor.count, ION_MEMORY_ALIGNMENT_SIZE);
                    memcpy(buf, &buffer.data[accessor.byteOffset + bufferView.byteOffset], accessor.count * sizeof(ionU16));
                    for (ionSize index = 0; index < accessor.count; index++)
                    {
                        if (_generateNormalWhenMissing || _generateTangentWhenMissing)
                        {
                            indexToBeUsedDuringNormalization.push_back((Index)(buf[index] + vertexStart));
                        }
                        _meshRenderer->PushBackIndex(buf[index] + vertexStart);
                    }
                    eosDeleteRaw(buf);
                    break;
                }
                case TINYGLTF_PARAMETER_TYPE_UNSIGNED_BYTE: 
                {
                    ionU8 *buf = (ionU8 *)eosNewRaw(sizeof(ionU8) * accessor.count, ION_MEMORY_ALIGNMENT_SIZE);
                    memcpy(buf, &buffer.data[accessor.byteOffset + bufferView.byteOffset], accessor.count * sizeof(ionU8));
                    for (ionSize index = 0; index < accessor.count; index++)
                    {
                        if (_generateNormalWhenMissing || _generateTangentWhenMissing)
                        {
                            indexToBeUsedDuringNormalization.push_back((Index)(buf[index] + vertexStart));
                        }
                        _meshRenderer->PushBackIndex((Index)(buf[index] + vertexStart));
                    }
                    eosDeleteRaw(buf);
                    break;
                }
                default:
                    ionAssertReturnVoid(false, "Index component type is not supported!");
                }
            }


            // Write normal and tangent if they are missing
            eosVector(Vector) normalsGenerated;
            if (_generateNormalWhenMissing)
            {
                normalsGenerated.resize(positionToBeNormalized.size());
                GeometryHelper::CalculateNormalPerVertex(positionToBeNormalized.data(), indexToBeUsedDuringNormalization.data(), static_cast<ionU32>(indexToBeUsedDuringNormalization.size()), normalsGenerated.data());

                ionSize count = normalsGenerated.size();
                for (ionSize k = 0; k < count; ++k)
                {
                    _meshRenderer->GetVertex(k).SetNormal(normalsGenerated[k]);
                }
            }

            if (_generateTangentWhenMissing && !uvuvForTangents.empty())
            {
                eosVector(Vector) tangentsGenerated;

                // means we have generated and not get from model!
                if (normalForTangent.empty())
                {
                    tangentsGenerated.resize(positionToBeNormalized.size());
                    GeometryHelper::CalculateTangent(positionToBeNormalized.data(), normalsGenerated.data(), uvuvForTangents.data(), static_cast<ionU32>(positionToBeNormalized.size()), indexToBeUsedDuringNormalization.data(), static_cast<ionU32>(indexToBeUsedDuringNormalization.size()), tangentsGenerated.data());
                }
                else
                {
                    tangentsGenerated.resize(positionToBeNormalized.size());
                    GeometryHelper::CalculateTangent(positionToBeNormalized.data(), normalForTangent.data(), uvuvForTangents.data(), static_cast<ionU32>(positionToBeNormalized.size()), indexToBeUsedDuringNormalization.data(), static_cast<ionU32>(indexToBeUsedDuringNormalization.size()), tangentsGenerated.data());
                }

                ionSize count = tangentsGenerated.size();
                for (ionSize k = 0; k < count; ++k)
                {
                    _meshRenderer->GetVertex(k).SetTangent(tangentsGenerated[k]);
                    if (_setBitangentSign)
                    {
                        _meshRenderer->GetVertex(k).SetBiTangent(tangentsGenerated[k]);
                    }
                } 
            }

            // add material and add all to primitive
            if (_model.materials.size() > 0)
            {
                Material* material = ionMaterialManger().GetMaterial(_materialIndexToMaterialName[primitive.material]);

                switch (primitive.mode)
                {
                case TINYGLTF_MODE_POINTS:
                    material->SetTopology(VK_PRIMITIVE_TOPOLOGY_POINT_LIST);
                    break;
                case TINYGLTF_MODE_LINE:
                    material->SetTopology(VK_PRIMITIVE_TOPOLOGY_LINE_LIST);
                    break;
                case TINYGLTF_MODE_LINE_LOOP:
                    material->SetTopology(VK_PRIMITIVE_TOPOLOGY_LINE_STRIP);
                    break;
                case TINYGLTF_MODE_TRIANGLES:
                    material->SetTopology(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);
                    break;
                case TINYGLTF_MODE_TRIANGLE_STRIP:
                    material->SetTopology(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP);
                    break;
                case TINYGLTF_MODE_TRIANGLE_FAN:
                    material->SetTopology(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_FAN);
                    break;
                default:
                    material->SetTopology(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);
                    break;
                }

                if (material->IsValidPBR() || material->IsValidSpecularGlossiness())
                {
                    enum ESettingType
                    {
                        ESettingType_Base = 0,
                        ESettingType_Physical,
                        ESettingType_Normal,
                        ESettingType_Occlusion,
                        ESettingType_Emissive,

                        ESettingType_Count
                    };

                    ionFloat constantTexturesSettings[ESettingType_Count];
                    for (ionU32 constantCounter = 0; constantCounter < ESettingType_Count; ++constantCounter)
                    {
                        constantTexturesSettings[constantCounter] = 1.0f;
                    }

                    {
                        ionU32 bindingIndex = 0;

                        //
                        UniformBinding uniformVertex;
                        uniformVertex.m_bindingIndex = bindingIndex++;
                        uniformVertex.m_parameters.push_back(ION_MODEL_MATRIX_PARAM);
                        uniformVertex.m_type.push_back(EBufferParameterType_Matrix);
                        uniformVertex.m_parameters.push_back(ION_VIEW_MATRIX_PARAM);
                        uniformVertex.m_type.push_back(EBufferParameterType_Matrix);
                        uniformVertex.m_parameters.push_back(ION_PROJ_MATRIX_PARAM);
                        uniformVertex.m_type.push_back(EBufferParameterType_Matrix);

                        //
                        UniformBinding uniformFragment;
                        uniformFragment.m_bindingIndex = bindingIndex++;
                        uniformFragment.m_parameters.push_back(ION_MAIN_CAMERA_POSITION_VECTOR_PARAM);
                        uniformFragment.m_type.push_back(EBufferParameterType_Vector);
                        uniformFragment.m_parameters.push_back(ION_DIRECTIONAL_LIGHT_DIR_VECTOR_PARAM);
                        uniformFragment.m_type.push_back(EBufferParameterType_Vector);
                        uniformFragment.m_parameters.push_back(ION_DIRECTIONAL_LIGHT_COL_VECTOR_PARAM);
                        uniformFragment.m_type.push_back(EBufferParameterType_Vector);
                        uniformFragment.m_parameters.push_back(ION_EXPOSURE_FLOAT_PARAM);
                        uniformFragment.m_type.push_back(EBufferParameterType_Float);
                        uniformFragment.m_parameters.push_back(ION_GAMMA_FLOAT_PARAM);
                        uniformFragment.m_type.push_back(EBufferParameterType_Float);
                        uniformFragment.m_parameters.push_back(ION_PREFILTERED_CUBE_MIP_LEVELS_FLOAT_PARAM);
                        uniformFragment.m_type.push_back(EBufferParameterType_Float);

                        //
                        SamplerBinding samplerIrradiance;
                        samplerIrradiance.m_bindingIndex = bindingIndex++;
                        samplerIrradiance.m_texture = ionRenderManager().GetIrradianceCubemap();

                        SamplerBinding prefilteredMap;
                        prefilteredMap.m_bindingIndex = bindingIndex++;
                        prefilteredMap.m_texture = ionRenderManager().GetPrefilteredEnvironmentCubemap();

                        SamplerBinding samplerBRDFLUT;
                        samplerBRDFLUT.m_bindingIndex = bindingIndex++;
                        samplerBRDFLUT.m_texture = ionRenderManager().GetBRDF();

                        //
                        ionBool usingSpecularGlossiness = material->IsSpecularGlossiness();

                        //
                        SamplerBinding albedoMap;
                        albedoMap.m_bindingIndex = bindingIndex++;
                        albedoMap.m_texture = usingSpecularGlossiness ? material->GetSpecularGlossiness().GetBaseColorTexture() : material->GetBasePBR().GetBaseColorTexture();
                        if (albedoMap.m_texture == nullptr)
                        {
                            constantTexturesSettings[ESettingType_Base] = 0.0f;
                            albedoMap.m_texture = ionRenderManager().GetNullTexure();
                        }

                        SamplerBinding normalMap;
                        normalMap.m_bindingIndex = bindingIndex++;
                        normalMap.m_texture = material->GetAdvancePBR().GetNormalTexture();
                        if (normalMap.m_texture == nullptr)
                        {
                            constantTexturesSettings[ESettingType_Normal] = 0.0f;
                            normalMap.m_texture = ionRenderManager().GetNullTexure();
                        }

                        SamplerBinding aoMap;
                        aoMap.m_bindingIndex = bindingIndex++;
                        aoMap.m_texture = material->GetAdvancePBR().GetOcclusionTexture();
                        if (aoMap.m_texture == nullptr)
                        {
                            constantTexturesSettings[ESettingType_Occlusion] = 0.0f;
                            aoMap.m_texture = ionRenderManager().GetNullTexure();
                        }

                        SamplerBinding physicalDescriptorMap;
                        physicalDescriptorMap.m_bindingIndex = bindingIndex++;
                        physicalDescriptorMap.m_texture = usingSpecularGlossiness ? material->GetSpecularGlossiness().GetSpecularGlossinessTexture() : material->GetBasePBR().GetMetalRoughnessTexture();
                        if (physicalDescriptorMap.m_texture == nullptr)
                        {
                            constantTexturesSettings[ESettingType_Physical] = 0.0f;
                            physicalDescriptorMap.m_texture = ionRenderManager().GetNullTexure();
                        }

                        SamplerBinding emissiveMap;
                        emissiveMap.m_bindingIndex = bindingIndex++;
                        emissiveMap.m_texture = material->GetAdvancePBR().GetEmissiveTexture();
                        if (emissiveMap.m_texture == nullptr)
                        {
                            constantTexturesSettings[ESettingType_Emissive] = 0.0f;
                            emissiveMap.m_texture = ionRenderManager().GetNullTexure();
                        }

                        // set the shaders layout
                        ShaderLayoutDef vertexLayout;
                        vertexLayout.m_uniforms.push_back(uniformVertex);

                        ShaderLayoutDef fragmentLayout;
                        fragmentLayout.m_uniforms.push_back(uniformFragment);
                        fragmentLayout.m_samplers.push_back(samplerIrradiance);
                        fragmentLayout.m_samplers.push_back(prefilteredMap);
                        fragmentLayout.m_samplers.push_back(samplerBRDFLUT);
                        fragmentLayout.m_samplers.push_back(albedoMap);
                        fragmentLayout.m_samplers.push_back(normalMap);
                        fragmentLayout.m_samplers.push_back(aoMap);
                        fragmentLayout.m_samplers.push_back(physicalDescriptorMap);
                        fragmentLayout.m_samplers.push_back(emissiveMap);

                        //
                        ConstantsBindingDef constants;
                        constants.m_shaderStages = EPushConstantStage::EPushConstantStage_Fragment;
                        constants.m_values.push_back(material->GetBasePBR().GetColor()[0]);
                        constants.m_values.push_back(material->GetBasePBR().GetColor()[1]);
                        constants.m_values.push_back(material->GetBasePBR().GetColor()[2]);
                        constants.m_values.push_back(material->GetBasePBR().GetColor()[3]);
                        constants.m_values.push_back(material->GetAdvancePBR().GetEmissiveColor()[0]);
                        constants.m_values.push_back(material->GetAdvancePBR().GetEmissiveColor()[1]);
                        constants.m_values.push_back(material->GetAdvancePBR().GetEmissiveColor()[2]);
                        constants.m_values.push_back(material->GetAdvancePBR().GetEmissiveColor()[3]);
                        constants.m_values.push_back(material->GetSpecularGlossiness().GetBaseColor()[0]);
                        constants.m_values.push_back(material->GetSpecularGlossiness().GetBaseColor()[1]);
                        constants.m_values.push_back(material->GetSpecularGlossiness().GetBaseColor()[2]);
                        constants.m_values.push_back(material->GetSpecularGlossiness().GetBaseColor()[3]);
                        constants.m_values.push_back(material->GetSpecularGlossiness().GetSpecularColor()[0]);
                        constants.m_values.push_back(material->GetSpecularGlossiness().GetSpecularColor()[1]);
                        constants.m_values.push_back(material->GetSpecularGlossiness().GetSpecularColor()[2]);
                        constants.m_values.push_back(material->GetSpecularGlossiness().GetSpecularColor()[3]);
                        constants.m_values.push_back(material->GetSpecularGlossiness().GetGlossinessColor()[0]);
                        constants.m_values.push_back(material->GetSpecularGlossiness().GetGlossinessColor()[1]);
                        constants.m_values.push_back(material->GetSpecularGlossiness().GetGlossinessColor()[2]);
                        constants.m_values.push_back(material->GetSpecularGlossiness().GetGlossinessColor()[3]);
                        constants.m_values.push_back(usingSpecularGlossiness);
                        constants.m_values.push_back(constantTexturesSettings[ESettingType_Base]);
                        constants.m_values.push_back(constantTexturesSettings[ESettingType_Physical]);
                        constants.m_values.push_back(constantTexturesSettings[ESettingType_Normal]);
                        constants.m_values.push_back(constantTexturesSettings[ESettingType_Occlusion]);
                        constants.m_values.push_back(constantTexturesSettings[ESettingType_Emissive]);
                        constants.m_values.push_back(material->GetBasePBR().GetMetallicFactor());
                        constants.m_values.push_back(material->GetBasePBR().GetRoughnessFactor());
                        constants.m_values.push_back(material->GetAlphaMode() == EAlphaMode_Mask ? 1.0f : 0.0f);
                        constants.m_values.push_back(material->GetAdvancePBR().GetAlphaCutoff());

                        material->SetVertexShaderLayout(vertexLayout);
                        material->SetFragmentShaderLayout(fragmentLayout);
                        material->SetVertexLayout(_meshRenderer->GetLayout());
                        material->SetConstantsShaders(constants);

                        ionS32 vertexShaderIndex = ionShaderProgramManager().FindShader(ionFileSystemManager().GetShadersPath(), ION_PBR_SHADER_NAME, EShaderStage_Vertex);
                        ionS32 fragmentShaderIndex = ionShaderProgramManager().FindShader(ionFileSystemManager().GetShadersPath(), ION_PBR_SHADER_NAME, EShaderStage_Fragment);

                        material->SetShaders(vertexShaderIndex, fragmentShaderIndex);
                    }
                }
                else
                {
                    enum ESettingType
                    {
                        ESettingType_Base = 0,
                        ESettingType_Normal,

                        ESettingType_Count
                    };

                    ionFloat constantTexturesSettings[ESettingType_Count];
                    for (ionU32 constantCounter = 0; constantCounter < ESettingType_Count; ++constantCounter)
                    {
                        constantTexturesSettings[constantCounter] = 1.0f;
                    }

                    ionU32 bindingIndex = 0;

                    //
                    UniformBinding uniformVertex;
                    uniformVertex.m_bindingIndex = bindingIndex++;
                    uniformVertex.m_parameters.push_back(ION_MODEL_MATRIX_PARAM);
                    uniformVertex.m_type.push_back(EBufferParameterType_Matrix);
                    uniformVertex.m_parameters.push_back(ION_VIEW_MATRIX_PARAM);
                    uniformVertex.m_type.push_back(EBufferParameterType_Matrix);
                    uniformVertex.m_parameters.push_back(ION_PROJ_MATRIX_PARAM);
                    uniformVertex.m_type.push_back(EBufferParameterType_Matrix);

                    //
                    UniformBinding uniformFragment;
                    uniformFragment.m_bindingIndex = bindingIndex++;
                    uniformFragment.m_parameters.push_back(ION_MAIN_CAMERA_POSITION_VECTOR_PARAM);
                    uniformFragment.m_type.push_back(EBufferParameterType_Vector);
                    uniformFragment.m_parameters.push_back(ION_DIRECTIONAL_LIGHT_DIR_VECTOR_PARAM);
                    uniformFragment.m_type.push_back(EBufferParameterType_Vector);
                    uniformFragment.m_parameters.push_back(ION_DIRECTIONAL_LIGHT_COL_VECTOR_PARAM);
                    uniformFragment.m_type.push_back(EBufferParameterType_Vector);

                    //
                    SamplerBinding albedoMap;
                    albedoMap.m_bindingIndex = bindingIndex++;
                    albedoMap.m_texture = material->GetBasePBR().GetBaseColorTexture();
                    if (albedoMap.m_texture == nullptr)
                    {
                        constantTexturesSettings[ESettingType_Base] = 0.0f;
                        albedoMap.m_texture = ionRenderManager().GetNullTexure();
                    }

                    SamplerBinding normalMap;
                    normalMap.m_bindingIndex = bindingIndex++;
                    normalMap.m_texture = material->GetAdvancePBR().GetNormalTexture();
                    if (normalMap.m_texture == nullptr)
                    {
                        constantTexturesSettings[ESettingType_Normal] = 0.0f;
                        normalMap.m_texture = ionRenderManager().GetNullTexure();
                    }


                    // set the shaders layout
                    ShaderLayoutDef vertexLayout;
                    vertexLayout.m_uniforms.push_back(uniformVertex);

                    ShaderLayoutDef fragmentLayout;
                    fragmentLayout.m_uniforms.push_back(uniformFragment);
                    fragmentLayout.m_samplers.push_back(albedoMap);
                    fragmentLayout.m_samplers.push_back(normalMap);

                    //
                    ConstantsBindingDef constants;
                    constants.m_shaderStages = EPushConstantStage::EPushConstantStage_Fragment;
                    constants.m_values.push_back(material->GetBasePBR().GetColor()[0]);
                    constants.m_values.push_back(material->GetBasePBR().GetColor()[1]);
                    constants.m_values.push_back(material->GetBasePBR().GetColor()[2]);
                    constants.m_values.push_back(material->GetBasePBR().GetColor()[3]);
                    constants.m_values.push_back(constantTexturesSettings[ESettingType_Base]);
                    constants.m_values.push_back(constantTexturesSettings[ESettingType_Normal]);
                    constants.m_values.push_back(material->GetAlphaMode() == EAlphaMode_Mask ? 1.0f : 0.0f);
                    constants.m_values.push_back(material->GetAdvancePBR().GetAlphaCutoff());

                    material->SetVertexShaderLayout(vertexLayout);
                    material->SetFragmentShaderLayout(fragmentLayout);
                    material->SetVertexLayout(_meshRenderer->GetLayout());
                    material->SetConstantsShaders(constants);


                    ionS32 fragmentShaderIndex = 1;
                    ionS32 vertexShaderIndex = ionShaderProgramManager().FindShader(ionFileSystemManager().GetShadersPath(), ION_PBR_SHADER_NAME, EShaderStage_Vertex);

                    if (material->IsDiffuseLight())
                    {
                        fragmentShaderIndex = ionShaderProgramManager().FindShader(ionFileSystemManager().GetShadersPath(), ION_DIFFUSE_LIGHT_SHADER_NAME, EShaderStage_Fragment);
                    }
                    else
                    {
                        fragmentShaderIndex = ionShaderProgramManager().FindShader(ionFileSystemManager().GetShadersPath(), ION_UNLIT_SHADER_NAME, EShaderStage_Fragment);
                    }

                    material->SetShaders(vertexShaderIndex, fragmentShaderIndex);
                }

                //
                ionMesh.SetMaterial(material);
            }
            else
            {
                Material* material = ionMaterialManger().GetMaterial("Default");

                switch (primitive.mode)
                {
                case TINYGLTF_MODE_POINTS:
                    material->SetTopology(VK_PRIMITIVE_TOPOLOGY_POINT_LIST);
                    break;
                case TINYGLTF_MODE_LINE:
                    material->SetTopology(VK_PRIMITIVE_TOPOLOGY_LINE_LIST);
                    break;
                case TINYGLTF_MODE_LINE_LOOP:
                    material->SetTopology(VK_PRIMITIVE_TOPOLOGY_LINE_STRIP);
                    break;
                case TINYGLTF_MODE_TRIANGLES:
                    material->SetTopology(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);
                    break;
                case TINYGLTF_MODE_TRIANGLE_STRIP:
                    material->SetTopology(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP);
                    break;
                case TINYGLTF_MODE_TRIANGLE_FAN:
                    material->SetTopology(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_FAN);
                    break;
                default:
                    material->SetTopology(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);
                    break;
                }

                enum ESettingType
                {
                    ESettingType_Base = 0,
                    ESettingType_Normal,

                    ESettingType_Count
                };

                ionFloat constantTexturesSettings[ESettingType_Count];
                for (ionU32 constantCounter = 0; constantCounter < ESettingType_Count; ++constantCounter)
                {
                    constantTexturesSettings[constantCounter] = 1.0f;
                }

                ionU32 bindingIndex = 0;

                //
                UniformBinding uniformVertex;
                uniformVertex.m_bindingIndex = bindingIndex++;
                uniformVertex.m_parameters.push_back(ION_MODEL_MATRIX_PARAM);
                uniformVertex.m_type.push_back(EBufferParameterType_Matrix);
                uniformVertex.m_parameters.push_back(ION_VIEW_MATRIX_PARAM);
                uniformVertex.m_type.push_back(EBufferParameterType_Matrix);
                uniformVertex.m_parameters.push_back(ION_PROJ_MATRIX_PARAM);
                uniformVertex.m_type.push_back(EBufferParameterType_Matrix);


                //
                UniformBinding uniformFragment;
                uniformFragment.m_bindingIndex = bindingIndex++;
                uniformFragment.m_parameters.push_back(ION_MAIN_CAMERA_POSITION_VECTOR_PARAM);
                uniformFragment.m_type.push_back(EBufferParameterType_Vector);
                uniformFragment.m_parameters.push_back(ION_DIRECTIONAL_LIGHT_DIR_VECTOR_PARAM);
                uniformFragment.m_type.push_back(EBufferParameterType_Vector);
                uniformFragment.m_parameters.push_back(ION_DIRECTIONAL_LIGHT_COL_VECTOR_PARAM);
                uniformFragment.m_type.push_back(EBufferParameterType_Vector);

                //
                SamplerBinding albedoMap;
                albedoMap.m_bindingIndex = bindingIndex++;
                albedoMap.m_texture = material->GetBasePBR().GetBaseColorTexture();
                if (albedoMap.m_texture == nullptr)
                {
                    constantTexturesSettings[ESettingType_Base] = 0.0f;
                    albedoMap.m_texture = ionRenderManager().GetNullTexure();
                }

                SamplerBinding normalMap;
                normalMap.m_bindingIndex = bindingIndex++;
                normalMap.m_texture = material->GetAdvancePBR().GetNormalTexture();
                if (normalMap.m_texture == nullptr)
                {
                    constantTexturesSettings[ESettingType_Normal] = 0.0f;
                    normalMap.m_texture = ionRenderManager().GetNullTexure();
                }


                // set the shaders layout
                ShaderLayoutDef vertexLayout;
                vertexLayout.m_uniforms.push_back(uniformVertex);

                ShaderLayoutDef fragmentLayout;
                fragmentLayout.m_uniforms.push_back(uniformFragment);
                fragmentLayout.m_samplers.push_back(albedoMap);
                fragmentLayout.m_samplers.push_back(normalMap);

                //
                ConstantsBindingDef constants;
                constants.m_shaderStages = EPushConstantStage::EPushConstantStage_Fragment;
                constants.m_values.push_back(material->GetBasePBR().GetColor()[0]);
                constants.m_values.push_back(material->GetBasePBR().GetColor()[1]);
                constants.m_values.push_back(material->GetBasePBR().GetColor()[2]);
                constants.m_values.push_back(material->GetBasePBR().GetColor()[3]);
                constants.m_values.push_back(constantTexturesSettings[ESettingType_Base]);
                constants.m_values.push_back(constantTexturesSettings[ESettingType_Normal]);
                constants.m_values.push_back(material->GetAlphaMode() == EAlphaMode_Mask ? 1.0f : 0.0f);
                constants.m_values.push_back(material->GetAdvancePBR().GetAlphaCutoff());

                material->SetVertexShaderLayout(vertexLayout);
                material->SetFragmentShaderLayout(fragmentLayout);
                material->SetVertexLayout(_meshRenderer->GetLayout());
                material->SetConstantsShaders(constants);

                ionS32 fragmentShaderIndex = 1;
                ionS32 vertexShaderIndex = ionShaderProgramManager().FindShader(ionFileSystemManager().GetShadersPath(), ION_PBR_SHADER_NAME, EShaderStage_Vertex);

                if (material->IsDiffuseLight())
                {
                    fragmentShaderIndex = ionShaderProgramManager().FindShader(ionFileSystemManager().GetShadersPath(), ION_DIFFUSE_LIGHT_SHADER_NAME, EShaderStage_Fragment);
                }
                else
                {
                    fragmentShaderIndex = ionShaderProgramManager().FindShader(ionFileSystemManager().GetShadersPath(), ION_UNLIT_SHADER_NAME, EShaderStage_Fragment);
                }


                material->SetShaders(vertexShaderIndex, fragmentShaderIndex);

                ionMesh.SetMaterial(material);
            }

            entityPtr->PushBackMesh(ionMesh);
        }

    }
}


void LoadAnimations(const eosString& _filenameNoExt, const tinygltf::Model& _model, Entity* _entityPtr, eosMap(ionU32, Node*)& _nodeIndexToNodePointer)
{
    if (_model.animations.size() > 0)
    {
        AnimationRenderer* animationRenderer = _entityPtr->AddAnimationRenderer();

        const ionSize animSize = _model.animations.size();
        for (ionSize i = 0; i < animSize; ++i)
        {
            const tinygltf::Animation& gltfAnim = _model.animations[i];

            ion::Animation ionAnim;
            if (gltfAnim.name.empty())
            {
                ionAnim.SetName(_filenameNoExt + "#" + std::to_string(i + 1).c_str() + "#" + std::to_string(animSize).c_str());
            }
            else
            {
                ionAnim.SetName(gltfAnim.name.c_str());
            }

            //////////////////////////////////////////////////////////////////////////
            // samplers
            const ionSize samplerSize = gltfAnim.samplers.size();
            for (ionSize j = 0; j < samplerSize; ++j)
            {
                const tinygltf::AnimationSampler& gltfSampler = gltfAnim.samplers[j];

                ion::AnimationSampler ionSampler;

                // interpolation
                if (gltfSampler.interpolation == "LINEAR")
                {
                    ionSampler.SetInterpolation(EAnimationInterpolationType_Linear);
                }
                else if (gltfSampler.interpolation == "STEP")
                {
                    ionSampler.SetInterpolation(EAnimationInterpolationType_Step);
                }
                else if (gltfSampler.interpolation == "CUBICSPLINE")
                {
                    ionSampler.SetInterpolation(EAnimationInterpolationType_CubicSpline);
                }
                else
                {
                    ionAssertReturnVoid(false, "Unsupported interpolation");
                }

                // sampler input and time value
                {
                    const tinygltf::Accessor &accessor = _model.accessors[gltfSampler.input];
                    const tinygltf::BufferView &bufferView = _model.bufferViews[accessor.bufferView];
                    const tinygltf::Buffer &buffer = _model.buffers[bufferView.buffer];

                    switch (accessor.componentType)
                    {
                    case TINYGLTF_PARAMETER_TYPE_BYTE:
                    {
                        ionS8 *buf = (ionS8 *)eosNewRaw(sizeof(ionS8) * accessor.count, ION_MEMORY_ALIGNMENT_SIZE);
                        memcpy(buf, &buffer.data[accessor.byteOffset + bufferView.byteOffset], accessor.count * sizeof(ionS8));
                        for (ionSize index = 0; index < accessor.count; index++)
                        {
                            ionFloat value = std::max(buf[index] / 127.0f, -1.0f);
                            ionSampler.PushBackInput(value);
                        }
                        eosDeleteRaw(buf);
                        break;
                    }
                    case TINYGLTF_PARAMETER_TYPE_SHORT:
                    {
                        ionS16 *buf = (ionS16 *)eosNewRaw(sizeof(ionS16) * accessor.count, ION_MEMORY_ALIGNMENT_SIZE);
                        memcpy(buf, &buffer.data[accessor.byteOffset + bufferView.byteOffset], accessor.count * sizeof(ionS16));
                        for (ionSize index = 0; index < accessor.count; index++)
                        {
                            ionFloat value = std::max(buf[index] / 32767.0f, -1.0f);
                            ionSampler.PushBackInput(value);
                        }
                        eosDeleteRaw(buf);
                        break;
                    }
                    case TINYGLTF_PARAMETER_TYPE_UNSIGNED_BYTE:
                    {
                        ionU8 *buf = (ionU8 *)eosNewRaw(sizeof(ionU8) * accessor.count, ION_MEMORY_ALIGNMENT_SIZE);
                        memcpy(buf, &buffer.data[accessor.byteOffset + bufferView.byteOffset], accessor.count * sizeof(ionU8));
                        for (ionSize index = 0; index < accessor.count; index++)
                        {
                            ionFloat value = buf[index] / 255.0f;
                            ionSampler.PushBackInput(value);
                        }
                        eosDeleteRaw(buf);
                        break;
                    }
                    case TINYGLTF_PARAMETER_TYPE_UNSIGNED_SHORT:
                    {
                        ionU16 *buf = (ionU16 *)eosNewRaw(sizeof(ionU16) * accessor.count, ION_MEMORY_ALIGNMENT_SIZE);
                        memcpy(buf, &buffer.data[accessor.byteOffset + bufferView.byteOffset], accessor.count * sizeof(ionU16));
                        for (ionSize index = 0; index < accessor.count; index++)
                        {
                            ionFloat value = buf[index] / 65535.0f;
                            ionSampler.PushBackInput(value);
                        }
                        eosDeleteRaw(buf);
                        break;
                    }
                    case TINYGLTF_PARAMETER_TYPE_FLOAT:
                    {
                        ionFloat *buf = (ionFloat *)eosNewRaw(sizeof(ionFloat) * accessor.count, ION_MEMORY_ALIGNMENT_SIZE);
                        memcpy(buf, &buffer.data[accessor.byteOffset + bufferView.byteOffset], accessor.count * sizeof(ionFloat));
                        for (ionSize index = 0; index < accessor.count; index++)
                        {
                            ionSampler.PushBackInput(buf[index]);
                        }
                        eosDeleteRaw(buf);
                        break;
                    }
                    default:
                        ionAssertReturnVoid(false, "Index component type is not supported!");
                    }


                    eosVector(ionFloat)::const_iterator begin = ionSampler.InputsIteratorBeginConst(), end = ionSampler.InputsIteratorEndConst(), it = begin;
                    for (; it != end; ++it)
                    {
                        ionFloat input = *it;
                        if (input < ionAnim.GetStart())
                        {
                            ionAnim.SetStart(input);
                        }

                        if (input > ionAnim.GetEnd())
                        {
                            ionAnim.SetEnd(input);
                        }
                    }
                }

                // sampler output
                {
                    const tinygltf::Accessor &accessor = _model.accessors[gltfSampler.output];
                    const tinygltf::BufferView &bufferView = _model.bufferViews[accessor.bufferView];
                    const tinygltf::Buffer &buffer = _model.buffers[bufferView.buffer];

                    switch (accessor.type) 
                    {

                    case TINYGLTF_TYPE_VEC3:
                    {
                        const ionFloat *bufferVector = reinterpret_cast<const ionFloat *>(&(buffer.data[accessor.byteOffset + bufferView.byteOffset]));
                        for (ionSize a = 0; a < accessor.count; ++a)
                        {
                            Vector v((&bufferVector[a * 3])[0], ((&bufferVector[a * 3])[1]), (&bufferVector[a * 3])[2], 1.0f);
                            ionSampler.PushBackOutputLinearPath(v);
                        }
                        break;
                    }

                    case TINYGLTF_TYPE_VEC4:
                    {
                        switch (accessor.componentType)
                        {
                            case TINYGLTF_PARAMETER_TYPE_FLOAT:
                            {
                                const ionFloat *bufferVector = reinterpret_cast<const ionFloat *>(&(buffer.data[accessor.byteOffset + bufferView.byteOffset]));
                                for (ionSize a = 0; a < accessor.count; ++a)
                                {
                                    Vector v((&bufferVector[a * 3])[0], ((&bufferVector[a * 3])[1]), (&bufferVector[a * 3])[2], (&bufferVector[a * 3])[3]);
                                    ionSampler.PushBackOutputLinearPath(v);
                                }
                                break;
                            }

                            case TINYGLTF_PARAMETER_TYPE_UNSIGNED_SHORT:
                            {
                                const ionU16 *bufferVector = reinterpret_cast<const ionU16 *>(&(buffer.data[accessor.byteOffset + bufferView.byteOffset]));
                                for (ionSize a = 0; a < accessor.count; ++a)
                                {
                                    Vector v(ionFloat((&bufferVector[a * 3])[0]) / 65535.0f, ionFloat((&bufferVector[a * 3])[1]) / 65535.0f, ionFloat((&bufferVector[a * 3])[2]) / 65535.0f, ionFloat((&bufferVector[a * 3])[3]) / 65535.0f);
                                    ionSampler.PushBackOutputLinearPath(v);
                                }
                                break;
                            }

                            case TINYGLTF_PARAMETER_TYPE_UNSIGNED_BYTE:
                            {
                                const ionU8 *bufferVector = reinterpret_cast<const ionU8 *>(&(buffer.data[accessor.byteOffset + bufferView.byteOffset]));
                                for (ionSize a = 0; a < accessor.count; ++a)
                                {
                                    Vector v(ionFloat((&bufferVector[a * 3])[0]) / 255.0f, ionFloat((&bufferVector[a * 3])[1]) / 255.0f, ionFloat((&bufferVector[a * 3])[2]) / 255.0f, ionFloat((&bufferVector[a * 3])[3]) / 255.0f);
                                    ionSampler.PushBackOutputLinearPath(v);
                                }
                                break;
                            }

                            case TINYGLTF_PARAMETER_TYPE_SHORT:
                            {
                                const ionS16 *bufferVector = reinterpret_cast<const ionS16 *>(&(buffer.data[accessor.byteOffset + bufferView.byteOffset]));
                                for (ionSize a = 0; a < accessor.count; ++a)
                                {
                                    Vector v(std::max(ionFloat((&bufferVector[a * 3])[0]) / 32767.0f, -1.0f), std::max(ionFloat((&bufferVector[a * 3])[1]) / 32767.0f, -1.0f), std::max(ionFloat((&bufferVector[a * 3])[2]) / 32767.0f, -1.0f), std::max(ionFloat((&bufferVector[a * 3])[3]) / 32767.0f, -1.0f));
                                    ionSampler.PushBackOutputLinearPath(v);
                                }
                                break;
                            }

                            case TINYGLTF_PARAMETER_TYPE_BYTE:
                            {
                                const ionS8 *bufferVector = reinterpret_cast<const ionS8 *>(&(buffer.data[accessor.byteOffset + bufferView.byteOffset]));
                                for (ionSize a = 0; a < accessor.count; ++a)
                                {
                                    Vector v(std::max(ionFloat((&bufferVector[a * 3])[0]) / 127.0f, -1.0f), std::max(ionFloat((&bufferVector[a * 3])[1]) / 127.0f, -1.0f), std::max(ionFloat((&bufferVector[a * 3])[2]) / 127.0f, -1.0f), std::max(ionFloat((&bufferVector[a * 3])[3]) / 127.0f, -1.0f));
                                    ionSampler.PushBackOutputLinearPath(v);
                                }
                                break;
                            }
                        }
                        break;
                    }
                    case TINYGLTF_TYPE_SCALAR:
                    {
                        switch (accessor.componentType)
                        {
                            case TINYGLTF_PARAMETER_TYPE_FLOAT:
                            {
                                const ionFloat *bufferVector = reinterpret_cast<const ionFloat *>(&(buffer.data[accessor.byteOffset + bufferView.byteOffset]));
                                for (ionSize a = 0; a < accessor.count; ++a)
                                {
                                    ionFloat f = bufferVector[a];
                                    ionSampler.PushBackOutputMorphTarget(f);
                                }
                                break;
                            }

                            case TINYGLTF_PARAMETER_TYPE_UNSIGNED_SHORT:
                            {
                                const ionU16 *bufferVector = reinterpret_cast<const ionU16 *>(&(buffer.data[accessor.byteOffset + bufferView.byteOffset]));
                                for (ionSize a = 0; a < accessor.count; ++a)
                                {
                                    ionFloat f = ionFloat(bufferVector[a]) / 65535.0f;
                                    ionSampler.PushBackOutputMorphTarget(f);
                                }
                                break;
                            }

                            case TINYGLTF_PARAMETER_TYPE_UNSIGNED_BYTE:
                            {
                                const ionU8 *bufferVector = reinterpret_cast<const ionU8 *>(&(buffer.data[accessor.byteOffset + bufferView.byteOffset]));
                                for (ionSize a = 0; a < accessor.count; ++a)
                                {
                                    ionFloat f = ionFloat(bufferVector[a]) / 255.0f;
                                    ionSampler.PushBackOutputMorphTarget(f);
                                }
                                break;
                            }

                            case TINYGLTF_PARAMETER_TYPE_SHORT:
                            {
                                const ionS16 *bufferVector = reinterpret_cast<const ionS16 *>(&(buffer.data[accessor.byteOffset + bufferView.byteOffset]));
                                for (ionSize a = 0; a < accessor.count; ++a)
                                {
                                    ionFloat f = std::max(ionFloat(bufferVector[a]) / 32767.0f, -1.0f);
                                    ionSampler.PushBackOutputMorphTarget(f);
                                }
                                break;
                            }

                            case TINYGLTF_PARAMETER_TYPE_BYTE:
                            {
                                const ionS8 *bufferVector = reinterpret_cast<const ionS8 *>(&(buffer.data[accessor.byteOffset + bufferView.byteOffset]));
                                for (ionSize a = 0; a < accessor.count; ++a)
                                {
                                    ionFloat f = std::max(ionFloat(bufferVector[a]) / 127.0f, -1.0f);
                                    ionSampler.PushBackOutputMorphTarget(f);
                                }
                                break;
                            }
                        }
                        break;
                    }
                    default:
                        ionAssertReturnVoid(false, "Index component type is not supported!");
                        break;
                    }

                }

                // add
                ionAnim.PushBackSampler(ionSampler);
            }

            //////////////////////////////////////////////////////////////////////////
            // channels
            const ionSize channlSize = gltfAnim.channels.size();
            for (ionSize j = 0; j < channlSize; ++j)
            {
                const tinygltf::AnimationChannel& gltfChannel = gltfAnim.channels[j];

                ion::AnimationChannel ionChannel;

                if (gltfChannel.target_path == "rotation")
                {
                    ionChannel.SetPath(EAnimationPathType_Rotation);
                }
                else if (gltfChannel.target_path == "translation")
                {
                    ionChannel.SetPath(EAnimationPathType_Translation);
                }
                else if (gltfChannel.target_path == "scale")
                {
                    ionChannel.SetPath(EAnimationPathType_Scale);
                }
                else if (gltfChannel.target_path == "weights")
                {
                    ionChannel.SetPath(EAnimationPathType_WeightMorphTarget);
                }
                else
                {
                    ionAssertReturnVoid(false, "Unsupported animation path");
                }

                ionChannel.SetSamplerIndex(gltfChannel.sampler);

                ionChannel.SetNode(_nodeIndexToNodePointer[(ionU32)gltfChannel.target_node]);
                if (ionChannel.GetNode() == nullptr)
                {
                    continue;
                }

                // add
                ionAnim.PushBackChannel(ionChannel);
            }

            //////////////////////////////////////////////////////////////////////////
            // final add
            animationRenderer->PushBackAnimation(ionAnim);
        }
    }
}

ionBool LoaderGLTF::Load(const eosString & _filePath, Camera* _camToUpdatePtr, ObjectHandler& _entity, ionBool _generateNormalWhenMissing /*= false*/, ionBool _generateTangentWhenMissing /*= false*/, ionBool _setBitangentSign /*= false*/)
{
    //
    eosString dir;
    eosString filename;
    eosString filenameNoExt;
    eosString ext;
    eosMap(ionS32, eosString) textureIndexToTextureName;
    eosMap(ionS32, eosString) materialIndexToMaterialName;
    eosMap(ionU32, Node*) nodeIndexToNodePointer;

    //
    tinygltf::Model     model;
    tinygltf::TinyGLTF  gltf;
    std::string         err;

    //
    if (_filePath.find_last_of('/') != std::string::npos)
    {
        dir = _filePath.substr(0, _filePath.find_last_of('/'));
    }

    if (dir.empty())
    {
        if (_filePath.find_last_of('\\') != std::string::npos)
        {
            dir = _filePath.substr(0, _filePath.find_last_of('\\'));
        }
    }

    ionAssertReturnValue(!dir.empty(), "Path invalid", false);

    //
    if (_filePath.rfind('\\', _filePath.length()) != std::string::npos)
    {
        filename = _filePath.substr(_filePath.rfind('\\', _filePath.length()) + 1, _filePath.length() - _filePath.rfind('\\', _filePath.length()));
    }
    if (filename.empty())
    {
        if (_filePath.rfind('/', _filePath.length()) != std::string::npos)
        {
            filename = _filePath.substr(_filePath.rfind('/', _filePath.length()) + 1, _filePath.length() - _filePath.rfind('/', _filePath.length()));
        }
    }

    ionAssertReturnValue(!filename.empty(), "Filename invalid", false);

    //
    filenameNoExt = filename.substr(0, filename.find_last_of('.'));

    //
    if (_filePath.find_last_of('.') != std::string::npos)
    {
        ext = _filePath.substr(_filePath.find_last_of('.') + 1);
    }

    ionAssertReturnValue(!ext.empty(), "Extension invalid", false);

    //
    ionBool ret = false;
    if (ext.compare("glb") == 0 || ext.compare("bin") == 0)         // FULL BINARY
    {
        ret = gltf.LoadBinaryFromFile(&model, &err, _filePath.c_str());
    }
    else if (ext.compare("gltf") == 0)                                // FULL TEXT OR HYBRID
    {
        ret = gltf.LoadASCIIFromFile(&model, &err, _filePath.c_str());
    }
    else
    {
        ionAssertReturnValue(false, "Invalid file format extension", false);
    }

    ionAssertReturnValue(err.empty(), err.c_str(), false);
    ionAssertReturnValue(ret, "Failed to parse glTF", false);

    ionAssertReturnValue(err.empty(), err.c_str(), false);

    //
    // START PARSING
    //

    const eosString underscore = "_";
    const eosString backslash = "/";

    //////////////////////////////////////////////////////////////////////////
    // 1. Load all the texture inside the texture manager
    for (ionSize i = 0; i < model.textures.size(); ++i)
    {
        const tinygltf::Texture& tex = model.textures[i];

        const tinygltf::Image& image = model.images[tex.source];
  
        ETextureRepeat repeatU = ETextureRepeat_Repeat;
        ETextureRepeat repeatV = ETextureRepeat_Repeat;
        ETextureRepeat repeatW = ETextureRepeat_Repeat;

        ETextureFilterMag filterMag = ETextureFilterMag_Linear;
        ETextureFilterMin filterMin = ETextureFilterMin_Linear_MipMap_Linear;

        if (tex.sampler > -1)
        {
            const tinygltf::Sampler& sampler = model.samplers[tex.sampler];

            switch (sampler.magFilter)
            {
            case 9728: 
                filterMag = ETextureFilterMag_Nearest; 
                break;
            case 9729: 
            default:
                filterMag = ETextureFilterMag_Linear;
                break;
            }

            switch (sampler.minFilter)
            {
            case 9728:
                filterMin = ETextureFilterMin_Nearest;
                break;
            case 9729:
                filterMin = ETextureFilterMin_Linear;
                break;
            case 9984:
                filterMin = ETextureFilterMin_Nearest_MipMap_Nearest;
                break;
            case 9985:
                filterMin = ETextureFilterMin_Linear_MipMap_Nearest;
                break;
            case 9986:
                filterMin = ETextureFilterMin_Nearest_MipMap_Linear;
                break;
            case 9987:
            default:
                filterMin = ETextureFilterMin_Linear_MipMap_Linear;
                break;
            }

            if (sampler.wrapS == 33071)
            {
                repeatU = ETextureRepeat_Clamp;
            }
            else if (sampler.wrapS == 33648)
            {
                repeatU = ETextureRepeat_Mirrored;
            }
            else
            {
                repeatU = ETextureRepeat_Repeat;
            }

            if (sampler.wrapT == 33071)
            {
                repeatV = ETextureRepeat_Clamp;
            }
            else if (sampler.wrapT == 33648)
            {
                repeatV = ETextureRepeat_Mirrored;
            }
            else
            {
                repeatV = ETextureRepeat_Repeat;
            }

            if (sampler.wrapR == 33071)
            {
                repeatW = ETextureRepeat_Clamp;
            }
            else if (sampler.wrapR == 33648)
            {
                repeatW = ETextureRepeat_Mirrored;
            }
            else
            {
                repeatW = ETextureRepeat_Repeat;
            }
        }
        
        if (image.uri.empty())                    // no uri, so image could be stored in binary format
        {
            eosString name = image.name.c_str();  // no filename, I just give you one
            if (name.empty())
            {
                eosString val = std::to_string(i).c_str();
                name = filenameNoExt + underscore + val;
            }

            textureIndexToTextureName.insert(std::pair<ionS32, eosString>((ionS32)i, name.c_str()));

            ionTextureManger().CreateTextureFromBuffer(name, image.width, image.height, image.component, &image.image[0], image.image.size(), filterMin, filterMag, ETextureRepeat_Custom, ETextureUsage_RGBA, ETextureType_2D, 1U, repeatU, repeatV, repeatW);
        }
        else
        {
            eosString val = image.uri.c_str();
            eosString path = dir + backslash + val;
            eosString filename;

            if (path.rfind('\\', path.length()) != std::string::npos)
            {
                filename = path.substr(path.rfind('\\', path.length()) + 1, path.length() - path.rfind('\\', path.length()));
            }
            if (filename.empty())
            {
                if (path.rfind('/', path.length()) != std::string::npos)
                {
                    filename = path.substr(path.rfind('/', path.length()) + 1, path.length() - path.rfind('/', path.length()));
                }
            }

            textureIndexToTextureName.insert(std::pair<ionS32, eosString>((ionS32)i, filename.c_str()));

            ionTextureManger().CreateTextureFromFile(filename, path, filterMin, filterMag, ETextureRepeat_Custom, ETextureUsage_RGBA, ETextureType_2D, 1U, repeatU, repeatV, repeatW);
        }
    }

    //
    // NOTE: this part MUST be re-factored when I finish the demo. Because should be handleable from different materials
    //
    // 2. Load all materials inside the material manager
    if (model.materials.size() > 0)
    {
        for (ionSize i = 0; i < model.materials.size(); ++i)
        {
            const tinygltf::Material& mat = model.materials[i];

            const eosString materialName = filenameNoExt + "#" + mat.name.c_str();

            materialIndexToMaterialName.insert(std::pair<ionS32, eosString>((ionS32)i, materialName));

            Material* material = ionMaterialManger().CreateMaterial(materialName, 0u);

            material->SetAlphaMode(EAlphaMode_Opaque);

            // set default values
            material->GetBasePBR().SetBaseColor(1.0f, 1.0f, 1.0f, 1.0f);
            material->GetBasePBR().SetRoughnessFactor(1.0f);
            material->GetBasePBR().SetMetallicFactor(1.0f);
            material->GetAdvancePBR().SetEmissiveColor(1.0f, 1.0f, 1.0f);
            material->GetAdvancePBR().SetAlphaCutoff(0.5f);
            material->GetSpecularGlossiness().SetBaseColor(1.0f, 1.0f, 1.0f, 1.0f);
            material->GetSpecularGlossiness().SetGlossinessColor(1.0f, 1.0f, 1.0f, 1.0f);
            material->GetSpecularGlossiness().SetSpecularColor(1.0f, 1.0f, 1.0f, 1.0f);

            material->GetState().SetCullingMode(ECullingMode_Back);
            material->GetState().SetDepthFunctionMode(EDepthFunction_Less);
            material->GetState().SetStencilFrontFunctionMode(EStencilFrontFunction_LesserOrEqual);
            material->GetState().SetBlendStateMode(EBlendState_SourceBlend_One);
            material->GetState().SetBlendStateMode(EBlendState_DestBlend_Zero);
            material->GetState().SetBlendOperatorMode(EBlendOperator_Add);

            // base PBR
            for (auto const& x : mat.values)
            {
                const std::string& key = (x.first);
                const tinygltf::Parameter& param = (x.second);

                // TEXTURES
                {
                    // this is the base texture
                    if (key == "baseColorTexture")
                    {
                        material->GetBasePBR().SetBaseColorTexture(ionTextureManger().GetTexture(textureIndexToTextureName[param.TextureIndex()]));
                        continue;
                    }

                    //the texture contain Metallic and Roughness in 2 different channel, red and green
                    if (key == "metallicRoughnessTexture")
                    {
                        material->GetBasePBR().SetMetalRoughnessTexture(ionTextureManger().GetTexture(textureIndexToTextureName[param.TextureIndex()]));
                        continue;
                    }
                }

                // FACTORS
                {
                    if (key == "baseColorFactor")   // this is a vector, because is the diffuse of the not metal
                    {
                        material->GetBasePBR().SetBaseColor((ionFloat)param.ColorFactor()[0], (ionFloat)param.ColorFactor()[1], (ionFloat)param.ColorFactor()[2], (ionFloat)param.ColorFactor()[3]);
                        continue;
                    }

                    // these 2 are metal factors, so is the color is the specific measured reflectance value at normal incidence (F0)
                    if (key == "roughnessFactor")
                    {
                        material->GetBasePBR().SetRoughnessFactor((ionFloat)param.Factor());
                        continue;
                    }

                    if (key == "metallicFactor")
                    {
                        material->GetBasePBR().SetMetallicFactor((ionFloat)param.Factor());
                        continue;
                    }
                }
            }

            for (auto const& x : mat.additionalValues)
            {
                const std::string& key = (x.first);
                const tinygltf::Parameter& param = (x.second);

                // TEXTURES
                {
                    if (key == "normalTexture")
                    {
                        material->GetAdvancePBR().SetNormalTexture(ionTextureManger().GetTexture(textureIndexToTextureName[param.TextureIndex()]));
                        continue;
                    }

                    if (key == "occlusionTexture")
                    {
                        material->GetAdvancePBR().SetOcclusionTexture(ionTextureManger().GetTexture(textureIndexToTextureName[param.TextureIndex()]));
                        continue;
                    }

                    if (key == "emissiveTexture")
                    {
                        material->GetAdvancePBR().SetEmissiveTexture(ionTextureManger().GetTexture(textureIndexToTextureName[param.TextureIndex()]));
                        continue;
                    }
                }

                // FACTORS
                {
                    if (key == "emissiveFactor")
                    {
                        material->GetAdvancePBR().SetEmissiveColor((ionFloat)param.ColorFactor()[0], (ionFloat)param.ColorFactor()[1], (ionFloat)param.ColorFactor()[2]);
                        continue;
                    }
                }

                if (key == "alphaMode")
                {
                    material->GetState().UnsetBlendStateMode(EBlendState_SourceBlend_One);
                    material->GetState().UnsetBlendStateMode(EBlendState_DestBlend_Zero);
                    material->GetState().UnsetBlendOperatorMode(EBlendOperator_Add);

                    // mesh sorting for blend
                    // this could be anything, glass for instance
                    if (param.string_value == "BLEND")
                    {
                        material->SetAlphaMode(EAlphaMode_Blend);

                        material->GetState().SetBlendStateMode(EBlendState_SourceBlend_Source_Alpha);
                        material->GetState().SetBlendStateMode(EBlendState_DestBlend_One_Minus_Source_Alpha);
                        material->GetState().SetBlendOperatorMode(EBlendOperator_Add);

                        continue;
                    }

                    // this would like leaves or grass
                    if (param.string_value == "MASK")
                    {
                        material->SetAlphaMode(EAlphaMode_Mask);

                        material->GetState().SetBlendStateMode(EBlendState_SourceBlend_One);
                        material->GetState().SetBlendStateMode(EBlendState_DestBlend_Zero);
                        material->GetState().SetBlendOperatorMode(EBlendOperator_Add);
                        continue;
                    }

                    // is is set OPAQUE, do nothing, is the default value of rendering
                    continue;
                }

                // if "MASK" there is this other parameter which specifies the cutoff threshold. If the alpha value is greater than or equal to the alphaCutoff value then it is rendered as fully opaque, otherwise, it is rendered as fully transparent.
                if (key == "alphaCutoff")
                {
                    material->GetAdvancePBR().SetAlphaCutoff((ionFloat)param.Factor());
                    continue;
                }

                if (key == "doubleSided")
                {
                    //if (param.bool_value) // still not clear if the key exists I have to set anyway double side without caring about the boolean flag
                    {
                        material->GetState().UnsetCullingMode(ECullingMode_Back);
                        material->GetState().SetCullingMode(ECullingMode_TwoSide);
                    }
                    continue;
                }
            }

            for (auto const& x : mat.extPBRValues)
            {
                const std::string& key = (x.first);
                const tinygltf::Parameter& param = (x.second);

                // SPECULAR GLOSSINESS
                {
                    if (key == "specularGlossinessTexture") 
                    {
                        material->GetSpecularGlossiness().SetSpecularGlossinessTexture(ionTextureManger().GetTexture(textureIndexToTextureName[param.TextureIndex()]));
                        continue;
                    }
                    if (key == "diffuseTexture")
                    {
                        material->GetSpecularGlossiness().SetBaseColorTexture(ionTextureManger().GetTexture(textureIndexToTextureName[param.TextureIndex()]));
                        continue;
                    }
                    if (key == "diffuseFactor")
                    {
                        material->GetSpecularGlossiness().SetBaseColor((ionFloat)param.ColorFactor()[0], (ionFloat)param.ColorFactor()[1], (ionFloat)param.ColorFactor()[2], (ionFloat)param.ColorFactor()[3]);
                        continue;
                    }
                    if (key == "glossinessFactor")
                    {
                        material->GetSpecularGlossiness().SetGlossinessColor((ionFloat)param.Factor(), (ionFloat)param.Factor(), (ionFloat)param.Factor(), 1.0f);
                        continue;
                    }
                    if (key == "specularFactor")
                    {
                        material->GetSpecularGlossiness().SetSpecularColor((ionFloat)param.ColorFactor()[0], (ionFloat)param.ColorFactor()[1], (ionFloat)param.ColorFactor()[2], 1.0f);
                        continue;
                    }
                }
            }

            if (!material->IsValidPBR())
            {
                if (material->IsValidSpecularGlossiness())
                {
                    material->SetSpecularGloss(true);
                }
                else
                {
                    material->SetDiffuseLight(true);
                    if (model.extensionsUsed.size() > 0)
                    {
                        if (std::find(model.extensionsUsed.begin(), model.extensionsUsed.end(), "KHR_materials_unlit") != model.extensionsUsed.end())
                        {
                            material->SetDiffuseLight(false);
                            material->SetUnlit(true);
                        }
                    }
                    
                }
            }
        }
    }
    else
    {
        // DEFAULT MATERIAL
        // For now is here, I need to more somewhere else!
        Material* material = ionMaterialManger().CreateMaterial("Default", 0u);
        material->GetBasePBR().SetBaseColor(1.0f, 1.0f, 1.0f, 1.0f);
        material->GetBasePBR().SetMetallicFactor(1.0f);
        material->GetBasePBR().SetRoughnessFactor(1.0f);
        material->GetAdvancePBR().SetEmissiveColor(1.0f, 1.0f, 1.0f);
        material->GetAdvancePBR().SetAlphaCutoff(0.5f);
        material->GetState().SetCullingMode(ECullingMode_Back);
        material->GetState().SetDepthFunctionMode(EDepthFunction_Less);
        material->GetState().SetStencilFrontFunctionMode(EStencilFrontFunction_LesserOrEqual);
        material->GetState().SetBlendStateMode(EBlendState_SourceBlend_One);
        material->GetState().SetBlendStateMode(EBlendState_DestBlend_Zero);
        material->GetState().SetBlendOperatorMode(EBlendOperator_Add);
        material->SetUnlit(true);
    }
    
    //
    // 3. Load all meshes..
    ionS32 mainIndex = model.defaultScene == -1 ? 0 : model.defaultScene;
    const tinygltf::Scene &scene = model.scenes[mainIndex];
    const ionSize nodeCount = scene.nodes.size();

    // for GLTF always full vertex
    Entity* entityPtr = dynamic_cast<Entity*>(_entity.GetPtr());
    MeshRenderer* meshRenderer = entityPtr->AddMeshRenderer<MeshRenderer>();

    if (nodeCount == 1)
    {
        const tinygltf::Node node = model.nodes[scene.nodes[0]];

        nodeIndexToNodePointer.insert(std::pair<ionU32, Node*>((ionU32)scene.nodes[0], entityPtr));
        LoadNode(node, model, meshRenderer, _entity, nodeIndexToNodePointer, textureIndexToTextureName, materialIndexToMaterialName, _generateNormalWhenMissing, _generateTangentWhenMissing, _setBitangentSign);
    }
    else
    {
        for (ionSize i = 0; i < nodeCount; ++i)
        {
            const tinygltf::Node node = model.nodes[scene.nodes[i]];

            Entity* child = eosNew(Entity, ION_MEMORY_ALIGNMENT_SIZE, node.name.c_str());
            ObjectHandler childHandle(child);

            nodeIndexToNodePointer.insert(std::pair<ionU32, Node*>((ionU32)scene.nodes[i], child));
            LoadNode(node, model, meshRenderer, childHandle, nodeIndexToNodePointer, textureIndexToTextureName, materialIndexToMaterialName, _generateNormalWhenMissing, _generateTangentWhenMissing, _setBitangentSign);

            child->AttachToParent(_entity);
        }
    }

    //
    // 4. load animations if any
    LoadAnimations(filenameNoExt, model, entityPtr, nodeIndexToNodePointer);

    //
    // 5. for the main bounding box: if missing create, if present expand to the maximum one
    UpdateBoundingBox(_entity, *_entity->GetBoundingBox());

    //
    // 6. camera set, for now just one and perspective
    if (model.cameras.size() > 0)
    {
        if (model.cameras[0].type == "perspective")
        {
            _camToUpdatePtr->SetPerspectiveProjection(NIX_RAD_TO_DEG(model.cameras[0].perspective.yfov), model.cameras[0].perspective.aspectRatio, model.cameras[0].perspective.znear, model.cameras[0].perspective.zfar);
        }
    }

    textureIndexToTextureName.clear();
    materialIndexToMaterialName.clear();
    nodeIndexToNodePointer.clear();

    return true;
}


ION_NAMESPACE_END