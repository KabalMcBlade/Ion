#pragma once


#include "../Dependencies/Eos/Eos/Eos.h"
#include "../Dependencies/Nix/Nix/Nix.h"

#include "../Core/CoreDefs.h"
#include "../Core/MemoryWrapper.h"

#include "RenderDefs.h"



// The hardware converts a byte to a float by division with 255 and in the
// vertex programs we convert the floating-point value in the range [0, 1]
// to the range [-1, 1] by multiplying with 2 and subtracting 1.

#define ION_NORMALIZED_VERTEX_DIV_WEIGHT    0.0039215686274509803921568627450980392156862745098039215686f // 1.0f / 255.0f

#define ION_VERTEX_DIV_RANGE                0.0078431372549019607843137254901960784313725490196078431372f // 2.0f / 255.0f
#define ION_VERTEX_BYTE_TO_FLOAT(x)            ( (x) * ( ION_VERTEX_DIV_RANGE ) - 1.0f )
#define ION_VERTEX_FLOAT_TO_BYTE(x)            eos::Utilities::FloatToByte( ( (x) + 1.0f ) * ( ION_VERTEX_DIV_RANGE ) + 0.5f )


// REMEMBER TO ADD THE ONE WITH THE HASH!
#define ION_MODEL_MATRIX_PARAM  "model"
#define ION_VIEW_MATRIX_PARAM   "view"
#define ION_PROJ_MATRIX_PARAM   "proj"

#define ION_MODEL_MATRIX_PARAM_HASH  11377574333308104762
#define ION_VIEW_MATRIX_PARAM_HASH   18322600418174592600
#define ION_PROJ_MATRIX_PARAM_HASH   9615908744109468950

// PBR
#define ION_MAIN_CAMERA_POSITION_VECTOR_PARAM       "mainCameraPos"
#define ION_DIRECTIONAL_LIGHT_DIR_VECTOR_PARAM      "directionalLight"
#define ION_DIRECTIONAL_LIGHT_COL_VECTOR_PARAM      "directionalLightColor"
#define ION_EXPOSURE_FLOAT_PARAM                    "exposure"
#define ION_GAMMA_FLOAT_PARAM                       "gamma"
#define ION_PREFILTERED_CUBE_MIP_LEVELS_FLOAT_PARAM "prefilteredCubeMipLevels"
#define ION_WEIGHTS_FLOATS_ARRAY_PARAM              "weights"

#define ION_MAIN_CAMERA_POSITION_VECTOR_PARAM_HASH          15235675884061678663
#define ION_DIRECTIONAL_LIGHT_DIR_VECTOR_PARAM_HASH         11254454282578422209
#define ION_DIRECTIONAL_LIGHT_COL_VECTOR_PARAM_HASH         8321831752619492932
#define ION_EXPOSURE_FLOAT_PARAM_HASH                       7428861124213858496
#define ION_GAMMA_FLOAT_PARAM_HASH                          2490902623560640874
#define ION_PREFILTERED_CUBE_MIP_LEVELS_FLOAT_PARAM_HASH    16846083055572138945
//#define ION_WEIGHTS_FLOATS_ARRAY_PARAM_HASH                 12774824757867200798  // can use because is an array

// ANIMATION
#define ION_MAX_WEIGHT_COUNT    8

EOS_USING_NAMESPACE
NIX_USING_NAMESPACE


ION_NAMESPACE_BEGIN


enum EFrameStatus
{
    EFrameStatus_Success = 0,
    EFrameStatus_NeedUpdate,
    EFrameStatus_Error
};

enum EFramebufferLoad
{
    EFramebufferLoad_Load = 0,
    EFramebufferLoad_Clear,
    EFramebufferLoad_Undefined
};



enum EShaderStage 
{
    EShaderStage_Vertex             = 0b00000000,
    EShaderStage_Fragment           = 0b00000010,
    EShaderStage_Tessellation_Ctrl  = 0b00000100,
    EShaderStage_Tessellation_Eval  = 0b00001000,
    EShaderStage_Geometry           = 0b00010000,
    EShaderStage_Compute            = 0b00100000,

    EShaderStage_Mandatory      = EShaderStage_Vertex,
    EShaderStage_MandatoryAll   = EShaderStage_Vertex | EShaderStage_Fragment,
    EShaderStage_Optional       = EShaderStage_Tessellation_Ctrl | EShaderStage_Tessellation_Eval  | EShaderStage_Geometry,

    EShaderStage_All            = EShaderStage_Mandatory | EShaderStage_Optional
};

enum EShaderBinding
{
    EShaderBinding_Uniform = 0,
    EShaderBinding_Sampler,
    EShaderBinding_Storage,

    EShaderBinding_Count
};

enum EVertexMask
{
    EVertexMask_Position        = 0b00000000,   // position always
    EVertexMask_TextureCoord    = 0b00001000,   // UV texture coordinates
    EVertexMask_Normal          = 0b00000010,   // normal
    EVertexMask_Tangent         = 0b00001001,   // tangent so also UV and color is present
    EVertexMask_Color1          = 0b00000011,   // if color is present also the normal is included
    EVertexMask_Color2          = 0b00000100    // other color, also weight for skinning
};


//////////////////////////////////////////////////////////////////////////

using ShaderVertexLayoutAllocator = MemoryAllocator<FreeListBestSearchAllocationPolicy, MultiThreadPolicy, MemoryBoundsCheck, MemoryTag, MemoryLog>;


ION_MEMORY_ALIGNMENT(ION_MEMORY_ALIGNMENT_SIZE) struct ShaderVertexLayout
{
	static ShaderVertexLayoutAllocator* GetAllocator();

    VkPipelineVertexInputStateCreateInfo            m_inputState;
    ionVector<VkVertexInputBindingDescription, ShaderVertexLayoutAllocator, GetAllocator>      m_bindinggDescription;
    ionVector<VkVertexInputAttributeDescription, ShaderVertexLayoutAllocator, GetAllocator>    m_attributegDescription;
};

//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////

// 64 -> 64
ION_MEMORY_ALIGNMENT(ION_MEMORY_ALIGNMENT_SIZE) struct Vertex
{
    Vector4              m_position;             // 16 byte
    ionFloat            m_textureCoordUV0[2];   // 8 byte
    ionFloat            m_textureCoordUV1[2];   // 8 byte
    ionFloat            m_joints[4];            // 16 byte
    ionU8               m_normal[4];            // 4 byte
    ionU8               m_tangent[4];           // 4 byte
    ionU8               m_color[4];             // 4 byte
    ionU8               m_weights[4];           // 4 byte

    Vertex()
    {
        Clear();
    }

    //////////////////////////////////////////////////////////////////////////
    // CLEAR

    ION_INLINE void Clear() 
    {
        m_position = Vector4(kZero);
        this->m_textureCoordUV0[0] = 0.0f;
        this->m_textureCoordUV0[1] = 0.0f;
        this->m_textureCoordUV1[0] = 0.0f;
        this->m_textureCoordUV1[1] = 0.0f;
        memset(m_joints, 0, sizeof(m_joints));
        memset(m_normal, 0, sizeof(m_normal));
        memset(m_tangent, 0, sizeof(m_tangent));
        memset(m_color, 0, sizeof(m_color));
        memset(m_weights, 0, sizeof(m_weights));
    }


    //////////////////////////////////////////////////////////////////////////
    // GETTER

    ION_INLINE Vector4 GetPosition() const
    {
        return m_position;
    }

    ION_INLINE Vector4 GetNormal() const
    {
        Vector4 v(ION_VERTEX_BYTE_TO_FLOAT(m_normal[0]), ION_VERTEX_BYTE_TO_FLOAT(m_normal[1]), ION_VERTEX_BYTE_TO_FLOAT(m_normal[2]));
        v.Normalize();
        return v;
    }

    ION_INLINE Vector4 GetTangent() const
    {
        Vector4 v(ION_VERTEX_BYTE_TO_FLOAT(m_tangent[0]), ION_VERTEX_BYTE_TO_FLOAT(m_tangent[1]), ION_VERTEX_BYTE_TO_FLOAT(m_tangent[2]));
        v.Normalize();
        return v;
    }

    ION_INLINE ionFloat GetBiTangentSign() const 
    {
        return (m_tangent[3] < 128) ? -1.0f : 1.0f;
    }

    ION_INLINE ionU8 GetBiTangentSignBit() const
    {
        return (m_tangent[3] < 128) ? 1 : 0;
    }

    ION_INLINE Vector4 GetBiTangent() const
    {
        const Vector4& v = GetNormal();
        Vector4 b = v.Cross(GetTangent());
        b *= GetBiTangentSign();
        return b;
    }

    ION_INLINE Vector4 GetColorV() const
    {
        Vector4 v(ION_VERTEX_BYTE_TO_FLOAT(m_color[0]), ION_VERTEX_BYTE_TO_FLOAT(m_color[1]), ION_VERTEX_BYTE_TO_FLOAT(m_color[2]), ION_VERTEX_BYTE_TO_FLOAT(m_color[3]));
        v.Normalize();
        return v;
    }

    ION_INLINE ionU64 GetColor() const
    {
        *reinterpret_cast<const ionU64*>(this->m_color);
    }

    ION_INLINE Vector4 GetWeightsV() const
    {
        Vector4 v(ION_VERTEX_BYTE_TO_FLOAT(m_weights[0]), ION_VERTEX_BYTE_TO_FLOAT(m_weights[1]), ION_VERTEX_BYTE_TO_FLOAT(m_weights[2]), ION_VERTEX_BYTE_TO_FLOAT(m_weights[3]));
        v.Normalize();
        return v;
    }

    ION_INLINE ionU64 GetWeights() const
    {
        *reinterpret_cast<const ionU64*>(this->m_weights);
    }

    ION_INLINE Vector4 GetTexCoordUVUV0() const
    {
        return Vector4(m_textureCoordUV0[0], m_textureCoordUV0[1], m_textureCoordUV0[0], m_textureCoordUV0[1]);
    }

    ION_INLINE Vector4 GetTexCoordUUVV0() const
    {
        return Vector4(m_textureCoordUV0[0], m_textureCoordUV0[0], m_textureCoordUV0[1], m_textureCoordUV0[1]);
    }

    ION_INLINE Vector4 GetTexCoordVUVU0() const
    {
        return Vector4(m_textureCoordUV0[1], m_textureCoordUV0[0], m_textureCoordUV0[1], m_textureCoordUV0[0]);
    }

    ION_INLINE ionFloat GetTexCoordU0() const
    {
        return m_textureCoordUV0[0];
    }

    ION_INLINE ionFloat GetTexCoordV0() const
    {
        return m_textureCoordUV0[1];
    }

    ION_INLINE Vector4 GetTexCoordUVUV1() const
    {
        return Vector4(m_textureCoordUV1[0], m_textureCoordUV1[1], m_textureCoordUV1[0], m_textureCoordUV1[1]);
    }

    ION_INLINE Vector4 GetTexCoordUUVV1() const
    {
        return Vector4(m_textureCoordUV1[0], m_textureCoordUV1[0], m_textureCoordUV1[1], m_textureCoordUV1[1]);
    }

    ION_INLINE Vector4 GetTexCoordVUVU1() const
    {
        return Vector4(m_textureCoordUV1[1], m_textureCoordUV1[0], m_textureCoordUV1[1], m_textureCoordUV1[0]);
    }

    ION_INLINE ionFloat GetTexCoordU1() const
    {
        return m_textureCoordUV1[0];
    }

    ION_INLINE ionFloat GetTexCoordV1() const
    {
        return m_textureCoordUV1[1];
    }

    ION_INLINE ionFloat GetJoint0() const
    {
        return m_joints[0];
    }

    ION_INLINE ionFloat GetJoint1() const
    {
        return m_joints[1];
    }

    ION_INLINE ionFloat GetJoint2() const
    {
        return m_joints[2];
    }

    ION_INLINE ionFloat GetJoint3() const
    {
        return m_joints[3];
    }

    //////////////////////////////////////////////////////////////////////////
    // SETTER

    ION_INLINE void SetPosition(const Vector4& _position)
    {
        m_position = _position;
    }

    ION_INLINE void SetPosition(ionFloat _x, ionFloat _y, ionFloat _z)
    {
        m_position = Vector4(_x, _y, _z, 1.0f);
    }

    ION_INLINE void SetNormal(const Vector4& _normal)
    {
        Utilities::VectorToByte(_normal, m_normal);
    }

    ION_INLINE void SetNormal(ionFloat _x, ionFloat _y, ionFloat _z)
    {
        Utilities::VectorToByte(_x, _y, _z, m_normal);
    }

    ION_INLINE void SetTangent(const Vector4& _tangent)
    {
        Utilities::VectorToByte(_tangent, m_tangent);
    }

    ION_INLINE void SetTangent(ionFloat _x, ionFloat _y, ionFloat _z)
    {
        Utilities::VectorToByte(_x, _y, _z, m_tangent);
    }

    ION_INLINE void SetBiTangentSign(ionFloat _sign)
    {
        m_tangent[3] = (_sign < 0.0f) ? 0 : 255;
    }

    ION_INLINE void SetBiTangentSignBit(ionU8 _sign)
    {
        m_tangent[3] = _sign ? 0 : 255;
    }

    ION_INLINE void SetBiTangent(const Vector4& _tangent)
    {
        const Vector4& v = GetNormal();
        Vector4 b = v.Cross(GetTangent());
        SetBiTangentSign(Helper::ExtractX(b.Dot3(_tangent)));
    }

    ION_INLINE void SetBiTangent(ionFloat _x, ionFloat _y, ionFloat _z)
    {
        Vector4 v(_x, _y, _z);
        SetBiTangent(v);
    }

    ION_INLINE void SetColor(ionU64 _color)
    {
        *reinterpret_cast<ionU64*>(this->m_color) = _color;
    }

    ION_INLINE void SetColor(ionFloat _r, ionFloat _g, ionFloat _b, ionFloat _a)
    {
        Utilities::VectorToByte(_r, _g, _b, _a, m_color);
    }

    ION_INLINE void SetColor(const Vector4& _color)
    {
        Utilities::VectorToByte(_color, m_color);
    }

    ION_INLINE void SetWeights(ionU64 _weights)
    {
        *reinterpret_cast<ionU64*>(this->m_weights) = _weights;
    }

    ION_INLINE void SetWeights(ionFloat _x, ionFloat _y, ionFloat _z, ionFloat _w)
    {
        Utilities::VectorToByte(_x, _y, _z, _w, m_weights);
    }

    ION_INLINE void SetWeights(const Vector4& _weights)
    {
        Utilities::VectorToByte(_weights, m_weights);
    }

    ION_INLINE void SetTexCoordU0(ionFloat _u)
    {
        m_textureCoordUV0[0] = _u;
    }

    ION_INLINE void SetTexCoordV0(ionFloat _v)
    {
        m_textureCoordUV0[1] = _v;
    }

    ION_INLINE void SetTexCoordUV0(ionFloat _u, ionFloat _v)
    {
        SetTexCoordU0(_u);
        SetTexCoordV0(_v);
    }

    ION_INLINE void SetTexCoordUV0(const Vector4& _uvuv)
    {
        SetTexCoordU0(Helper::ExtractX(_uvuv));
        SetTexCoordV0(Helper::ExtractY(_uvuv));
    }

    ION_INLINE void SetTexCoordU1(ionFloat _u)
    {
        m_textureCoordUV1[0] = _u;
    }

    ION_INLINE void SetTexCoordV1(ionFloat _v)
    {
        m_textureCoordUV1[1] = _v;
    }

    ION_INLINE void SetTexCoordUV1(ionFloat _u, ionFloat _v)
    {
        SetTexCoordU1(_u);
        SetTexCoordV1(_v);
    }

    ION_INLINE void SetTexCoordUV1(const Vector4& _uvuv)
    {
        SetTexCoordU1(Helper::ExtractX(_uvuv));
        SetTexCoordV1(Helper::ExtractY(_uvuv));
    }

    ION_INLINE void SetJoint0(ionFloat _joint)
    {
        m_joints[0] = _joint;
    }

    ION_INLINE void SetJoint1(ionFloat _joint)
    {
        m_joints[1] = _joint;
    }

    ION_INLINE void SetJoint2(ionFloat _joint)
    {
        m_joints[2] = _joint;
    }

    ION_INLINE void SetJoint3(ionFloat _joint)
    {
        m_joints[3] = _joint;
    }

    ION_INLINE void SetJoint(ionU16 _joint0, ionU16 _joint1, ionU16 _joint2, ionU16 _joint3)
    {
        SetJoint0(_joint0);
        SetJoint1(_joint1);
        SetJoint2(_joint2);
        SetJoint3(_joint3);
    }

    //////////////////////////////////////////////////////////////////////////
    // FUNCTIONS

    ION_INLINE void Lerp(const Vertex& _a, const Vertex& _b, const ionFloat _t)
    {
        const Vector4 t(_t);

        m_position = Helper::Lerp(_a.GetPosition(), _b.GetPosition(), t);

        const Vector4 aUVUV0 = _a.GetTexCoordUVUV0();
        const Vector4 bUVUV0 = _b.GetTexCoordUVUV0();
        const Vector4 lerpUV0 = Helper::Lerp(aUVUV0, bUVUV0, t);
        SetTexCoordUV0(Helper::ExtractX(lerpUV0), Helper::ExtractY(lerpUV0));

        const Vector4 aUVUV1 = _a.GetTexCoordUVUV1();
        const Vector4 bUVUV1 = _b.GetTexCoordUVUV1();
        const Vector4 lerpUV1 = Helper::Lerp(aUVUV1, bUVUV1, t);
        SetTexCoordUV1(Helper::ExtractX(lerpUV1), Helper::ExtractY(lerpUV1));
    }


    ION_INLINE void LerpAll(const Vertex& _a, const Vertex& _b, const ionFloat _t)
    {
        Lerp(_a, _b, _t);

        const Vector4 t(_t);

        Vector4 normal = Helper::Lerp(_a.GetNormal(), _b.GetNormal(), t);
        Vector4 tangent = Helper::Lerp(_a.GetTangent(), _b.GetTangent(), t);
        Vector4 bitangent = Helper::Lerp(_a.GetBiTangent(), _b.GetBiTangent(), t);
        normal.Normalize();
        tangent.Normalize();
        bitangent.Normalize();
        SetNormal(normal);
        SetTangent(tangent);
        SetBiTangent(bitangent);

        m_color[0] = (ionU8)(_a.m_color[0] + _t * (_b.m_color[0] - _a.m_color[0]));
        m_color[1] = (ionU8)(_a.m_color[1] + _t * (_b.m_color[1] - _a.m_color[1]));
        m_color[2] = (ionU8)(_a.m_color[2] + _t * (_b.m_color[2] - _a.m_color[2]));
        m_color[3] = (ionU8)(_a.m_color[3] + _t * (_b.m_color[3] - _a.m_color[3]));

        m_weights[0] = (ionU8)(_a.m_weights[0] + _t * (_b.m_weights[0] - _a.m_weights[0]));
        m_weights[1] = (ionU8)(_a.m_weights[1] + _t * (_b.m_weights[1] - _a.m_weights[1]));
        m_weights[2] = (ionU8)(_a.m_weights[2] + _t * (_b.m_weights[2] - _a.m_weights[2]));
        m_weights[3] = (ionU8)(_a.m_weights[3] + _t * (_b.m_weights[3] - _a.m_weights[3]));

        m_joints[0] = (ionFloat)(_a.m_joints[0] + _t * (_b.m_joints[0] - _a.m_joints[0]));
        m_joints[1] = (ionFloat)(_a.m_joints[1] + _t * (_b.m_joints[1] - _a.m_joints[1]));
        m_joints[2] = (ionFloat)(_a.m_joints[2] + _t * (_b.m_joints[2] - _a.m_joints[2]));
        m_joints[3] = (ionFloat)(_a.m_joints[3] + _t * (_b.m_joints[3] - _a.m_joints[3]));
    }


    // 4 joints!
    ION_INLINE Vertex GetSkinnedVertex(const Matrix4x4* _joints)
    {
        if (_joints == nullptr)
        {
            return *this;
        }

        const Matrix4x4& j0 = _joints[static_cast<ionU32>(m_joints[0])];
        const Matrix4x4& j1 = _joints[static_cast<ionU32>(m_joints[1])];
        const Matrix4x4& j2 = _joints[static_cast<ionU32>(m_joints[2])];
        const Matrix4x4& j3 = _joints[static_cast<ionU32>(m_joints[3])];

        const ionFloat w0 = m_weights[0] * ION_NORMALIZED_VERTEX_DIV_WEIGHT;
        const ionFloat w1 = m_weights[1] * ION_NORMALIZED_VERTEX_DIV_WEIGHT;
        const ionFloat w2 = m_weights[2] * ION_NORMALIZED_VERTEX_DIV_WEIGHT;
        const ionFloat w3 = m_weights[3] * ION_NORMALIZED_VERTEX_DIV_WEIGHT;


        Matrix4x4 accum = j0 * w0;
        accum += j1 * w1;
        accum += j2 * w2;
        accum += j3 * w3;

        Vertex result;
        result.m_position = accum * m_position;
        result.SetTexCoordUV0(GetTexCoordU0(), GetTexCoordV0());
        result.SetTexCoordUV1(GetTexCoordU1(), GetTexCoordV1());
        result.SetNormal(accum * GetNormal());
        result.SetTangent(accum * GetTangent());
        result.m_tangent[3] = m_tangent[3];
        for (ionU32 i = 0; i < 4; ++i)
        {
            result.m_joints[i] = m_joints[i];
            result.m_weights[i] = m_weights[i];
        }
        return result;
    }

    // 4 joints!
    ION_INLINE Vector4 GetSkinnedVertexPosition(const Matrix4x4* _joints)
    {
        if (_joints == nullptr)
        {
            return m_position;
        }

        const Matrix4x4& j0 = _joints[static_cast<ionU32>(m_joints[0])];
        const Matrix4x4& j1 = _joints[static_cast<ionU32>(m_joints[1])];
        const Matrix4x4& j2 = _joints[static_cast<ionU32>(m_joints[2])];
        const Matrix4x4& j3 = _joints[static_cast<ionU32>(m_joints[3])];

        const ionFloat w0 = m_weights[0] * ION_NORMALIZED_VERTEX_DIV_WEIGHT;
        const ionFloat w1 = m_weights[1] * ION_NORMALIZED_VERTEX_DIV_WEIGHT;
        const ionFloat w2 = m_weights[2] * ION_NORMALIZED_VERTEX_DIV_WEIGHT;
        const ionFloat w3 = m_weights[3] * ION_NORMALIZED_VERTEX_DIV_WEIGHT;

        Matrix4x4 accum = j0 * w0;
        accum += j1 * w1;
        accum += j2 * w2;
        accum += j3 * w3;

        return accum * m_position;
    }
};

//////////////////////////////////////////////////////////////////////////

// 28 -> 32
ION_MEMORY_ALIGNMENT(ION_MEMORY_ALIGNMENT_SIZE) struct VertexSimple
{
    Vector4              m_position;             // 16 byte
    ionFloat            m_textureCoordUV[2];    // 8 byte
    ionU8               m_normal[4];            // 4 byte
 
    VertexSimple()
    {
        Clear();
    }

    //////////////////////////////////////////////////////////////////////////
    // CLEAR

    ION_INLINE void Clear()
    {
        m_position = Vector4(kZero);
        this->m_textureCoordUV[0] = 0.0f;
        this->m_textureCoordUV[1] = 0.0f;
        memset(m_normal, 0, sizeof(m_normal));
    }


    //////////////////////////////////////////////////////////////////////////
    // GETTER

    ION_INLINE Vector4 GetPosition() const
    {
        return m_position;
    }

    ION_INLINE Vector4 GetNormal() const
    {
        Vector4 v(ION_VERTEX_BYTE_TO_FLOAT(m_normal[0]), ION_VERTEX_BYTE_TO_FLOAT(m_normal[1]), ION_VERTEX_BYTE_TO_FLOAT(m_normal[2]));
        v.Normalize();
        return v;
    }

    ION_INLINE Vector4 GetTexCoordUVUV() const
    {
        return Vector4(m_textureCoordUV[0], m_textureCoordUV[1], m_textureCoordUV[0], m_textureCoordUV[1]);
    }

    ION_INLINE Vector4 GetTexCoordUUVV() const
    {
        return Vector4(m_textureCoordUV[0], m_textureCoordUV[0], m_textureCoordUV[1], m_textureCoordUV[1]);
    }

    ION_INLINE Vector4 GetTexCoordVUVU() const
    {
        return Vector4(m_textureCoordUV[1], m_textureCoordUV[0], m_textureCoordUV[1], m_textureCoordUV[0]);
    }

    ION_INLINE ionFloat GetTexCoordU() const
    {
        return m_textureCoordUV[0];
    }

    ION_INLINE ionFloat GetTexCoordV() const
    {
        return m_textureCoordUV[1];
    }

    //////////////////////////////////////////////////////////////////////////
    // SETTER

    ION_INLINE void SetPosition(const Vector4& _position)
    {
        m_position = _position;
    }

    ION_INLINE void SetPosition(ionFloat _x, ionFloat _y, ionFloat _z)
    {
        m_position = Vector4(_x, _y, _z, 1.0f);
    }

    ION_INLINE void SetNormal(const Vector4& _normal)
    {
        Utilities::VectorToByte(_normal, m_normal);
    }

    ION_INLINE void SetNormal(ionFloat _x, ionFloat _y, ionFloat _z)
    {
        Utilities::VectorToByte(_x, _y, _z, m_normal);
    }

    ION_INLINE void SetTexCoordU(ionFloat _u)
    {
        m_textureCoordUV[0] = _u;
    }

    ION_INLINE void SetTexCoordV(ionFloat _v)
    {
        m_textureCoordUV[1] = _v;
    }

    ION_INLINE void SetTexCoordUV(ionFloat _u, ionFloat _v)
    {
        SetTexCoordU(_u);
        SetTexCoordV(_v);
    }

    ION_INLINE void SetTexCoordUV(const Vector4& _uvuv)
    {
        SetTexCoordU(Helper::ExtractX(_uvuv));
        SetTexCoordV(Helper::ExtractY(_uvuv));
    }

    //////////////////////////////////////////////////////////////////////////
    // FUNCTIONS

    ION_INLINE void Lerp(const VertexSimple& _a, const VertexSimple& _b, const ionFloat _t)
    {
        const Vector4 t(_t);

        m_position = Helper::Lerp(_a.GetPosition(), _b.GetPosition(), t);

        const Vector4 aUVUV = _a.GetTexCoordUVUV();
        const Vector4 bUVUV = _b.GetTexCoordUVUV();
        const Vector4 lerpUV = Helper::Lerp(aUVUV, bUVUV, t);
        SetTexCoordUV(Helper::ExtractX(lerpUV), Helper::ExtractY(lerpUV));
    }
};



//////////////////////////////////////////////////////////////////////////

// 20 -> 32
ION_MEMORY_ALIGNMENT(ION_MEMORY_ALIGNMENT_SIZE) struct VertexNormal
{
    Vector4              m_position;             // 16 byte
    ionU8               m_normal[4];            // 4 byte

    VertexNormal()
    {
        Clear();
    }

    //////////////////////////////////////////////////////////////////////////
    // CLEAR

    ION_INLINE void Clear()
    {
        m_position = Vector4(kZero);
        memset(m_normal, 0, sizeof(m_normal));
    }


    //////////////////////////////////////////////////////////////////////////
    // GETTER

    ION_INLINE Vector4 GetPosition() const
    {
        return m_position;
    }

    ION_INLINE Vector4 GetNormal() const
    {
        Vector4 v(ION_VERTEX_BYTE_TO_FLOAT(m_normal[0]), ION_VERTEX_BYTE_TO_FLOAT(m_normal[1]), ION_VERTEX_BYTE_TO_FLOAT(m_normal[2]));
        v.Normalize();
        return v;
    }

    //////////////////////////////////////////////////////////////////////////
    // SETTER

    ION_INLINE void SetPosition(const Vector4& _position)
    {
        m_position = _position;
    }

    ION_INLINE void SetPosition(ionFloat _x, ionFloat _y, ionFloat _z)
    {
        m_position = Vector4(_x, _y, _z, 1.0f);
    }

    ION_INLINE void SetNormal(const Vector4& _normal)
    {
        Utilities::VectorToByte(_normal, m_normal);
    }

    ION_INLINE void SetNormal(ionFloat _x, ionFloat _y, ionFloat _z)
    {
        Utilities::VectorToByte(_x, _y, _z, m_normal);
    }

    //////////////////////////////////////////////////////////////////////////
    // FUNCTIONS

    ION_INLINE void Lerp(const VertexNormal& _a, const VertexNormal& _b, const ionFloat _t)
    {
        const Vector4 t(_t);
        m_position = Helper::Lerp(_a.GetPosition(), _b.GetPosition(), t);
    }
};


//////////////////////////////////////////////////////////////////////////

// 20 -> 32
ION_MEMORY_ALIGNMENT(ION_MEMORY_ALIGNMENT_SIZE) struct VertexColored
{
    Vector4              m_position; // 16 byte
    ionU8               m_color[4]; // 4 byte  

    VertexColored()
    {
        Clear();
    }

    ION_INLINE void Clear()
    {
        m_position = Vector4(kZero);
        memset(m_color, 0, sizeof(m_color));
    }

    ION_INLINE Vector4 GetPosition() const
    {
        return m_position;
    }

    ION_INLINE Vector4 GetColorV() const
    {
        Vector4 v(ION_VERTEX_BYTE_TO_FLOAT(m_color[0]), ION_VERTEX_BYTE_TO_FLOAT(m_color[1]), ION_VERTEX_BYTE_TO_FLOAT(m_color[2]), ION_VERTEX_BYTE_TO_FLOAT(m_color[3]));
        v.Normalize();
        return v;
    }

    ION_INLINE ionU64 GetColor() const
    {
        *reinterpret_cast<const ionU64*>(this->m_color);
    }

    ION_INLINE void SetPosition(ionFloat _x, ionFloat _y, ionFloat _z)
    {
        m_position = Vector4(_x, _y, _z, 1.0f);
    }

    ION_INLINE void SetPosition(const Vector4& _position)
    {
        m_position = _position;
    }

    ION_INLINE void SetColor(ionU64 _color)
    {
        *reinterpret_cast<ionU64*>(this->m_color) = _color;
    }

    ION_INLINE void SetColor(ionFloat _r, ionFloat _g, ionFloat _b, ionFloat _a)
    {
        Utilities::VectorToByte(_r, _g, _b, _a, m_color);
    }

    ION_INLINE void SetColor(const Vector4& _color)
    {
        Utilities::VectorToByte(_color, m_color);
    }

    ION_INLINE void Lerp(const VertexColored& _a, const VertexColored& _b, const ionFloat _t)
    {
        const Vector4 t(_t);

        m_position = Helper::Lerp(_a.GetPosition(), _b.GetPosition(), t);
    }
};

//////////////////////////////////////////////////////////////////////////

// 24 -> 32
ION_MEMORY_ALIGNMENT(ION_MEMORY_ALIGNMENT_SIZE) struct VertexUV
{
    Vector4              m_position;             // 16 byte
    ionFloat            m_textureCoordUV[2];    // 8 byte

    VertexUV()
    {
        Clear();
    }

    ION_INLINE void Clear()
    {
        m_position = Vector4(kZero);
        this->m_textureCoordUV[0] = 0.0f;
        this->m_textureCoordUV[1] = 0.0f;
    }

    ION_INLINE Vector4 GetPosition() const
    {
        return m_position;
    }

    ION_INLINE Vector4 GetTexCoordUVUV() const
    {
        return Vector4(m_textureCoordUV[0], m_textureCoordUV[1], m_textureCoordUV[0], m_textureCoordUV[1]);
    }

    ION_INLINE Vector4 GetTexCoordUUVV() const
    {
        return Vector4(m_textureCoordUV[0], m_textureCoordUV[0], m_textureCoordUV[1], m_textureCoordUV[1]);
    }

    ION_INLINE Vector4 GetTexCoordVUVU() const
    {
        return Vector4(m_textureCoordUV[1], m_textureCoordUV[0], m_textureCoordUV[1], m_textureCoordUV[0]);
    }

    ION_INLINE ionFloat GetTexCoordU() const
    {
        return m_textureCoordUV[0];
    }

    ION_INLINE ionFloat GetTexCoordV() const
    {
        return m_textureCoordUV[1];
    }

    ION_INLINE void SetPosition(ionFloat _x, ionFloat _y, ionFloat _z)
    {
        m_position = Vector4(_x, _y, _z, 1.0f);
    }

    ION_INLINE void SetPosition(const Vector4& _position)
    {
        m_position = _position;
    }

    ION_INLINE void SetTexCoordU(ionFloat _u)
    {
        m_textureCoordUV[0] = _u;
    }

    ION_INLINE void SetTexCoordV(ionFloat _v)
    {
        m_textureCoordUV[1] = _v;
    }

    ION_INLINE void SetTexCoordUV(ionFloat _u, ionFloat _v)
    {
        SetTexCoordU(_u);
        SetTexCoordV(_v);
    }

    ION_INLINE void SetTexCoordUV(const Vector4& _uvuv)
    {
        SetTexCoordU(Helper::ExtractX(_uvuv));
        SetTexCoordV(Helper::ExtractY(_uvuv));
    }

    ION_INLINE void Lerp(const VertexUV& _a, const VertexUV& _b, const ionFloat _t)
    {
        const Vector4 t(_t);

        m_position = Helper::Lerp(_a.GetPosition(), _b.GetPosition(), t);
    }
};

//////////////////////////////////////////////////////////////////////////

// 16 -> 16
ION_MEMORY_ALIGNMENT(ION_MEMORY_ALIGNMENT_SIZE) struct VertexPlain
{
    Vector4                m_position;   // 16 byte

    VertexPlain()
    {
        Clear();
    }

    ION_INLINE void Clear()
    {
        m_position = Vector4(kZero);
    }

    ION_INLINE Vector4 GetPosition() const
    {
        return m_position;
    }

    ION_INLINE void SetPosition(const Vector4& _position)
    {
        m_position = _position;
    }

    ION_INLINE void SetPosition(ionFloat _x, ionFloat _y, ionFloat _z)
    {
        m_position = Vector4(_x, _y, _z, 1.0f);
    }

    ION_INLINE void Lerp(const Vertex& _a, const Vertex& _b, const ionFloat _t)
    {
        const Vector4 t(_t);

        m_position = Helper::Lerp(_a.GetPosition(), _b.GetPosition(), t);
    }
};

//////////////////////////////////////////////////////////////////////////

// THIS IS A SPECIAL VERTEX NOT ADDED TO THE VERTEX CREATION:
// IT IS USED FOR MORPH TARGET DYNAMICALLY!

// 24 -> 32
ION_MEMORY_ALIGNMENT(ION_MEMORY_ALIGNMENT_SIZE) struct VertexMorphTarget
{
    Vector4              m_position;     // 16 byte
    ionU8               m_normal[4];    // 4 byte
    ionU8               m_tangent[4];   // 4 byte

    VertexMorphTarget()
    {
        Clear();
    }

    ION_INLINE void Clear()
    {
        m_position = Vector4(kZero);
        memset(m_normal, 0, sizeof(m_normal));
        memset(m_tangent, 0, sizeof(m_tangent));
    }

    // Getters

    ION_INLINE Vector4 GetPosition() const
    {
        return m_position;
    }

    ION_INLINE Vector4 GetNormal() const
    {
        Vector4 v(ION_VERTEX_BYTE_TO_FLOAT(m_normal[0]), ION_VERTEX_BYTE_TO_FLOAT(m_normal[1]), ION_VERTEX_BYTE_TO_FLOAT(m_normal[2]));
        v.Normalize();
        return v;
    }

    ION_INLINE Vector4 GetTangent() const
    {
        Vector4 v(ION_VERTEX_BYTE_TO_FLOAT(m_tangent[0]), ION_VERTEX_BYTE_TO_FLOAT(m_tangent[1]), ION_VERTEX_BYTE_TO_FLOAT(m_tangent[2]));
        v.Normalize();
        return v;
    }

    // Setters

    ION_INLINE void SetPosition(const Vector4& _position)
    {
        m_position = _position;
    }

    ION_INLINE void SetPosition(ionFloat _x, ionFloat _y, ionFloat _z)
    {
        m_position = Vector4(_x, _y, _z, 1.0f);
    }

    ION_INLINE void SetNormal(const Vector4& _normal)
    {
        Utilities::VectorToByte(_normal, m_normal);
    }

    ION_INLINE void SetNormal(ionFloat _x, ionFloat _y, ionFloat _z)
    {
        Utilities::VectorToByte(_x, _y, _z, m_normal);
    }

    ION_INLINE void SetTangent(const Vector4& _tangent)
    {
        Utilities::VectorToByte(_tangent, m_tangent);
    }

    ION_INLINE void SetTangent(ionFloat _x, ionFloat _y, ionFloat _z)
    {
        Utilities::VectorToByte(_x, _y, _z, m_tangent);
    }

    ION_INLINE void SetBiTangentSign(ionFloat _sign)
    {
        m_tangent[3] = (_sign < 0.0f) ? 0 : 255;
    }

    ION_INLINE void SetBiTangentSignBit(ionU8 _sign)
    {
        m_tangent[3] = _sign ? 0 : 255;
    }

    ION_INLINE void SetBiTangent(const Vector4& _tangent)
    {
        const Vector4& v = GetNormal();
        Vector4 b = v.Cross(GetTangent());
        SetBiTangentSign(Helper::ExtractX(b.Dot3(_tangent)));
    }

    ION_INLINE void SetBiTangent(ionFloat _x, ionFloat _y, ionFloat _z)
    {
        Vector4 v(_x, _y, _z);
        SetBiTangent(v);
    }

    // Functions

    ION_INLINE void Lerp(const Vertex& _a, const Vertex& _b, const ionFloat _t)
    {
        const Vector4 t(_t);

        m_position = Helper::Lerp(_a.GetPosition(), _b.GetPosition(), t);
    }

    ION_INLINE void LerpAll(const Vertex& _a, const Vertex& _b, const ionFloat _t)
    {
        Lerp(_a, _b, _t);

        const Vector4 t(_t);

        Vector4 normal = Helper::Lerp(_a.GetNormal(), _b.GetNormal(), t);
        Vector4 tangent = Helper::Lerp(_a.GetTangent(), _b.GetTangent(), t);
        Vector4 bitangent = Helper::Lerp(_a.GetBiTangent(), _b.GetBiTangent(), t);
        normal.Normalize();
        tangent.Normalize();
        bitangent.Normalize();
        SetNormal(normal);
        SetTangent(tangent);
        SetBiTangent(bitangent);
    }
};



//////////////////////////////////////////////////////////////////////////

// vertex indices
typedef ionU32 ION_MEMORY_ALIGNMENT(ION_MEMORY_ALIGNMENT_SIZE) Index;


//////////////////////////////////////////////////////////////////////////

// vertCacheHandle_t packs size, offset, and frame number into 64 bits
typedef ionU64 VertexCacheHandler;

//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////




//////////////////////////////////////////////////////////////////////////

class Material;
class Node;

struct DrawSurface final
{
    Matrix4x4              m_modelMatrix;
    Matrix4x4              m_viewMatrix;
    Matrix4x4              m_projectionMatrix;
    Vector4              m_mainCameraPos;
    Vector4              m_directionalLight;
    Vector4              m_directionalLightColor;
    ionU64              m_extraGLState;
    VertexCacheHandler  m_vertexCache;
    VertexCacheHandler  m_indexCache;
    VertexCacheHandler  m_jointCache;
    Node*               m_nodeRef;
    Material*           m_material;
    ionU32              m_indexStart;
    ionU32              m_indexCount;
    ionU32              m_meshIndexRef;
    ionFloat            m_exposure;
    ionFloat            m_gamma;
    ionFloat            m_prefilteredCubeMipLevels;
    ionU8               m_sortingIndex; // 0 = opaque, 1 = mask, 2 = blend
    ionBool             m_visible;

    void Clear()
    {
        m_nodeRef = nullptr;
        m_material = nullptr;
    }

    DrawSurface()
    {
        m_vertexCache = 0;
        m_indexCache = 0;
        m_jointCache = 0;
        m_extraGLState = 0;
        m_exposure = 4.5f;
        m_gamma = 2.2f;
        m_prefilteredCubeMipLevels = 10.0f;
        m_nodeRef = nullptr;
        m_visible = true;
        m_indexStart = 0;
        m_indexCount = 0;
        m_material = nullptr;
        m_sortingIndex = 0;
        m_meshIndexRef = 0;
    }

    ~DrawSurface()
    {
        Clear();
    }

    ionBool operator <(const DrawSurface& _other) const
    {
        return (m_sortingIndex < _other.m_sortingIndex);
    }
};

ION_NAMESPACE_END