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


#define ION_VERTCACHE_INDEX_MEMORY_PER_FRAME    31 * 1024 * 1024
#define ION_VERTCACHE_VERTEX_MEMORY_PER_FRAME   31 * 1024 * 1024
#define ION_VERTCACHE_JOINT_MEMORY_PER_FRAME    256 * 1024


#define ION_STATIC_INDEX_MEMORY                 31 * 1024 * 1024
#define ION_STATIC_VERTEX_MEMORY                31 * 1024 * 1024	// make sure it fits in ION_VERTCACHE_OFFSET_MASK!


#define ION_VERTCACHE_STATIC        1					// in the static set, not the per-frame set
#define ION_VERTCACHE_SIZE_SHIFT    1
#define ION_VERTCACHE_SIZE_MASK     0x7fffff		// 8 megs 
#define ION_VERTCACHE_OFFSET_SHIFT  24
#define ION_VERTCACHE_OFFSET_MASK   0x1ffffff	// 32 megs 
#define ION_VERTCACHE_FRAME_SHIFT   49
#define ION_VERTCACHE_FRAME_MASK    0x7fff		// 15 bits = 32k frames to wrap around

#define ION_VERTEX_CACHE_ALIGN      32
#define ION_INDEX_CACHE_ALIGN       16
#define ION_JOINT_CACHE_ALIGN       16


NIX_USING_NAMESPACE
ION_NAMESPACE_BEGIN


enum ECacheType 
{
    ECacheType_Vertex = 0,
    ECacheType_Index,
    ECacheType_Joint
};

struct GeometryBufferSet final
{
private:
    typedef std::atomic_size_t   AtomicSize;

public:
    IndexBuffer			m_indexBuffer;
    VertexBuffer		m_vertexBuffer;
    UniformBuffer		m_jointBuffer;
    ionU8*				m_mappedVertexBase;
    ionU8*              m_mappedIndexBase;
    ionU8*              m_mappedJointBase;
    AtomicSize	        m_indexMemUsed;
    AtomicSize	        m_vertexMemUsed;
    AtomicSize	        m_jointMemUsed;
    ionS32				m_allocations;	// index and vertex allocations count combined
};


class VertexCacheManager final
{
public:
    ION_NO_INLINE static void Create();
    ION_NO_INLINE static void Destroy();

    ION_NO_INLINE static VertexCacheManager& Instance();

    ionBool Init(const VkDevice& _device, VkDeviceSize _uniformBufferOffsetAlignment);
    void    Shutdown();

    ionS32 GetCurrentFrame()    { return m_currentFrame; }
    ionS32 GetFrameListCount()  { return m_listNum; }
    ionS32 GetDrawListCount()   { return m_drawListNum; }
    VkDeviceSize GetUniformBufferOffsetAlignment()   { return m_uniformBufferOffsetAlignment; }

    const GeometryBufferSet& GetStaticData() const { return m_staticData; }
    const GeometryBufferSet& GetFrameData(ionU32 _frame) const { return m_frameData[_frame % ION_RENDER_BUFFER_COUNT]; }
    const GeometryBufferSet* GetFrameData() const { return m_frameData; }

    // purge everything
    void PurgeAll();

    // if we load static data like a persistent level, we can free calling this function when a new one is loaded
    void FreeStaticData();

    // this data is only valid for one frame of rendering
    VertexCacheHandler	AllocVertex(const void* _data, ionS32 _num, ionSize _size = sizeof(Vertex));
    VertexCacheHandler	AllocIndex(const void* _data, ionS32 _num, ionSize _size = sizeof(Index));
    VertexCacheHandler	AllocJoint(const void* _data, ionS32 _num, ionSize _size = sizeof(Matrix));

    // this data is valid until the next level load
    VertexCacheHandler	AllocStaticVertex(const void* _data, ionSize _bytes);
    VertexCacheHandler	AllocStaticIndex(const void* _data, ionSize _bytes);

    ionU8* MappedVertexBuffer(VertexCacheHandler _handler);
    ionU8* MappedIndexBuffer(VertexCacheHandler _handler);

    // Returns false if it's been purged
    // This can only be called by the front end, the back end should only be looking at
    // VertexCacheHandler that are already validated.
    ionBool			CacheIsCurrent(const VertexCacheHandler _handler);
    static ionBool	CacheIsStatic(const VertexCacheHandler _handler) { return (_handler & ION_VERTCACHE_STATIC) != 0; }

    // vb/ib is a temporary reference -- don't store it
    ionBool			GetVertexBuffer(VertexCacheHandler _handler, VertexBuffer* _vb);
    ionBool			GetIndexBuffer(VertexCacheHandler _handler, IndexBuffer* _ib);
    ionBool			GetJointBuffer(VertexCacheHandler _handler, UniformBuffer* _jb);

    void			BeginFrame();

public:
    VertexCacheManager();
    ~VertexCacheManager();

private:
    VertexCacheManager(const VertexCacheManager& _Orig) = delete;
    VertexCacheManager& operator = (const VertexCacheManager&) = delete;

    void Alloc(GeometryBufferSet& _buffer, ionSize _vertexBytes, ionSize _indexBytes, ionSize _jointBytes, EBufferUsage _usage);
    VertexCacheHandler Alloc(GeometryBufferSet& _buffer, const void* _data, ionSize _bytes, ECacheType _type);

    void ClearGeometryBufferSet(GeometryBufferSet& _buffer);
    void MapGeometryBufferSet(GeometryBufferSet& _buffer);
    void UnmapGeometryBufferSet(GeometryBufferSet& _buffer);

private:
    ionS32				m_currentFrame;	// for determining the active buffers
    ionS32				m_listNum;		// currentFrame % NUM_FRAME_DATA
    ionS32				m_drawListNum;	// (currentFrame-1) % NUM_FRAME_DATA
    VkDeviceSize		m_uniformBufferOffsetAlignment;

    // High water marks for the per-frame buffers
    ionSize				m_mostUsedVertex;
    ionSize				m_mostUsedIndex;
    ionSize				m_mostUsedJoint;

    VkDevice            m_device;
    GeometryBufferSet	m_staticData;
    GeometryBufferSet	m_frameData[ION_RENDER_BUFFER_COUNT];

private:
    static VertexCacheManager *s_instance;
};

ION_NAMESPACE_END


#define ionVertexCacheManager() ion::VertexCacheManager::Instance()