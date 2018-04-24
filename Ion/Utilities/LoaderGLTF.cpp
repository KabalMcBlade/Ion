#include "LoaderGLTF.h"

#include "../Scene/Entity.h"

#include "../Dependencies/Miscellaneous/tiny_gltf.h"

#include "../Texture/TextureManager.h"


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
  

        //
        // load all the texture inside the texture manager
        for (ionSize i = 0; i < model.images.size(); ++i)
        {
            eosString path = dir + "/" + model.images[i].uri;
            ionTextureManger().CreateTextureFromFile(_vkDevice, filename, path);
        }
        return true;
    }

};


ION_NAMESPACE_END