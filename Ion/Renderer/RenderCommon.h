#pragma once


#include "../Dependencies/Eos/Eos/Eos.h"
#include "../Dependencies/Nix/Nix/Nix.h"

#include "../Core/CoreDefs.h"

#include "RenderDefs.h"



// The hardware converts a byte to a float by division with 255 and in the
// vertex programs we convert the floating-point value in the range [0, 1]
// to the range [-1, 1] by multiplying with 2 and subtracting 1.

#define ION_NORMALIZED_VERTEX_DIV_WEIGHT    0.0039215686274509803921568627450980392156862745098039215686f // 1.0f / 255.0f

#define ION_VERTEX_DIV_RANGE                0.0078431372549019607843137254901960784313725490196078431372f // 2.0f / 255.0f
#define ION_VERTEX_BYTE_TO_FLOAT(x)            ( (x) * ( ION_VERTEX_DIV_RANGE ) - 1.0f )
#define ION_VERTEX_FLOAT_TO_BYTE(x)            eos::MathHelper::FloatToByte( ( (x) + 1.0f ) * ( ION_VERTEX_DIV_RANGE ) + 0.5f )


// REMEMBER TO ADD THE ONE WITH THE HASH!
#define ION_MODEL_MATRIX_PARAM_TEXT  "model"
#define ION_VIEW_MATRIX_PARAM_TEXT   "view"
#define ION_PROJ_MATRIX_PARAM_TEXT   "proj"

#define ION_MODEL_MATRIX_PARAM_HASH  11377574333308104762
#define ION_VIEW_MATRIX_PARAM_HASH   18322600418174592600
#define ION_PROJ_MATRIX_PARAM_HASH   9615908744109468950


EOS_USING_NAMESPACE
NIX_USING_NAMESPACE


ION_NAMESPACE_BEGIN


enum EShaderStage 
{
    EShaderStage_Vertex             = 0b00000000,
    EShaderStage_Fragment           = 0b00000010,
    EShaderStage_Tessellation_Ctrl  = 0b00000100,
    EShaderStage_Tessellation_Eval  = 0b00001000,
    EShaderStage_Geometry           = 0b00010000,

    EShaderStage_Mandatory      = EShaderStage_Vertex,
    EShaderStage_MandatoryAll   = EShaderStage_Vertex | EShaderStage_Fragment,
    EShaderStage_Optional       = EShaderStage_Tessellation_Ctrl | EShaderStage_Tessellation_Eval  | EShaderStage_Geometry,

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
    EVertexMask_Position        = 0b00000000,   // position always
    EVertexMask_TextureCoord    = 0b00001000,   // UV texture coordinates
    EVertexMask_Normal          = 0b00000010,   // normal
    EVertexMask_Tangent         = 0b00001001,   // tangent so also UV and color is present
    EVertexMask_Color1          = 0b00000011,   // if color is present also the normal is included
    EVertexMask_Color2          = 0b00000100    // other color, also weight for skinning
};


//////////////////////////////////////////////////////////////////////////

ION_MEMORY_ALIGNMENT(ION_MEMORY_ALIGNMENT_SIZE) struct ShaderVertexLayout
{
    VkPipelineVertexInputStateCreateInfo            m_inputState;
    eosVector(VkVertexInputBindingDescription)      m_bindinggDescription;
    eosVector(VkVertexInputAttributeDescription)    m_attributegDescription;
};

//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////

// 56 -> 64
ION_MEMORY_ALIGNMENT(ION_MEMORY_ALIGNMENT_SIZE) struct Vertex
{
    Vector              m_position;             // 16 byte
    ionFloat            m_textureCoordUV0[2];   // 8 byte
    ionFloat            m_textureCoordUV1[2];   // 8 byte
    ionU16              m_joints[4];            // 8 byte
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
        m_position = VectorHelper::GetZero();
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

    ION_INLINE Vector GetPosition() const
    {
        return m_position;
    }

    ION_INLINE Vector GetNormal() const
    {
        Vector v(ION_VERTEX_BYTE_TO_FLOAT(m_normal[0]), ION_VERTEX_BYTE_TO_FLOAT(m_normal[1]), ION_VERTEX_BYTE_TO_FLOAT(m_normal[2]));
        v.Normalize();
        return v;
    }

    ION_INLINE Vector GetTangent() const
    {
        Vector v(ION_VERTEX_BYTE_TO_FLOAT(m_tangent[0]), ION_VERTEX_BYTE_TO_FLOAT(m_tangent[1]), ION_VERTEX_BYTE_TO_FLOAT(m_tangent[2]));
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

    ION_INLINE Vector GetBiTangent() const
    {
        const Vector& v = GetNormal();
        Vector b = v.Cross(GetTangent());
        b *= GetBiTangentSign();
        return b;
    }

    ION_INLINE Vector GetColorV() const
    {
        Vector v(ION_VERTEX_BYTE_TO_FLOAT(m_color[0]), ION_VERTEX_BYTE_TO_FLOAT(m_color[1]), ION_VERTEX_BYTE_TO_FLOAT(m_color[2]), ION_VERTEX_BYTE_TO_FLOAT(m_color[3]));
        v.Normalize();
        return v;
    }

    ION_INLINE ionU64 GetColor() const
    {
        *reinterpret_cast<const ionU64*>(this->m_color);
    }

    ION_INLINE Vector GetWeightsV() const
    {
        Vector v(ION_VERTEX_BYTE_TO_FLOAT(m_weights[0]), ION_VERTEX_BYTE_TO_FLOAT(m_weights[1]), ION_VERTEX_BYTE_TO_FLOAT(m_weights[2]), ION_VERTEX_BYTE_TO_FLOAT(m_weights[3]));
        v.Normalize();
        return v;
    }

    ION_INLINE ionU64 GetWeights() const
    {
        *reinterpret_cast<const ionU64*>(this->m_weights);
    }

    ION_INLINE Vector GetTexCoordUVUV0() const
    {
        return Vector(m_textureCoordUV0[0], m_textureCoordUV0[1], m_textureCoordUV0[0], m_textureCoordUV0[1]);
    }

    ION_INLINE Vector GetTexCoordUUVV0() const
    {
        return Vector(m_textureCoordUV0[0], m_textureCoordUV0[0], m_textureCoordUV0[1], m_textureCoordUV0[1]);
    }

    ION_INLINE Vector GetTexCoordVUVU0() const
    {
        return Vector(m_textureCoordUV0[1], m_textureCoordUV0[0], m_textureCoordUV0[1], m_textureCoordUV0[0]);
    }

    ION_INLINE ionFloat GetTexCoordU0() const
    {
        return m_textureCoordUV0[0];
    }

    ION_INLINE ionFloat GetTexCoordV0() const
    {
        return m_textureCoordUV0[1];
    }

    ION_INLINE Vector GetTexCoordUVUV1() const
    {
        return Vector(m_textureCoordUV1[0], m_textureCoordUV1[1], m_textureCoordUV1[0], m_textureCoordUV1[1]);
    }

    ION_INLINE Vector GetTexCoordUUVV1() const
    {
        return Vector(m_textureCoordUV1[0], m_textureCoordUV1[0], m_textureCoordUV1[1], m_textureCoordUV1[1]);
    }

    ION_INLINE Vector GetTexCoordVUVU1() const
    {
        return Vector(m_textureCoordUV1[1], m_textureCoordUV1[0], m_textureCoordUV1[1], m_textureCoordUV1[0]);
    }

    ION_INLINE ionFloat GetTexCoordU1() const
    {
        return m_textureCoordUV1[0];
    }

    ION_INLINE ionFloat GetTexCoordV1() const
    {
        return m_textureCoordUV1[1];
    }

    ION_INLINE ionU16 GetJoint0()
    {
        return m_joints[0];
    }

    ION_INLINE ionU16 GetJoint1()
    {
        return m_joints[1];
    }

    ION_INLINE ionU16 GetJoint2()
    {
        return m_joints[2];
    }

    ION_INLINE ionU16 GetJoint3()
    {
        return m_joints[3];
    }

    //////////////////////////////////////////////////////////////////////////
    // SETTER

    ION_INLINE void SetPosition(const Vector& _position)
    {
        m_position = _position;
    }

    ION_INLINE void SetPosition(ionFloat _x, ionFloat _y, ionFloat _z)
    {
        m_position = Vector(_x, _y, _z, 1.0f);
    }

    ION_INLINE void SetNormal(const Vector& _normal)
    {
        MathHelper::VectorToByte(_normal, m_normal);
    }

    ION_INLINE void SetNormal(ionFloat _x, ionFloat _y, ionFloat _z)
    {
        MathHelper::VectorToByte(_x, _y, _z, m_normal);
    }

    ION_INLINE void SetTangent(const Vector& _tangent)
    {
        MathHelper::VectorToByte(_tangent, m_tangent);
    }

    ION_INLINE void SetTangent(ionFloat _x, ionFloat _y, ionFloat _z)
    {
        MathHelper::VectorToByte(_x, _y, _z, m_tangent);
    }

    ION_INLINE void SetBiTangentSign(ionFloat _sign)
    {
        m_tangent[3] = (_sign < 0.0f) ? 0 : 255;
    }

    ION_INLINE void SetBiTangentSignBit(ionU8 _sign)
    {
        m_tangent[3] = _sign ? 0 : 255;
    }

    ION_INLINE void SetBiTangent(const Vector& _tangent)
    {
        const Vector& v = GetNormal();
        Vector b = v.Cross(GetTangent());
        SetBiTangentSign(VectorHelper::ExtractElement_0(b.Dot(_tangent)));
    }

    ION_INLINE void SetBiTangent(ionFloat _x, ionFloat _y, ionFloat _z)
    {
        Vector v(_x, _y, _z);
        SetBiTangent(v);
    }

    ION_INLINE void SetColor(ionU64 _color)
    {
        *reinterpret_cast<ionU64*>(this->m_color) = _color;
    }

    ION_INLINE void SetColor(ionFloat _r, ionFloat _g, ionFloat _b, ionFloat _a)
    {
        MathHelper::VectorToByte(_r, _g, _b, _a, m_color);
    }

    ION_INLINE void SetColor(const Vector& _color)
    {
        MathHelper::VectorToByte(_color, m_color);
    }

    ION_INLINE void SetWeights(ionU64 _weights)
    {
        *reinterpret_cast<ionU64*>(this->m_weights) = _weights;
    }

    ION_INLINE void SetWeights(ionFloat _x, ionFloat _y, ionFloat _z, ionFloat _w)
    {
        MathHelper::VectorToByte(_x, _y, _z, _w, m_weights);
    }

    ION_INLINE void SetWeights(const Vector& _weights)
    {
        MathHelper::VectorToByte(_weights, m_weights);
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

    ION_INLINE void SetTexCoordUV0(const Vector& _uvuv)
    {
        SetTexCoordU0(VectorHelper::ExtractElement_0(_uvuv));
        SetTexCoordV0(VectorHelper::ExtractElement_1(_uvuv));
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

    ION_INLINE void SetTexCoordUV1(const Vector& _uvuv)
    {
        SetTexCoordU1(VectorHelper::ExtractElement_0(_uvuv));
        SetTexCoordV1(VectorHelper::ExtractElement_1(_uvuv));
    }

    ION_INLINE void SetJoint0(ionU16 _joint)
    {
        m_joints[0] = _joint;
    }

    ION_INLINE void SetJoint1(ionU16 _joint)
    {
        m_joints[1] = _joint;
    }

    ION_INLINE void SetJoint2(ionU16 _joint)
    {
        m_joints[2] = _joint;
    }

    ION_INLINE void SetJoint3(ionU16 _joint)
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
        const Vector t(_t);

        m_position = VectorHelper::Lerp(_a.GetPosition(), _b.GetPosition(), t);

        const Vector aUVUV0 = _a.GetTexCoordUVUV0();
        const Vector bUVUV0 = _b.GetTexCoordUVUV0();
        const Vector lerpUV0 = VectorHelper::Lerp(aUVUV0, bUVUV0, t);
        SetTexCoordUV0(VectorHelper::ExtractElement_0(lerpUV0), VectorHelper::ExtractElement_1(lerpUV0));

        const Vector aUVUV1 = _a.GetTexCoordUVUV1();
        const Vector bUVUV1 = _b.GetTexCoordUVUV1();
        const Vector lerpUV1 = VectorHelper::Lerp(aUVUV1, bUVUV1, t);
        SetTexCoordUV1(VectorHelper::ExtractElement_0(lerpUV1), VectorHelper::ExtractElement_1(lerpUV1));
    }


    ION_INLINE void LerpAll(const Vertex& _a, const Vertex& _b, const ionFloat _t)
    {
        Lerp(_a, _b, _t);

        const Vector t(_t);

        Vector normal = VectorHelper::Lerp(_a.GetNormal(), _b.GetNormal(), t);
        Vector tangent = VectorHelper::Lerp(_a.GetTangent(), _b.GetTangent(), t);
        Vector bitangent = VectorHelper::Lerp(_a.GetBiTangent(), _b.GetBiTangent(), t);
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

        m_joints[0] = (ionU16)(_a.m_joints[0] + _t * (_b.m_joints[0] - _a.m_joints[0]));
        m_joints[1] = (ionU16)(_a.m_joints[1] + _t * (_b.m_joints[1] - _a.m_joints[1]));
        m_joints[2] = (ionU16)(_a.m_joints[2] + _t * (_b.m_joints[2] - _a.m_joints[2]));
        m_joints[3] = (ionU16)(_a.m_joints[3] + _t * (_b.m_joints[3] - _a.m_joints[3]));
    }


    // 4 joints!
    ION_INLINE Vertex GetSkinnedVertex(const Matrix* _joints)
    {
        if (_joints == nullptr)
        {
            return *this;
        }

        const Matrix& j0 = _joints[m_joints[0]];
        const Matrix& j1 = _joints[m_joints[1]];
        const Matrix& j2 = _joints[m_joints[2]];
        const Matrix& j3 = _joints[m_joints[3]];

        const ionFloat w0 = m_weights[0] * ION_NORMALIZED_VERTEX_DIV_WEIGHT;
        const ionFloat w1 = m_weights[1] * ION_NORMALIZED_VERTEX_DIV_WEIGHT;
        const ionFloat w2 = m_weights[2] * ION_NORMALIZED_VERTEX_DIV_WEIGHT;
        const ionFloat w3 = m_weights[3] * ION_NORMALIZED_VERTEX_DIV_WEIGHT;


        Matrix accum = j0 * w0;
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
    ION_INLINE Vector GetSkinnedVertexPosition(const Matrix* _joints)
    {
        if (_joints == nullptr)
        {
            return m_position;
        }

        const Matrix& j0 = _joints[m_joints[0]];
        const Matrix& j1 = _joints[m_joints[1]];
        const Matrix& j2 = _joints[m_joints[2]];
        const Matrix& j3 = _joints[m_joints[3]];

        const ionFloat w0 = m_weights[0] * ION_NORMALIZED_VERTEX_DIV_WEIGHT;
        const ionFloat w1 = m_weights[1] * ION_NORMALIZED_VERTEX_DIV_WEIGHT;
        const ionFloat w2 = m_weights[2] * ION_NORMALIZED_VERTEX_DIV_WEIGHT;
        const ionFloat w3 = m_weights[3] * ION_NORMALIZED_VERTEX_DIV_WEIGHT;

        Matrix accum = j0 * w0;
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
    Vector              m_position;             // 16 byte
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
        m_position = VectorHelper::GetZero();
        this->m_textureCoordUV[0] = 0.0f;
        this->m_textureCoordUV[1] = 0.0f;
        memset(m_normal, 0, sizeof(m_normal));
    }


    //////////////////////////////////////////////////////////////////////////
    // GETTER

    ION_INLINE Vector GetPosition() const
    {
        return m_position;
    }

    ION_INLINE Vector GetNormal() const
    {
        Vector v(ION_VERTEX_BYTE_TO_FLOAT(m_normal[0]), ION_VERTEX_BYTE_TO_FLOAT(m_normal[1]), ION_VERTEX_BYTE_TO_FLOAT(m_normal[2]));
        v.Normalize();
        return v;
    }

    ION_INLINE Vector GetTexCoordUVUV() const
    {
        return Vector(m_textureCoordUV[0], m_textureCoordUV[1], m_textureCoordUV[0], m_textureCoordUV[1]);
    }

    ION_INLINE Vector GetTexCoordUUVV() const
    {
        return Vector(m_textureCoordUV[0], m_textureCoordUV[0], m_textureCoordUV[1], m_textureCoordUV[1]);
    }

    ION_INLINE Vector GetTexCoordVUVU() const
    {
        return Vector(m_textureCoordUV[1], m_textureCoordUV[0], m_textureCoordUV[1], m_textureCoordUV[0]);
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

    ION_INLINE void SetPosition(const Vector& _position)
    {
        m_position = _position;
    }

    ION_INLINE void SetPosition(ionFloat _x, ionFloat _y, ionFloat _z)
    {
        m_position = Vector(_x, _y, _z, 1.0f);
    }

    ION_INLINE void SetNormal(const Vector& _normal)
    {
        MathHelper::VectorToByte(_normal, m_normal);
    }

    ION_INLINE void SetNormal(ionFloat _x, ionFloat _y, ionFloat _z)
    {
        MathHelper::VectorToByte(_x, _y, _z, m_normal);
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

    ION_INLINE void SetTexCoordUV(const Vector& _uvuv)
    {
        SetTexCoordU(VectorHelper::ExtractElement_0(_uvuv));
        SetTexCoordV(VectorHelper::ExtractElement_1(_uvuv));
    }

    //////////////////////////////////////////////////////////////////////////
    // FUNCTIONS

    ION_INLINE void Lerp(const VertexSimple& _a, const VertexSimple& _b, const ionFloat _t)
    {
        const Vector t(_t);

        m_position = VectorHelper::Lerp(_a.GetPosition(), _b.GetPosition(), t);

        const Vector aUVUV = _a.GetTexCoordUVUV();
        const Vector bUVUV = _b.GetTexCoordUVUV();
        const Vector lerpUV = VectorHelper::Lerp(aUVUV, bUVUV, t);
        SetTexCoordUV(VectorHelper::ExtractElement_0(lerpUV), VectorHelper::ExtractElement_1(lerpUV));
    }
};

//////////////////////////////////////////////////////////////////////////

// 20 -> 32
ION_MEMORY_ALIGNMENT(ION_MEMORY_ALIGNMENT_SIZE) struct VertexColored
{
    Vector              m_position; // 16 byte
    ionU8               m_color[4]; // 4 byte  

    VertexColored()
    {
        Clear();
    }

    ION_INLINE void Clear()
    {
        m_position = VectorHelper::GetZero();
        memset(m_color, 0, sizeof(m_color));
    }

    ION_INLINE Vector GetPosition() const
    {
        return m_position;
    }

    ION_INLINE Vector GetColorV() const
    {
        Vector v(ION_VERTEX_BYTE_TO_FLOAT(m_color[0]), ION_VERTEX_BYTE_TO_FLOAT(m_color[1]), ION_VERTEX_BYTE_TO_FLOAT(m_color[2]), ION_VERTEX_BYTE_TO_FLOAT(m_color[3]));
        v.Normalize();
        return v;
    }

    ION_INLINE ionU64 GetColor() const
    {
        *reinterpret_cast<const ionU64*>(this->m_color);
    }

    ION_INLINE void SetPosition(ionFloat _x, ionFloat _y, ionFloat _z)
    {
        m_position = Vector(_x, _y, _z, 1.0f);
    }

    ION_INLINE void SetPosition(const Vector& _position)
    {
        m_position = _position;
    }

    ION_INLINE void SetColor(ionU64 _color)
    {
        *reinterpret_cast<ionU64*>(this->m_color) = _color;
    }

    ION_INLINE void SetColor(ionFloat _r, ionFloat _g, ionFloat _b, ionFloat _a)
    {
        MathHelper::VectorToByte(_r, _g, _b, _a, m_color);
    }

    ION_INLINE void SetColor(const Vector& _color)
    {
        MathHelper::VectorToByte(_color, m_color);
    }

    ION_INLINE void Lerp(const VertexColored& _a, const VertexColored& _b, const ionFloat _t)
    {
        const Vector t(_t);

        m_position = VectorHelper::Lerp(_a.GetPosition(), _b.GetPosition(), t);
    }
};

//////////////////////////////////////////////////////////////////////////

// 24 -> 32
ION_MEMORY_ALIGNMENT(ION_MEMORY_ALIGNMENT_SIZE) struct VertexUV
{
    Vector              m_position;             // 16 byte
    ionFloat            m_textureCoordUV[2];    // 8 byte

    VertexUV()
    {
        Clear();
    }

    ION_INLINE void Clear()
    {
        m_position = VectorHelper::GetZero();
        this->m_textureCoordUV[0] = 0.0f;
        this->m_textureCoordUV[1] = 0.0f;
    }

    ION_INLINE Vector GetPosition() const
    {
        return m_position;
    }

    ION_INLINE Vector GetTexCoordUVUV() const
    {
        return Vector(m_textureCoordUV[0], m_textureCoordUV[1], m_textureCoordUV[0], m_textureCoordUV[1]);
    }

    ION_INLINE Vector GetTexCoordUUVV() const
    {
        return Vector(m_textureCoordUV[0], m_textureCoordUV[0], m_textureCoordUV[1], m_textureCoordUV[1]);
    }

    ION_INLINE Vector GetTexCoordVUVU() const
    {
        return Vector(m_textureCoordUV[1], m_textureCoordUV[0], m_textureCoordUV[1], m_textureCoordUV[0]);
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
        m_position = Vector(_x, _y, _z, 1.0f);
    }

    ION_INLINE void SetPosition(const Vector& _position)
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

    ION_INLINE void SetTexCoordUV(const Vector& _uvuv)
    {
        SetTexCoordU(VectorHelper::ExtractElement_0(_uvuv));
        SetTexCoordV(VectorHelper::ExtractElement_1(_uvuv));
    }

    ION_INLINE void Lerp(const VertexUV& _a, const VertexUV& _b, const ionFloat _t)
    {
        const Vector t(_t);

        m_position = VectorHelper::Lerp(_a.GetPosition(), _b.GetPosition(), t);
    }
};

//////////////////////////////////////////////////////////////////////////

// 16 -> 16
ION_MEMORY_ALIGNMENT(ION_MEMORY_ALIGNMENT_SIZE) struct VertexPlain
{
    Vector                m_position;   // 16 byte

    VertexPlain()
    {
        Clear();
    }

    ION_INLINE void Clear()
    {
        m_position = VectorHelper::GetZero();
    }

    ION_INLINE Vector GetPosition() const
    {
        return m_position;
    }

    ION_INLINE void SetPosition(const Vector& _position)
    {
        m_position = _position;
    }

    ION_INLINE void SetPosition(ionFloat _x, ionFloat _y, ionFloat _z)
    {
        m_position = Vector(_x, _y, _z, 1.0f);
    }

    ION_INLINE void Lerp(const Vertex& _a, const Vertex& _b, const ionFloat _t)
    {
        const Vector t(_t);

        m_position = VectorHelper::Lerp(_a.GetPosition(), _b.GetPosition(), t);
    }
};

//////////////////////////////////////////////////////////////////////////

// vertex indices
typedef ionU32 Index;


//////////////////////////////////////////////////////////////////////////

// vertCacheHandle_t packs size, offset, and frame number into 64 bits
typedef ionU64 VertexCacheHandler;

//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////




//////////////////////////////////////////////////////////////////////////

class Material;
struct DrawSurface final
{
    ionFloat            m_modelMatrix[16];
    ionFloat            m_viewMatrix[16];
    ionFloat            m_projectionMatrix[16];
    ionU32              m_indexStart;
    ionU32              m_indexCount;
    VertexCacheHandler  m_vertexCache;
    VertexCacheHandler  m_indexCache;
    VertexCacheHandler  m_jointCache;
    ionU64                m_extraGLState;
    const Material*     m_material;

    DrawSurface()
    {
        memset(&m_modelMatrix, 0, sizeof(m_modelMatrix));
        memset(&m_viewMatrix, 0, sizeof(m_viewMatrix));
        memset(&m_projectionMatrix, 0, sizeof(m_projectionMatrix));
        m_indexStart = 0;
        m_indexCount = 0;
        m_vertexCache = 0;
        m_indexCache = 0;
        m_jointCache = 0;
        m_extraGLState = 0;
        m_material = nullptr;
    }

    ~DrawSurface()
    {
        m_material = nullptr;
    }
};



ION_NAMESPACE_END