#pragma once

#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>

#include "../Core/CoreDefs.h"

#include "../Dependencies/Eos/Eos/Eos.h"

#include "../Scene/Node.h"

EOS_USING_NAMESPACE

ION_NAMESPACE_BEGIN

class LoaderGLTF final
{
public:
    LoaderGLTF();
    ~LoaderGLTF();

    ionBool Load(const eosString& _fileName, ObjectHandler& _entity, ionBool _generateNormalWhenMissing = false, ionBool _generateTangentWhenMissing = false, ionBool _setBitangentSign = false);
};


ION_NAMESPACE_END