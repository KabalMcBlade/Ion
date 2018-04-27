#include "LoaderGLTF.h"

#include "../Scene/Entity.h"

#include "../Texture/TextureManager.h"
#include "../Material/MaterialManager.h"

/*
#define ION_GLTF_ACCESSOR_COMPONENT_TYPE_BYTE           5120
#define ION_GLTF_ACCESSOR_COMPONENT_TYPE_UNSIGNED_BYTE  5121
#define ION_GLTF_ACCESSOR_COMPONENT_TYPE_SHORT          5122
#define ION_GLTF_ACCESSOR_COMPONENT_TYPE_UNSIGNED_SHORT 5123
#define ION_GLTF_ACCESSOR_COMPONENT_TYPE_FLOAT          5126
*/


#define TINYGLTF_IMPLEMENTATION
// #define TINYGLTF_NOEXCEPTION // optional. disable exception handling.
#include "../Dependencies/Miscellaneous/tiny_gltf.h"

EOS_USING_NAMESPACE

ION_NAMESPACE_BEGIN

LoaderGLTF::LoaderGLTF()
{
}

LoaderGLTF::~LoaderGLTF()
{
}

ionBool LoaderGLTF::Load(const eosString & _filePath, VkDevice _vkDevice, Entity& _entity)
{
    m_vkDevice = _vkDevice;
    //_entity.m_meshes.empty();

    tinygltf::Model     model;
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
    if (m_ext.compare("glb") == 0)          // FULL BINARY
    {
        ret = gltf.LoadBinaryFromFile(&model, &err, _filePath.c_str());
    }
    else if (m_ext.compare("gltf") == 0)    // FULL TEXT OR HYBRID
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
    for (ionSize i = 0; i < model.images.size(); ++i)
    {
        ionS32 textureIndex = (ionS32)i;
        if (model.images[i].uri.empty())                    // no uri, so image could be stored in binary format
        {
            eosString name = model.images[i].name.c_str();  // no filename, I just give you one
            if (name.empty())
            {
                eosString val = std::to_string(i).c_str();
                name = m_filenameNoExt + underscore + val;
            }

            ionTextureManger().CreateTextureFromBinary(m_vkDevice, name, model.images[i].width, model.images[i].height, model.images[i].component, &model.images[i].image[0], model.images[i].image.size(), textureIndex, ETextureFilter_Default, ETextureRepeat_Clamp, ETextureUsage_LookUp_RGBA, ETextureType_2D);
        }
        else
        {
            eosString val = model.images[i].uri.c_str();
            eosString path = m_dir + backslash + val;
            ionTextureManger().CreateTextureFromFile(m_vkDevice, m_filename, path, textureIndex);
        }
    }

    //
    // 2. Load all materials inside the material manager
    for (ionSize i = 0; i < model.materials.size(); ++i)
    {
        const tinygltf::Material& mat = model.materials[i];

        Material* material = ionMaterialManger().CreateMaterial(mat.name.c_str());

        for (auto const& x : mat.values)
        {
            const std::string& key = (x.first);
            const tinygltf::Parameter& param = (x.second);

            if (key == "baseColorTexture")
            {
                material->SetMetalnessMap(ionTextureManger().GetTexture(param.TextureIndex()));
            }
            if (key == "metallicRoughnessTexture")
            {
                material->SetRoughnessMap(ionTextureManger().GetTexture(param.TextureIndex()));
            }
        }

        for (auto const& x : mat.additionalValues)
        {
            const std::string& key = (x.first);
            const tinygltf::Parameter& param = (x.second);

            if (key == "emissiveTexture")
            {
                material->SetEmissiveMap(ionTextureManger().GetTexture(param.TextureIndex()));
            }
            if (key == "emissiveFactor")
            {
                material->SetEmissiveColor((ionFloat)param.ColorFactor()[0], (ionFloat)param.ColorFactor()[1], (ionFloat)param.ColorFactor()[2], (ionFloat)param.ColorFactor()[3]);
            }

            if (key == "normalTexture")
            {
                material->SetNormalMap(ionTextureManger().GetTexture(param.TextureIndex()));
            }
            if (key == "occlusionTexture")
            {
                material->SetOcclusionMap(ionTextureManger().GetTexture(param.TextureIndex()));
            }
        }

        /*
        Texture* albedoMap = nullptr;
        Texture* normalMap = nullptr;
        Texture* roughnessMap = nullptr;
        Texture* metalnessMap = nullptr;
        Texture* ambientOcclusionMap = nullptr;
        Texture* emissiveMap = nullptr;
        */


        // OR THIS WAY
        /*
        "baseColorFactor": [1, 1, 1, 1],
        "baseColorTexture": "texture_Default_albedo_9098",
        "roughnessFactor": 0,
        "metallicFactor": 0,
        "metallicTexture": "texture_Default_MetalSmooth_21596_metallic",
        "roughnessTexture": "texture_Default_MetalSmooth_21596_roughness",
        "normalFactor": 1,
        "normalTexture": "texture_Default_normal_22700",
        "aoFactor": 1,
        "aoTexture": "texture_Default_AO_25064",
        "emissiveFactor": [1, 1, 1, 1],
        "emissiveTexture": "texture_Default_emissive_22780"
        */

        // OR THIS WAY
        /*
         "materials" : [
        {
            "emissiveFactor" : [
                1.0,
                1.0,
                1.0
            ],
            "emissiveTexture" : {
                "index" : 2
            },
            "name" : "Material_MR",
            "normalTexture" : {
                "index" : 4
            },
            "occlusionTexture" : {
                "index" : 3
            },
            "pbrMetallicRoughness" : {
                "baseColorTexture" : {
                    "index" : 0
                },
                "metallicRoughnessTexture" : {
                    "index" : 1
                }
            }
        }
        ],
        */

        // I NEED A MAP....

        /*
        if (material.values.find("pbrMetallicRoughness") != material.values.end())
        {
            */
            /*
            ionS32 textureIndex = material.values["pbrMetallicRoughness"].TextureIndex();

            ionFloat red = material.values["pbrMetallicRoughness"].ColorFactor()[0];
            ionFloat green = (ionFloat)material.values["pbrMetallicRoughness"].ColorFactor()[1];
            ionFloat blue = (ionFloat)material.values["pbrMetallicRoughness"].ColorFactor()[2];
            ionFloat alpha = (ionFloat)material.values["pbrMetallicRoughness"].ColorFactor()[3];

            ionFloat factor = (ionFloat)material.values["pbrMetallicRoughness"].Factor();
            */
            /*
            }
            */


        //const auto &fields = material.get<picojson::object>();
        //const auto &pbrTextures = fields.at("pbrMetallicRoughness").get<picojson::object>();

        //material.baseColorTexture = &textures[gltfModel.textures[mat.values["baseColorTexture"].TextureIndex()].source];
    }

    return true;
}


ION_NAMESPACE_END