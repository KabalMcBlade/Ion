#pragma once

#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>

#include "../Core/CoreDefs.h"

#include "../Dependencies/Eos/Eos/Eos.h"

#include "../Scene/Node.h"

EOS_USING_NAMESPACE

ION_NAMESPACE_BEGIN

class Camera;
class LoaderGLTF final
{
public:

    enum ESerializationLevel : ionU32
    {
        ESerializationLevel_Mini = 0,
        ESerializationLevel_Normal,
        ESerializationLevel_Full
    };

    LoaderGLTF();
    ~LoaderGLTF();

    ionBool Load(const eosString& _filePath, Camera* _camToUpdatePtr, ObjectHandler& _entity);
    void Dump(const eosString& _filePath, const ObjectHandler& _entity, ESerializationLevel _level = ESerializationLevel_Normal);
};


ION_NAMESPACE_END