// Copyright (c) 2025-2025 Michele Condo'
// File: C:\Projects\Ion\Ion\Utilities\LoaderGLTF.h
// Licensed under the GPL-3.0 License. See LICENSE file in the project root for full license information.


#pragma once

#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>

#include "../Core/CoreDefs.h"

#include "../Dependencies/Eos/Eos/Eos.h"

#include "../Scene/Node.h"

#include "../Core/MemorySettings.h"


EOS_USING_NAMESPACE

ION_NAMESPACE_BEGIN

using LoaderGLTFAllocator = MemoryAllocator<FreeListBestSearchAllocationPolicy, MultiThreadPolicy, MemoryBoundsCheck, MemoryTag, MemoryLog>;


class Camera;
class ION_DLL LoaderGLTF final
{
public:
	static LoaderGLTFAllocator* GetAllocator();

public:

    enum ESerializationLevel : ionU32
    {
        ESerializationLevel_Mini = 0,
        ESerializationLevel_Normal,
        ESerializationLevel_Full
    };

    LoaderGLTF();
    ~LoaderGLTF();

    ionBool Load(const ionString& _filePath, Camera* _camToUpdatePtr, Node*& _entity);
    void Dump(const ionString& _filePath,  Node* _entity, ESerializationLevel _level = ESerializationLevel_Normal);
};


ION_NAMESPACE_END