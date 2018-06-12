#pragma once

#define ION_FRAME_ALLOC_ALIGNMENT                   128
#define ION_RENDER_MAX_IMAGE_PARMS                  16
#define ION_RENDER_QUERY_POOL                       16

#define ION_MAX_DESCRIPTOR_SETS                     16384
#define ION_MAX_DESCRIPTOR_UNIFORM_BUFFERS          8192
#define ION_MAX_DESCRIPTOR_IMAGE_SAMPLERS           12384
#define ION_MAX_DESCRIPTOR_SET_WRITES               32
#define ION_MAX_DESCRIPTOR_SET_UNIFORMS             48
#define ION_MAX_IMAGE_PARMAMETERS                   16
#define ION_MAX_UNIFORM_BUFFER_OBJECT_PARMAMETERS   2


ION_NAMESPACE_BEGIN

enum EVertexLayout
{
    EVertexLayout_Unknow = -1,

    EVertexLayout_Full,             // Vertex
    EVertexLayout_Pos_UV_Normal,    // VertexSimple
    EVertexLayout_Pos_Color,        // VertexColored
    EVertexLayout_Pos_UV,           // VertexUV
    EVertexLayout_Pos,              // VertexPlain

    EVertexLayout_Empty,

    EVertexLayout_Count
};

ION_NAMESPACE_END