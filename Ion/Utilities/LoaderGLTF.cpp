#include "LoaderGLTF.h"

#include "../Scene/Entity.h"

#include "../Texture/TextureManager.h"
#include "../Material/MaterialManager.h"

#include "../Dependencies/Eos/Eos/Eos.h"
#include "../Dependencies/Nix/Nix/Nix.h"


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
void LoadNode(const tinygltf::Node& _node, const tinygltf::Model& _model, Entity& _entity, eosMap(ionS32, eosString)& _textureIndexToTextureName, eosMap(ionS32, eosString)& _materialIndexToMaterialName)
{
    Vector position;
    if (_node.translation.size() == 3)
    {
        position = Vector((ionFloat)_node.translation[0], (ionFloat)_node.translation[1], (ionFloat)_node.translation[2]);
    }

    Quaternion rotation;
    if (_node.rotation.size() == 4)
    {
        rotation = Quaternion((ionFloat)_node.rotation[0], (ionFloat)_node.rotation[1], (ionFloat)_node.rotation[2], (ionFloat)_node.rotation[3]);
    }

    Vector scale(1.0f);
    if (_node.rotation.size() == 3)
    {
        scale = Vector((ionFloat)_node.scale[0], (ionFloat)_node.scale[1], (ionFloat)_node.scale[2]);
    }

    _entity.GetTransformHandle()->SetPosition(position);
    _entity.GetTransformHandle()->SetRotation(rotation);
    _entity.GetTransformHandle()->SetScale(scale);

    // calculate matrix for all children if any
    if (_node.children.size() > 0)
    {
        for (ionSize i = 0; i < _node.children.size(); ++i)
        {
            Entity child;
            child.AttachToParent(_entity);
            LoadNode(_model.nodes[_node.children[i]], _model, child, _textureIndexToTextureName, _materialIndexToMaterialName);
        }
    }

    ionU32 prevIndexSize = 0;
    ionU32 prevVertexSize = 0;
    for (ionSize i = 0; i < (_node.mesh + 1); ++i)
    {
        const tinygltf::Mesh mesh = _model.meshes[i];

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

			// Vertices
			{
				const ionFloat *bufferPos = nullptr;
				const ionFloat *bufferNormals = nullptr;
				const ionFloat *bufferTangent = nullptr;
				const ionFloat *bufferTexCoords = nullptr;
				const ionFloat *bufferColor0 = nullptr;
				const ionFloat *bufferColor1 = nullptr;

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
				}

				if (primitive.attributes.find("TANGENT") != primitive.attributes.end())
				{
					const tinygltf::Accessor &tangentAccessor = _model.accessors[primitive.attributes.find("TANGENT")->second];
					const tinygltf::BufferView &tangentView = _model.bufferViews[tangentAccessor.bufferView];
					bufferTangent = reinterpret_cast<const ionFloat *>(&(_model.buffers[tangentView.buffer].data[tangentAccessor.byteOffset + tangentView.byteOffset]));
				}

				if (primitive.attributes.find("TEXCOORD_0") != primitive.attributes.end()) 
				{
					const tinygltf::Accessor &uvAccessor = _model.accessors[primitive.attributes.find("TEXCOORD_0")->second];
					const tinygltf::BufferView &uvView = _model.bufferViews[uvAccessor.bufferView];
					bufferTexCoords = reinterpret_cast<const ionFloat *>(&(_model.buffers[uvView.buffer].data[uvAccessor.byteOffset + uvView.byteOffset]));
				}

				if (primitive.attributes.find("COLOR_0") != primitive.attributes.end())
				{
					const tinygltf::Accessor &color0Accessor = _model.accessors[primitive.attributes.find("COLOR_0")->second];
					const tinygltf::BufferView &color0View = _model.bufferViews[color0Accessor.bufferView];
					bufferColor0 = reinterpret_cast<const ionFloat *>(&(_model.buffers[color0View.buffer].data[color0Accessor.byteOffset + color0View.byteOffset]));
				}

				if (primitive.attributes.find("COLOR_1") != primitive.attributes.end())
				{
					const tinygltf::Accessor &color1Accessor = _model.accessors[primitive.attributes.find("COLOR_1")->second];
					const tinygltf::BufferView &color1View = _model.bufferViews[color1Accessor.bufferView];
					bufferColor1 = reinterpret_cast<const ionFloat *>(&(_model.buffers[color1View.buffer].data[color1Accessor.byteOffset + color1View.byteOffset]));
				}

				for (ionSize v = 0; v < posAccessor.count; v++) 
				{
					// the Y value of all vectors is inverted due Vulkan coordinate system

					Vertex vert;
					vert.SetPosition((&bufferPos[v * 3])[0], -((&bufferPos[v * 3])[1]), (&bufferPos[v * 3])[2]);

                    _entity.GetBoundingBox().Expande(vert.GetPosition(), vert.GetPosition());
                    
					if (bufferNormals != nullptr)
					{
						vert.SetNormal((&bufferNormals[v * 3])[0], -((&bufferNormals[v * 3])[1]), (&bufferNormals[v * 3])[2]);
					}
                    else
                    {
                        vert.SetNormal(0.0f, 0.0f, 0.0f);
                    }

					if (bufferTangent != nullptr)
					{
						vert.SetTangent((&bufferTangent[v * 3])[0], -((&bufferTangent[v * 3])[1]), (&bufferTangent[v * 3])[2]);
					}
                    else
                    {
                        vert.SetTangent(0.0f, 0.0f, 0.0f);
                    }

					if (bufferTexCoords != nullptr)
					{
						vert.SetTexCoordUV((&bufferTexCoords[v * 2])[0], (&bufferTexCoords[v * 2])[1]);
                    }
                    else
                    {
                        vert.SetTexCoordUV(0.0f, 0.0f);
                    }

					if (bufferColor0 != nullptr)
					{
						vert.SetColor1((&bufferColor0[v * 4])[0], (&bufferColor0[v * 4])[1], (&bufferColor0[v * 4])[2], (&bufferColor0[v * 4])[3]);
					}
                    else
                    {
                        vert.SetColor1(1.0f, 1.0f, 1.0f, 1.0f);
                    }

					if (bufferColor1 != nullptr)
					{
						vert.SetColor2((&bufferColor1[v * 4])[0], (&bufferColor1[v * 4])[1], (&bufferColor1[v * 4])[2], (&bufferColor1[v * 4])[3]);
					}
                    else
                    {
                        vert.SetColor2(1.0f, 1.0f, 1.0f, 1.0f);
                    }

                    ionMesh->PushBackVertex(vert);
				}
			}


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
                    ionMesh->SetIndexType(VK_INDEX_TYPE_UINT32);
                    ionU32 *buf = (ionU32 *)eosNewRaw(sizeof(ionU32) * accessor.count, ION_MEMORY_ALIGNMENT_SIZE);
                    memcpy(buf, &buffer.data[accessor.byteOffset + bufferView.byteOffset], accessor.count * sizeof(ionU32));
                    for (ionSize index = 0; index < accessor.count; index++)
                    {
                        ionMesh->PushBackIndex((Index)(buf[index] + vertexStart));
                    }
                    eosDeleteRaw(buf);
                    break;
                }

                // just this is valid
                case TINYGLTF_PARAMETER_TYPE_UNSIGNED_SHORT:
                {
                    ionMesh->SetIndexType(VK_INDEX_TYPE_UINT16);
                    ionU16 *buf = (ionU16 *)eosNewRaw(sizeof(ionU16) * accessor.count, ION_MEMORY_ALIGNMENT_SIZE);
                    memcpy(buf, &buffer.data[accessor.byteOffset + bufferView.byteOffset], accessor.count * sizeof(ionU16));
                    for (ionSize index = 0; index < accessor.count; index++)
                    {
                        ionMesh->PushBackIndex(buf[index] + vertexStart);
                    }
                    eosDeleteRaw(buf);
                    break;
                }
                case TINYGLTF_PARAMETER_TYPE_UNSIGNED_BYTE: 
                {
                    ionMesh->SetIndexType(VK_INDEX_TYPE_UINT16);
                    ionU8 *buf = (ionU8 *)eosNewRaw(sizeof(ionU8) * accessor.count, ION_MEMORY_ALIGNMENT_SIZE);
                    memcpy(buf, &buffer.data[accessor.byteOffset + bufferView.byteOffset], accessor.count * sizeof(ionU8));
                    for (ionSize index = 0; index < accessor.count; index++)
                    {
                        ionMesh->PushBackIndex((Index)(buf[index] + vertexStart));
                    }
                    eosDeleteRaw(buf);
                    break;
                }
                default:
                    ionAssertReturnVoid(false, "Index component type is not supported!");
                }
            }

            // add material and add all to primitive
            ionMesh->SetMaterial(ionMaterialManger().GetMaterial(_materialIndexToMaterialName[primitive.material]));

            prevIndexSize = static_cast<ionU32>(ionMesh->GetIndexSize());
            prevVertexSize = static_cast<ionU32>(ionMesh->GetVertexSize());

            // add to mesh list
            //_entity.GetMeshList().push_back(ionMesh);
        }

        // Bone weight for morph targets (NEXT: After the base renderer will works)
        {
        }
    }
}


ionBool LoaderGLTF::Load(const eosString & _filePath, Entity& _entity)
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

            ionTextureManger().CreateTextureFromBuffer(name, _model.images[i].width, _model.images[i].height, _model.images[i].component, &_model.images[i].image[0], _model.images[i].image.size(), ETextureFilter_Default, ETextureRepeat_Clamp, ETextureUsage_RGBA, ETextureType_2D);
        }
        else
        {
            eosString val = _model.images[i].uri.c_str();
            eosString path = m_dir + backslash + val;

            m_textureIndexToTextureName.insert(std::pair<ionS32, eosString>((ionS32)i, m_filename.c_str()));

            ionTextureManger().CreateTextureFromFile(m_filename, path);
        }
    }

    //
    // NOTE: this part MUST be re-factored when I finish the demo. Because should be handleable from different materials
    //
    // 2. Load all materials inside the material manager
    for (ionSize i = 0; i < _model.materials.size(); ++i)
    {
        const tinygltf::Material& mat = _model.materials[i];


        m_materialIndexToMaterialName.insert(std::pair<ionS32, eosString>((ionS32)i, mat.name.c_str()));

        Material* material = ionMaterialManger().CreateMaterial(mat.name.c_str(), 0u);

        for (auto const& x : mat.values)
        {
            const std::string& key = (x.first);
            const tinygltf::Parameter& param = (x.second);

            if (key == "baseColorTexture")
            {
                material->SetMetalnessMap(ionTextureManger().GetTexture(m_textureIndexToTextureName[param.TextureIndex()]));
            }

            if (key == "metallicRoughnessTexture")
            {
                material->SetRoughnessMap(ionTextureManger().GetTexture(m_textureIndexToTextureName[param.TextureIndex()]));
            }

            if (key == "roughnessFactor")
            {
                material->SetRoughnessFactor((ionFloat)param.Factor());
            }

            if (key == "metallicFactor")
            {
                material->SetMetallicFactor((ionFloat)param.Factor());
            }
        }

        for (auto const& x : mat.additionalValues)
        {
            const std::string& key = (x.first);
            const tinygltf::Parameter& param = (x.second);

            if (key == "emissiveTexture")
            {
                material->SetEmissiveMap(ionTextureManger().GetTexture(m_textureIndexToTextureName[param.TextureIndex()]));
            }
            if (key == "emissiveFactor")
            {
                material->SetEmissiveColor((ionFloat)param.ColorFactor()[0], (ionFloat)param.ColorFactor()[1], (ionFloat)param.ColorFactor()[2], (ionFloat)param.ColorFactor()[3]);
            }

            if (key == "normalTexture")
            {
                material->SetNormalMap(ionTextureManger().GetTexture(m_textureIndexToTextureName[param.TextureIndex()]));
            }
            if (key == "occlusionTexture")
            {
                material->SetOcclusionMap(ionTextureManger().GetTexture(m_textureIndexToTextureName[param.TextureIndex()]));
            }

            if (key == "alphaMode")
            {
                if (param.string_value == "BLEND") 
                {
                    material->SetColorMaskMode(EColorMask_Depth);
                    material->SetCullingMode(ECullingMode_TwoSide);
                }
                if (param.string_value == "MASK") 
                {
                    material->SetColorMaskMode(EColorMask_Color);
                    material->SetBlendStateMode(EBlendState_Source_Source_Alpha);
                    material->SetBlendStateMode(EBlendState_Dest_One_Minus_Source_Alpha);
                }
            }

            if (key == "alphaCutoff")
            {
                material->SetAlphaCutoff((ionFloat)param.Factor());
            }
        }

        //
        // 3. Load all meshes..
        const tinygltf::Scene &scene = _model.scenes[_model.defaultScene];
        for (ionSize i = 0; i < scene.nodes.size(); ++i) 
        {
            const tinygltf::Node node = _model.nodes[scene.nodes[i]];

            LoadNode(node, _model, _entity, m_textureIndexToTextureName, m_materialIndexToMaterialName);
        }

    }

    return true;
}


ION_NAMESPACE_END