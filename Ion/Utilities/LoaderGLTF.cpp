#include "LoaderGLTF.h"

#include "../Scene/Entity.h"

#include "../Texture/TextureManager.h"
#include "../Material/MaterialManager.h"

#define ION_GLTF_ACCESSOR_COMPONENT_TYPE_BYTE           5120
#define ION_GLTF_ACCESSOR_COMPONENT_TYPE_UNSIGNED_BYTE  5121
#define ION_GLTF_ACCESSOR_COMPONENT_TYPE_SHORT          5122
#define ION_GLTF_ACCESSOR_COMPONENT_TYPE_UNSIGNED_SHORT 5123
#define ION_GLTF_ACCESSOR_COMPONENT_TYPE_FLOAT          5126


EOS_USING_NAMESPACE


eosString& operator+(eosString & lhs, const char* rhs)
{
    eosString t(rhs);
    return lhs += t;
}

eosString& operator+(eosString & lhs, const std::string & rhs)
{
    eosString t(rhs.c_str());
    return lhs += t;
}

ION_NAMESPACE_BEGIN


namespace GLTF
{
    // static
    ionBool Load(const eosString & _filePath, VkDevice _vkDevice, Entity& _entity)
    {
        //_entity.m_meshes.empty();

        tinygltf::Model     model;
        tinygltf::TinyGLTF  gltf;
        std::string         err;
        eosString           dir;
        eosString           filename;
        eosString           ext;

        //
        if (_filePath.find_last_of('/') != std::string::npos)
        {
            dir = _filePath.substr(0, _filePath.find_last_of('/'));
        }

        ionAssertReturnValue(!dir.empty(), "Path invalid", false);

        //
        if (_filePath.rfind('\\', _filePath.length()) != std::string::npos)
        {
            filename = _filePath.substr(_filePath.rfind('\\', _filePath.length()) + 1, _filePath.length() - _filePath.rfind('\\', _filePath.length()));
        }

        ionAssertReturnValue(!filename.empty(), "Filename invalid", false);

        //
        if (_filePath.find_last_of('.') != std::string::npos)
        {
            ext = _filePath.substr(_filePath.find_last_of('.') + 1);
        }

        ionAssertReturnValue(!ext.empty(), "Extension invalid", false);

        //
        ionBool ret = false;
        if (ext.compare("glb") == 0)
        {
            ret = gltf.LoadBinaryFromFile(&model, &err, _filePath.c_str());
        }
        else if (ext.compare("gltf") == 0)
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

        // 1. Load all the texture inside the texture manager
        _private::LoadTextures(_vkDevice, filename, dir, model.images);

        // 2. Load all materials inside the material manager
        _private::LoadMaterials(_vkDevice, filename, dir, model.images, model.materials);

        return true;
    }

    namespace _private
    {
        void LoadTextures(VkDevice _vkDevice, const eosString & _fileName, const eosString& _dir, const std::vector<tinygltf::Image>& _images)
        {
            for (ionSize i = 0; i < _images.size(); ++i)
            {
                eosString path = _dir + "/" + _images[i].uri;
                ionTextureManger().CreateTextureFromFile(_vkDevice, _fileName, path);
            }
        }

        void LoadMaterials(VkDevice _vkDevice, const eosString & _fileName, const eosString& _dir, const std::vector<tinygltf::Image>& _images, const std::vector<tinygltf::Material>& _materials)
        {
            for (ionSize i = 0; i < _materials.size(); ++i)
            {
                const tinygltf::Material& material = _materials[i];

                Texture* albedoMap = nullptr;
                Texture* normalMap = nullptr;
                Texture* roughnessMap = nullptr;
                Texture* metalnessMap = nullptr;
                Texture* ambientOcclusionMap = nullptr;
                Texture* emissiveMap = nullptr;


            }
        }
    }
};


ION_NAMESPACE_END