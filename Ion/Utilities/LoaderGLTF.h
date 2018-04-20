#pragma once

#include "../Core/CoreDefs.h"

#include "../Dependencies/Eos/Eos/Eos.h"
#include "../Dependencies/Nix/Nix/Nix.h"


EOS_USING_NAMESPACE
NIX_USING_NAMESPACE

ION_NAMESPACE_BEGIN

namespace GLTF 
{
    enum EComponentType
    {
        EComponentType_s8 = 5120,
        EComponentType_u8 = 5121,
        EComponentType_s16 = 5122,
        EComponentType_u16 = 5123,
        EComponentType_Float = 5126
    };

    struct Accessor
    {
        ionU32 m_bufferView;
        ionU32 m_byteOffset;
        EComponentType m_componentType;
        ionU32 m_count; // number of aggregates (not components)
        eosString m_type;
    };

    struct BufferView
    {
        ionU32 buffer;
        ionU32 byteOffset;
        ionU32 byteLength;
    };

    typedef eosVector(ionS8) Buffer;

    template <typename T>
    class BufferIterator
    {
    public:
        BufferIterator(const T *b, const T *e) : cur(b), end(e) {}

        ionBool HasNext() const { return cur != end; }

        T GetNext() { return *cur++; }

    private:
        const T *cur = nullptr;
        const T *end = nullptr;
    };


    struct Image
    {
        ionU32 m_width;
        ionU32 m_height;
        ionU32 m_component;
        ionU32 m_levelCount;
        eosVector(ionS8) m_buffer;
    };

    struct Texture
    {
        ionU32 m_sampler;
        ionU32 m_imageIndex;
    };

    struct Material
    {
        ionU32 m_albedoTexture = std::numeric_limits<ionU32>::max();
        ionU32 m_normalTexture = std::numeric_limits<ionU32>::max();
        ionU32 m_roughnessTexture = std::numeric_limits<ionU32>::max();
        ionU32 m_metallicTexture = std::numeric_limits<ionU32>::max();
        ionU32 m_aoTexture = std::numeric_limits<ionU32>::max();
        ionU32 m_emissiveTexture = std::numeric_limits<ionU32>::max();
    };

    // Mesh is defined as an aggregate of all the geometry of the same material
    struct Mesh
    {
        eosVector(ionFloat) m_positions;
        eosVector(ionFloat) m_normals;
        eosVector(ionFloat) m_texCoords;
        eosVector(ionU32) m_indices;

        Image m_albedoMap;
        Image m_normalMap;
        Image m_roughnessMap;
        Image m_metallicMap;
        Image m_aoMap;
        Image m_emissiveMap;
    };

    struct Node
    {
        eosVector(ionU32) m_children;
        ionU32 m_mesh = std::numeric_limits<ionU32>::max();
        Matrix m_localToParent;
    };

    struct Scene
    {
        eosVector(Mesh) m_meshes;
    };


    class ION_DLL Loader
    {
    public:

        Loader();
        ~Loader();

    private:
        //eosUnorderedMap(eosString, ionU32) m_attrType2CompCnt;
        //eosUnorderedMap(EComponentType, ionU32) m_compType2ByteSize;

        
        Loader(const Loader& _Orig) = delete;
        Loader& operator = (const Loader&) = delete;
    };

};

ION_NAMESPACE_END