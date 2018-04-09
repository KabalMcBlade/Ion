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
#define ION_VERTEX_BYTE_TO_FLOAT(x)		    ( (x) * ( ION_VERTEX_DIV_RANGE ) - 1.0f )
#define ION_VERTEX_FLOAT_TO_BYTE(x)		    eos::MathHelper::FloatToByte( ( (x) + 1.0f ) * ( ION_VERTEX_DIV_RANGE ) + 0.5f )



EOS_USING_NAMESPACE
NIX_USING_NAMESPACE


ION_NAMESPACE_BEGIN

enum EShaderStage 
{
    EShaderStage_Vertex         = 0b00000000,
    EShaderStage_Fragment       = 0b00000010,
    EShaderStage_Tessellation   = 0b00000100,
    EShaderStage_Geometry       = 0b00001000,

    EShaderStage_Mandatory      = EShaderStage_Vertex,
    EShaderStage_MandatoryAll   = EShaderStage_Vertex | EShaderStage_Fragment,
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
    EVertexMask_Position        = 0b00000000,   // position always
    EVertexMask_TextureCoord    = 0b00001000,   // UV texture coordinates
    EVertexMask_Normal          = 0b00000010,   // normal
    EVertexMask_Tangent         = 0b00001001,   // tangent so also UV and color is present
    EVertexMask_Color1          = 0b00000011,   // if color is present also the normal is included
    EVertexMask_Color2          = 0b00000100    // other color, also weight for skinning
};

enum EVertexLayout
{
    EVertexLayout_Unknow = -1,
    EVertexLayout_Vertices,
    EVertexLayout_Vertices_Plain,
    EVertexLayout_Count
};

//////////////////////////////////////////////////////////////////////////

ION_MEMORY_ALIGNMENT(ION_MEMORY_ALIGNMENT_SIZE) struct ShaderVertexLayout
{
    VkPipelineVertexInputStateCreateInfo            m_inputState;
    eosVector(VkVertexInputBindingDescription)      m_bindinggDescription;
    eosVector(VkVertexInputAttributeDescription)    m_attributegDescription;
};

//////////////////////////////////////////////////////////////////////////

// 40 bytes aligned to 16 -> 48
ION_MEMORY_ALIGNMENT(ION_MEMORY_ALIGNMENT_SIZE) struct Vertex
{
    Vector				m_position;		        // 16 bytes
    ionFloat            m_textureCoordUV[2];    // 8 bytes
    ionU8				m_normal[4];	        // 4 bytes
    ionU8				m_tangent[4];		    // 4 bytes
    ionU8				m_color1[4];		    // 4 bytes
    ionU8				m_color2[4];		    // 4 bytes: if with skinning here will be placed the weights

    Vertex()
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
        *reinterpret_cast<ionU64*>(this->m_normal) = 0x00FF8080;	// x=0, y=0, z=1
        *reinterpret_cast<ionU64*>(this->m_tangent) = 0xFF8080FF;	// x=1, y=0, z=0
        *reinterpret_cast<ionU64*>(this->m_color1) = 0;
        *reinterpret_cast<ionU64*>(this->m_color2) = 0;
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

    ION_INLINE Vector GetColor1V() const
    {
        Vector v(ION_VERTEX_BYTE_TO_FLOAT(m_color1[0]), ION_VERTEX_BYTE_TO_FLOAT(m_color1[1]), ION_VERTEX_BYTE_TO_FLOAT(m_color1[2]), ION_VERTEX_BYTE_TO_FLOAT(m_color1[3]));
        v.Normalize();
        return v;
    }

    ION_INLINE ionU64 GetColor1() const
    {
        *reinterpret_cast<const ionU64*>(this->m_color1);
    }

    ION_INLINE Vector GetColor2V() const
    {
        Vector v(ION_VERTEX_BYTE_TO_FLOAT(m_color2[0]), ION_VERTEX_BYTE_TO_FLOAT(m_color2[1]), ION_VERTEX_BYTE_TO_FLOAT(m_color2[2]), ION_VERTEX_BYTE_TO_FLOAT(m_color2[3]));
        v.Normalize();
        return v;
    }

    ION_INLINE ionU64 GetColor2() const
    {
        *reinterpret_cast<const ionU64*>(this->m_color2);
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
        m_position = Vector(_x, _y, _z);
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

    ION_INLINE void SetColor1(ionU64 _color)
    {
        *reinterpret_cast<ionU64*>(this->m_color1) = _color;
    }

    ION_INLINE void SetColor1(ionFloat _r, ionFloat _g, ionFloat _b, ionFloat _a)
    {
        MathHelper::VectorToByte(_r, _g, _b, _a, m_color1);
    }

    ION_INLINE void SetColor1(const Vector& _color)
    {
        MathHelper::VectorToByte(_color, m_color1);
    }

    ION_INLINE void SetColor2(ionU64 _color)
    {
        *reinterpret_cast<ionU64*>(this->m_color2) = _color;
    }

    ION_INLINE void SetColor2(ionFloat _r, ionFloat _g, ionFloat _b, ionFloat _a)
    {
        MathHelper::VectorToByte(_r, _g, _b, _a, m_color2);
    }

    ION_INLINE void SetColor2(const Vector& _color)
    {
        MathHelper::VectorToByte(_color, m_color2);
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

    //////////////////////////////////////////////////////////////////////////
    // FUNCTIONS

    ION_INLINE void Lerp(const Vertex& _a, const Vertex& _b, const ionFloat _t)
    {
        const Vector t(_t);

        m_position = VectorHelper::Lerp(_a.GetPosition(), _b.GetPosition(), t);

        const Vector aUVUV = _a.GetTexCoordUVUV();
        const Vector bUVUV = _b.GetTexCoordUVUV();
        const Vector lerpUV = VectorHelper::Lerp(aUVUV, bUVUV, t);
        SetTexCoordUV(VectorHelper::ExtractElement_0(lerpUV), VectorHelper::ExtractElement_1(lerpUV));
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

        m_color1[0] = (ionU8)(_a.m_color1[0] + _t * (_b.m_color1[0] - _a.m_color1[0]));
        m_color1[1] = (ionU8)(_a.m_color1[1] + _t * (_b.m_color1[1] - _a.m_color1[1]));
        m_color1[2] = (ionU8)(_a.m_color1[2] + _t * (_b.m_color1[2] - _a.m_color1[2]));
        m_color1[3] = (ionU8)(_a.m_color1[3] + _t * (_b.m_color1[3] - _a.m_color1[3]));

        m_color2[0] = (ionU8)(_a.m_color2[0] + _t * (_b.m_color2[0] - _a.m_color2[0]));
        m_color2[1] = (ionU8)(_a.m_color2[1] + _t * (_b.m_color2[1] - _a.m_color2[1]));
        m_color2[2] = (ionU8)(_a.m_color2[2] + _t * (_b.m_color2[2] - _a.m_color2[2]));
        m_color2[3] = (ionU8)(_a.m_color2[3] + _t * (_b.m_color2[3] - _a.m_color2[3]));
    }


    // 4 joints!
    ION_INLINE Vertex GetSkinnedVertex(const Matrix* _joints)
    {
        if (_joints == nullptr)
        {
            return *this;
        }

        const Matrix& j0 = _joints[m_color1[0]];
        const Matrix& j1 = _joints[m_color1[1]];
        const Matrix& j2 = _joints[m_color1[2]];
        const Matrix& j3 = _joints[m_color1[3]];

        const ionFloat w0 = m_color2[0] * ION_NORMALIZED_VERTEX_DIV_WEIGHT;
        const ionFloat w1 = m_color2[1] * ION_NORMALIZED_VERTEX_DIV_WEIGHT;
        const ionFloat w2 = m_color2[2] * ION_NORMALIZED_VERTEX_DIV_WEIGHT;
        const ionFloat w3 = m_color2[3] * ION_NORMALIZED_VERTEX_DIV_WEIGHT;


        Matrix accum = j0 * w0;
        accum += j1 * w1;
        accum += j2 * w2;
        accum += j3 * w3;

        Vertex result;
        result.m_position = accum * m_position;
        result.SetTexCoordUV(GetTexCoordU(), GetTexCoordV());
        result.SetNormal(accum * GetNormal());
        result.SetTangent(accum * GetTangent());
        result.m_tangent[3] = m_tangent[3];
        for (ionU32 i = 0; i < 4; ++i)
        {
            result.m_color1[i] = m_color1[i];
            result.m_color2[i] = m_color2[i];
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

        const Matrix& j0 = _joints[m_color1[0]];
        const Matrix& j1 = _joints[m_color1[1]];
        const Matrix& j2 = _joints[m_color1[2]];
        const Matrix& j3 = _joints[m_color1[3]];

        const ionFloat w0 = m_color2[0] * ION_NORMALIZED_VERTEX_DIV_WEIGHT;
        const ionFloat w1 = m_color2[1] * ION_NORMALIZED_VERTEX_DIV_WEIGHT;
        const ionFloat w2 = m_color2[2] * ION_NORMALIZED_VERTEX_DIV_WEIGHT;
        const ionFloat w3 = m_color2[3] * ION_NORMALIZED_VERTEX_DIV_WEIGHT;

        Matrix accum = j0 * w0;
        accum += j1 * w1;
        accum += j2 * w2;
        accum += j3 * w3;

        return accum * m_position;
    }
};


//////////////////////////////////////////////////////////////////////////

ION_MEMORY_ALIGNMENT(ION_MEMORY_ALIGNMENT_SIZE) struct PlainVertex
{
    Vector				m_position;		        // 16 bytes

    PlainVertex()
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

    ION_INLINE void Lerp(const Vertex& _a, const Vertex& _b, const ionFloat _t)
    {
        const Vector t(_t);

        m_position = VectorHelper::Lerp(_a.GetPosition(), _b.GetPosition(), t);
    }
};

ION_NAMESPACE_END