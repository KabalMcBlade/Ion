#pragma once

#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>

#include "../Core/CoreDefs.h"

#include "../Dependencies/Miscellaneous/tiny_gltf.h"
#include "../Dependencies/Eos/Eos/Eos.h"

EOS_USING_NAMESPACE

ION_NAMESPACE_BEGIN

class Entity;

namespace GLTF
{
    static ionBool Load(const eosString & _fileName, VkDevice _vkDevice, Entity& _entity);

    namespace _private
    {
        static void LoadTextures(VkDevice _vkDevice, const eosString & _fileName, const eosString& _dir, const std::vector<tinygltf::Image>& _images);
        static void LoadMaterials(VkDevice _vkDevice, const eosString & _fileName, const eosString& _dir, const std::vector<tinygltf::Image>& _images, const std::vector<tinygltf::Material>& _materials);
    }
};


ION_NAMESPACE_END