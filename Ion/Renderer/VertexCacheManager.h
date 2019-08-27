#pragma once

#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>

#include "../Dependencies/Nix/Nix/Nix.h"

#include "../Core/CoreDefs.h"

#include "RenderDefs.h"
#include "RenderCommon.h"

#include "BaseBufferObject.h"
#include "UniformBufferObject.h"
#include "VertexBufferObject.h"
#include "IndexBufferObject.h"
#include "StorageBufferObject.h"

#define ION_VERTCACHE_INDEX_MEMORY_PER_FRAME    511ULL * 1024ULL * 1024ULL
#define ION_VERTCACHE_VERTEX_MEMORY_PER_FRAME   1023ULL * 1024ULL * 1024ULL
#define ION_VERTCACHE_STORAGE_MEMORY_PER_FRAME  1023ULL * 1024ULL * 1024ULL
//#define ION_VERTCACHE_JOINT_MEMORY_PER_FRAME    256 * 1024
#define ION_VERTCACHE_JOINT_MEMORY_PER_FRAME    0       // I'm not going to use join at the moment


#define ION_STATIC_INDEX_MEMORY                 255ULL * 1024ULL * 1024ULL
#define ION_STATIC_VERTEX_MEMORY                255ULL * 1024ULL * 1024ULL    // make sure it fits in ION_VERTCACHE_OFFSET_MASK!
#define ION_STATIC_STORAGE_MEMORY               255ULL * 1024ULL * 1024ULL    // make sure it fits in ION_VERTCACHE_OFFSET_MASK!

#define ION_VERTCACHE_STATIC        1                    // in the static set, not the per-frame set
#define ION_VERTCACHE_SIZE_SHIFT    1
#define ION_VERTCACHE_SIZE_MASK     0x1FFFFFF       // 32 megs 
#define ION_VERTCACHE_OFFSET_SHIFT  29
#define ION_VERTCACHE_OFFSET_MASK   0x7FFFFFFF        // 2048 megs        //0x3FFFFFFF       // 1024 megs 

#define ION_VERTEX_CACHE_ALIGN      16
#define ION_INDEX_CACHE_ALIGN       16
#define ION_STORAGE_CACHE_ALIGN     16
//#define ION_JOINT_CACHE_ALIGN       16


NIX_USING_NAMESPACE
ION_NAMESPACE_BEGIN


enum ECacheType 
{
    ECacheType_Vertex = 0,
    ECacheType_Index,
    ECacheType_Joint,
    ECacheType_Storage
};

struct GeometryBufferSet final
{
private:
    typedef std::atomic_size_t   AtomicSize;

public:
    IndexBuffer         m_indexBuffer;
    VertexBuffer        m_vertexBuffer;
    StorageBuffer       m_storageBuffer;
    UniformBuffer       m_jointBuffer;
    ionU8*              m_mappedVertexBase;
    ionU8*              m_mappedIndexBase;
    ionU8*              m_mappedStorageBase;
    ionU8*              m_mappedJointBase;
    AtomicSize          m_indexMemUsed;
    AtomicSize          m_vertexMemUsed;
    AtomicSize          m_storageMemUsed;
    AtomicSize          m_jointMemUsed;
    ionS32              m_allocations;    // index and vertex allocations count combined
};


class VertexCacheManager final
{
public:
    static VertexCacheManager& Instance();

    ionBool Init(const VkDevice& _device, VkDeviceSize _uniformBufferOffsetAlignment);
    void    Shutdown();

    VkDeviceSize GetUniformBufferOffsetAlignment()   { return m_uniformBufferOffsetAlignment; }

    const GeometryBufferSet& GetStaticData() const { return m_staticData; }
    const GeometryBufferSet& GetFrameData() const { return m_frameData; }

    // purge everything
    void PurgeAll();

    // if we load static data like a persistent level, we can free calling this function when a new one is loaded
    void FreeStaticData();

    // this data is only valid for one frame of rendering
    VertexCacheHandler    AllocVertex(const void* _data, ionSize _num, ionSize _size = sizeof(Vertex));
    VertexCacheHandler    AllocIndex(const void* _data, ionSize _num, ionSize _size = sizeof(Index));
    VertexCacheHandler    AllocStorage(const void* _data, ionSize _num, ionSize _size = sizeof(VertexMorphTarget));
    VertexCacheHandler    AllocJoint(const void* _data, ionSize _num, ionSize _size = sizeof(Matrix));

    // this data is valid until the next level load
    VertexCacheHandler    AllocStaticVertex(const void* _data, ionSize _bytes);
    VertexCacheHandler    AllocStaticIndex(const void* _data, ionSize _bytes);
    VertexCacheHandler    AllocStaticStorage(const void* _data, ionSize _bytes);

    ionU8* MappedVertexBuffer(VertexCacheHandler _handler);
    ionU8* MappedIndexBuffer(VertexCacheHandler _handler);
    ionU8* MappedStorageBuffer(VertexCacheHandler _handler);

    static ionBool    CacheIsStatic(const VertexCacheHandler _handler) { return (_handler & ION_VERTCACHE_STATIC) != 0; }

    // vb/ib is a temporary reference -- don't store it
    ionBool            GetVertexBuffer(VertexCacheHandler _handler, VertexBuffer* _vb);
    ionBool            GetIndexBuffer(VertexCacheHandler _handler, IndexBuffer* _ib);
    ionBool            GetStorageBuffer(VertexCacheHandler _handler, StorageBuffer* _sb);
    ionBool            GetJointBuffer(VertexCacheHandler _handler, UniformBuffer* _jb);

    void            BeginMapping();
    void            EndMapping();

public:
    VertexCacheManager();
    ~VertexCacheManager();

private:
    VertexCacheManager(const VertexCacheManager& _Orig) = delete;
    VertexCacheManager& operator = (const VertexCacheManager&) = delete;

    void Alloc(GeometryBufferSet& _buffer, ionSize _vertexBytes, ionSize _indexBytes, ionSize _storageBytes, ionSize _jointBytes, EBufferUsage _usage);
    VertexCacheHandler Alloc(GeometryBufferSet& _buffer, const void* _data, ionSize _bytes, ECacheType _type);

    void ClearGeometryBufferSet(GeometryBufferSet& _buffer);
    void MapGeometryBufferSet(GeometryBufferSet& _buffer);
    void UnmapGeometryBufferSet(GeometryBufferSet& _buffer);

private:
    VkDeviceSize        m_uniformBufferOffsetAlignment;

    // High water marks for the per-frame buffers
    ionSize                m_mostUsedVertex;
    ionSize                m_mostUsedIndex;
    ionSize                m_mostUsedStorage;
    ionSize                m_mostUsedJoint;

    VkDevice            m_device;
    GeometryBufferSet    m_staticData;
    GeometryBufferSet    m_frameData;
};

ION_NAMESPACE_END


#define ionVertexCacheManager() ion::VertexCacheManager::Instance()