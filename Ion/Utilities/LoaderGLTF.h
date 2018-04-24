#pragma once

#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>

#include "../Core/CoreDefs.h"

#include "../Dependencies/Eos/Eos/Eos.h"

EOS_USING_NAMESPACE

ION_NAMESPACE_BEGIN

class Entity;

namespace GLTF
{
    static ionBool Load(const eosString & _fileName, VkDevice _vkDevice, Entity& _entity);
};


ION_NAMESPACE_END