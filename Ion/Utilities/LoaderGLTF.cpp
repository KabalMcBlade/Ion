#include "LoaderGLTF.h"


ION_NAMESPACE_BEGIN

namespace GLTF
{
    Loader::Loader()
    {
        m_attrType2CompCnt.insert({ "SCALAR", 1 });
        m_attrType2CompCnt.insert({ "VEC2", 2 });
        m_attrType2CompCnt.insert({ "VEC3", 3 });
        m_attrType2CompCnt.insert({ "VEC4", 4 });
        m_attrType2CompCnt.insert({ "MAT2", 4 });
        m_attrType2CompCnt.insert({ "MAT3", 9 });
        m_attrType2CompCnt.insert({ "MAT4", 16 });

        m_compType2ByteSize.insert({ EComponentType_s8, 1 });
        m_compType2ByteSize.insert({ EComponentType_u8, 1 });
        m_compType2ByteSize.insert({ EComponentType_s16, 2 });
        m_compType2ByteSize.insert({ EComponentType_u16, 2 });
        m_compType2ByteSize.insert({ EComponentType_Float, 4 });
    }

    Loader::~Loader()
    {

    }

};

ION_NAMESPACE_END
