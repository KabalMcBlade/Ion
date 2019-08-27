#include "MemoryManager.h"

ION_NAMESPACE_BEGIN


MemoryManager::MemoryManager()
	: m_genericHeapAllocatorArea(/*0, */ION_MAX_MEMORY_SIZE_GENERIC), m_vectorHeapAllocatorArea(/*0, */ION_MAX_MEMORY_SIZE_VECTOR), m_mapHeapAllocatorArea(/*0, */ION_MAX_MEMORY_SIZE_MAP),
	m_stringHeapAllocatorArea(/*0, */ION_MAX_MEMORY_SIZE_STRING), m_smartPointerHeapAllocatorArea(/*0, */ION_MAX_MEMORY_SIZE_SMARTPTR), /*m_streamPointerHeapAllocatorArea(),*/

    m_genericHeapAllocator(m_genericHeapAllocatorArea, "Generic_HeapAllocator"), m_vectorHeapAllocator(m_vectorHeapAllocatorArea, "Vector_HeapAllocator"), m_mapHeapAllocator(m_mapHeapAllocatorArea, "Map_HeapAllocator"),
    m_stringHeapAllocator(m_stringHeapAllocatorArea, "String_HeapAllocator"), m_smartPointerHeapAllocator(m_smartPointerHeapAllocatorArea, "SmartPointer_HeapAllocator")/*, m_streamPointerHeapAllocator(m_streamPointerHeapAllocatorArea, "Stream_HeapAllocator")*/
{

}

ION_NAMESPACE_END