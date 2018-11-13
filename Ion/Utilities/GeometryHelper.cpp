#include "GeometryHelper.h"

#include "../Dependencies/Eos/Eos/Eos.h"

#include "../Dependencies/Miscellaneous/mikktspace.h"

EOS_USING_NAMESPACE
NIX_USING_NAMESPACE


ION_NAMESPACE_BEGIN

//////////////////////////////////////////////////////////////////////////


void GeometryHelper::CalculateNormals(const Vector* _vectorArray, const ionU32 _vectorCount, const ionU32* _indexList, const ionU32 _indexCount, Vector* _outNormalVectorArray)
{
    for (ionU32 i = 0; i < _vectorCount; ++i)
    {
        _outNormalVectorArray[i] = Helper::Splat(0.0f);
    }

    ionU32 faceCount = _indexCount / 3;
    for (ionU32 i = 0; i < faceCount; ++i)
    {
        ionU32 ia = _indexList[i * 3];
        ionU32 ib = _indexList[i * 3 + 1];
        ionU32 ic = _indexList[i * 3 + 2];

        const Vector e1 = _vectorArray[ia] - _vectorArray[ib];
        const Vector e2 = _vectorArray[ic] - _vectorArray[ib];
        const Vector no = e1.Cross(e2);

        _outNormalVectorArray[ia] += no;
        _outNormalVectorArray[ib] += no;
        _outNormalVectorArray[ic] += no;
    }

    for (ionU32 i = 0; i < _vectorCount; ++i)
    {
        _outNormalVectorArray[i] = _outNormalVectorArray[i].Normalize3();
    }
}


// It is a very simple implementation and is not optimized, I don't care at this stage of development!
// It can be used ONLY for simple primitive geometry!!
void GeometryHelper::CalculateUVs(const Vector* _vectorArray, const ionU32 _vectorCount, Vector* _outUVUVVectorArray)
{
    for (ionU32 i = 0; i < _vectorCount; ++i)
    {
        const ionFloat x = Helper::ExtractX(_vectorArray[i]);
        const ionFloat y = Helper::ExtractY(_vectorArray[i]);
        const ionFloat z = Helper::ExtractZ(_vectorArray[i]);

        // to polar
        /*
        ionFloat radius = std::sqrtf(x * x + y * y + z * z);
        ionFloat theta = std::atan2f(y, x);
        ionFloat phi = std::acosf(z / radius);
        ionFloat len = std::sqrt(phi * phi + theta * theta);
        if (len != 0.0f)
        {
            phi = ((phi / len) + 1.0f) / 2.0f;
            theta = ((theta / len) + 1.0f) / 2.0f;
        }
        _outUVUVVectorArray[i] = Helper::Set(phi, theta, phi, theta);
        */
        ionFloat phi = std::atan2f(x, z);
        ionFloat theta = std::atan2f(std::hypot(x, z), y);
        ionFloat len = std::sqrt(phi * phi + theta * theta);

        if (len != 0.0f) 
        {
            phi = ((phi / len) + 1.0f) / 2.0f;
            theta = ((theta / len) + 1.0f) / 2.0f;
        }

        _outUVUVVectorArray[i] = Helper::Set(phi, theta, phi, theta);
    }
}


//////////////////////////////////////////////////////////////////////////
// mikktspace algorithm 
//////////////////////////////////////////////////////////////////////////


struct SMikkTSpaceXYZ
{
    ionFloat m_x;
    ionFloat m_y;
    ionFloat m_z;

    SMikkTSpaceXYZ() : m_x(0.0f), m_y(0.0f), m_z(0.0f) {}
};

struct SMikkTSpaceUV
{
    ionFloat m_u;
    ionFloat m_v;

    SMikkTSpaceUV() : m_u(0.0f), m_v(0.0f) {}
};

struct SMikkTSpaceVertex
{
    SMikkTSpaceXYZ  m_position;
    SMikkTSpaceXYZ  m_normal;
    SMikkTSpaceUV   m_uv;

    SMikkTSpaceXYZ  m_tangent;
    ionFloat        m_bitangentSign;
};

struct SMikkTSpaceFace
{
    SMikkTSpaceVertex m_vertices[3];
};

struct SMikkTSpaceConverter
{
    eosVector(SMikkTSpaceFace) m_faces;
    ionU32 m_faceCount;
};

int GetNumFaces(const SMikkTSpaceContext *x) 
{
    SMikkTSpaceConverter* converter = static_cast<SMikkTSpaceConverter*>(x->m_pUserData);
    return converter->m_faceCount;
}

int GetNumVerticesFace(const SMikkTSpaceContext *x, int f) 
{
    return 3;
}

void GetPosition(const SMikkTSpaceContext *x, float *dst, int f, int v)
{
    SMikkTSpaceConverter* converter = static_cast<SMikkTSpaceConverter*>(x->m_pUserData);
    
    dst[0] = converter->m_faces[f].m_vertices[v].m_position.m_x;
    dst[1] = converter->m_faces[f].m_vertices[v].m_position.m_y;
    dst[2] = converter->m_faces[f].m_vertices[v].m_position.m_z;
}

void GetNormal(const SMikkTSpaceContext *x, float *dst, int f, int v)
{
    SMikkTSpaceConverter* converter = static_cast<SMikkTSpaceConverter*>(x->m_pUserData);

    dst[0] = converter->m_faces[f].m_vertices[v].m_normal.m_x;
    dst[1] = converter->m_faces[f].m_vertices[v].m_normal.m_y;
    dst[2] = converter->m_faces[f].m_vertices[v].m_normal.m_z;
}

void GetTexCoord(const SMikkTSpaceContext *x, float *dst, int f, int v)
{
    SMikkTSpaceConverter* converter = static_cast<SMikkTSpaceConverter*>(x->m_pUserData);

    dst[0] = converter->m_faces[f].m_vertices[v].m_uv.m_u;
    dst[1] = converter->m_faces[f].m_vertices[v].m_uv.m_v;
}

void SetTangentSpaceBasic(
    const SMikkTSpaceContext *x,
    const float *t,
    float s,
    int f,
    int v) 
{
    SMikkTSpaceConverter* converter = static_cast<SMikkTSpaceConverter*>(x->m_pUserData);

    converter->m_faces[f].m_vertices[v].m_tangent.m_x = t[0];
    converter->m_faces[f].m_vertices[v].m_tangent.m_y = t[1];
    converter->m_faces[f].m_vertices[v].m_tangent.m_z = t[2];

    converter->m_faces[f].m_vertices[v].m_bitangentSign = s;
}

/*
// do not use, base should be fine
void SetTangentSpace(
    const SMikkTSpaceContext *x,
    const float *t,
    const float *b,
    float mag_s,
    float mag_t,
    tbool op,
    int f,
    int v) 
{
    SetTangentSpaceBasic(x, t, op != 0 ? 1.0f : -1.0f, f, v);
}
*/

void GeometryHelper::CalculateTangents(
    const Vector* _vectorArray, const Vector* _normalArray, const Vector* _textCoordUVUVArray, const ionU32 _vectorCount,   // to iterate and get the value to use 
    const ionU32* _indexList, const ionU32 _indexCount,                                                                     // to generate face (triangle)
    Vector* _outTangentVectorArray, ionFloat* _outTangentSignArray                                                          // output tangent and bi-tangent sign
)
{
    SMikkTSpaceConverter converter;

    converter.m_faceCount = _indexCount / 3;
    converter.m_faces.resize(converter.m_faceCount);

    for (ionU32 i = 0; i < converter.m_faceCount; ++i)
    {
        ionU32 index0 = _indexList[i * 3];
        ionU32 index1 = _indexList[i * 3 + 1];
        ionU32 index2 = _indexList[i * 3 + 2];

        const Vector vector0 = _vectorArray[index0];
        const Vector vector1 = _vectorArray[index1];
        const Vector vector2 = _vectorArray[index2];

        const Vector normal0 = _normalArray[index0];
        const Vector normal1 = _normalArray[index1];
        const Vector normal2 = _normalArray[index2];

        const Vector uv0 = _textCoordUVUVArray[index0];
        const Vector uv1 = _textCoordUVUVArray[index1];
        const Vector uv2 = _textCoordUVUVArray[index2];

        // 0
        {
            converter.m_faces[i].m_vertices[0].m_position.m_x = Helper::ExtractX(vector0);
            converter.m_faces[i].m_vertices[0].m_position.m_y = Helper::ExtractY(vector0);
            converter.m_faces[i].m_vertices[0].m_position.m_z = Helper::ExtractZ(vector0);

            converter.m_faces[i].m_vertices[0].m_normal.m_x = Helper::ExtractX(normal0);
            converter.m_faces[i].m_vertices[0].m_normal.m_y = Helper::ExtractY(normal0);
            converter.m_faces[i].m_vertices[0].m_normal.m_z = Helper::ExtractZ(normal0);

            converter.m_faces[i].m_vertices[0].m_uv.m_u = Helper::ExtractX(uv0);
            converter.m_faces[i].m_vertices[0].m_uv.m_v = Helper::ExtractY(uv0);
        }

        // 1
        {
            converter.m_faces[i].m_vertices[1].m_position.m_x = Helper::ExtractX(vector1);
            converter.m_faces[i].m_vertices[1].m_position.m_y = Helper::ExtractY(vector1);
            converter.m_faces[i].m_vertices[1].m_position.m_z = Helper::ExtractZ(vector1);

            converter.m_faces[i].m_vertices[1].m_normal.m_x = Helper::ExtractX(normal1);
            converter.m_faces[i].m_vertices[1].m_normal.m_y = Helper::ExtractY(normal1);
            converter.m_faces[i].m_vertices[1].m_normal.m_z = Helper::ExtractZ(normal1);

            converter.m_faces[i].m_vertices[1].m_uv.m_u = Helper::ExtractX(uv1);
            converter.m_faces[i].m_vertices[1].m_uv.m_v = Helper::ExtractY(uv1);
        }

        // 2
        {
            converter.m_faces[i].m_vertices[2].m_position.m_x = Helper::ExtractX(vector2);
            converter.m_faces[i].m_vertices[2].m_position.m_y = Helper::ExtractY(vector2);
            converter.m_faces[i].m_vertices[2].m_position.m_z = Helper::ExtractZ(vector2);

            converter.m_faces[i].m_vertices[2].m_normal.m_x = Helper::ExtractX(normal2);
            converter.m_faces[i].m_vertices[2].m_normal.m_y = Helper::ExtractY(normal2);
            converter.m_faces[i].m_vertices[2].m_normal.m_z = Helper::ExtractZ(normal2);

            converter.m_faces[i].m_vertices[2].m_uv.m_u = Helper::ExtractX(uv2);
            converter.m_faces[i].m_vertices[2].m_uv.m_v = Helper::ExtractY(uv2);
        }
    }

    SMikkTSpaceInterface logicInterface;
    logicInterface.m_getNumFaces = GetNumFaces;
    logicInterface.m_getNumVerticesOfFace = GetNumVerticesFace;
    logicInterface.m_getPosition = GetPosition;
    logicInterface.m_getNormal =GetNormal;
    logicInterface.m_getTexCoord = GetTexCoord;
    logicInterface.m_setTSpaceBasic = SetTangentSpaceBasic;
    logicInterface.m_setTSpace = nullptr;   // do not use, base should be fine

    SMikkTSpaceContext context;
    context.m_pInterface = &logicInterface;
    context.m_pUserData = &converter;

    genTangSpaceDefault(&context);

    // unroll the result
    for (ionU32 i = 0; i < _indexCount; ++i)
    {
        ionU32 index = _indexList[i];

        ionU32 f = (ionU32)std::floor(i / 3);
        ionU32 v = i % 3;

        SMikkTSpaceXYZ t = converter.m_faces[f].m_vertices[v].m_tangent;
        ionFloat s = converter.m_faces[f].m_vertices[v].m_bitangentSign;

        _outTangentVectorArray[index] = Helper::Set(t.m_x, t.m_y, t.m_z, 0.0f);
        _outTangentSignArray[index] = s;
    }
}


ION_NAMESPACE_END