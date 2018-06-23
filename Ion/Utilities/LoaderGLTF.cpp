#include "LoaderGLTF.h"

#include "../Scene/Entity.h"

#include "../Texture/TextureManager.h"
#include "../Material/MaterialManager.h"

#include "../Dependencies/Eos/Eos/Eos.h"
#include "../Dependencies/Nix/Nix/Nix.h"

#include "../Utilities/GeometryHelper.h"

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

// for some reasons, tinygltf must be declared in source file: I was unable to declare any of its structures in header file
void LoadNode(const tinygltf::Node& _node, const tinygltf::Model& _model, const Matrix& _parentMatrix, Entity& _entity, eosMap(ionS32, eosString)& _textureIndexToTextureName, eosMap(ionS32, eosString)& _materialIndexToMaterialName, ionBool _generateNormalWhenMissing, ionBool _generateTangentWhenMissing, ionBool _setBitangentSign)
{
    Vector position(0.0f, 0.0f, 0.0f, 1.0f);
    Quaternion rotation;
    Vector scale(1.0f);

    if (_node.matrix.size() == 16)
    {
        // SCALE
        ionFloat scaleFactor = std::sqrtf((ionFloat)_node.matrix.data()[0] * (ionFloat)_node.matrix.data()[0] + (ionFloat)_node.matrix.data()[1] * (ionFloat)_node.matrix.data()[1] + (ionFloat)_node.matrix.data()[2] * (ionFloat)_node.matrix.data()[2]);
        scale = VectorHelper::Set(scaleFactor, scaleFactor, scaleFactor, 1.0f);


        // ROTATION
        Matrix rotationMatrix;
        rotationMatrix = Matrix(
            (ionFloat)_node.matrix.data()[0], (ionFloat)_node.matrix.data()[1], (ionFloat)_node.matrix.data()[2],   0.0f,
            (ionFloat)_node.matrix.data()[4], (ionFloat)_node.matrix.data()[5], (ionFloat)_node.matrix.data()[6],   0.0f,
            (ionFloat)_node.matrix.data()[8], (ionFloat)_node.matrix.data()[9], (ionFloat)_node.matrix.data()[10],  0.0f,
            0.0f,                             0.0f,                             0.0f,                               1.0f);

        rotationMatrix = (1.0f / scaleFactor) * rotationMatrix;


        ionFloat trace = (ionFloat)_node.matrix.data()[0] + (ionFloat)_node.matrix.data()[5] + (ionFloat)_node.matrix.data()[10];
        if (trace > 0)
        {
            ionFloat s = 0.5f / std::sqrtf(trace + 1.0f);
            rotation = VectorHelper::Set(
                ((ionFloat)_node.matrix.data()[6] - (ionFloat)_node.matrix.data()[9]) * s, 
                ((ionFloat)_node.matrix.data()[8] - (ionFloat)_node.matrix.data()[2]) * s,
                ((ionFloat)_node.matrix.data()[1] - (ionFloat)_node.matrix.data()[4]) * s,
                0.25f / s);
        }
        else 
        {
            if ((ionFloat)_node.matrix.data()[0] > (ionFloat)_node.matrix.data()[5] && (ionFloat)_node.matrix.data()[0] > (ionFloat)_node.matrix.data()[10])
            {
                ionFloat s = 2.0f * std::sqrtf(1.0f + (ionFloat)_node.matrix.data()[0] - (ionFloat)_node.matrix.data()[5] - (ionFloat)_node.matrix.data()[10]);
                rotation = VectorHelper::Set(
                    0.25f * s,
                    ((ionFloat)_node.matrix.data()[4] + (ionFloat)_node.matrix.data()[1]) / s,
                    ((ionFloat)_node.matrix.data()[8] + (ionFloat)_node.matrix.data()[2]) / s,
                    ((ionFloat)_node.matrix.data()[6] - (ionFloat)_node.matrix.data()[9]) / s);
            }
            else if ((ionFloat)_node.matrix.data()[5] > (ionFloat)_node.matrix.data()[10])
            {
                ionFloat s = 2.0f * std::sqrtf(1.0f + (ionFloat)_node.matrix.data()[5] - (ionFloat)_node.matrix.data()[0] - (ionFloat)_node.matrix.data()[10]);
                rotation = VectorHelper::Set(
                    ((ionFloat)_node.matrix.data()[4] + (ionFloat)_node.matrix.data()[1]) / s,
                    0.25f * s,
                    ((ionFloat)_node.matrix.data()[9] + (ionFloat)_node.matrix.data()[6]) / s,
                    ((ionFloat)_node.matrix.data()[8] - (ionFloat)_node.matrix.data()[2]) / s);
            }
            else
            {
                ionFloat s = 2.0f * std::sqrtf(1.0f + (ionFloat)_node.matrix.data()[10] - (ionFloat)_node.matrix.data()[0] - (ionFloat)_node.matrix.data()[5]);
                rotation = VectorHelper::Set(
                    ((ionFloat)_node.matrix.data()[8] + (ionFloat)_node.matrix.data()[2]) / s,
                    ((ionFloat)_node.matrix.data()[9] + (ionFloat)_node.matrix.data()[6]) / s,
                    0.25f * s,
                    ((ionFloat)_node.matrix.data()[1] - (ionFloat)_node.matrix.data()[4]) / s);
            }
        }

        // TRANSLATION
        position = VectorHelper::Set((ionFloat)_node.matrix.data()[12], (ionFloat)_node.matrix.data()[13], (ionFloat)_node.matrix.data()[14], 1.0f);
    }
    else 
    {   
        if (_node.translation.size() == 3)
        {
            position = Vector((ionFloat)_node.translation[0], (ionFloat)_node.translation[1], (ionFloat)_node.translation[2], 1.0f);
        }

        Quaternion rotation;
        if (_node.rotation.size() == 4)
        {
            rotation = Quaternion((ionFloat)_node.rotation[0], (ionFloat)_node.rotation[1], (ionFloat)_node.rotation[2], (ionFloat)_node.rotation[3]);
        }

        
        if (_node.rotation.size() == 3)
        {
            scale = Vector((ionFloat)_node.scale[0], (ionFloat)_node.scale[1], (ionFloat)_node.scale[2]);
        }

        //localNodeMatrix = localNodeMatrix.Translate(position);
        //localNodeMatrix = localNodeMatrix * rotation.ToMatrix();
        //localNodeMatrix = localNodeMatrix.Scale(scale);
    }

    _entity.GetTransformHandle()->SetPosition(position);
    _entity.GetTransformHandle()->SetRotation(rotation);
    _entity.GetTransformHandle()->SetScale(scale);

    _entity.GetTransformHandle()->UpdateTransform(_parentMatrix);
    Matrix localNodeMatrix =_entity.GetTransformHandle()->GetMatrix();


    // calculate matrix for all children if any
    if (_node.children.size() > 0)
    {
        for (ionSize i = 0; i < _node.children.size(); ++i)
        {
            Entity child;
            child.AttachToParent(_entity);
            LoadNode(_model.nodes[_node.children[i]], _model, localNodeMatrix, child, _textureIndexToTextureName, _materialIndexToMaterialName, _generateNormalWhenMissing, _generateTangentWhenMissing, _setBitangentSign);
        }
    }

    ionU32 prevIndexSize = 0;
    ionU32 prevVertexSize = 0;
    if (_node.mesh > -1) 
    {
        const tinygltf::Mesh mesh = _model.meshes[_node.mesh];

        for (ionSize j = 0; j < mesh.primitives.size(); ++j)
        {
            const tinygltf::Primitive &primitive = mesh.primitives[j];
            if (primitive.indices < 0)
            {
                continue;
            }

            Mesh* ionMesh = _entity.AddMesh<Mesh>();

            ionMesh->SetIndexStart(prevIndexSize);
            ionU32 vertexStart = prevVertexSize;


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

                for (ionSize v = 0; v < posAccessor.count; v++) 
                {
                    // the Y value of all vectors is inverted due Vulkan coordinate system

                    Vertex vert;

                    Vector pos((&bufferPos[v * 3])[0], ((&bufferPos[v * 3])[1]), (&bufferPos[v * 3])[2], 1.0f);
                    pos = localNodeMatrix * pos;

                    if (_generateNormalWhenMissing || _generateTangentWhenMissing)
                    {
                        positionToBeNormalized.push_back(pos);
                    }

                    vert.SetPosition(pos);

                    _entity.GetBoundingBox().Expande(vert.GetPosition(), vert.GetPosition());
                    
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
                    normal = localNodeMatrix * normal;


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
                    tangent = localNodeMatrix * tangent;
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

                    ionMesh->PushBackVertex(vert);
                }
            }


            eosVector(Index) indexToBeUsedDuringNormalization;

            // Indices
            {
                const tinygltf::Accessor &accessor = _model.accessors[primitive.indices];
                const tinygltf::BufferView &bufferView = _model.bufferViews[accessor.bufferView];
                const tinygltf::Buffer &buffer = _model.buffers[bufferView.buffer];

                ionMesh->SetIndexCount(static_cast<ionU32>(accessor.count));

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
                        ionMesh->PushBackIndex((Index)(buf[index] + vertexStart));
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
                        ionMesh->PushBackIndex(buf[index] + vertexStart);
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
                        ionMesh->PushBackIndex((Index)(buf[index] + vertexStart));
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
                    ionMesh->GetVertex(k).SetNormal(normalsGenerated[k]);
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
                    ionMesh->GetVertex(k).SetTangent(tangentsGenerated[k]);
                    if (_setBitangentSign)
                    {
                        ionMesh->GetVertex(k).SetBiTangent(tangentsGenerated[k]);
                    }
                } 
            }

            // add material and add all to primitive
            if (_model.materials.size() > 0)
            {
                ionMesh->SetMaterial(ionMaterialManger().GetMaterial(_materialIndexToMaterialName[primitive.material]));
            }
            else
            {
                ionMesh->SetMaterial(ionMaterialManger().GetMaterial("Default"));
            }
            

            prevIndexSize = static_cast<ionU32>(ionMesh->GetIndexSize());
            prevVertexSize = static_cast<ionU32>(ionMesh->GetVertexSize());
        }

        // Bone weight for morph targets (NEXT: After the base renderer will works)
        {
        }
    }
}


ionBool LoaderGLTF::Load(const eosString & _filePath, Entity& _entity, ionBool _generateNormalWhenMissing /*= false*/, ionBool _generateTangentWhenMissing /*= false*/, ionBool _setBitangentSign /*= false*/)
{
    tinygltf::Model     _model;
    tinygltf::TinyGLTF  gltf;
    std::string         err;

    //
    if (_filePath.find_last_of('/') != std::string::npos)
    {
        m_dir = _filePath.substr(0, _filePath.find_last_of('/'));
    }

    ionAssertReturnValue(!m_dir.empty(), "Path invalid", false);

    //
    if (_filePath.rfind('\\', _filePath.length()) != std::string::npos)
    {
        m_filename = _filePath.substr(_filePath.rfind('\\', _filePath.length()) + 1, _filePath.length() - _filePath.rfind('\\', _filePath.length()));
    }
    if (m_filename.empty())
    {
        if (_filePath.rfind('/', _filePath.length()) != std::string::npos)
        {
            m_filename = _filePath.substr(_filePath.rfind('/', _filePath.length()) + 1, _filePath.length() - _filePath.rfind('/', _filePath.length()));
        }
    }

    ionAssertReturnValue(!m_filename.empty(), "Filename invalid", false);

    //
    m_filenameNoExt = m_filename.substr(0, m_filename.find_last_of('.'));

    //
    if (_filePath.find_last_of('.') != std::string::npos)
    {
        m_ext = _filePath.substr(_filePath.find_last_of('.') + 1);
    }

    ionAssertReturnValue(!m_ext.empty(), "Extension invalid", false);

    //
    ionBool ret = false;
    if (m_ext.compare("glb") == 0 || m_ext.compare("bin") == 0)         // FULL BINARY
    {
        ret = gltf.LoadBinaryFromFile(&_model, &err, _filePath.c_str());
    }
    else if (m_ext.compare("gltf") == 0)                                // FULL TEXT OR HYBRID
    {
        ret = gltf.LoadASCIIFromFile(&_model, &err, _filePath.c_str());
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
    for (ionSize i = 0; i < _model.images.size(); ++i)
    {
        if (_model.images[i].uri.empty())                    // no uri, so image could be stored in binary format
        {
            eosString name = _model.images[i].name.c_str();  // no filename, I just give you one
            if (name.empty())
            {
                eosString val = std::to_string(i).c_str();
                name = m_filenameNoExt + underscore + val;
            }

            m_textureIndexToTextureName.insert(std::pair<ionS32, eosString>((ionS32)i, name.c_str()));

            ionTextureManger().CreateTextureFromBuffer(name, _model.images[i].width, _model.images[i].height, _model.images[i].component, &_model.images[i].image[0], _model.images[i].image.size(), ETextureFilter_Default, ETextureRepeat_Repeat, ETextureUsage_RGBA, ETextureType_2D);
        }
        else
        {
            eosString val = _model.images[i].uri.c_str();
            eosString path = m_dir + backslash + val;
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

            m_textureIndexToTextureName.insert(std::pair<ionS32, eosString>((ionS32)i, filename.c_str()));

            ionTextureManger().CreateTextureFromFile(filename, path);
        }
    }

    //
    // NOTE: this part MUST be re-factored when I finish the demo. Because should be handleable from different materials
    //
    // 2. Load all materials inside the material manager
    if (_model.materials.size() > 0)
    {
        for (ionSize i = 0; i < _model.materials.size(); ++i)
        {
            const tinygltf::Material& mat = _model.materials[i];


            m_materialIndexToMaterialName.insert(std::pair<ionS32, eosString>((ionS32)i, mat.name.c_str()));

            Material* material = ionMaterialManger().CreateMaterial(mat.name.c_str(), 0u);

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
                        material->GetBasePBR().SetBaseColorTexture(ionTextureManger().GetTexture(m_textureIndexToTextureName[param.TextureIndex()]));
                    }

                    //the texture contain Metallic and Roughness in 2 different channel, red and green
                    if (key == "metallicRoughnessTexture")
                    {
                        material->GetBasePBR().SetMetalRoughnessTexture(ionTextureManger().GetTexture(m_textureIndexToTextureName[param.TextureIndex()]));
                    }
                }

                // FACTORS
                {
                    if (key == "baseColorFactor")   // this is a vector, because is the diffuse of the not metal
                    {
                        material->GetBasePBR().SetBaseColor((ionFloat)param.ColorFactor()[0], (ionFloat)param.ColorFactor()[1], (ionFloat)param.ColorFactor()[2], (ionFloat)param.ColorFactor()[3]);
                    }

                    // these 2 are metal factors, so is the color is the specific measured reflectance value at normal incidence (F0)
                    if (key == "roughnessFactor")
                    {
                        material->GetBasePBR().SetRoughnessFactor((ionFloat)param.Factor());
                    }

                    if (key == "metallicFactor")
                    {
                        material->GetBasePBR().SetMetallicFactor((ionFloat)param.Factor());
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
                        material->GetAdvancePBR().SetNormalTexture(ionTextureManger().GetTexture(m_textureIndexToTextureName[param.TextureIndex()]));
                    }

                    if (key == "occlusionTexture")
                    {
                        material->GetAdvancePBR().SetOcclusionTexture(ionTextureManger().GetTexture(m_textureIndexToTextureName[param.TextureIndex()]));
                    }

                    if (key == "emissiveTexture")
                    {
                        material->GetAdvancePBR().SetEmissiveTexture(ionTextureManger().GetTexture(m_textureIndexToTextureName[param.TextureIndex()]));
                    }
                }

                // FACTORS
                {
                    if (key == "emissiveFactor")
                    {
                        material->GetAdvancePBR().SetEmissiveColor((ionFloat)param.ColorFactor()[0], (ionFloat)param.ColorFactor()[1], (ionFloat)param.ColorFactor()[2]);
                    }
                }

                if (key == "alphaMode")
                {
                    // mesh sorting for blend
                    // this could be anything, glass for instance
                    if (param.string_value == "BLEND")
                    {
                       // material->GetState().SetColorMaskMode(EColorMask_Depth);
                        material->GetState().SetBlendStateMode(EBlendState_Source_One);
                        material->GetState().SetBlendStateMode(EBlendState_Dest_One);
                        material->GetState().SetBlendOperatorMode(EBlendOperator_Add);
                    }

                    // this would like leaves or grass
                    if (param.string_value == "MASK")
                    {
                        //material->GetState().SetColorMaskMode(EColorMask_Color);
                        material->GetState().SetBlendStateMode(EBlendState_Source_Source_Alpha);
                        material->GetState().SetBlendStateMode(EBlendState_Dest_One_Minus_Source_Alpha);
                        material->GetState().SetBlendOperatorMode(EBlendOperator_Add);
                    }

                    // is is set OPAQUE, do nothing, is the default value of rendering
                }

                // if "MASK" there is this other parameter which specifies the cutoff threshold. If the alpha value is greater than or equal to the alphaCutoff value then it is rendered as fully opaque, otherwise, it is rendered as fully transparent.
                if (key == "alphaCutoff")
                {
                    material->GetAdvancePBR().SetAlphaCutoff((ionFloat)param.Factor());
                }

                if (key == "doubleSided")
                {
                    material->GetState().SetCullingMode(ECullingMode_TwoSide);
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
        material->GetAdvancePBR().SetEmissiveColor(0.0f, 0.0f, 0.0f);
        material->GetAdvancePBR().SetAlphaCutoff(0.5f);
    }
    
    //
    // 3. Load all meshes..
    Matrix parent;
    const tinygltf::Scene &scene = _model.scenes[_model.defaultScene];
    for (ionSize i = 0; i < scene.nodes.size(); ++i)
    {
        const tinygltf::Node node = _model.nodes[scene.nodes[i]];

        LoadNode(node, _model, parent, _entity, m_textureIndexToTextureName, m_materialIndexToMaterialName, _generateNormalWhenMissing, _generateTangentWhenMissing, _setBitangentSign);
    }

    return true;
}


ION_NAMESPACE_END