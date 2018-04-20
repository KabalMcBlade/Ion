#include "LoaderGLTF.h"

#include "../Scene/Entity.h"

#include "../Dependencies/Miscellaneous/tiny_gltf.h"

ION_NAMESPACE_BEGIN

// static
ionBool LoadFromGLTF(const eosString & _fileName, Entity& _entity)  // just version 2
{
    //_entity.m_meshes.empty();

    tinygltf::Model     model;
    tinygltf::TinyGLTF  gltf;
    std::string         err;
    eosString           ext;

    if (_fileName.find_last_of(".") != std::string::npos)
    {
        ext = _fileName.substr(_fileName.find_last_of(".") + 1);
    }

    ionBool ret = false;
    if (ext.compare("glb") == 0)
    {
        ret = gltf.LoadBinaryFromFile(&model, &err, _fileName.c_str());
    }
    else if (ext.compare("gltf") == 0)
    {
        ret = gltf.LoadASCIIFromFile(&model, &err, _fileName.c_str());
    }
    else
    {
        ionAssertReturnValue(false, "Invalid file format extension", false);
    }

    ionAssertReturnValue(err.empty(), err.c_str(), false);
    ionAssertReturnValue(ret, "Failed to parse glTF", false);

    // PARSE HERE
    eosVector(Mesh)& meshList = _entity.GetMeshList();

    return true;
}


ION_NAMESPACE_END