#include "VertexCacheManager.h"

#include "UniformBufferObject.h"

#include "../Dependencies/Eos/Eos/Eos.h"

EOS_USING_NAMESPACE

ION_NAMESPACE_BEGIN


VertexCacheManager *VertexCacheManager::s_instance = nullptr;


VertexCacheManager::VertexCacheManager()
{
}

VertexCacheManager::~VertexCacheManager()
{
}

ionBool VertexCacheManager::Init(const VkDevice& _device, VkDeviceSize _uniformBufferOffsetAlignment)
{
    m_device = _device;

    m_uniformBufferOffsetAlignment = _uniformBufferOffsetAlignment;

    m_mostUsedVertex = 0;
    m_mostUsedIndex = 0;
    m_mostUsedJoint = 0;
    m_mostUsedStorage = 0;

    Alloc(m_frameData, ION_VERTCACHE_VERTEX_MEMORY_PER_FRAME, ION_VERTCACHE_INDEX_MEMORY_PER_FRAME, ION_VERTCACHE_STORAGE_MEMORY_PER_FRAME, ION_VERTCACHE_JOINT_MEMORY_PER_FRAME, EBufferUsage_Dynamic);
    Alloc(m_staticData, ION_STATIC_VERTEX_MEMORY, ION_STATIC_INDEX_MEMORY, ION_STATIC_STORAGE_MEMORY, 0, EBufferUsage_Static);  // I don't want joint or storage in the static for now at least!

    //MapGeometryBufferSet(m_frameData);

    return true;
}

void VertexCacheManager::Shutdown()
{
    m_staticData.m_vertexBuffer.Free();
    m_staticData.m_indexBuffer.Free();
    m_staticData.m_storageBuffer.Free();
    m_staticData.m_jointBuffer.Free();

    m_frameData.m_vertexBuffer.Free();
    m_frameData.m_indexBuffer.Free();
    m_frameData.m_storageBuffer.Free();
    m_frameData.m_jointBuffer.Free();
}

void VertexCacheManager::Create()
{
    if (!s_instance)
    {
        s_instance = eosNew(VertexCacheManager, ION_MEMORY_ALIGNMENT_SIZE);
    }
}

void VertexCacheManager::Destroy()
{
    if (s_instance)
    {
        eosDelete(s_instance);
        s_instance = nullptr;
    }
}

VertexCacheManager& VertexCacheManager::Instance()
{
    return *s_instance;
}

void VertexCacheManager::PurgeAll()
{
    Shutdown();
    Init(m_device, m_uniformBufferOffsetAlignment);
}

void VertexCacheManager::FreeStaticData()
{
    ClearGeometryBufferSet(m_staticData);
    m_mostUsedVertex = 0;
    m_mostUsedIndex = 0;
    m_mostUsedStorage = 0;
    m_mostUsedJoint = 0;
}

void VertexCacheManager::ClearGeometryBufferSet(GeometryBufferSet& _buffer)
{
    _buffer.m_indexMemUsed = 0;
    _buffer.m_vertexMemUsed = 0;
    _buffer.m_storageMemUsed = 0;
    _buffer.m_jointMemUsed = 0;
    _buffer.m_allocations = 0;
}

void VertexCacheManager::MapGeometryBufferSet(GeometryBufferSet& _buffer)
{
    if (_buffer.m_mappedVertexBase == nullptr)
    {
        _buffer.m_mappedVertexBase = (ionU8*)_buffer.m_vertexBuffer.MapBuffer(EBufferMappingType_Write);
    }
    if (_buffer.m_mappedIndexBase == nullptr)
    {
        _buffer.m_mappedIndexBase = (ionU8*)_buffer.m_indexBuffer.MapBuffer(EBufferMappingType_Write);
    }
    if (_buffer.m_mappedStorageBase == nullptr)
    {
        _buffer.m_mappedStorageBase = (ionU8*)_buffer.m_storageBuffer.MapBuffer(EBufferMappingType_Write);
    }
    if (_buffer.m_mappedJointBase == nullptr && _buffer.m_jointBuffer.GetAllocedSize() != 0)
    {
        _buffer.m_mappedJointBase = (ionU8*)_buffer.m_jointBuffer.MapBuffer(EBufferMappingType_Write);
    }
}

void VertexCacheManager::UnmapGeometryBufferSet(GeometryBufferSet& _buffer)
{
    if (_buffer.m_mappedVertexBase != nullptr)
    {
        _buffer.m_vertexBuffer.UnmapBuffer();
        _buffer.m_mappedVertexBase = nullptr;
    }
    if (_buffer.m_mappedIndexBase != nullptr) 
    {
        _buffer.m_indexBuffer.UnmapBuffer();
        _buffer.m_mappedIndexBase = nullptr;
    }
    if (_buffer.m_mappedStorageBase != nullptr)
    {
        _buffer.m_storageBuffer.UnmapBuffer();
        _buffer.m_mappedStorageBase = nullptr;
    }
    if (_buffer.m_mappedJointBase != nullptr)
    {
        _buffer.m_jointBuffer.UnmapBuffer();
        _buffer.m_mappedJointBase = nullptr;
    }
}

void VertexCacheManager::Alloc(GeometryBufferSet& _buffer, ionSize _vertexBytes, ionSize _indexBytes, ionSize _storageBytes, ionSize _jointBytes, EBufferUsage _usage)
{
    _buffer.m_vertexBuffer.Alloc(m_device, nullptr, _vertexBytes, _usage);
    _buffer.m_indexBuffer.Alloc(m_device, nullptr, _indexBytes, _usage);
    _buffer.m_storageBuffer.Alloc(m_device, nullptr, _storageBytes, _usage);

    if (_jointBytes > 0)
    {
        _buffer.m_jointBuffer.Alloc(m_device, nullptr, _jointBytes, _usage);
    }

    ClearGeometryBufferSet(_buffer);
}

VertexCacheHandler VertexCacheManager::Alloc(GeometryBufferSet& _buffer, const void* _data, ionSize _bytes, ECacheType _type)
{
    if (_bytes == 0)
    {
        return (VertexCacheHandler)0;
    }
    
    ionAssertReturnValue((((uintptr_t)(_data)) & 15) == 0, "Pointer is misaligned", (VertexCacheHandler)0);
    ionAssertReturnValue((_bytes & 15) == 0, "size is misaligned", (VertexCacheHandler)0);

    ionSize    endPos = 0;
    ionSize offset = 0;

    switch (_type)
    {
    case ECacheType_Index:
    {
        _buffer.m_indexMemUsed.fetch_add(_bytes, std::memory_order_relaxed);
        endPos = _buffer.m_indexMemUsed.load();
        if (endPos > _buffer.m_indexBuffer.GetAllocedSize())
        {
            ionAssertReturnValue(false, "Out of index cache", (VertexCacheHandler)0);
        }

        offset = endPos - _bytes;

        if (_data != nullptr) 
        {
            if (_buffer.m_indexBuffer.GetUsage() == EBufferUsage_Dynamic)
            {
                MapGeometryBufferSet(_buffer);
            }
            _buffer.m_indexBuffer.Update(_data, _bytes, offset);
        }

        break;
    }
    case ECacheType_Vertex: 
    {
        _buffer.m_vertexMemUsed.fetch_add(_bytes, std::memory_order_relaxed);
        endPos = _buffer.m_vertexMemUsed.load();
        if (endPos > _buffer.m_vertexBuffer.GetAllocedSize())
        {
            ionAssertReturnValue(false, "Out of vertex cache", (VertexCacheHandler)0);
        }

        offset = endPos - _bytes;

        if (_data != nullptr) 
        {
            if (_buffer.m_vertexBuffer.GetUsage() == EBufferUsage_Dynamic)
            {
                MapGeometryBufferSet(_buffer);
            }
            _buffer.m_vertexBuffer.Update(_data, _bytes, offset);
        }

        break;
    }
    case ECacheType_Storage:
    {
        _buffer.m_storageMemUsed.fetch_add(_bytes, std::memory_order_relaxed);
        endPos = _buffer.m_storageMemUsed.load();
        if (endPos > _buffer.m_storageBuffer.GetAllocedSize())
        {
            ionAssertReturnValue(false, "Out of vertex cache", (VertexCacheHandler)0);
        }

        offset = endPos - _bytes;

        if (_data != nullptr)
        {
            if (_buffer.m_storageBuffer.GetUsage() == EBufferUsage_Dynamic)
            {
                MapGeometryBufferSet(_buffer);
            }
            _buffer.m_storageBuffer.Update(_data, _bytes, offset);
        }

        break;
    }
    case ECacheType_Joint: 
    {
        _buffer.m_jointMemUsed.fetch_add(_bytes, std::memory_order_relaxed);
        endPos = _buffer.m_jointMemUsed.load();
        if (endPos > _buffer.m_jointBuffer.GetAllocedSize())
        {
            ionAssertReturnValue(false, "Out of joint cache", (VertexCacheHandler)0);
        }

        offset = endPos - _bytes;

        if (_data != nullptr) 
        {
            if (_buffer.m_jointBuffer.GetUsage() == EBufferUsage_Dynamic)
            {
                MapGeometryBufferSet(_buffer);
            }
            _buffer.m_jointBuffer.Update(_data, _bytes, offset);
        }

        break;
    }
    default:
        assert(false);
    }

    ++_buffer.m_allocations;

    VertexCacheHandler handler = ((ionU64)(offset & ION_VERTCACHE_OFFSET_MASK) << ION_VERTCACHE_OFFSET_SHIFT) | ((ionU64)(_bytes & ION_VERTCACHE_SIZE_MASK) << ION_VERTCACHE_SIZE_SHIFT);
    if (&_buffer == &m_staticData) 
    {
        handler |= ION_VERTCACHE_STATIC;
    }
    return handler;
}


VertexCacheHandler VertexCacheManager::AllocVertex(const void* _data, ionSize _num, ionSize _size /*= sizeof(Vertex)*/)
{
    eosSize uiMask = ION_VERTEX_CACHE_ALIGN - 1;
    eosSize uiSize = ((_num * _size) + uiMask) & ~uiMask;

    return Alloc(m_frameData, _data, uiSize, ECacheType_Vertex);
}

VertexCacheHandler VertexCacheManager::AllocIndex(const void* _data, ionSize _num, ionSize _size /*= sizeof(Index)*/)
{
    eosSize uiMask = ION_INDEX_CACHE_ALIGN - 1;
    eosSize uiSize = ((_num * _size) + uiMask) & ~uiMask;

    return Alloc(m_frameData, _data, uiSize, ECacheType_Index);
}

VertexCacheHandler VertexCacheManager::AllocStorage(const void* _data, ionSize _num, ionSize _size /*= sizeof(VertexMorphTarget)*/)
{
    eosSize uiMask = ION_STORAGE_CACHE_ALIGN - 1;
    eosSize uiSize = ((_num * _size) + uiMask) & ~uiMask;

    return Alloc(m_frameData, _data, uiSize, ECacheType_Storage);
}

VertexCacheHandler VertexCacheManager::AllocJoint(const void* _data, ionSize _num, ionSize _size /*= sizeof(Matrix)*/)
{
    eosSize uiMask = m_uniformBufferOffsetAlignment - 1;
    eosSize uiSize = ((_num * _size) + uiMask) & ~uiMask;

    return Alloc(m_frameData, _data, uiSize, ECacheType_Joint);
}

VertexCacheHandler VertexCacheManager::AllocStaticVertex(const void* _data, ionSize _bytes)
{
    ionAssertReturnValue(m_staticData.m_vertexMemUsed.load() + _bytes <= ION_STATIC_VERTEX_MEMORY, "AllocStaticVertex failed", (VertexCacheHandler)0);
    return Alloc(m_staticData, _data, _bytes, ECacheType_Vertex);
}

VertexCacheHandler VertexCacheManager::AllocStaticIndex(const void* _data, ionSize _bytes)
{
    ionAssertReturnValue(m_staticData.m_indexMemUsed.load() + _bytes <= ION_STATIC_INDEX_MEMORY, "AllocStaticIndex failed", (VertexCacheHandler)0);
    return Alloc(m_staticData, _data, _bytes, ECacheType_Index);
}

VertexCacheHandler VertexCacheManager::AllocStaticStorage(const void* _data, ionSize _bytes)
{
    ionAssertReturnValue(m_staticData.m_storageMemUsed.load() + _bytes <= ION_STATIC_STORAGE_MEMORY, "AllocStaticStorage failed", (VertexCacheHandler)0);
    return Alloc(m_staticData, _data, _bytes, ECacheType_Storage);
}

ionU8* VertexCacheManager::MappedVertexBuffer(VertexCacheHandler _handler)
{
    ionAssertReturnValue(!CacheIsStatic(_handler), "Cache is static!", nullptr);
    const ionU64 offset = (ionS32)(_handler >> ION_VERTCACHE_OFFSET_SHIFT) & ION_VERTCACHE_OFFSET_MASK;
    return m_frameData.m_mappedVertexBase + offset;
}

ionU8* VertexCacheManager::MappedIndexBuffer(VertexCacheHandler _handler)
{
    ionAssertReturnValue(!CacheIsStatic(_handler), "Cache is static!", nullptr);
    const ionU64 offset = (ionS32)(_handler >> ION_VERTCACHE_OFFSET_SHIFT) & ION_VERTCACHE_OFFSET_MASK;
    return m_frameData.m_mappedIndexBase + offset;
}

ionU8* VertexCacheManager::MappedStorageBuffer(VertexCacheHandler _handler)
{
    ionAssertReturnValue(!CacheIsStatic(_handler), "Cache is static!", nullptr);
    const ionU64 offset = (ionS32)(_handler >> ION_VERTCACHE_OFFSET_SHIFT) & ION_VERTCACHE_OFFSET_MASK;
    return m_frameData.m_mappedStorageBase + offset;
}

ionBool VertexCacheManager::GetVertexBuffer(VertexCacheHandler _handler, VertexBuffer* _vb)
{
    if (_handler == 0)
    {
        return false;
    }

    const ionS32 isStatic = _handler & ION_VERTCACHE_STATIC;
    const ionU64 size = (ionS32)(_handler >> ION_VERTCACHE_SIZE_SHIFT) & ION_VERTCACHE_SIZE_MASK;
    const ionU64 offset = (ionS32)(_handler >> ION_VERTCACHE_OFFSET_SHIFT) & ION_VERTCACHE_OFFSET_MASK;

    if (isStatic) 
    {
        _vb->ReferenceTo(m_staticData.m_vertexBuffer, offset, size);
        return true;
    }

    _vb->ReferenceTo(m_frameData.m_vertexBuffer, offset, size);
    return true;
}

ionBool VertexCacheManager::GetIndexBuffer(VertexCacheHandler _handler, IndexBuffer* _ib)
{
    if (_handler == 0)
    {
        return false;
    }

    const ionS32 isStatic = _handler & ION_VERTCACHE_STATIC;
    const ionU64 size = (ionS32)(_handler >> ION_VERTCACHE_SIZE_SHIFT) & ION_VERTCACHE_SIZE_MASK;
    const ionU64 offset = (ionS32)(_handler >> ION_VERTCACHE_OFFSET_SHIFT) & ION_VERTCACHE_OFFSET_MASK;

    if (isStatic)
    {
        _ib->ReferenceTo(m_staticData.m_indexBuffer, offset, size);
        return true;
    }
    _ib->ReferenceTo(m_frameData.m_indexBuffer, offset, size);
    return true;
}

ionBool VertexCacheManager::GetStorageBuffer(VertexCacheHandler _handler, StorageBuffer* _sb)
{
    if (_handler == 0)
    {
        return false;
    }

    const ionS32 isStatic = _handler & ION_VERTCACHE_STATIC;
    const ionU64 size = (ionS32)(_handler >> ION_VERTCACHE_SIZE_SHIFT) & ION_VERTCACHE_SIZE_MASK;
    const ionU64 offset = (ionS32)(_handler >> ION_VERTCACHE_OFFSET_SHIFT) & ION_VERTCACHE_OFFSET_MASK;

    if (isStatic)
    {
        _sb->ReferenceTo(m_staticData.m_storageBuffer, offset, size);
        return true;
    }

    _sb->ReferenceTo(m_frameData.m_storageBuffer, offset, size);
    return true;
}

ionBool VertexCacheManager::GetJointBuffer(VertexCacheHandler _handler, UniformBuffer* _jb)
{
    if (_handler == 0)
    {
        return false;
    }

    const ionS32 isStatic = _handler & ION_VERTCACHE_STATIC;
    const ionU64 size = (ionS32)(_handler >> ION_VERTCACHE_SIZE_SHIFT) & ION_VERTCACHE_SIZE_MASK;
    const ionU64 offset = (ionS32)(_handler >> ION_VERTCACHE_OFFSET_SHIFT) & ION_VERTCACHE_OFFSET_MASK;

    if (isStatic)
    {
        _jb->ReferenceTo(m_staticData.m_jointBuffer, offset, size);
        return true;
    }

    _jb->ReferenceTo(m_frameData.m_jointBuffer, offset, size);
    return true;
}


void VertexCacheManager::BeginMapping()
{
    MapGeometryBufferSet(m_frameData);
    ClearGeometryBufferSet(m_frameData);
}

void VertexCacheManager::EndMapping()
{
    m_mostUsedVertex = std::max(m_mostUsedVertex, m_frameData.m_vertexMemUsed.load());
    m_mostUsedIndex = std::max(m_mostUsedIndex, m_frameData.m_indexMemUsed.load());
    m_mostUsedStorage = std::max(m_mostUsedStorage, m_frameData.m_storageMemUsed.load());
    m_mostUsedJoint = std::max(m_mostUsedJoint, m_frameData.m_jointMemUsed.load());

    // unmap the current frame so the GPU can read it
    UnmapGeometryBufferSet(m_frameData);
    UnmapGeometryBufferSet(m_staticData);
}


ION_NAMESPACE_END