// Copyright (c) 2025-2025 Michele Condo'
// File: C:\Projects\Ion\Ion\Renderer\BaseBufferObject.cpp
// Licensed under the GPL-3.0 License. See LICENSE file in the project root for full license information.


#include "BaseBufferObject.h"



ION_NAMESPACE_BEGIN

BaseBufferObject::BaseBufferObject()
{
    m_size = 0;
    m_offsetInOtherBuffer = ION_BUFFER_OBJECT_MAPPED_FLAG;
    m_usage = EBufferUsage_Static;
    m_object = VK_NULL_HANDLE;
    m_device = VK_NULL_HANDLE;
}

ION_NAMESPACE_END
