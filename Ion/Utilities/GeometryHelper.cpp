#include "GeometryHelper.h"

#include "../Dependencies/Eos/Eos/Eos.h"


EOS_USING_NAMESPACE
NIX_USING_NAMESPACE


ION_NAMESPACE_BEGIN


Vector GeometryHelper::CalculateAvarageVector(const Vector* _vectorArray, const ionU32 _count)
{
    Vector vResult = VectorHelper::GetZero();

    for (ionU32 i = 0; i < _count; ++i)
    {
        vResult += _vectorArray[i];
    }

    ionFloat scale = 1.0f / _count;

    vResult *= scale;

    return vResult;
}

Vector GeometryHelper::CalculateSurfaceNormalTriangle(const Vector* _vectorArray, const ionU32 _index)
{
    const Vector v1 = _vectorArray[(_index + 1) % 3] - _vectorArray[_index];
    const Vector v2 = _vectorArray[(_index + 2) % 3] - _vectorArray[_index];
    Vector vSurfaceNormal = v1.Cross(v2);
    vSurfaceNormal.Normalize3();
    return vSurfaceNormal;
}

Vector GeometryHelper::CalculateSurfaceNormalQuad(const Vector* _vectorArray)
{
    const Vector vA1 = _vectorArray[1] - _vectorArray[0];
    const Vector vA2 = _vectorArray[2] - _vectorArray[0];
    const Vector vB1 = _vectorArray[3] - _vectorArray[2];
    const Vector vB2 = _vectorArray[0] - _vectorArray[2];

    Vector vSurfaceNormals[2] = { vA1.Cross(vA2), vB1.Cross(vB2) };

    Vector vSurfaceNormal = CalculateAvarageVector(vSurfaceNormals, 2);

    vSurfaceNormal.Normalize3();
    return vSurfaceNormal;
}

#define GEOMETRY_HELPER_INVALID_INDEX (ionU32)-1
void GeometryHelper::CalculateNormalPerVertex(const Vector* _vectorArray, const ionU32* _indexList, const ionU32 _indexCount, Vector* _outNormalVectorArray)
{
    //
    // Internal declaration
    struct _Edge
    {
        ionU32 u;
        ionU32 v;

        _Edge()
        {
            u = GEOMETRY_HELPER_INVALID_INDEX;
            v = GEOMETRY_HELPER_INVALID_INDEX;
        }

        void Set(ionU32 _u, ionU32 _v)
        {
            u = _u;
            v = _v;
        }
    };

    struct _Face
    {
        _Edge e[3];

        _Face() {}
        _Face(ionU32 _a, ionU32 _b, ionU32 _c)
        {
            e[0].Set(_a, _b);
            e[1].Set(_b, _c);
            e[2].Set(_c, _a);
        }
    };


    //
    ionU32 triangleCount = _indexCount / 3;
    eosVector(_Face*) faces;

    // create faces
    for (ionU32 i = 0; i < triangleCount; ++i)
    {
        faces.push_back(eosNew(_Face, ION_MEMORY_ALIGNMENT_SIZE, _indexList[i * 3], _indexList[i * 3 + 1], _indexList[i * 3 + 2]));
    }

    eosVector(Vector) normals;
    for (ionU32 i = 0; i < _indexCount; ++i)
    {
        //Vector normal = VectorHelper::GetZero();
        for (ionU32 j = 0; j < faces.size(); ++j)
        {
            if (faces[j]->e[0].u == i || faces[j]->e[0].v == i || faces[j]->e[1].u == i || faces[j]->e[1].v == i || faces[j]->e[2].u == i || faces[j]->e[2].v == i)
            {
                //normal += CalculateSurfaceNormalTriangle(_vectorArray, _indexList[i]);
                normals.push_back(CalculateSurfaceNormalTriangle(_vectorArray, _indexList[i]));
            }
        }

        //_outNormalVectorArray[_indexList[i]] = normal;
        _outNormalVectorArray[_indexList[i]] = CalculateAvarageVector(normals.data(), static_cast<ionU32>(normals.size()));
    }

    //
    // Clear
    for (ionU32 i = 0; i < faces.size(); ++i)
    {
        eosDelete(faces[i]);
    }
    faces.clear();
}

//
// THIS FUNCTION IS NOT OPTIMIZED! DO NOT A RUNTIME!
// FROM:
// http://www.terathon.com/code/tangent.html
void GeometryHelper::CalculateTangent(
    const Vector* _vectorArray, const Vector* _normalArray, const Vector* _textCoordUVUVArray, const ionU32 _vectorCount,   /* to iterate and get the value to use */
    const ionU32* _indexList, const ionU32 _indexCount,                                                                     /* to generate face (triangle) */
    Vector* _outTangentVectorArray                                                                                          /* output tangent */
)
{
    //
    // Internal declaration
    struct _Edge
    {
        ionU32 u;
        ionU32 v;

        _Edge()
        {
            u = GEOMETRY_HELPER_INVALID_INDEX;
            v = GEOMETRY_HELPER_INVALID_INDEX;
        }

        void Set(ionU32 _u, ionU32 _v)
        {
            u = _u;
            v = _v;
        }
    };

    struct _Face
    {
        _Edge e[3];

        _Face() {}
        _Face(ionU32 _a, ionU32 _b, ionU32 _c)
        {
            e[0].Set(_a, _b);
            e[1].Set(_b, _c);
            e[2].Set(_c, _a);
        }
    };


    //
    ionU32 triangleCount = _indexCount / 3;
    eosVector(_Face*) faces;

    // create faces
    for (ionU32 i = 0; i < triangleCount; ++i)
    {
        faces.push_back(eosNew(_Face, ION_MEMORY_ALIGNMENT_SIZE, _indexList[i * 3], _indexList[i * 3 + 1], _indexList[i * 3 + 2]));
    }


    //
    Vector* tan1 = (Vector*)eosNewRaw(sizeof(Vector) * _vectorCount * 2, ION_MEMORY_ALIGNMENT_SIZE);
    Vector* tan2 = tan1 + _vectorCount;
    memset(tan1, 0, _vectorCount * sizeof(Vector) * 2);


    for (ionU32 i = 0; i < triangleCount; ++i)
    {
        const ionU32 i1 = faces[i]->e[0].u;
        const ionU32 i2 = faces[i]->e[1].u;
        const ionU32 i3 = faces[i]->e[2].u;

        //
        const Vector& v1 = _vectorArray[i1];
        const Vector& v2 = _vectorArray[i2];
        const Vector& v3 = _vectorArray[i3];

        const ionFloat v1_x = VectorHelper::ExtractElement_0(v1);
        const ionFloat v1_y = VectorHelper::ExtractElement_1(v1);
        const ionFloat v1_z = VectorHelper::ExtractElement_2(v1);

        const ionFloat v2_x = VectorHelper::ExtractElement_0(v2);
        const ionFloat v2_y = VectorHelper::ExtractElement_1(v2);
        const ionFloat v2_z = VectorHelper::ExtractElement_2(v2);

        const ionFloat v3_x = VectorHelper::ExtractElement_0(v3);
        const ionFloat v3_y = VectorHelper::ExtractElement_1(v3);
        const ionFloat v3_z = VectorHelper::ExtractElement_2(v3);

        //
        const Vector& w1 = _textCoordUVUVArray[i1];
        const Vector& w2 = _textCoordUVUVArray[i2];
        const Vector& w3 = _textCoordUVUVArray[i3];

        const ionFloat w1_u = VectorHelper::ExtractElement_0(w1);
        const ionFloat w1_v = VectorHelper::ExtractElement_1(w1);

        const ionFloat w2_u = VectorHelper::ExtractElement_0(w2);
        const ionFloat w2_v = VectorHelper::ExtractElement_1(w2);

        const ionFloat w3_u = VectorHelper::ExtractElement_0(w3);
        const ionFloat w3_v = VectorHelper::ExtractElement_1(w3);

        //
        const ionFloat x1 = v2_x - v1_x;
        const ionFloat x2 = v3_x - v1_x;
        const ionFloat y1 = v2_y - v1_y;
        const ionFloat y2 = v3_y - v1_y;
        const ionFloat z1 = v2_z - v1_z;
        const ionFloat z2 = v3_z - v1_z;

        const ionFloat s1 = w2_u - w1_u;
        const ionFloat s2 = w3_u - w1_u;
        const ionFloat t1 = w2_v - w1_v;
        const ionFloat t2 = w3_v - w1_v;

        //
        ionFloat r = 1.0f / (s1 * t2 - s2 * t1);
        Vector sdir((t2 * x1 - t1 * x2) * r, (t2 * y1 - t1 * y2) * r, (t2 * z1 - t1 * z2) * r, 1.0f);
        Vector tdir((s1 * x2 - s2 * x1) * r, (s1 * y2 - s2 * y1) * r, (s1 * z2 - s2 * z1) * r, 1.0f);

        //
        tan1[i1] += sdir;
        tan1[i2] += sdir;
        tan1[i3] += sdir;

        tan2[i1] += tdir;
        tan2[i2] += tdir;
        tan2[i3] += tdir;
    }

    for (ionU32 i = 0; i < _vectorCount; ++i)
    {
        const Vector& n = _normalArray[i];
        const Vector& t = tan1[i];

        // Gram-Schmidt orthogonalized
        _outTangentVectorArray[i] = (t - n * n.Dot3(t)).Normalize();

        // Calculate handedness
        const Vector a = n.Cross(t);
        const Vector d = a.Dot3(tan2[i]);
        const ionFloat dot = VectorHelper::ExtractElement_0(d);
        const ionFloat tangentW = (dot < 0.0f) ? -1.0f : 1.0f;
        
        // Very ugly, but because I need just here I'm no doing a new SSE helper for now
        const ionFloat tangentX = VectorHelper::ExtractElement_0(_outTangentVectorArray[i]);
        const ionFloat tangentY = VectorHelper::ExtractElement_1(_outTangentVectorArray[i]);
        const ionFloat tangentZ = VectorHelper::ExtractElement_2(_outTangentVectorArray[i]);

        _outTangentVectorArray[i] = VectorHelper::Set(tangentX, tangentY, tangentZ, tangentW);
    }

    //
    // Clear
    eosDeleteRaw(tan1);

    for (ionU32 i = 0; i < faces.size(); ++i)
    {
        eosDelete(faces[i]);
    }
    faces.clear();
}


ION_NAMESPACE_END