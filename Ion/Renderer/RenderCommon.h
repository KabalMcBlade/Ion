#pragma once

#include "../Core/CoreDefs.h"


ION_NAMESPACE_BEGIN

enum ERenderType : ionU32
{
    ERenderType_Linear = 1,
    ERenderType_DoubleBuffer = 2,
    ERenderType_TripleBuffer = 3
};


// Shader specific

enum EShaderStage 
{
    EShaderStage_Vertex         = 0b00000000,
    EShaderStage_Fragment       = 0b00000010,
    EShaderStage_Tessellation   = 0b00000100,
    EShaderStage_Geometry       = 0b00001000,

    EShaderStage_Mandatory      = EShaderStage_Vertex | EShaderStage_Fragment,
    EShaderStage_Optional       = EShaderStage_Tessellation | EShaderStage_Geometry,

    EShaderStage_All            = EShaderStage_Mandatory | EShaderStage_Optional
};

enum EShaderBinding
{
    EShaderBinding_Uniform = 0,
    EShaderBinding_Sampler,

    EShaderBinding_Count
};

enum EVertexMask
{
    EVertexMask_Position        = 0b00000000,
    EVertexMask_TextureCoord    = 0b00001000,
    EVertexMask_Normal          = 0b00000010,
    EVertexMask_Color1          = 0b00000011,
    EVertexMask_Color2          = 0b00000100,
    EVertexMask_Tangent         = 0b00001001
};

enum EVertexLayout
{
    EVertexLayout_Unknow = -1,
    EVertexLayout_Vertices,
    EVertexLayout_Vertices_Shadow,
    EVertexLayout_Vertices_Skinned,
    EVertexLayout_Count
};


ION_NAMESPACE_END