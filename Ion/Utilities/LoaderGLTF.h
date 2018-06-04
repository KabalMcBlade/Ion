#pragma once

#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>

#include "../Core/CoreDefs.h"

#include "../Dependencies/Eos/Eos/Eos.h"


EOS_USING_NAMESPACE

ION_NAMESPACE_BEGIN

class Entity;

class LoaderGLTF
{
public:
    LoaderGLTF();
    ~LoaderGLTF();

    ionBool Load(const eosString& _fileName, VkDevice _vkDevice, Entity& _entity);

private:
    void LoadTextures();
    void LoadMaterials();

private:
    VkDevice m_vkDevice;
    eosString m_dir;
    eosString m_filename;
    eosString m_filenameNoExt;
    eosString m_ext;

    eosMap(ionS32, eosString) m_textureIndexToTextureName;
    eosMap(ionS32, eosString) m_materialIndexToMaterialName;
};


ION_NAMESPACE_END